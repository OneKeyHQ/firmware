#include <stdio.h>
#include <string.h>

#include "common.h"
#include "flash.h"
#include "memzero.h"
#include "secbool.h"

#include "aes/aes.h"
#include "bip32.h"
#include "curves.h"
#include "rand.h"

#include "se_thd89.h"
#include "thd89.h"

#define CURVE_NIST256P1 (0x00)
#define CURVE_SECP256K1 (0x01)

#define ECDH_NIST256P1 (0x00)
#define ECDH_SECP256K1 (0x01)
#define ECDH_CURVE25519 (0x08)

#define SE_INS_READ_DATA 0xE3
#define SE_INS_WRITE_DATA 0xE4
#define SE_INS_PIN 0xE5
#define SE_INS_SESSION 0xE6
#define SE_INS_DERIVE 0xE7
#define SE_INS_SIGN 0xE8
#define SE_INS_ECDH 0xE9
#define SE_INS_AES 0xEA
#define SE_INS_COINJOIN 0xEC
#define SE_INS_HASHR 0xED
#define SE_INS_HASHRAM 0xEE

#define SESSION_KEYLEN (16)

#define SE_PIN_RETRY_MAX 10

#define SE_DATA_MAX_LEN (1024)
#define SE_BUF_MAX_LEN (1024 + 64)

static uint8_t se_session_key[SESSION_KEYLEN];
static bool se_session_init = false;

static uint8_t se_send_buffer[SE_BUF_MAX_LEN];
static uint8_t se_recv_buffer[SE_BUF_MAX_LEN];
static uint16_t se_recv_len;

#define APDU_CLA (se_send_buffer[0])
#define APDU_INS (se_send_buffer[1])
#define APDU_P1 (se_send_buffer[2])
#define APDU_P2 (se_send_buffer[3])
#define APDU_P3 (se_send_buffer[4])

#define APDU_DATA (se_send_buffer + 5)
#define APDU (se_send_buffer)

static UI_WAIT_CALLBACK ui_callback = NULL;

static void xor_cal(uint8_t *data1, uint8_t *data2, uint16_t len,
                    uint8_t * xor) {
  uint16_t i;

  for (i = 0; i < len; i++) {
    xor[i] = data1[i] ^ data2[i];
  }
}

void se_set_ui_callback(UI_WAIT_CALLBACK callback) { ui_callback = callback; }

secbool se_get_rand(uint8_t *rand, uint8_t rand_len) {
  uint8_t rand_cmd[7] = {0x00, 0x84, 0x00, 0x00, 0x02};
  uint16_t resp_len = rand_len;

  if (rand_len > 0x20) {
    return secfalse;
  }

  rand_cmd[5] = (rand_len >> 8) & 0xff;
  rand_cmd[6] = rand_len & 0xff;
  return thd89_transmit(rand_cmd, sizeof(rand_cmd), rand, &resp_len);
}

secbool se_reset_se(void) {
  uint8_t cmd[5] = {0x00, 0xF0, 0x00, 0x00, 0x00};
  uint16_t resp_len;

  return thd89_transmit(cmd, sizeof(cmd), NULL, &resp_len);
}

static void cal_mac(uint8_t *data, uint32_t len, uint8_t *mac) {
  uint8_t pad_buf[16], mac_buf[16], iv[16];
  uint32_t pad_len, res_len;
  aes_encrypt_ctx ctxe;

  res_len = len % AES_BLOCK_SIZE;
  pad_len = AES_BLOCK_SIZE - res_len;

  memset(pad_buf, 0x00, sizeof(pad_buf));
  memset(iv, 0x00, sizeof(iv));

  if (res_len) {
    memcpy(pad_buf, data + len - res_len, res_len);
  }

  pad_buf[res_len] = 0x80;
  aes_encrypt_key128(se_session_key, &ctxe);
  len += pad_len;
  for (uint32_t i = 0; i < (len - AES_BLOCK_SIZE); i += AES_BLOCK_SIZE) {
    aes_cbc_encrypt(data + i, mac_buf, AES_BLOCK_SIZE, iv, &ctxe);
    memcpy(iv, mac_buf, AES_BLOCK_SIZE);
  }
  aes_cbc_encrypt(pad_buf, mac_buf, AES_BLOCK_SIZE, iv, &ctxe);
  memcpy(mac, mac_buf, 4);
}

