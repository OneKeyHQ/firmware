#include "atca_api.h"
#include "atca_command.h"

#include "common.h"
#include "memory.h"
#include "otp.h"
#include "rng.h"
#include "sha2.h"
#include "util.h"

#define PIN_MAX_TRIES 10

// slot config
// |write-config|writekey|is secret|encryptread|limituse|nomac|readkey|
// |   15-12    |  11-8  |    7    |     6     |    5   |  4  |  3-0  |

// key config
// |x509id|RFU|persistentdisable|authkey|reqauth|reqrandom|lockable|keytype|pubinfo|private|
// |15-14 |13 |        12       | 11-8  |  7    |    6    |    5   | 4-2   |   1
// |   0   |

static const ATCAConfiguration atca_init_config = {
    .count_match = 0x01 | (SLOT_COUNTER_MATCH << 4),
    .chip_mode = 0x00,
    .slot_config =
        {
            0x008F,  // slot 0 private, permanet,int sing,ext sign,ecdh
            0x8F8F,  // slot 1,io protect ,no read,no write
            0x418F,  // slot 2 ,pin ,no read,encrypt write,wirte key 1
            0x8FAF,  // slot 3,pin attempt,no read and write,limiteuse
            0x410F,  // slot 4 ,counter match ,clear read,encrypt write,wirte
                     // key 1
            0x410F,  // slot 5 ,last good counter ,clear read,encrypt
                     // write,wirte key 1
            0x41C1,  // slot 6 ,user data,encrypt read and write,key 1
            0x410F,  // slot 7,clear read,encrypt write,key 1
            0x0F0F,  // slot 8,certificate ,clear read and write
            0x41C1,  // slot 9,encrypt read and write,key 1
            0x41C1,  // slot 10,encrypt read and write,key 1
            0x41C1,  // slot 11,encrypt read and write,key 1
            0x410F,  // slot 12,clear read,encrypt write,key 1
            0x410F,  // slot 13,clear read,encrypt write,key 1
            0x0F0F,  // slot 14,clear read and write
            0x0F0F   // slot 15,clear read and write
        },
    .counter0 = {0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
    .counter1 = {0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
    .use_lock = 0x00,
    .volatilekey_permission = 0x00,
    .secure_boot = 0x0000,
    .kdfiv_loc = 0x00,
    .kdfiv_str = 0x00,
    .reserved_2 = {0x00},
    .user_extra = 0x00,
    .user_extra_add = 0x00,
    .lock_value = ATCA_UNLOCKED,
    .lock_config = ATCA_UNLOCKED,
    .slot_locked = 0xffff,
    .chip_options = CONFIG_CHIP_OPTION_IO_PROTECT |
                    (SLOT_IO_PROTECT_KEY << 12) | CONFIG_CHIP_OPTION_KDF_PORT |
                    CONFIG_CHIP_OPTION_ECDH_PORT,
    .x509format = {0x00},
    .key_config = {
        0x01B3,  // slot 0,private key ,reqauth,authkey=1,lockable
        0x007C,  // slot 1,io protect,reqrandom,lockable
        0x01FC,  // slot 2,user pin,reqrandom,reqauth,authkey=1,lockable
        0x01BC,  // slot 3,pin attempt,reqauth,authkey=1,lockable
        0x01BC,  // slot 4,counter match,reqauth,authkey=1,lockable
        0x01BC,  // slot 5,last good counter,reqauth,authkey=1,lockable
        0x02BC,  // slot 6,user data,reqauth,authkey=2,locable
        0x01BC,  // slot 7,reqauth,authkey=1,lockable
        0x003C,  // slot 8,lockable
        0x01BC,  // slot 9,reqauth,authkey=1,lockable
        0x01BC,  // slot 10,reqauth,authkey=1,lockable
        0x01BC,  // slot 11,reqauth,authkey=1,lockable
        0x01BC,  // slot 12,reqauth,authkey=1,lockable
        0x01BC,  // slot 13,reqauth,authkey=1,lockable
        0x003C,  // slot 14,lockable
        0x003C   // slot 15,lockable
    }};

static ATCAPairingInfo pair_info_obj = {0};
ATCAPairingInfo *pair_info = &pair_info_obj;

static ATCAConfiguration atca_configuration;

#define atca_assert(expr, msg) \
  (((expr) == ATCA_SUCCESS)    \
       ? (void)0               \
       : __fatal_error(#expr, msg, __FILE__, __LINE__, __func__))

// aligned to 32
static const uint32_t counter0_init[8] = {32, 32, 0};

ATCA_STATUS atca_get_config(ATCAConfiguration *config) {
  ATCA_STATUS status;
  status = atca_read_config_zone((uint8_t *)config);
  return status;
}

ATCA_STATUS atca_get_state(ATCADeviceState *atca_stat) {
  ATCA_STATUS status;
  uint8_t buf[4];
  status = atca_info_base(INFO_MODE_STATE, 0x00, buf);
  if (status == ATCA_SUCCESS) {
    atca_stat->tempkey_nomac = (buf[0] >> 7) & 0x01;
    atca_stat->tempkey_genkey = (buf[0] >> 6) & 0x01;
    atca_stat->tempkey_gendig = (buf[0] >> 5) & 0x01;
    atca_stat->tempkey_source = (buf[0] >> 4) & 0x01;
    atca_stat->tempkey_keyid = buf[0] & 0x0f;
    atca_stat->tempkey_valid = (buf[1] >> 7) & 0x01;
    atca_stat->auth_valid = (buf[1] >> 2) & 0x01;
    atca_stat->auth_keyid = (buf[1] >> 3) & 0x0f;
  }
  return status;
}

void atca_config_init(void) {
  uint8_t zeros[32] = {0};
  uint8_t serial_no[32] = {0};
  uint8_t rand_buffer[32] = {0};

  memset(serial_no, 0xff, sizeof(serial_no));
  memcpy(&pair_info_obj,
         FLASH_PTR(FLASH_OTP_BASE +
                   FLASH_OTP_BLOCK_608_SERIAL * FLASH_OTP_BLOCK_SIZE),
         sizeof(pair_info_obj));

  atca_assert(atca_read_config_zone((uint8_t *)&atca_configuration),
              "get config");

  if (atca_configuration.lock_config == ATCA_UNLOCKED) {
    atca_assert(atca_write_config_zone((uint8_t *)&atca_init_config),
                "set config");

    atca_assert(atca_lock_config_zone(), "lock config");
    atca_assert(atca_read_config_zone((uint8_t *)&atca_configuration),
                "get config");
  }

  memcpy(serial_no, atca_configuration.sn1, ATECC608_SN1_SIZE);
  memcpy(serial_no + ATECC608_SN1_SIZE, atca_configuration.sn2,
         ATECC608_SN2_SIZE);

  // new st
  if (check_all_ones(pair_info->serial, sizeof(pair_info->serial))) {
    if (!flash_otp_is_locked(FLASH_OTP_BLOCK_608_SERIAL)) {
      ensure_ex(flash_otp_write_safe(FLASH_OTP_BLOCK_608_SERIAL, 0, serial_no,
                                     FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_BLOCK_608_SERIAL);
    } else {
      atca_assert(ATCA_BAD_PARAM, "OTP data err");
    }
  } else {
    if (memcmp(pair_info->serial, serial_no, ATECC608_SN_SIZE)) {
      atca_assert(ATCA_BAD_PARAM, "se changed");
    }
  }

  if (check_all_ones(pair_info->protect_key, sizeof(pair_info->protect_key))) {
    atca_assert(atca_random(rand_buffer), "get random");
    if (!flash_otp_is_locked(FLASH_OTP_BLOCK_608_PROTECT_KEY)) {
      ensure_ex(flash_otp_write_safe(FLASH_OTP_BLOCK_608_PROTECT_KEY, 0,
                                     rand_buffer, FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_BLOCK_608_PROTECT_KEY);
    } else {
      atca_assert(ATCA_BAD_PARAM, "OTP data err");
    }
  }

  if (check_all_ones(pair_info->init_pin, sizeof(pair_info->init_pin))) {
    atca_assert(atca_random(rand_buffer), "get random");
    if (!flash_otp_is_locked(FLASH_OTP_BLOCK_608_INIT_PIN)) {
      ensure_ex(flash_otp_write_safe(FLASH_OTP_BLOCK_608_INIT_PIN, 0,
                                     rand_buffer, FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_BLOCK_608_INIT_PIN);
    } else {
      atca_assert(ATCA_BAD_PARAM, "OTP data err");
    }
  }

  if (check_all_ones(pair_info->hash_mix, sizeof(pair_info->hash_mix))) {
    atca_assert(atca_random(rand_buffer), "get random");
    if (!flash_otp_is_locked(FLASH_OTP_BLOCK_608_MIX_PIN)) {
      ensure_ex(flash_otp_write_safe(FLASH_OTP_BLOCK_608_MIX_PIN, 0,
                                     rand_buffer, FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_BLOCK_608_MIX_PIN);
    } else {
      atca_assert(ATCA_BAD_PARAM, "OTP data err");
    }
  }

  memcpy(&pair_info_obj,
         FLASH_PTR(FLASH_OTP_BASE +
                   FLASH_OTP_BLOCK_608_SERIAL * FLASH_OTP_BLOCK_SIZE),
         sizeof(pair_info_obj));

  if (atca_configuration.lock_value == ATCA_LOCKED) {
    return;
  }

  for (int i = 0; i <= ATCA_KEY_ID_MAX; i++) {
    if (!(atca_configuration.slot_locked & (1 << i))) {
      continue;
    }
    switch (i) {
      case SLOT_PRIMARY_PRIVATE_KEY:
        atca_assert(atca_genkey(SLOT_PRIMARY_PRIVATE_KEY, NULL),
                    "genkey slot0");
        atca_assert(atca_lock_data_slot(SLOT_PRIMARY_PRIVATE_KEY),
                    "lock slot0 ");
        break;
      case SLOT_IO_PROTECT_KEY:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_IO_PROTECT_KEY, 0, 0,
                                    pair_info->protect_key, ATCA_BLOCK_SIZE),
                    "init IO key");
        atca_assert(atca_lock_data_slot(SLOT_IO_PROTECT_KEY), "lock slot1 ");
        break;
      case SLOT_USER_PIN:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_USER_PIN, 0, 0,
                                    pair_info->init_pin, ATCA_BLOCK_SIZE),
                    "init pin");
        break;
      case SLOT_PIN_ATTEMPT:
        atca_assert(atca_random(rand_buffer), "get random");
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_PIN_ATTEMPT, 0, 0,
                                    rand_buffer, ATCA_BLOCK_SIZE),
                    "init pin attempt");
        atca_assert(atca_lock_data_slot(SLOT_PIN_ATTEMPT), "lock slot3 ");
        break;
      case SLOT_COUNTER_MATCH:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_COUNTER_MATCH, 0, 0,
                                    (uint8_t *)counter0_init, ATCA_BLOCK_SIZE),
                    "init counter match");
        break;
      case SLOT_LAST_GOOD_COUNTER:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_LAST_GOOD_COUNTER, 0,
                                    0, zeros, ATCA_BLOCK_SIZE),
                    "init good counter");
        break;
      case SLOT_USER_SECRET:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_USER_SECRET, 0, 0,
                                    zeros, ATCA_BLOCK_SIZE),
                    "init secret");
        break;
      case SLOT_USER_SATATE:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_USER_SATATE, 0, 0,
                                    zeros, ATCA_BLOCK_SIZE),
                    "init secret");
        break;
      case SLOT_DEVICE_CERT:
        atca_assert(atca_write_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT, 0, 0,
                                    zeros, ATCA_BLOCK_SIZE),
                    "init cert slot");
        break;

      default:
        break;
    }
  }
  atca_assert(atca_lock_data_zone(), "lock data");
}