secbool se_transmit_mac(uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data,
                        uint16_t data_len, uint8_t *recv, uint16_t *recv_len) {
  uint8_t mac[4], iv_random[16];
  uint16_t pad_len;
  APDU_CLA = 0x84;
  APDU_INS = ins;
  APDU_P1 = p1;
  APDU_P2 = p2;
  APDU_P3 = 0x00;

  if (!se_get_rand(iv_random, 16)) {
    return secfalse;
  }

  if (data != NULL && data_len != 0) {
    pad_len = AES_BLOCK_SIZE - (data_len % AES_BLOCK_SIZE);
    memset(APDU_DATA + data_len, 0x00, pad_len);
    APDU_DATA[data_len] = 0x80;
    data_len += pad_len;
    // header + data + mac
    if (data_len > SE_BUF_MAX_LEN - 7 - 4) {
      return secfalse;
    }

    memmove(APDU_DATA, data, data_len - pad_len);

    aes_encrypt_ctx ctxe;
    uint8_t iv[16];
    memcpy(iv, iv_random, 16);
    aes_encrypt_key128(se_session_key, &ctxe);
    aes_cbc_encrypt(APDU_DATA, se_recv_buffer, data_len, iv, &ctxe);

    if (data_len > 255) {
      APDU_P3 = 0x00;
      APDU_DATA[0] = (data_len >> 8) & 0xFF;
      APDU_DATA[1] = data_len & 0xFF;
      data_len += 7;
      memcpy(APDU_DATA + 2, se_recv_buffer, data_len);

    } else {
      APDU_P3 = data_len & 0xFF;
      data_len += 5;
      memcpy(APDU_DATA, se_recv_buffer, data_len);
    }

    cal_mac(APDU, data_len, mac);
    memcpy(APDU + data_len, mac, 4);
    data_len += 4;
  } else {
    data_len = 5;
  }
  se_recv_len = sizeof(se_recv_buffer);
  if (!thd89_transmit(APDU, data_len, se_recv_buffer, &se_recv_len)) {
    memset(APDU, 0x00, sizeof(APDU));
    return secfalse;
  }
  if (se_recv_len) {
    if ((se_recv_len - 4) % AES_BLOCK_SIZE) {
      return secfalse;
    }

    cal_mac(se_recv_buffer, se_recv_len - 4, mac);
    if (memcmp(mac, se_recv_buffer + se_recv_len - 4, 4) != 0) {
      return secfalse;
    }

    se_recv_len -= 4;

    aes_decrypt_ctx dtxe;
    uint8_t iv[16];
    memcpy(iv, iv_random, 16);
    aes_decrypt_key128(se_session_key, &dtxe);
    aes_cbc_decrypt(se_recv_buffer, APDU, se_recv_len, iv, &dtxe);
    pad_len = 1;
    for (uint8_t i = 0; i < 16; i++) {
      if (APDU[se_recv_len - 1 - i] == 0x80) {
        break;
      } else if (APDU[se_recv_len - 1 - i] == 0x00) {
        pad_len++;
      } else {
        memset(APDU, 0x00, sizeof(APDU));
        return secfalse;
      }
    }
    se_recv_len -= pad_len;

    if (recv_len == NULL) {
      return sectrue;
    }

    if (*recv_len < se_recv_len) {
      memset(APDU, 0x00, sizeof(APDU));
      return secfalse;
    }
    *recv_len = se_recv_len;
    if (recv) {
      memcpy(recv, APDU, *recv_len);
    }
  }
  memset(APDU, 0x00, sizeof(APDU));
  return sectrue;
}

secbool se_random_encrypted(uint8_t *rand, uint16_t len) {
  uint8_t data[2];
  uint16_t recv_len = len;
  data[0] = (len >> 8) & 0xff;
  data[1] = len & 0xff;
  if (!se_transmit_mac(0x84, 0x00, 0x00, data, 2, rand, &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_sync_session_key(void) {
  uint8_t r1[16], r2[16], r3[32];
  uint8_t default_key[16] = {0xff};

  memset(default_key, 0xff, 16);
  uint8_t data_buf[64], hash_buf[32];
  uint8_t sync_cmd[5 + 48] = {0x00, 0xfa, 0x00, 0x00, 0x30};
  uint16_t recv_len = sizeof(data_buf);
  aes_encrypt_ctx en_ctxe;
  aes_decrypt_ctx de_ctxe;
  memzero(data_buf, sizeof(data_buf));
  ensure(flash_otp_read(FLASH_OTP_BLOCK_THD89_SESSION_KEY, 0, default_key, 16),
         NULL);

  // get random from se
  se_get_rand(r1, 16);
  // get random itself
  random_buffer(r2, 16);
  // organization data1
  memcpy(r3, r1, sizeof(r1));
  memcpy(r3 + sizeof(r1), r2, sizeof(r2));
  aes_init();
  aes_encrypt_key128(default_key, &en_ctxe);
  aes_ecb_encrypt(r3, data_buf, sizeof(r1) + sizeof(r2), &en_ctxe);

  // cal tmp sessionkey with x hash256
  memzero(r3, sizeof(r3));
  xor_cal(r1, r2, sizeof(r1), r3);
  memcpy(r3 + 16, default_key, 16);
  sha256_Raw(r3, 32, hash_buf);
  // use session key organization data2
  memcpy(se_session_key, hash_buf, 16);
  aes_encrypt_key128(se_session_key, &en_ctxe);
  aes_ecb_encrypt(r1, data_buf + 32, sizeof(r1), &en_ctxe);
  // send data1 + data2 to se and recv returned result
  memcpy(sync_cmd + 5, data_buf, 48);
  if (!thd89_transmit(sync_cmd, sizeof(sync_cmd), data_buf, &recv_len)) {
    memset(se_session_key, 0x00, SESSION_KEYLEN);
    return secfalse;
  }

  // handle the returned data
  aes_decrypt_key128(se_session_key, &de_ctxe);
  aes_ecb_decrypt(data_buf, r3, recv_len, &de_ctxe);
  if (memcmp(r2, r3, sizeof(r2)) != 0) {
    memset(se_session_key, 0x00, SESSION_KEYLEN);
    return secfalse;
  }

  se_session_init = true;

  return sectrue;
}

secbool se_derive_keys(HDNode *out, const char *curve,
                       const uint32_t *address_n, size_t address_n_count,
                       uint32_t *fingerprint) {
  uint8_t resp[256];
  uint16_t resp_len = sizeof(resp);

  uint8_t len = strlen(curve);
  APDU_DATA[0] = len;
  memcpy(APDU_DATA + 1, curve, len);
  len += 1;

  memcpy(APDU_DATA + len, (uint8_t *)address_n, address_n_count * 4);
  len += address_n_count * 4;

  if (!se_transmit_mac(SE_INS_DERIVE, 0x00, 0x00, APDU_DATA, len, resp,
                       &resp_len)) {
    return secfalse;
  }
  out->curve = get_curve_by_name(curve);
  if (fingerprint) {
    memcpy(fingerprint, resp, 4);
  }
  memcpy((void *)out, resp + 4, sizeof(HDNode) - 4);

  return sectrue;
}

secbool se_reset_storage(void) {
  uint8_t rand[16];

  if (!se_get_rand(rand, sizeof(rand))) {
    return secfalse;
  }

  if (!se_session_init) {
    ensure(se_sync_session_key(), "se sync session key failed");
  }

  if (!se_transmit_mac(0xE1, 0x00, 0x00, rand, sizeof(rand), NULL, NULL)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_set_sn(const char *serial, uint8_t len) {
  uint8_t cmd[40] = {0x00, 0xF6, 0x00, 0x0, 0x00};
  uint16_t resp_len = 0;
  if (len > 32) {
    return secfalse;
  }
  cmd[4] = len;
  memcpy(cmd + 5, serial, len);
  return thd89_transmit(cmd, len + 5, NULL, &resp_len);
}

secbool se_get_sn(char **serial) {
  uint8_t get_sn[5] = {0x00, 0xf5, 0x00, 0x00, 0x00};
  static char sn[32] = {0};
  uint16_t sn_len = sizeof(sn);

  if (!thd89_transmit(get_sn, sizeof(get_sn), (uint8_t *)sn, &sn_len)) {
    return secfalse;
  }
  if (sn_len > sizeof(sn)) {
    return secfalse;
  }
  *serial = sn;
  return sectrue;
}

char *se_get_version(void) {
  uint8_t get_ver[5] = {0x00, 0xf7, 0x00, 00, 0x00};
  static char ver[8] = {0};
  uint16_t ver_len = sizeof(ver);

  if (!thd89_transmit(get_ver, sizeof(get_ver), (uint8_t *)ver, &ver_len)) {
    return NULL;
  }

  return ver;
}

secbool se_get_pubkey(uint8_t *public_key) {
  uint8_t cmd[5] = {0x00, 0xF5, 0x00, 0x01, 0x00};
  uint16_t resp_len = 64;
  return thd89_transmit(cmd, sizeof(cmd), public_key, &resp_len);
}

secbool se_write_certificate(const uint8_t *cert, uint16_t len) {
  uint8_t cmd[1024] = {0x00, 0xF6, 0x00, 0x01, 0x00};
  uint16_t cmd_len = 0;
  uint16_t resp_len = 0;
  if (len > 255) {
    cmd[4] = 0x00;
    cmd[5] = (len >> 8) & 0xff;
    cmd[6] = len & 0xff;
    cmd_len = 7;
  } else {
    cmd[4] = len;
    cmd_len = 5;
  }
  memcpy(cmd + cmd_len, cert, len);
  return thd89_transmit(cmd, cmd_len + len, NULL, &resp_len);
}

secbool se_read_certificate(uint8_t *cert, uint16_t *len) {
  uint8_t cmd[5] = {0x00, 0xF5, 0x00, 0x02, 0x00};
  return thd89_transmit(cmd, 5, cert, (uint16_t *)len);
}

secbool se_has_cerrificate(void) {
  uint8_t cert[512];
  uint16_t cert_len = sizeof(cert);
  return se_read_certificate(cert, &cert_len);
}

secbool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature) {
  uint8_t sign[37] = {0x00, 0xF5, 0x00, 0x03, 0x20};
  uint16_t signature_len = 64;

  SHA256_CTX ctx = {0};
  uint8_t result[32] = {0};

  sha256_Init(&ctx);
  sha256_Update(&ctx, msg, msg_len);
  sha256_Final(&ctx, result);

  memcpy(sign + 5, result, 32);
  return thd89_transmit(sign, sizeof(sign), signature, &signature_len);
}

secbool se_set_session_key(const uint8_t *session_key) {
  uint8_t cmd[32] = {0x00, 0xF6, 0x00, 0x02, 0x10};
  uint16_t resp_len = 0;
  memcpy(cmd + 5, session_key, SESSION_KEYLEN);
  return thd89_transmit(cmd, 21, NULL, &resp_len);
}

secbool se_isInitialized(void) {
  uint8_t cmd[5] = {0x00, 0xf8, 0x00, 00, 0x00};
  uint8_t init = 0xff;
  uint16_t len = sizeof(init);
  if (!thd89_transmit(cmd, sizeof(cmd), &init, &len)) {
    return secfalse;
  }
  return sectrue * (init == 0x55);
}

secbool se_hasPin(void) {
  uint8_t hasPin = 0xff;
  uint16_t len = sizeof(hasPin);

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x00, NULL, 0, &hasPin, &len)) {
    return secfalse;
  }

  // 0x55 exist ,0xff not
  return sectrue * (hasPin == 0x55);
}

secbool se_verifyPin(const char *pin) {
  uint8_t pin_buf[50 + 1] = {0};

  pin_buf[0] = strlen(pin);
  memcpy(pin_buf + 1, pin, strlen(pin));

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x03, pin_buf, pin_buf[0] + 1, NULL,
                       NULL)) {
    memset(pin_buf, 0, sizeof(pin_buf));
    if (0x6f80 == thd89_last_error()) {
      error_reset("You have entered the", "wipe code. All private",
                  "data has been erased.", NULL);
    }

    return secfalse;
  }
  memset(pin_buf, 0, sizeof(pin_buf));
  return sectrue;
}