ATCA_STATUS atca_nonce_tempkey(uint8_t *temp_key) {
  ATCA_STATUS status;

  uint8_t mcu_random[20] = {0};

  uint8_t rand_out[32] = {0};

  do {
    random_buffer(mcu_random, sizeof(mcu_random));

    // atca nonce command
    if ((status = atca_nonce(NONCE_MODE_SEED_UPDATE, 0, mcu_random,
                             rand_out)) != ATCA_SUCCESS) {
      break;
    }

    // calculate tempkey
    SHA256_CTX ctx;
    uint8_t fixed[3] = {ATCA_NONCE, 0x00, 0x00};
    sha256_Init(&ctx);
    sha256_Update(&ctx, rand_out, sizeof(rand_out));
    sha256_Update(&ctx, mcu_random, sizeof(mcu_random));
    sha256_Update(&ctx, fixed, sizeof(fixed));
    sha256_Final(&ctx, temp_key);

  } while (0);
  return status;
}

ATCA_STATUS atca_checkmac_slot(uint16_t key_id, uint8_t *slot_key) {
  ATCA_STATUS status;
  uint8_t other_data[16] = {0};
  uint8_t temp_key[32] = {0};
  uint8_t zeros[8] = {0};
  uint8_t response[32] = {0};

  do {
    random_buffer(other_data, sizeof(other_data));

    //  nonce and get tempkey
    if ((status = atca_nonce_tempkey(temp_key)) != ATCA_SUCCESS) {
      break;
    }

    // calculate mac response
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, slot_key, 32);              // slot key
    sha256_Update(&ctx, temp_key, 32);              // tempkey
    sha256_Update(&ctx, other_data, 4);             // other data [0:3]
    sha256_Update(&ctx, zeros, 8);                  // zeros
    sha256_Update(&ctx, other_data + 4, 3);         // other data[4:6]
    sha256_Update(&ctx, pair_info->serial + 8, 1);  // serial number
    sha256_Update(&ctx, other_data + 7, 4);         // other data[7:10]
    sha256_Update(&ctx, pair_info->serial, 2);      // serial number
    sha256_Update(&ctx, other_data + 11, 2);        // other data[11:12]
    sha256_Final(&ctx, response);

    // check mac
    if ((status = atca_checkmac(CHECKMAC_MODE_BLOCK2_TEMPKEY, key_id, NULL,
                                response, other_data)) != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

ATCA_STATUS atca_mac_slot(uint16_t key_id, uint8_t *slot_key) {
  ATCA_STATUS status;
  uint8_t temp_key[32] = {0};
  uint8_t zeros[11] = {0};
  uint8_t mac[32] = {0}, mac_out[32] = {0};

  uint8_t mac_mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_INCLUDE_SN;
  uint8_t fixed[2] = {ATCA_MAC, mac_mode};

  do {
    //  nonce and get tempkey
    if ((status = atca_nonce_tempkey(temp_key)) != ATCA_SUCCESS) {
      break;
    }

    // calculate mac
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, slot_key, 32);              // slot key
    sha256_Update(&ctx, temp_key, 32);              // tempkey
    sha256_Update(&ctx, fixed, 2);                  // opcode + mode
    sha256_Update(&ctx, (uint8_t *)&key_id, 2);     // para2 keyid
    sha256_Update(&ctx, zeros, 11);                 // zeros
    sha256_Update(&ctx, pair_info->serial + 8, 1);  // serial number
    sha256_Update(&ctx, pair_info->serial + 4, 4);  // serial number
    sha256_Update(&ctx, pair_info->serial, 4);      // serial number
    sha256_Final(&ctx, mac);

    // check mac
    if ((status = atca_mac(mac_mode, key_id, NULL, mac_out)) != ATCA_SUCCESS) {
      break;
    }

    if (memcmp(mac, mac_out, 32) == 0) {
      return ATCA_SUCCESS;
    }
  } while (0);
  return ATCA_CHECKMAC_VERIFY_FAILED;
}

void atca_pair_unlock(void) {
  ATCADeviceState atca_satae = {0};

  atca_get_state(&atca_satae);
  if ((!atca_satae.auth_valid) ||
      (atca_satae.auth_keyid != SLOT_IO_PROTECT_KEY))
    atca_assert(atca_checkmac_slot(SLOT_IO_PROTECT_KEY, pair_info->protect_key),
                "pair io key");
}
// use io protect key
ATCA_STATUS atca_mac_tempkey(uint8_t *tempkey) {
  ATCA_STATUS status;
  uint16_t key_id = SLOT_IO_PROTECT_KEY;
  uint8_t zeros[11] = {0};
  uint8_t mac[32] = {0}, mac_out[32] = {0};

  uint8_t mac_mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_INCLUDE_SN;
  uint8_t fixed[2] = {ATCA_MAC, mac_mode};

  do {
    // calculate mac
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, pair_info->protect_key, 32);  // slot key
    sha256_Update(&ctx, tempkey, 32);                 // tempkey
    sha256_Update(&ctx, fixed, 2);                    // opcode + mode
    sha256_Update(&ctx, (uint8_t *)&key_id, 2);       // para2 keyid
    sha256_Update(&ctx, zeros, 11);                   // zeros
    sha256_Update(&ctx, pair_info->serial + 8, 1);    // serial number
    sha256_Update(&ctx, pair_info->serial + 4, 4);    // serial number
    sha256_Update(&ctx, pair_info->serial, 4);        // serial number
    sha256_Final(&ctx, mac);

    // check mac
    if ((status = atca_mac(mac_mode, key_id, NULL, mac_out)) != ATCA_SUCCESS) {
      break;
    }

    if (memcmp(mac, mac_out, 32) == 0) {
      return ATCA_SUCCESS;
    }
  } while (0);
  return ATCA_CHECKMAC_VERIFY_FAILED;
}