secbool se_setPin(const char *pin) {
  uint8_t pin_buf[64] = {0};

  pin_buf[0] = strlen(pin);
  memcpy(pin_buf + 1, pin, strlen(pin));

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x01, pin_buf, pin_buf[0] + 1, NULL,
                       NULL)) {
    memset(pin_buf, 0, sizeof(pin_buf));
    return secfalse;
  }
  memset(pin_buf, 0, sizeof(pin_buf));
  return sectrue;
}

secbool se_changePin(const char *oldpin, const char *newpin) {
  uint8_t pin_buff[110];

  pin_buff[0] = strlen(oldpin);
  memcpy(pin_buff + 1, (uint8_t *)oldpin, strlen(oldpin));
  pin_buff[strlen(oldpin) + 1] = strlen(newpin);
  memcpy(pin_buff + strlen(oldpin) + 2, (uint8_t *)newpin, strlen(newpin));

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x02, pin_buff,
                       strlen(oldpin) + strlen(newpin) + 2, NULL, NULL)) {
    memset(pin_buff, 0, sizeof(pin_buff));
    return secfalse;
  }
  memset(pin_buff, 0, sizeof(pin_buff));
  return sectrue;
}

uint32_t se_pinFailedCounter(void) {
  uint8_t retry_cnts = 0;
  if (!se_getRetryTimes(&retry_cnts)) {
    return 0;
  }

  return (uint32_t)(SE_PIN_RETRY_MAX - retry_cnts);
}

secbool se_getRetryTimes(uint8_t *ptimes) {
  uint8_t remain;
  uint16_t recv_len = 1;

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x05, NULL, 0, &remain, &recv_len)) {
    return secfalse;
  }
  *ptimes = remain;
  return sectrue;
}

secbool se_clearSecsta(void) {
  uint16_t recv_len = 0;
  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x06, NULL, 0, NULL, &recv_len)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_getSecsta(void) {
  uint8_t cur_secsta = 0xff;
  uint16_t recv_len = sizeof(cur_secsta);
  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x04, NULL, 0, &cur_secsta,
                       &recv_len)) {
    return secfalse;
  }
  // 0x55 is verified pin 0x00 is not verified pin
  return sectrue * (cur_secsta == 0x55);
}

secbool se_set_u2f_counter(uint32_t u2fcounter) { return sectrue; }

secbool se_get_u2f_counter(uint32_t *u2fcounter) { return sectrue; }

secbool se_set_mnemonic(const char *mnemonic, uint16_t len) {
  return se_transmit_mac(0xE2, 0x00, 0x00, (uint8_t *)mnemonic, len, NULL,
                         NULL);
}

secbool se_sessionStart(uint8_t *session_id_bytes) {
  uint16_t recv_len = 32;

  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x00, NULL, 0, session_id_bytes,
                       &recv_len)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_sessionOpen(uint8_t *session_id_bytes) {
  uint16_t recv_len = 32;
  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x01, session_id_bytes, 32,
                       session_id_bytes, &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_sessionClose(void) {
  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x02, NULL, 0, NULL, NULL)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_sessionClear(void) {
  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x03, NULL, 0, NULL, NULL)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_set_public_region(const uint16_t offset, const void *val_dest,
                             uint16_t len) {
  uint8_t cmd[4] = {0};
  if (offset > PUBLIC_REGION_SIZE) return secfalse;
  cmd[0] = (offset >> 8) & 0xFF;
  cmd[1] = offset & 0xFF;
  cmd[2] = (len >> 8) & 0xFF;
  cmd[3] = len & 0xFF;
  memcpy(APDU_DATA, cmd, 4);
  memcpy(APDU_DATA + 4, (uint8_t *)val_dest, len);
  if (!se_transmit_mac(SE_INS_WRITE_DATA, 0x00, 0x00, APDU_DATA, 4 + len, NULL,
                       NULL)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_get_public_region(uint16_t offset, void *val_dest, uint16_t len) {
  uint8_t cmd[4] = {0};
  uint16_t recv_len = len;
  if (offset > PUBLIC_REGION_SIZE) return secfalse;
  cmd[0] = (offset >> 8) & 0xFF;
  cmd[1] = offset & 0xFF;
  cmd[2] = (len >> 8) & 0xFF;
  cmd[3] = len & 0xFF;
  if (!se_transmit_mac(SE_INS_READ_DATA, 0x00, 0x00, cmd, sizeof(cmd), val_dest,
                       &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_set_private_region(uint16_t offset, const void *val_dest,
                              uint16_t len) {
  uint8_t cmd[4] = {0};
  if (offset + len > PRIVATE_REGION_SIZE) return secfalse;
  cmd[0] = (offset >> 8) & 0xFF;
  cmd[1] = offset & 0xFF;
  cmd[2] = (len >> 8) & 0xFF;
  cmd[3] = len & 0xFF;
  memcpy(APDU_DATA, cmd, 4);
  memcpy(APDU_DATA + 4, (uint8_t *)val_dest, len);
  if (!se_transmit_mac(SE_INS_WRITE_DATA, 0x00, 0x01, APDU_DATA, 4 + len, NULL,
                       NULL)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_get_private_region(uint16_t offset, void *val_dest, uint16_t len) {
  uint8_t cmd[4] = {0};
  uint16_t recv_len = len;
  if (offset + len > PRIVATE_REGION_SIZE) return secfalse;
  cmd[0] = (offset >> 8) & 0xFF;
  cmd[1] = offset & 0xFF;
  cmd[2] = (len >> 8) & 0xFF;
  cmd[3] = len & 0xFF;
  if (!se_transmit_mac(SE_INS_READ_DATA, 0x00, 0x01, cmd, sizeof(cmd), val_dest,
                       &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_containsMnemonic(const char *mnemonic) {
  uint8_t verify = 0xff;
  uint16_t len = sizeof(verify);

  if (!se_transmit_mac(0xE2, 0x00, 0x01, (uint8_t *)mnemonic, strlen(mnemonic),
                       &verify, &len)) {
    return secfalse;
  }

  return sectrue * (verify == 0x55);
}

secbool se_exportMnemonic(char *mnemonic, uint16_t dest_size) {
  uint16_t len = dest_size;

  if (!se_transmit_mac(0xE2, 0x00, 0x02, NULL, 0, (uint8_t *)mnemonic, &len)) {
    return secfalse;
  }
  mnemonic[len] = 0;
  return sectrue;
}

secbool se_set_needs_backup(bool needs_backup) {
  if (!se_transmit_mac(0xE2, 0x00, 0x03, (uint8_t *)&needs_backup, 1, NULL,
                       NULL)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_get_needs_backup(bool *needs_backup) {
  uint16_t len = 1;
  uint8_t needs_backup_buf = 0xff;
  if (!se_transmit_mac(0xE2, 0x00, 0x04, NULL, 0, &needs_backup_buf, &len)) {
    return secfalse;
  }
  *needs_backup = needs_backup_buf == 0 ? false : true;

  return sectrue;
}

secbool se_hasWipeCode(void) {
  uint8_t wipe_code = 0xff;
  uint16_t len = sizeof(wipe_code);

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x07, NULL, 0, &wipe_code, &len)) {
    return secfalse;
  }

  // 0x55 exist ,0xff not
  return sectrue * (wipe_code == 0x55);
}
secbool se_changeWipeCode(const char *pin, const char *wipe_code) {
  uint8_t pin_buff[110];

  pin_buff[0] = strlen(pin);
  memcpy(pin_buff + 1, (uint8_t *)pin, strlen(pin));
  pin_buff[strlen(pin) + 1] = strlen(wipe_code);
  memcpy(pin_buff + strlen(pin) + 2, (uint8_t *)wipe_code, strlen(wipe_code));

  if (!se_transmit_mac(SE_INS_PIN, 0x00, 0x08, pin_buff,
                       strlen(pin) + strlen(wipe_code) + 2, NULL, NULL)) {
    memset(pin_buff, 0, sizeof(pin_buff));
    return secfalse;
  }
  memset(pin_buff, 0, sizeof(pin_buff));
  return sectrue;
}

int se_ecdsa_sign_digest(const uint8_t curve, const uint8_t canonical,
                         const uint8_t *hash, uint8_t *sig, uint8_t *pby) {
  uint8_t resp[68], tmp[40] = {0};
  uint16_t resp_len = sizeof(resp);

  tmp[0] = curve;
  tmp[1] = canonical;
  memcpy(tmp + 2, hash, 32);

  if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x01, tmp, 34, resp, &resp_len)) {
    return -1;
  }

  if (pby) *pby = resp[0];
  memcpy(sig, resp + 1, 64);
  // if (is_canonical && !is_canonical(*pby, sig)) return -1;
  return 0;
}

int se_secp256k1_sign_digest(const uint8_t canonical, const uint8_t *digest,
                             uint8_t *sig, uint8_t *pby) {
  return se_ecdsa_sign_digest(CURVE_SECP256K1, canonical, digest, sig, pby);
}

int se_nist256p1_sign_digest(const uint8_t *digest, uint8_t *sig,
                             uint8_t *pby) {
  return se_ecdsa_sign_digest(CURVE_NIST256P1, 0, digest, sig, pby);
}

#define HASH_FLAG_INIT 0x40
#define HASH_FLAG_UPDATE 0x00
#define HASH_FLAG_FINAL 0x80

#define ED25519_HASH_DEFAULT 0
#define ED25519_HASH_EXT 1
#define ED25519_HASH_KECCAK 2

static int _se_ed25519_send_msg(uint8_t ins, uint8_t type, const uint8_t *msg,
                                uint16_t msg_len) {
  uint8_t flag = HASH_FLAG_INIT;
  bool first = true;

  while (msg_len) {
    uint16_t len = msg_len > SE_DATA_MAX_LEN ? SE_DATA_MAX_LEN : msg_len;
    if (first) {
      flag = HASH_FLAG_INIT;
      first = false;
    } else {
      flag = HASH_FLAG_UPDATE;
    }
    if (msg_len - len == 0) {
      flag |= HASH_FLAG_FINAL;
    }
    if (!se_transmit_mac(ins, type, flag, (uint8_t *)msg, len, NULL, NULL)) {
      return -1;
    }
    msg += len;
    msg_len -= len;
  }
  return 0;
}

static int _se_ed25519_sign_digest(uint8_t type, uint8_t *sig) {
  uint16_t resp_len = 64;
  if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x08, &type, 1, sig, &resp_len)) {
    return -1;
  }
  return 0;
}

static int se_ed25519_sign_digest(const uint8_t *msg, uint16_t msg_len,
                                  uint8_t type, uint8_t *sig) {
  if (_se_ed25519_send_msg(SE_INS_HASHR, type, msg, msg_len) != 0) {
    return -1;
  }
  if (_se_ed25519_send_msg(SE_INS_HASHRAM, type, msg, msg_len) != 0) {
    return -1;
  }
  if (!_se_ed25519_sign_digest(type, sig)) {
    return -1;
  }
  return 0;
}

int se_ed25519_sign(const uint8_t *msg, uint16_t msg_len, uint8_t *sig) {
  uint8_t resp[64];
  uint16_t resp_len = sizeof(resp);
  if (msg_len > SE_DATA_MAX_LEN) {
    if (!se_ed25519_sign_digest(msg, msg_len, ED25519_HASH_DEFAULT, resp)) {
      return -1;
    }
  } else {
    if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x02, (uint8_t *)msg, msg_len, resp,
                         &resp_len)) {
      return -1;
    }
  }
  memcpy(sig, resp, resp_len);
  return 0;
}

int se_ed25519_sign_ext(const uint8_t *msg, uint16_t msg_len, uint8_t *sig) {
  uint8_t resp[64];
  uint16_t resp_len = sizeof(resp);
  if (msg_len > SE_DATA_MAX_LEN) {
    if (!se_ed25519_sign_digest(msg, msg_len, ED25519_HASH_EXT, resp)) {
      return -1;
    }
  } else {
    if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x03, (uint8_t *)msg, msg_len, resp,
                         &resp_len)) {
      return -1;
    }
  }
  memcpy(sig, resp, resp_len);
  return 0;
}

int se_ed25519_sign_keccak(const uint8_t *msg, uint16_t msg_len, uint8_t *sig) {
  uint8_t resp[64];
  uint16_t resp_len = sizeof(resp);
  if (msg_len > SE_DATA_MAX_LEN) {
    if (!se_ed25519_sign_digest(msg, msg_len, ED25519_HASH_KECCAK, resp)) {
      return -1;
    }
  } else {
    if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x04, (uint8_t *)msg, msg_len, resp,
                         &resp_len)) {
      return -1;
    }
  }
  memcpy(sig, resp, resp_len);
  return 0;
}

secbool se_get_session_seed_state(uint8_t *state) {
  uint16_t recv_len = 1;

  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x04, NULL, 0, state, &recv_len)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_session_is_open() {
  uint8_t state = 0;
  uint16_t recv_len = 1;

  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x07, NULL, 0, &state,
                       &recv_len)) {
    return secfalse;
  }

  return sectrue * (state == 0x55);
}