ATCA_STATUS atca_gendig_slot(uint16_t key_id, uint8_t *slot_key,
                             uint8_t *gendig) {
  ATCA_STATUS status;
  uint8_t temp_key[32] = {0};
  uint8_t zeros[25] = {0};
  uint8_t other_data[4] = {0};

  do {
    // Supply OtherData so GenDig behavior is the same for keys with
    // SlotConfig.NoMac set
    other_data[0] = ATCA_GENDIG;
    other_data[1] = GENDIG_ZONE_DATA;
    other_data[2] = (uint8_t)(key_id);
    other_data[3] = (uint8_t)(key_id >> 8);

    atca_pair_unlock();

    //  nonce and get tempkey
    if ((status = atca_nonce_tempkey(temp_key)) != ATCA_SUCCESS) {
      break;
    }

    // Send the GenDig command
    if ((status = atca_gendig(GENDIG_ZONE_DATA, key_id, other_data,
                              sizeof(other_data))) != ATCA_SUCCESS) {
      break;
    }

    // calculate enc
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, slot_key, 32);              // writekey
    sha256_Update(&ctx, other_data, 4);             // other data
    sha256_Update(&ctx, pair_info->serial + 8, 1);  // serial number
    sha256_Update(&ctx, pair_info->serial, 2);      // serial number
    sha256_Update(&ctx, zeros, 25);                 // zeros
    sha256_Update(&ctx, temp_key, 32);              // tempkey
    sha256_Final(&ctx, gendig);
  } while (0);
  return status;
}