secbool session_generate_master_seed(const char *passphrase, uint8_t *percent) {
  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x05, (uint8_t *)passphrase,
                       strlen(passphrase), NULL, NULL)) {
    if (thd89_last_error() == 0x6c00) {
      *percent = 0;
      return sectrue;
    }
    return secfalse;
  }
  *percent = 100;
  return sectrue;
}

secbool session_generate_cardano_seed(const char *passphrase,
                                      uint8_t *percent) {
  if (!se_transmit_mac(SE_INS_SESSION, 0x00, 0x06, (uint8_t *)passphrase,
                       strlen(passphrase), NULL, NULL)) {
    if (thd89_last_error() == 0x6c00) {
      *percent = 0;
      return sectrue;
    }
    return secfalse;
  }
  *percent = 100;
  return sectrue;
}

secbool session_generate_seed_percent(uint8_t *percent) {
  uint8_t cmd[5] = {0x80, SE_INS_SESSION, 0x00, 0x08, 0x00};
  uint16_t recv_len;
  uint16_t sw1sw2;

  if (!thd89_transmit(cmd, sizeof(cmd), percent, &recv_len)) {
    sw1sw2 = thd89_last_error();
    if ((sw1sw2 & 0xff00) == 0x6c00) {
      *percent = sw1sw2 & 0xff;
      *percent = *percent == 100 ? 99 : *percent;
      return sectrue;
    }
    return secfalse;
  }
  *percent = 100;
  return sectrue;
}

uint8_t *se_session_startSession(const uint8_t *received_session_id) {
  static uint8_t act_session_id[32];

  if (received_session_id == NULL) {
    // se create session
    secbool ret = se_sessionStart(act_session_id);
    if (ret) {  // se open session
      if (!se_sessionOpen(act_session_id)) {
        // session open failed
        memzero(act_session_id, sizeof(act_session_id));
      }
    } else {
      memzero(act_session_id, sizeof(act_session_id));
    }
  } else {
    // se open session
    secbool ret = se_sessionOpen((uint8_t *)received_session_id);
    if (ret) {
      memcpy(act_session_id, received_session_id, sizeof(act_session_id));
    } else {  // session open failed
      memzero(act_session_id, sizeof(act_session_id));
    }
  }

  return act_session_id;
}

secbool se_node_sign_digest(const uint8_t *hash, uint8_t *sig, uint8_t *by) {
  uint8_t resp[68];
  uint16_t resp_len = sizeof(resp);

  if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x00, (uint8_t *)hash, 32, resp,
                       &resp_len)) {
    return secfalse;
  }

  memcpy(sig, resp + 1, 64);
  if (by) *by = resp[0];
  return sectrue;
}

secbool se_gen_session_seed(const char *passphrase, bool cardano) {
  uint8_t status = 0;
  uint8_t percent;
  if (!se_get_session_seed_state(&status)) {
    return secfalse;
  }
  if (cardano) {
    if (status & 0x40) {
      return sectrue;
    }
    if (!session_generate_cardano_seed(passphrase, &percent)) {
      return secfalse;
    }
    while (percent != 100) {
      if (ui_callback) {
        ui_callback(0, percent * 10, NULL);
      }
      if (!session_generate_seed_percent(&percent)) {
        return secfalse;
      }
      hal_delay(100);
    }
  } else {
    if (status & 0x80) {
      return sectrue;
    }
    if (!session_generate_master_seed(passphrase, &percent)) {
      return secfalse;
    }
    while (percent != 100) {
      if (ui_callback) {
        ui_callback(0, percent * 10, NULL);
      }
      if (!session_generate_seed_percent(&percent)) {
        return secfalse;
      }
      hal_delay(100);
    }
  }

  return sectrue;
}

int se_ecdsa_ecdh(const uint8_t *publickey, uint8_t *sessionkey) {
  uint8_t resp[128];
  uint16_t resp_len = sizeof(resp);

  if (!se_transmit_mac(SE_INS_ECDH, 0x00, 0x00, (uint8_t *)publickey, 64, resp,
                       &resp_len)) {
    return -1;
  }
  memcpy(sessionkey, resp, resp_len);
  return 0;
}

int se_curve25519_ecdh(const uint8_t *publickey, uint8_t *sessionkey) {
  uint8_t resp[128];
  uint16_t resp_len = sizeof(resp);

  if (!se_transmit_mac(SE_INS_ECDH, 0x00, 0x01, (uint8_t *)publickey, 32, resp,
                       &resp_len)) {
    return -1;
  }
  memcpy(sessionkey, resp, resp_len);
  return 0;
}

int se_get_shared_key(const char *curve, const uint8_t *peer_public_key,
                      uint8_t *session_key) {
  if (strcmp(curve, NIST256P1_NAME) == 0 ||
      strcmp(curve, SECP256K1_NAME) == 0) {
    return se_ecdsa_ecdh(peer_public_key + 1, session_key);
  } else if (strcmp(curve, CURVE25519_NAME) == 0) {
    return se_curve25519_ecdh(peer_public_key, session_key);
  }
  return -1;
}

secbool se_derive_tweak_private_keys(const uint8_t *root_hash) {
  uint8_t *data = NULL;
  uint16_t data_len = 0;
  if (root_hash) {
    data = (uint8_t *)root_hash;
    data_len = 32;
  }
  if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x06, data, data_len, NULL, NULL)) {
    return secfalse;
  }
  return sectrue;
}

int se_bip340_sign_digest(const uint8_t *digest, uint8_t sig[64]) {
  uint8_t resp[64];
  uint16_t resp_len = sizeof(resp);
  if (!se_transmit_mac(SE_INS_SIGN, 0x00, 0x07, (uint8_t *)digest, 32, resp,
                       &resp_len)) {
    return -1;
  }
  if (resp_len != 64) return -1;
  memcpy(sig, resp, resp_len);
  return 0;
}