ATCA_STATUS atca_gendig_counter(uint16_t counter_id, uint32_t counter_value,
                                uint8_t *gendig) {
  ATCA_STATUS status;
  uint8_t temp_key[32] = {0};
  uint8_t zeros[32] = {0};
  uint8_t other_data[4] = {0};

  do {
    // Supply OtherData so GenDig behavior is the same for keys with
    // SlotConfig.NoMac set
    other_data[0] = ATCA_GENDIG;
    other_data[1] = GENDIG_ZONE_COUNTER;
    other_data[2] = (uint8_t)(counter_id);
    other_data[3] = (uint8_t)(counter_id >> 8);

    atca_pair_unlock();

    //  nonce and get tempkey
    if ((status = atca_nonce_tempkey(temp_key)) != ATCA_SUCCESS) {
      break;
    }

    // Send the GenDig command
    if ((status = atca_gendig(GENDIG_ZONE_COUNTER, counter_id, NULL, 0)) !=
        ATCA_SUCCESS) {
      break;
    }

    // calculate enc
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, zeros, 32);                     // zeros
    sha256_Update(&ctx, other_data, 4);                 // other data
    sha256_Update(&ctx, pair_info->serial + 8, 1);      // serial number
    sha256_Update(&ctx, pair_info->serial, 2);          // serial number
    sha256_Update(&ctx, zeros, 1);                      // zeros
    sha256_Update(&ctx, (uint8_t *)&counter_value, 4);  // counter value
    sha256_Update(&ctx, zeros, 20);                     // zeros
    sha256_Update(&ctx, temp_key, 32);                  // tempkey
    sha256_Final(&ctx, gendig);
  } while (0);
  return status;
}