int se_aes256_encrypt(const uint8_t *data, uint16_t data_len, const uint8_t *iv,
                      uint8_t *value, uint16_t value_len, uint8_t *out) {
  uint32_t len = 0;
  uint16_t resp_len = value_len;
  APDU_DATA[0] = (data_len >> 8) & 0xff;
  APDU_DATA[1] = data_len & 0xff;
  len += 2;
  memcpy(APDU_DATA + len, data, data_len);
  len += data_len;
  APDU_DATA[len] = (value_len >> 8) & 0xff;
  APDU_DATA[len + 1] = value_len & 0xff;
  len += 2;
  memcpy(APDU_DATA + len, value, value_len);
  len += value_len;
  if (iv != NULL) {
    memcpy(APDU_DATA + len, iv, 16);
    len += 16;
  }

  if (!se_transmit_mac(SE_INS_AES, 0x00, 0x00, APDU_DATA, len, out,
                       &resp_len)) {
    return -1;
  }
  return 0;
}

int se_aes256_decrypt(const uint8_t *data, uint16_t data_len, const uint8_t *iv,
                      uint8_t *value, uint16_t value_len, uint8_t *out) {
  uint32_t len = 0;
  uint16_t resp_len = value_len;
  APDU_DATA[0] = (data_len >> 8) & 0xff;
  APDU_DATA[1] = data_len & 0xff;
  len += 2;
  memcpy(APDU_DATA + len, data, data_len);
  len += data_len;
  APDU_DATA[len] = (value_len >> 8) & 0xff;
  APDU_DATA[len + 1] = value_len & 0xff;
  len += 2;
  memcpy(APDU_DATA + len, value, value_len);
  len += value_len;
  if (iv != NULL) {
    memcpy(APDU_DATA + len, iv, 16);
    len += 16;
  }

  if (!se_transmit_mac(SE_INS_AES, 0x00, 0x01, APDU_DATA, len, out,
                       &resp_len)) {
    return -1;
  }
  return 0;
}

int se_nem_aes256_encrypt(const uint8_t *ed25519_pubkey, const uint8_t *iv,
                          const uint8_t *salt, uint8_t *payload, uint16_t size,
                          uint8_t *out) {
  uint32_t len = 0;
  uint16_t resp_len = (size + AES_BLOCK_SIZE) / AES_BLOCK_SIZE * AES_BLOCK_SIZE;
  memcpy(APDU_DATA + len, ed25519_pubkey, 32);
  len += 32;
  memcpy(APDU_DATA + len, iv, 16);
  len += 16;
  memcpy(APDU_DATA + len, salt, 32);
  len += 32;
  memcpy(APDU_DATA + len, payload, size);
  len += size;

  if (!se_transmit_mac(SE_INS_AES, 0x00, 0x02, APDU_DATA, len, out,
                       &resp_len)) {
    return -1;
  }
  return 0;
}

int se_nem_aes256_decrypt(const uint8_t *ed25519_pubkey, const uint8_t *iv,
                          const uint8_t *salt, uint8_t *payload, uint16_t size,
                          uint8_t *out) {
  uint32_t len = 0;
  uint16_t resp_len = size;
  memcpy(APDU_DATA + len, ed25519_pubkey, 32);
  len += 32;
  memcpy(APDU_DATA + len, iv, 16);
  len += 16;
  memcpy(APDU_DATA + len, salt, 32);
  len += 32;
  memcpy(APDU_DATA + len, payload, size);
  len += size;

  if (!se_transmit_mac(SE_INS_AES, 0x00, 0x03, APDU_DATA, len, out,
                       &resp_len)) {
    return -1;
  }
  return 0;
}

int se_slip21_node(uint8_t *data) {
  uint16_t resp_len = 64;

  if (!se_transmit_mac(0xEB, 0x00, 0x00, NULL, 0, data, &resp_len)) {
    return -1;
  }
  return 0;
}

secbool se_authorization_set(const uint32_t authorization_type,
                             const uint8_t *authorization,
                             uint32_t authorization_len) {
  uint8_t data[128];
  if (authorization_len > MAX_AUTHORIZATION_LEN) {
    return secfalse;
  }
  memcpy(data, &authorization_type, 4);
  memcpy(data + 4, authorization, authorization_len);

  if (!se_transmit_mac(SE_INS_COINJOIN, 0x00, 0x00, data, authorization_len + 4,
                       NULL, NULL)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_authorization_get_type(uint32_t *authorization_type) {
  uint32_t type = 0;
  uint16_t resp_len = 4;
  if (!se_transmit_mac(SE_INS_COINJOIN, 0x00, 0x01, NULL, 0, (uint8_t *)&type,
                       &resp_len)) {
    return secfalse;
  }
  *authorization_type = type;
  return sectrue;
}

secbool se_authorization_get_data(uint8_t *authorization_data,
                                  uint32_t *authorization_len) {
  uint16_t resp_len = MAX_AUTHORIZATION_LEN;
  if (!se_transmit_mac(SE_INS_COINJOIN, 0x00, 0x02, NULL, 0, authorization_data,
                       &resp_len)) {
    return secfalse;
  }
  *authorization_len = resp_len;
  return sectrue;
}

void se_authorization_clear(void) {
  se_transmit_mac(SE_INS_COINJOIN, 0x00, 0x03, NULL, 0, NULL, NULL);
}