ATCA_STATUS atca_sha_hmac(const uint8_t *data, size_t data_size,
                          uint16_t key_slot, uint8_t *digest) {
  atca_hmac_sha256_ctx_t ctx;

  atca_pair_unlock();

  atca_assert(atca_sha_hmac_init(&ctx, key_slot), "hmac init");

  atca_assert(atca_sha_hmac_update(&ctx, data, data_size), "hmac update");

  atca_assert(atca_sha_hmac_finish(&ctx, digest, SHA_MODE_TARGET_OUT_ONLY),
              "hmac final");
  return ATCA_SUCCESS;
}

/** \brief Executes the Write command, which performs an encrypted write of
 *          a 32 byte block into given slot.
 *
 * The function takes clear text bytes and encrypts them for writing over the
 * wire. Data zone must be locked and the slot configuration must be set to
 * encrypted write for the block to be successfully written.
 *
 *  \param[in] key_id      Slot ID to write to.
 *  \param[in] block       Index of the 32 byte block to write in the slot.
 *  \param[in] data        32 bytes of clear text data to be written to the slot
 *  \param[in] enc_key     WriteKey to encrypt with for writing
 *  \param[in] enc_key_id  The KeyID of the WriteKey
 *
 *  returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_write_enc(uint16_t key_id, uint8_t block, uint8_t *data,
                           uint8_t *enc_key, uint16_t enc_key_id) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  ATCADeviceState atca_satae = {0};

  uint8_t mac[32] = {0};
  uint8_t other_data[4] = {0};
  uint8_t temp_key[32] = {0};
  uint8_t data_enc[ATCA_BLOCK_SIZE] = {0};
  uint8_t zeros[25] = {0};
  uint16_t addr;

  do {
    // Verify inputs parameters
    if (data == NULL || enc_key == NULL) {
      status = ATCA_BAD_PARAM;
      break;
    }
    if (atca_gendig_slot(enc_key_id, enc_key, temp_key) != ATCA_SUCCESS) {
      break;
    }
    // The get address function checks the remaining variables
    if ((status = atca_get_addr(ATCA_ZONE_DATA, key_id, block, 0, &addr)) !=
        ATCA_SUCCESS) {
      break;
    }

    // Encrypt by XOR-ing Data with the TempKey
    for (int i = 0; i < ATCA_BLOCK_SIZE; i++) {
      data_enc[i] = data[i] ^ temp_key[i];
    }

    other_data[0] = ATCA_WRITE;
    other_data[1] = ATCA_ZONE_DATA | ATCA_ZONE_READWRITE_32;
    other_data[2] = (uint8_t)(addr);
    other_data[3] = (uint8_t)(addr >> 8);

    // calc mac
    SHA256_CTX ctx;
    sha256_Init(&ctx);
    sha256_Update(&ctx, temp_key, 32);              // temp_key
    sha256_Update(&ctx, other_data, 4);             // other data
    sha256_Update(&ctx, pair_info->serial + 8, 1);  // serial number
    sha256_Update(&ctx, pair_info->serial, 2);      // serial number
    sha256_Update(&ctx, zeros, 25);                 // zeros
    sha256_Update(&ctx, data, 32);                  // data
    sha256_Final(&ctx, mac);

    atca_get_state(&atca_satae);

    status = atca_write(ATCA_ZONE_DATA | ATCA_ZONE_READWRITE_32, addr, data_enc,
                        mac);

  } while (0);

  return status;
}

/** \brief Executes Read command on a slot configured for encrypted reads and
 *          decrypts the data to return it as plaintext.
 *
 * Data zone must be locked for this command to succeed. Can only read 32 byte
 * blocks.
 *
 *  \param[in]  key_id      The slot ID to read from.
 *  \param[in]  block       Index of the 32 byte block within the slot to read.
 *  \param[out] data        Decrypted (plaintext) data from the read is returned
 *                          here (32 bytes).
 *  \param[in]  enc_key     32 byte ReadKey for the slot being read.
 *  \param[in]  enc_key_id  KeyID of the ReadKey being used.
 *
 *  returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_read_enc(uint16_t key_id, uint8_t block, uint8_t *data,
                          uint8_t *enc_key, uint16_t enc_key_id) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  uint8_t temp_key[32];
  int i = 0;

  do {
    // Verify inputs parameters
    if (data == NULL || enc_key == NULL) {
      status = ATCA_BAD_PARAM;
      break;
    }

    if (atca_gendig_slot(enc_key_id, enc_key, temp_key) != ATCA_SUCCESS) {
      break;
    }

    // Read Encrypted
    if ((status = atca_read_zone(ATCA_ZONE_DATA | ATCA_ZONE_READWRITE_32,
                                 key_id, block, 0, data, ATCA_BLOCK_SIZE)) !=
        ATCA_SUCCESS) {
      break;
    }

    // Decrypt
    for (i = 0; i < ATCA_BLOCK_SIZE; i++) {
      data[i] = data[i] ^ temp_key[i];
    }

    status = ATCA_SUCCESS;

  } while (0);

  return status;
}

// read slot data as counter
void atca_read_slot_counter(uint16_t key_id, uint32_t *counter) {
  uint8_t temp[ATCA_BLOCK_SIZE] = {0};

  atca_pair_unlock();
  atca_read_zone(ATCA_ZONE_DATA, key_id, 0, 0, temp, ATCA_BLOCK_SIZE);
  atca_assert(atca_mac_slot(key_id, temp), "counter mac");

  *counter = temp[0] + (temp[1] << 8) + (temp[2] << 16) + (temp[3] << 24);
}

ATCA_STATUS atca_add_counter_safety(uint16_t counrer_id, int steps,
                                    uint32_t *counter_value) {
  uint8_t digest[32] = {0};
  for (int i = 0; i < steps; i++) {
    atca_assert(atca_counter_increment(counrer_id, counter_value),
                "counter increment");
  }
  atca_assert(atca_gendig_counter(counrer_id, *counter_value, digest),
              "gendig by counter");
  return atca_mac_tempkey(digest);
}

void atca_read_slot_data(uint16_t key_id, uint8_t data[ATCA_BLOCK_SIZE]) {
  atca_pair_unlock();
  atca_read_zone(ATCA_ZONE_DATA, key_id, 0, 0, data, ATCA_BLOCK_SIZE);
  atca_assert(atca_mac_slot(key_id, data), "slot mac");
}

void atca_update_counter(void) {
  uint32_t counter = 0;
  uint32_t match_counter = 0;
  uint32_t last_counter = 0;
  uint32_t steps = 0;
  uint32_t buf[8] = {0};

  atca_assert(atca_counter_read(0, &counter), "read counter");
  atca_read_slot_counter(SLOT_COUNTER_MATCH, &match_counter);
  atca_read_slot_counter(SLOT_LAST_GOOD_COUNTER, &last_counter);

  if (counter + PIN_MAX_TRIES < match_counter) {
    buf[0] = counter;
    atca_assert(atca_write_enc(SLOT_LAST_GOOD_COUNTER, 0, (uint8_t *)buf,
                               pair_info->protect_key, SLOT_IO_PROTECT_KEY),
                "update counter");
  } else {
    match_counter = (counter + PIN_MAX_TRIES + 32) & ~31;
    steps = 32 - (counter % 32);
    buf[0] = match_counter;
    buf[1] = match_counter;
    atca_assert(atca_write_enc(SLOT_COUNTER_MATCH, 0, (uint8_t *)buf,
                               pair_info->protect_key, SLOT_IO_PROTECT_KEY),
                "update counter");
    atca_assert(atca_add_counter_safety(0, steps, &counter), "update counter");

    buf[0] = counter;
    buf[1] = 0;
    atca_assert(atca_write_enc(SLOT_LAST_GOOD_COUNTER, 0, (uint8_t *)buf,
                               pair_info->protect_key, SLOT_IO_PROTECT_KEY),
                "update counter");
  }
}

uint32_t atca_get_failed_counter(void) {
  uint32_t counter = 0;
  uint32_t last_counter = 0;

  atca_assert(atca_counter_read(0, &counter), "read counter");
  atca_read_slot_counter(SLOT_LAST_GOOD_COUNTER, &last_counter);

  return counter - last_counter;
}

bool atca_cal_pubkey(uint8_t pubkey[64]) {
  if (!pubkey) return false;
  atca_pair_unlock();
  if (ATCA_SUCCESS == atca_get_pubkey(SLOT_PRIMARY_PRIVATE_KEY, pubkey)) {
    return true;
  }
  return false;
}

bool atca_write_certificate(const uint8_t *cert, uint32_t cert_len) {
  ATCA_STATUS status = ATCA_BAD_PARAM;
  uint8_t buffer[ATCA_WORD_SIZE] = {0};
  uint32_t len = 0, res_len = cert_len % ATCA_WORD_SIZE;

  if ((atca_configuration.slot_locked & (1 << SLOT_DEVICE_CERT)) &&
      cert_len <= (416 - 4)) {
    if (ATCA_SUCCESS == atca_write_bytes_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT,
                                              0, (uint8_t *)&cert_len, 4)) {
      if (res_len) {
        len = (cert_len / ATCA_WORD_SIZE) * ATCA_WORD_SIZE;
        memcpy(buffer, cert + len, res_len);
      } else {
        len = cert_len;
      }
      status =
          atca_write_bytes_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT, 4, cert, len);
      if (res_len) {
        status |= atca_write_bytes_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT,
                                        4 + len, buffer, ATCA_WORD_SIZE);
      }
      if (status == ATCA_SUCCESS) {
        status |= atca_lock_data_slot(SLOT_DEVICE_CERT);
        atca_get_config(&atca_configuration);
      }
    }
  }
  return (status == ATCA_SUCCESS);
}

bool atca_get_certificate_len(uint32_t *cert_len) {
  uint32_t len = 0;

  *cert_len = 0;
  if (!(atca_configuration.slot_locked & (1 << SLOT_DEVICE_CERT))) {
    if (ATCA_SUCCESS == atca_read_bytes_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT,
                                             0, (uint8_t *)&len, 4)) {
      if (len <= (416 - 4)) {
        *cert_len = len;
        return true;
      }
    }
  }
  return false;
}

bool atca_read_certificate(uint8_t *cert, uint32_t *cert_len) {
  uint32_t len = 0;

  *cert_len = 0;
  if (!(atca_configuration.slot_locked & (1 << SLOT_DEVICE_CERT))) {
    if (ATCA_SUCCESS == atca_read_bytes_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT,
                                             0, (uint8_t *)&len, 4)) {
      if (len <= (416 - 4)) {
        atca_read_bytes_zone(ATCA_ZONE_DATA, SLOT_DEVICE_CERT, 4, cert, len);
        *cert_len = len;
        return true;
      }
    }
  }
  return false;
}
