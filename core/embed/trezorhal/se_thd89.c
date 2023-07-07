#include <stdio.h>
#include <string.h>

#include "common.h"
#include "memzero.h"
#include "secbool.h"

#include "aes/aes.h"
#include "bip32.h"
#include "curves.h"
#include "rand.h"

#include "se_thd89.h"
#include "thd89.h"

#define SE_ENCRYPT 0x00
#define SE_PLAIN 0x80

#define GET_SESTORE_DATA (0x00)
#define SET_SESTORE_DATA (0x01)
#define DELETE_SESTORE_DATA (0x02)
#define DEVICEINIT_DATA (0x03)

#define APP (0x01 << 8)

#define SE_INITIALIZED (14 | APP)         // byte
#define SE_PIN (20 | APP)                 // uint32
#define SE_PIN_VALIDTIME (21 | APP)       // byte
#define SE_APPLY_PINVALIDTIME (39 | APP)  // byte
#define SE_VERIFYPIN (22 | APP)           // uint32
#define SE_RESET (27 | APP)
#define SE_SEEDSTRENGTH (30 | APP)    // uint32
#define SE_PIN_RETRYTIMES (37 | APP)  // byte
#define SE_SECSTATUS (38 | APP)       // byte
#define SE_U2FCOUNTER (9 | APP)       // uint32
#define SE_MNEMONIC (2 | APP)         // string(241)
#define SE_ENTROPY SE_MNEMONIC        // bytes(64)
#define SE_PIN_RETRY_MAX 10

#define SE_CMD_WR_PIN (0xE1)
#define SE_CMD_AES (0xE2)
#define SE_CMD_ECC_EDDSA (0xE3)
#define SE_CMD_SCHNOOR (0xE4)
#define SE_CMD_READ_SESTOR_REGION (0xE5)
#define SE_CMD_WRITE_SESTOR_REGION (0xE6)
#define SE_CMD_WR_SESSION (0xE7)
#define SE_CMD_WR_MNEMONIC (0xE8)
#define SE_CMD_WR_WIPECODE (0xE9)

// ecc ed2519 index
#define ECC_INDEX_GITPUBKEY (0x00)
#define ECC_INDEX_SIGN (0x01)
#define SCHNOOR_INDEX_SIGN ECC_INDEX_SIGN
#define ECC_INDEX_VERIFY (0x02)
#define EDDSA_INDEX_GITPUBKEY (0x03)
#define EDDSA_INDEX_SIGN (0x04)
#define EDDSA_INDEX_VERIFY (0x05)
#define EDDSA_INDEX_CHILDKEY (0x06)
#define EDDSA_INDEX_U2FKEY (0x07)
#define EDDSA_INDEX_ECDH (0x08)

#define DERIVE_NIST256P1 (0x00)
#define DERIVE_SECP256K1 (0x01)
#define DERIVE_ED25519_DONNA (0x02)
#define DERIVE_SR25519 (0x03)
#define DERIVE_ED25519_SLIP10 (0x04)
#define DERIVE_ED25519_ICARUS (0x05)
#define DERIVE_BIP86 (0x06)
#define DERIVE_BIP86_TWEAK (0x07)
#define DERIVE_CURVE25519 (0x08)
#define DERIVE_POLKADOT (0x09)

#define CURVE_NIST256P1 (0x40)
#define CURVE_SECP256K1 (0x00)
#define CURVE_ED25519 (0x02)
#define CURVE_SR25519 (0x03)
#define CURVE_ED25519_ICARUS (0x04)

#define ECDH_NIST256P1 (0x00)
#define ECDH_SECP256K1 (0x01)
#define ECDH_CURVE25519 (0x08)

#define EOS_ECDSA_SIGN (60)
#define ETH_ECDSA_SIGN (194)
#define COM_ECDSA_SIGN (0)
#define SEC_GENK_RFC6979 (0x43)
#define SEC_GENK_RAND (0x65)
#define SEC_GENK_MODE (SEC_GENK_RFC6979)

#define AES_ECB (0x00)
#define AES_CBC (0x01)

#define SESSION_KEYLEN (16)

#define SE_BUF_MAX_LEN (1024 + 512)

static uint8_t se_session_key[SESSION_KEYLEN] = "3170193D1737E974";

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

#define LITTLE_REVERSE32(w, x)                                       \
  {                                                                  \
    uint32_t ref = (w);                                              \
    ref = (ref >> 16) | (ref << 16);                                 \
    (x) = ((ref & 0xff00ff00UL) >> 8) | ((ref & 0x00ff00ffUL) << 8); \
  }

static void xor_cal(uint8_t *data1, uint8_t *data2, uint16_t len,
                    uint8_t * xor) {
  uint16_t i;

  for (i = 0; i < len; i++) {
    xor[i] = data1[i] ^ data2[i];
  }
}

void se_set_ui_callback(UI_WAIT_CALLBACK callback) { ui_callback = callback; }

secbool se_get_rand(uint8_t *rand, uint8_t rand_len) {
  uint8_t rand_cmd[5] = {0x00, 0x84, 0x00, 0x00, 0x00};
  uint16_t resp_len;

  if (rand_len > 0x10) {
    return secfalse;
  }

  rand_cmd[4] = rand_len;
  return thd89_transmit(rand_cmd, sizeof(rand_cmd), rand, &resp_len);
}

secbool se_random_encrypted(uint8_t *rand, uint8_t len) {
  uint8_t cmd[5 + 16 + 1] = {0x00, 0x85, 0x00, 0x00, 0x10};
  uint8_t mcu_rand_buf[16], recv_buf[128], ref_buf[128];
  uint16_t recv_len = 0xff;
  aes_decrypt_ctx aes_dec_ctx;

  if (len > 0x40) return false;
  // mcu get random
  random_buffer(mcu_rand_buf, 0x10);
  memcpy(cmd + 5, mcu_rand_buf, sizeof(mcu_rand_buf));
  cmd[5 + 16] = len;
  if (!thd89_transmit(cmd, sizeof(cmd), recv_buf, &recv_len)) {
    return secfalse;
  }

  if ((recv_len % 16) != 0) return false;
  aes_decrypt_key128(se_session_key, &aes_dec_ctx);
  aes_ecb_decrypt(recv_buf, ref_buf, recv_len, &aes_dec_ctx);
  if (memcmp(ref_buf, mcu_rand_buf, sizeof(mcu_rand_buf)) != 0) return false;

  memcpy(rand, ref_buf + 16, len);
  return true;
}

inline static secbool _se_transmit(uint8_t ins, uint8_t p1, uint8_t *data,
                                   uint16_t data_len, uint8_t *recv,
                                   uint16_t *recv_len, uint8_t mode,
                                   uint8_t flag, bool first) {
  uint8_t random[16], i;
  uint16_t pad_len;
  aes_encrypt_ctx ctxe;
  aes_decrypt_ctx ctxd;
  // se apdu
  if (SE_ENCRYPT == mode) {
    if ((SET_SESTORE_DATA & flag) || (DEVICEINIT_DATA & flag) ||
        (SE_WRFLG_SETPIN == flag) || (SE_WRFLG_CHGPIN == flag) ||
        (SE_WRFLG_GENSEED == flag) ||
        (SE_WRFLG_GENMINISECRET == flag)) {  // TODO. pin process
      // data aes encrypt
      se_get_rand(random, sizeof(random));
      memset(&ctxe, 0, sizeof(aes_encrypt_ctx));
      aes_encrypt_key128(se_session_key, &ctxe);
      memcpy(APDU_DATA, random, sizeof(random));
      memcpy(APDU_DATA + sizeof(random), data, data_len);
      data_len += sizeof(random);
      // add pad
      // if (data_len % AES_BLOCK_SIZE) {
      pad_len = AES_BLOCK_SIZE - (data_len % AES_BLOCK_SIZE);
      memset(APDU_DATA + data_len, 0x00, pad_len);
      APDU_DATA[data_len] = 0x80;
      data_len += pad_len;
      aes_ecb_encrypt(APDU_DATA, se_recv_buffer, data_len, &ctxe);
    } else {
      // data add random
      random_buffer(random, sizeof(random));
      memcpy(se_recv_buffer, random, sizeof(random));
      if (data_len > 0) {
        memcpy(se_recv_buffer + sizeof(random), data, data_len);
      }
      data_len += sizeof(random);
    }
  }

  if (first) {
    APDU_CLA = 0x90;
  } else {
    APDU_CLA = 0x80;
  }
  APDU_INS = ins;
  APDU_P1 = p1;
  APDU_P2 = flag | mode;
  if (data_len > 255) {
    APDU_P3 = 0x00;
    APDU_DATA[0] = (data_len >> 8) & 0xFF;
    APDU_DATA[1] = data_len & 0xFF;
    if (data_len > (SE_BUF_MAX_LEN - 7)) {
      return secfalse;
    }
    if (SE_ENCRYPT == mode) {
      memcpy(APDU_DATA + 2, se_recv_buffer, data_len);
    } else {
      memcpy(APDU_DATA + 2, data, data_len);
    }

    data_len += 7;
    // TODO. add le
    if (SE_CMD_ECC_EDDSA == ins) {
      APDU[data_len] = 0x00;
      APDU[data_len + 1] = 0x00;
      data_len += 2;
    }
  } else {
    APDU_P3 = data_len & 0xFF;
    if (SE_ENCRYPT == mode) {
      memcpy(APDU_DATA, se_recv_buffer, data_len);
    } else {
      memcpy(APDU_DATA, data, data_len);
    }
    data_len += 5;
    // TODO add le
    if (SE_CMD_ECC_EDDSA == ins) {
      APDU[data_len] = 0x00;
      data_len += 1;
    }
  }

  if (!thd89_transmit(APDU, data_len, se_recv_buffer, &se_recv_len)) {
    if (se_recv_len && recv) {
      *recv_len = se_recv_len;
      // *recv_len = *recv_len > se_recv_len ? se_recv_len : *recv_len;
      memcpy(recv, se_recv_buffer, *recv_len);
    }
    return secfalse;
  }

  if (SE_ENCRYPT == mode) {
    // aes dencrypt data
    if ((GET_SESTORE_DATA == flag) && (se_recv_len > 0) &&
        ((se_recv_len % 16 == 0x00))) {
      memset(&ctxd, 0, sizeof(aes_decrypt_ctx));
      aes_decrypt_key128(se_session_key, &ctxd);
      // TODO
      memzero(APDU_DATA, se_recv_len);
      aes_ecb_decrypt(se_recv_buffer, APDU_DATA, se_recv_len, &ctxd);

      if (memcmp(APDU_DATA, random, sizeof(random)) != 0) {
        return secfalse;
      }
      // delete pad
      for (i = 1; i < 0x11; i++) {
        if (APDU_DATA[se_recv_len - i] == 0x80) {
          for (pad_len = 1; pad_len < i; pad_len++) {
            if (APDU_DATA[se_recv_len - pad_len] != 0x00) {
              i = 0x11;
              break;
            }
          }
          break;
        }
      }

      if (i != 0x11) {
        se_recv_len = se_recv_len - i;
      }
      se_recv_len -= sizeof(random);
      if (recv != NULL) {
        memcpy(recv, APDU_DATA + sizeof(random), se_recv_len);
        *recv_len = se_recv_len;
        return sectrue;
      }
    }
  }
  if (recv != NULL) {
    memcpy(recv, se_recv_buffer, se_recv_len);
    *recv_len = se_recv_len;
  }
  return sectrue;
}

secbool se_transmit(uint8_t ins, uint8_t p1, uint8_t *data, uint16_t data_len,
                    uint8_t *recv, uint16_t *recv_len, uint8_t mode,
                    uint8_t flag) {
  return _se_transmit(ins, p1, data, data_len, recv, recv_len, mode, flag,
                      false);
}

secbool se_transmit_ex(uint8_t ins, uint8_t p1, uint8_t *data,
                       uint16_t data_len, uint8_t *recv, uint16_t *recv_len,
                       uint8_t mode, uint8_t flag, bool first) {
  return _se_transmit(ins, p1, data, data_len, recv, recv_len, mode, flag,
                      first);
}

secbool se_sync_session_key(void) {
  uint8_t r1[16], r2[16], r3[32];
  uint8_t default_key[16] = {0xff};

  memset(default_key, 0xff, 16);
  uint8_t data_buf[64], hash_buf[32];
  uint8_t sync_cmd[5 + 48] = {0x00, 0xfa, 0x00, 0x00, 0x30};
  uint16_t recv_len = 0xff;
  aes_encrypt_ctx en_ctxe;
  aes_decrypt_ctx de_ctxe;
  memzero(data_buf, sizeof(data_buf));
  // memcpy(default_key, se_session_key, 16);

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

  return sectrue;
}

// `seed`, `minisecret`, `icarus main secret`, `icarus extension main secret`
secbool generate_seed_steps(void) {
  // uint32_t total_process = 1000;
  uint32_t seed_steps = 200, mini_steps = 200, icarus_steps = 600;
  int base = 0;

#define SESSION_GENERATE(type, precent)                           \
  do {                                                            \
    se_generate_session_t session = {0};                          \
    se_generate_state_t state = se_beginGenerate(type, &session); \
    int step = 1;                                                 \
    while (state == STATE_GENERATING) {                           \
      int permil = base + step * (precent / 100);                 \
      if (ui_callback) {                                          \
        ui_callback(0, permil, "Generating session seed ...");    \
      }                                                           \
      step++;                                                     \
      state = se_generating(&session);                            \
    }                                                             \
    if (state != STATE_COMPLETE) return false;                    \
    base += precent;                                              \
  } while (0)

  // generate `icarus main secret`
  SESSION_GENERATE(TYPE_ICARUS_MAIN_SECRET, icarus_steps);

  // generate mini secret
  SESSION_GENERATE(TYPE_MINI_SECRET, mini_steps);

  // seed will be last because se switch lify cycle for init complete.
  SESSION_GENERATE(TYPE_SEED, seed_steps);

  return sectrue;
}

inline static secbool se_get_resp_by_ecdsa256(uint8_t mode,
                                              const uint32_t *address,
                                              uint8_t count, uint8_t *resp,
                                              uint16_t *resp_len) {
  return se_transmit(SE_CMD_ECC_EDDSA, EDDSA_INDEX_CHILDKEY, (uint8_t *)address,
                     count * 4, resp, resp_len, SE_PLAIN, mode);
}

inline static secbool se_get_derive_mode_by_name(const char *curve,
                                                 uint8_t *mode) {
  if (0 == strcmp(curve, NIST256P1_NAME)) {
    *mode = DERIVE_NIST256P1;
  } else if (0 == strcmp(curve, SECP256K1_NAME)) {
    *mode = DERIVE_SECP256K1;
  } else if (0 == strcmp(curve, ED25519_NAME)) {
    *mode = DERIVE_ED25519_SLIP10;
  } else if (0 == strcmp(curve, SR25519_NAME)) {
    *mode = DERIVE_SR25519;
  } else if (0 == strcmp(curve, ED25519_CARDANO_NAME)) {
    *mode = DERIVE_ED25519_ICARUS;
  } else if (0 == strcmp(curve, ED25519_LEDGER_NAME)) {
    *mode = DERIVE_POLKADOT;
  }
  // } else if (0 == strcmp(curve, ED25519_KECCAK_NAME)) {
  //   *mode = DERIVE_ED25519_DONNA;
  // }
  else if (0 == strcmp(curve, CURVE25519_NAME)) {
    *mode = DERIVE_CURVE25519;
  } else {
    return secfalse;
  }

  return sectrue;
}

secbool se_derive_keys(HDNode *out, const char *curve,
                       const uint32_t *address_n, size_t address_n_count,
                       uint32_t *fingerprint) {
  uint8_t resp[256];
  uint16_t resp_len;
  uint8_t mode;

  if (!se_get_derive_mode_by_name(curve, &mode)) return secfalse;

  if (!se_transmit(SE_CMD_ECC_EDDSA, EDDSA_INDEX_CHILDKEY, (uint8_t *)address_n,
                   address_n_count * 4, resp, &resp_len, SE_PLAIN, mode)) {
    return secfalse;
  }
  out->curve = get_curve_by_name(curve);
  switch (mode) {
    case DERIVE_NIST256P1:
    case DERIVE_SECP256K1:
      out->depth = resp[0];
      out->child_num = *(uint32_t *)(resp + 1);
      LITTLE_REVERSE32(out->child_num, out->child_num);
      memcpy(out->chain_code, resp + 1 + 4, 32);
      HDNode parent = {0};
      parent.curve = get_curve_by_name(curve);
      memcpy(parent.public_key, resp + 1 + 4 + 32, 33);
      if (fingerprint) {
        *fingerprint = hdnode_fingerprint(&parent);
      }
      memcpy(out->public_key, resp + 1 + 4 + 32 + 33, 33);
      break;
    case DERIVE_ED25519_DONNA:
    case DERIVE_SR25519:
      if (32 != resp_len) return false;
      if (fingerprint) fingerprint = NULL;
      memcpy(out->public_key, resp, resp_len);
      break;
    case DERIVE_ED25519_SLIP10:
    case DERIVE_ED25519_ICARUS:
    case DERIVE_CURVE25519:
    case DERIVE_POLKADOT:
      if (33 != resp_len) return secfalse;
      if (fingerprint) fingerprint = NULL;
      memcpy(out->public_key, resp, resp_len);
      // keep same `hdnode_fill_public_key` in bip32.c
      out->public_key[0] = 1;
      break;
    case DERIVE_BIP86:
      if (33 != resp_len) return false;
      if (fingerprint) fingerprint = NULL;
      memcpy(out->public_key, resp, resp_len);
      break;
    default:
      return secfalse;
  }

  return sectrue;
}

secbool se_set_value(uint16_t key, const void *val_dest, uint16_t len) {
  uint8_t flag = key >> 8;
  if (!se_transmit(SE_CMD_WR_PIN, (key & 0xFF), (uint8_t *)val_dest, len, NULL,
                   0, (flag & SE_PLAIN), SET_SESTORE_DATA)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_set_spec_value(uint16_t key, const void *val_dest, uint16_t len,
                          uint8_t wr_flg) {
  uint8_t flag = key >> 8;
  if (!se_transmit(SE_CMD_WR_PIN, (key & 0xFF), (uint8_t *)val_dest, len, NULL,
                   0, (flag & SE_PLAIN), wr_flg)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_get_value(uint16_t key, void *val_dest, uint16_t max_len,
                     uint16_t *len) {
  uint8_t flag = key >> 8;
  if (!se_transmit(SE_CMD_WR_PIN, (key & 0xFF), NULL, 0, val_dest, len,
                   (flag & SE_PLAIN), GET_SESTORE_DATA)) {
    return secfalse;
  }
  *len = *len > max_len ? max_len : *len;
  return sectrue;
}

void se_reset_storage(void) {
  se_transmit(SE_CMD_WR_PIN, (SE_RESET & 0xFF), NULL, 0, NULL, NULL, SE_ENCRYPT,
              SE_WRFLG_RESET);
}

secbool se_get_sn(char **serial, uint16_t len) {
  uint8_t get_sn[5] = {0x00, 0xf5, 0x01, 0x00, 0x10};
  static char sn[32] = {0};
  uint16_t sn_len = sizeof(sn);

  if (len > 0x10) len = 0x10;
  get_sn[4] = len;

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
  uint8_t get_ver[5] = {0x00, 0xf7, 0x00, 00, 0x02};
  uint8_t ver[2] = {0};
  uint16_t ver_len = sizeof(ver);
  static char ver_char[9] = {0};
  int i = 0;

  if (!thd89_transmit(get_ver, sizeof(get_ver), ver, &ver_len)) {
    return NULL;
  }

  ver_char[i++] = (ver[0] >> 4) + '0';
  ver_char[i++] = '.';
  ver_char[i++] = (ver[0] & 0x0f) + '0';
  ver_char[i++] = '.';
  ver_char[i++] = (ver[1] >> 4) + '0';
  ver_char[i++] = (ver[1] & 0x0f) + '0';

  return ver_char;
}

secbool se_get_pubkey(uint8_t pubkey[64]) {
  uint8_t get_pubkey[5] = {0x00, 0x73, 0x00, 00, 0x40};
  uint16_t pubkey_len = 0;
  if (!thd89_transmit(get_pubkey, sizeof(get_pubkey), pubkey, &pubkey_len)) {
    return secfalse;
  }
  if (pubkey_len != 64) {
    return secfalse;
  }
  return sectrue;
}

secbool se_write_certificate(const uint8_t *cert, uint32_t cert_len) {
  return sectrue;
}

secbool se_read_certificate(uint8_t *cert, uint32_t *cert_len) {
  uint8_t get_cert[5] = {0x00, 0xf8, 0x01, 0x00, 0x00};
  return thd89_transmit(get_cert, sizeof(get_cert), cert, (uint16_t *)cert_len);
}

secbool se_get_certificate_len(uint32_t *cert_len) {
  uint8_t cert[512];
  return se_read_certificate(cert, cert_len);
}

secbool se_has_cerrificate(void) {
  uint8_t cert[512];
  uint32_t cert_len = 0;
  return se_read_certificate(cert, &cert_len);
}

secbool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature) {
  uint8_t sign[37] = {0x00, 0x72, 0x00, 00, 0x20};
  uint16_t signature_len;

  if (msg_len != 0x20) {
    return secfalse;
  }
  memcpy(sign + 5, msg, msg_len);
  return thd89_transmit(sign, sizeof(sign), signature, &signature_len);
}

secbool se_isInitialized(void) {
  if (se_isLifecyComSta()) {
    return sectrue;
  }
  return secfalse;
}

secbool se_hasPin(void) {
  uint8_t cmd[5] = {0x80, 0xe1, 0x28, 00, 0x00};
  uint8_t hasPin = 0xff;
  uint16_t len = sizeof(hasPin);

  if (!thd89_transmit(cmd, sizeof(cmd), &hasPin, &len)) {
    return secfalse;
  }

  // 0x55 exist ,0xff not
  return sectrue * (hasPin == 0x55);
}

secbool se_verifyPin(const char *pin) {
  uint8_t retry = 0;
  uint16_t len = sizeof(retry);

  if (!se_transmit(SE_CMD_WR_PIN, (SE_VERIFYPIN & 0xFF), (uint8_t *)pin,
                   strlen(pin), &retry, &len, SE_ENCRYPT, SE_WRFLG_CHGPIN)) {
    if (0x6f80 == thd89_last_error()) {
      error_reset("You have entered the", "wipe code. All private",
                  "data has been erased.", NULL);
    }

    return secfalse;
  }

  return sectrue;
}

secbool se_setPin(const char *pin) {
  uint16_t recv_len = 0xff;

  if (!se_transmit(SE_CMD_WR_PIN, (SE_PIN & 0xFF), (uint8_t *)pin, strlen(pin),
                   NULL, &recv_len, SE_ENCRYPT, SE_WRFLG_SETPIN)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_changePin(const char *oldpin, const char *newpin) {
  uint8_t pin_buff[110];
  uint16_t recv_len = 0xff;

  pin_buff[0] = strlen(oldpin);
  memcpy(pin_buff + 1, (uint8_t *)oldpin, strlen(oldpin));
  pin_buff[strlen(oldpin) + 1] = strlen(newpin);
  memcpy(pin_buff + strlen(oldpin) + 2, (uint8_t *)newpin, strlen(newpin));

  if (!se_transmit(SE_CMD_WR_PIN, (SE_PIN & 0xFF), (uint8_t *)pin_buff,
                   strlen(oldpin) + strlen(newpin) + 1 + 1, NULL, &recv_len,
                   SE_ENCRYPT, SE_WRFLG_CHGPIN)) {
    return secfalse;
  }
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
  uint8_t cmd[5 + 16] = {0x80, 0xe1, 0x25, 0x00, 0x10};
  uint8_t recv_buf[0x20], ref_buf[0x20], rand_buf[0x10];
  uint16_t recv_len = 0xff;
  aes_decrypt_ctx aes_dec_ctx;

  // TODO. get se random 16 bytes
  random_buffer(rand_buf, 0x10);
  memcpy(cmd + 5, rand_buf, sizeof(rand_buf));
  if (!thd89_transmit(cmd, sizeof(cmd), recv_buf, &recv_len)) {
    return secfalse;
  }
  // TODO. parse returned data
  if (recv_len != 0x20) return false;
  aes_decrypt_key128(se_session_key, &aes_dec_ctx);
  aes_ecb_decrypt(recv_buf, ref_buf, recv_len, &aes_dec_ctx);
  if (memcmp(ref_buf, rand_buf, sizeof(rand_buf)) != 0) {
    return secfalse;
  }

  // TODO: retry cnts
  if (ref_buf[0x10] > SE_PIN_RETRY_MAX) {
    return secfalse;
  }
  *ptimes = ref_buf[0x10];

  return sectrue;
}

secbool se_clearSecsta(void) {
  uint8_t cmd[5] = {0x80, 0xe1, 0x26, 0x01, 0x00};
  uint16_t recv_len;
  if (!thd89_transmit(cmd, sizeof(cmd), NULL, &recv_len)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_getSecsta(void) {
  uint8_t cmd[5] = {0x80, 0xe1, 0x26, 0x00, 0x00};
  uint8_t cur_secsta = 0xff;
  uint16_t recv_len = 0xff;
  if (!thd89_transmit(cmd, sizeof(cmd), &cur_secsta, &recv_len)) {
    return secfalse;
  }
  // 0x55 is verified pin 0x00 is not verified pin

  return sectrue * (cur_secsta == 0x55);
}

secbool se_setPinValidtime(uint8_t data) {
  uint16_t recv_len = 0xff;
  if (!se_transmit(SE_CMD_WR_PIN, (SE_PIN_VALIDTIME & 0xFF), &data, 1, NULL,
                   &recv_len, SE_ENCRYPT, SET_SESTORE_DATA)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_getPinValidtime(uint8_t *data_buf) {
  uint8_t cmd[5 + 16] = {0x80, 0xe1, 0x15, 0x00, 0x10};
  uint8_t recv_buf[0x20], ref_buf[0x20], rand_buf[0x10];
  uint16_t recv_len = 0xff;  // 32 bytes session id
  aes_decrypt_ctx aes_dec_ctx;

  // TODO. get se random 16 bytes
  random_buffer(rand_buf, 0x10);
  memcpy(cmd + 5, rand_buf, sizeof(rand_buf));
  if (!thd89_transmit(cmd, sizeof(cmd), recv_buf, &recv_len)) {
    return secfalse;
  }
  // TODO. parse returned data
  if (recv_len != 0x20) return secfalse;
  aes_decrypt_key128(se_session_key, &aes_dec_ctx);
  aes_ecb_decrypt(recv_buf, ref_buf, recv_len, &aes_dec_ctx);
  if (memcmp(ref_buf, rand_buf, sizeof(rand_buf)) != 0) return secfalse;

  // TODO. setted valid time and remained valid time
  memcpy(data_buf, ref_buf + sizeof(rand_buf), 3);
  return sectrue;
}

secbool se_applyPinValidtime(void) {
  uint16_t recv_len = 0xff;
  if (!se_transmit(SE_CMD_WR_PIN, (SE_APPLY_PINVALIDTIME & 0xFF), NULL, 0, NULL,
                   &recv_len, SE_ENCRYPT, GET_SESTORE_DATA)) {
    return secfalse;
  }

  return sectrue;
}

se_generate_state_t se_beginGenerate(se_generate_type_t type,
                                     se_generate_session_t *session) {
  uint8_t cur_cnts = 0xff;
  uint16_t recv_len = 0;
  if (!se_transmit_ex(SE_CMD_WR_PIN, 0x12, NULL, 0, &cur_cnts, &recv_len,
                      SE_ENCRYPT, type, PROCESS_BEGIN)) {
    if (recv_len == 1) {
    } else {
      return STATE_FAILD;
    }
  }
  session->processing = PROCESS_GENERATING;
  session->type = type;
  return STATE_GENERATING;
}

se_generate_state_t se_generating(se_generate_session_t *session) {
  uint8_t cmd[5] = {0x80, 0xe1, 0x12, 0x00, 0x00};
  uint8_t cur_cnts = 0xff;
  uint16_t recv_len = 0;
  cmd[3] = session->type;
  if (!thd89_transmit(cmd, sizeof(cmd), &cur_cnts, &recv_len)) {
    return STATE_GENERATING;
  }

  return STATE_COMPLETE;
}

secbool se_isFactoryMode(void) {
  uint8_t cmd[5] = {0x00, 0xf8, 0x04, 00, 0x01};
  uint8_t mode = 0;
  uint16_t len = sizeof(mode);

  if (!thd89_transmit(cmd, sizeof(cmd), &mode, &len)) {
    return secfalse;
  }

  return sectrue * (mode == 0xff);
}

secbool se_set_u2f_counter(uint32_t u2fcounter) {
  return se_set_value(SE_U2FCOUNTER, &u2fcounter, sizeof(u2fcounter));
}

secbool se_get_u2f_counter(uint32_t *u2fcounter) {
  uint16_t len;
  return se_get_value(SE_U2FCOUNTER, u2fcounter, sizeof(uint32_t), &len);
}

secbool se_set_mnemonic(const void *mnemonic, uint16_t len) {
  return se_set_spec_value(SE_MNEMONIC, mnemonic, len, SE_WRFLG_MNEMONIC);
}

secbool se_get_entropy(uint8_t entropy[32]) {
  if (!se_get_rand(entropy, 0x10)) return secfalse;
  if (!se_get_rand(entropy + 0x10, 0x10)) return secfalse;
  return sectrue;
}

secbool se_set_entropy(const void *entropy, uint16_t len) {
  return se_set_spec_value(SE_ENTROPY, entropy, len, SE_WRFLG_ENTROPY);
}

// 0x81:Initializing  0x82:Initialized
secbool se_isLifecyComSta(void) {
  uint8_t cmd[5] = {0x00, 0xf8, 0x04, 00, 0x01};
  uint8_t mode = 0;
  uint16_t len = sizeof(mode);

  if (!thd89_transmit(cmd, sizeof(cmd), &mode, &len)) {
    return secfalse;
  }

  return sectrue * (mode == 0x82);
}

secbool se_sessionStart(OUT uint8_t *session_id_bytes) {
  uint8_t cmd[5 + 16] = {0x80, 0xe7, 0x00, 0x00, 0x00};
  uint8_t recv_buf[0x20];
  uint16_t recv_len = 0xff;  // 32 bytes session id

  if (!thd89_transmit(cmd, sizeof(cmd), recv_buf, &recv_len)) {
    return secfalse;
  }

  memcpy(session_id_bytes, recv_buf, 0x20);
  return sectrue;
}

secbool se_sessionOpen(IN uint8_t *session_id_bytes) {
  uint16_t recv_len = 0;
  if (!se_transmit(SE_CMD_WR_SESSION, 0x01, session_id_bytes, 32, NULL,
                   &recv_len, SE_ENCRYPT, GET_SESTORE_DATA)) {
    return secfalse;
  }
  return sectrue;
}

se_generate_state_t se_sessionBeginGenerate(const uint8_t *passphase,
                                            uint16_t len,
                                            se_generate_type_t type,
                                            se_generate_session_t *session) {
  uint8_t cur_cnts = 0xff;
  uint16_t recv_len = 0;
  if (sectrue == se_transmit_ex(SE_CMD_WR_SESSION, 0x02, (uint8_t *)passphase,
                                len, &cur_cnts, &recv_len, SE_ENCRYPT, type,
                                PROCESS_BEGIN)) {
    return STATE_COMPLETE;
  }
  session->processing = PROCESS_GENERATING;
  session->type = type;
  return STATE_GENERATING;
}

se_generate_state_t se_sessionGenerating(se_generate_session_t *session) {
  uint8_t cmd[5] = {0x80, 0xe7, 0x02, 0x00, 0x00};
  uint8_t cur_cnts = 0xff;
  uint16_t recv_len = 0;
  cmd[3] = session->type;
  if (!thd89_transmit(cmd, sizeof(cmd), &cur_cnts, &recv_len)) {
    return STATE_GENERATING;
  }

  return STATE_COMPLETE;
}

secbool se_sessionClose(void) {
  uint16_t recv_len = 0;
  if (!se_transmit(SE_CMD_WR_SESSION, 0x03, NULL, 0, NULL, &recv_len,
                   SE_ENCRYPT, GET_SESTORE_DATA)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_sessionClear(void) {
  uint16_t recv_len = 0;
  if (!se_transmit(SE_CMD_WR_SESSION, 0x04, NULL, 0, NULL, &recv_len,
                   SE_ENCRYPT, GET_SESTORE_DATA)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_set_public_region(const uint16_t offset, const void *val_dest,
                             uint16_t len) {
  uint8_t cmd[5] = {0x00, 0xE6, 0x00, 0x00, 0x10};
  uint8_t recv_buf[8];
  uint16_t recv_len = sizeof(recv_buf);
  if (offset > PUBLIC_REGION_SIZE) return secfalse;
  cmd[2] = (uint8_t)((uint16_t)offset >> 8 & 0x00FF);
  cmd[3] = (uint8_t)((uint16_t)offset & 0x00FF);
  cmd[4] = len;
  memcpy(APDU, cmd, 5);
  memcpy(APDU_DATA, (uint8_t *)val_dest, len);
  if (!thd89_transmit(APDU, 5 + len, recv_buf, &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_get_public_region(uint16_t offset, void *val_dest, uint16_t len) {
  uint8_t cmd[5] = {0x00, 0xE5, 0x00, 0x00, 0x10};
  uint16_t recv_len = len;
  if (offset > PUBLIC_REGION_SIZE) return secfalse;
  cmd[2] = (uint8_t)((uint16_t)offset >> 8 & 0x00FF);
  cmd[3] = (uint8_t)((uint16_t)offset & 0x00FF);
  cmd[4] = len;
  if (!thd89_transmit(cmd, sizeof(cmd), (uint8_t *)val_dest, &recv_len)) {
    return secfalse;
  }

  return sectrue;
}

secbool se_set_private_region(uint16_t offset, const void *val_dest,
                              uint16_t len) {
  uint8_t cmd[5] = {0x00, 0xE6, 0x00, 0x00, 0x10};
  uint8_t recv_buf[8];
  uint16_t recv_len = sizeof(recv_buf);
  if (offset + len > PRIVATE_REGION_SIZE) return secfalse;
  offset += SE_PRIVATE_REGION_BASE;
  cmd[2] = (uint8_t)((uint16_t)offset >> 8 & 0x00FF);
  cmd[3] = (uint8_t)((uint16_t)offset & 0x00FF);
  cmd[4] = len;
  memcpy(APDU, cmd, 5);
  memcpy(APDU_DATA, (uint8_t *)val_dest, len);
  if (!thd89_transmit(APDU, 5 + len, recv_buf, &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_get_private_region(uint16_t offset, void *val_dest, uint16_t len) {
  uint8_t cmd[5] = {0x00, 0xE5, 0x00, 0x00, 0x10};
  uint16_t recv_len = len;
  if (offset + len > PRIVATE_REGION_SIZE) return secfalse;
  offset += SE_PRIVATE_REGION_BASE;
  cmd[2] = (uint8_t)((uint16_t)offset >> 8 & 0x00FF);
  cmd[3] = (uint8_t)((uint16_t)offset & 0x00FF);
  cmd[4] = len;
  if (!thd89_transmit(cmd, sizeof(cmd), val_dest, &recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_containsMnemonic(const char *mnemonic) {
  uint8_t resp[256];
  uint16_t resp_len;
  if (!se_transmit(SE_CMD_WR_MNEMONIC, 0x00, (uint8_t *)mnemonic,
                   strlen(mnemonic), resp, &resp_len, SE_ENCRYPT,
                   GET_SESTORE_DATA)) {
    return secfalse;
  }
  if (resp[0] == 0x01) return sectrue;
  return secfalse;
}

secbool se_hasWipeCode(void) {
  uint8_t resp[256];
  uint16_t resp_len;
  if (!se_transmit(SE_CMD_WR_WIPECODE, 0x01, NULL, 0, resp, &resp_len,
                   SE_ENCRYPT, GET_SESTORE_DATA)) {
    return secfalse;
  }
  if (resp[0] == 0x01) return sectrue;
  return secfalse;
}
secbool se_changeWipeCode(const char *wipe_code) {
  uint16_t recv_len = 0xff;

  if (!se_transmit(SE_CMD_WR_WIPECODE, 0x00, (uint8_t *)wipe_code,
                   strlen(wipe_code), NULL, &recv_len, SE_ENCRYPT,
                   SE_WRFLG_SETPIN)) {
    return secfalse;
  }

  return sectrue;
}

static secbool _se_ecdsa_sign_digest(uint8_t curve, const uint8_t *hash,
                                     uint8_t *sig, uint8_t *v) {
  uint8_t resp[128], tmp[40];
  uint16_t resp_len = 0x41;
  uint32_t mode = ETH_ECDSA_SIGN;

  memset(tmp, 0x00, sizeof(tmp));
  LITTLE_REVERSE32(mode, mode);
  memcpy(tmp, &mode, sizeof(uint32_t));
  tmp[4] = SEC_GENK_RFC6979;
  memcpy(tmp + 5, hash, 32);  // for special sign add mode (4 bytes)+genk(1
                              // byte)+hash(32 byte),so total len is 37.
  if (!se_transmit(SE_CMD_ECC_EDDSA, ECC_INDEX_SIGN, tmp, (4 + 1 + 32), resp,
                   &resp_len, SE_ENCRYPT, curve)) {
    return secfalse;
  }
  memcpy(sig, resp + 1, 64);
  if (v) *v = resp[0];
  return sectrue;
}

secbool se_25519_sign(uint8_t curve, const uint8_t *msg, uint16_t msg_len,
                      uint8_t *sig) {
  uint8_t resp[128];
  uint16_t resp_len;
  if (!se_transmit(SE_CMD_ECC_EDDSA, EDDSA_INDEX_SIGN, (uint8_t *)msg, msg_len,
                   resp, &resp_len, SE_ENCRYPT, curve)) {
    return secfalse;
  }
  memcpy(sig, resp, resp_len);
  return sectrue;
}

int se_ed25519_sign(const uint8_t *msg, uint16_t msg_len, uint8_t *sig) {
  return sectrue == se_25519_sign(CURVE_ED25519, msg, msg_len, sig) ? 0 : -1;
}

// ed25519 ext sign
#define se_ed25519_icarus_sign(msg, msg_len, sig) \
  se_25519_sign(CURVE_ED25519_ICARUS, msg, msg_len, sig)

#define se_sr25519_sign(msg, msg_len, sig) \
  se_25519_sign(CURVE_SR25519, msg, msg_len, sig)

// TODO it will sign digest
secbool se_schnoor_sign_plain(const uint8_t *data, uint16_t data_len,
                              uint8_t *sig) {
  uint8_t resp[128];
  uint16_t resp_len;

  if (!se_transmit(SE_CMD_SCHNOOR, SCHNOOR_INDEX_SIGN, (uint8_t *)data,
                   data_len, resp, &resp_len, SE_ENCRYPT, GET_SESTORE_DATA)) {
    return secfalse;
  }
  if (resp_len != 64) {
    return secfalse;
  }
  memcpy(sig, resp, 64);
  return sectrue;
}

secbool se_aes_128_encrypt(uint8_t mode, uint8_t *key, uint8_t *iv,
                           uint8_t *send, uint16_t send_len, uint8_t *recv,
                           uint16_t *recv_len) {
  uint8_t cmd[5] = {0x80, 0xE2, 0x01, 0x00, 0x00};
  uint16_t data_len;

  if (AES_ECB != mode && AES_CBC != mode) return secfalse;
  data_len = 0;

  // TODO
  if (AES_CBC == mode) {
    memcpy(APDU_DATA + data_len, iv, 16);
    data_len += 16;
  }
  memcpy(APDU_DATA + data_len, key, 16);
  data_len += 16;
  cmd[3] = mode;  // p2 is work mode
  // TODO
  memcpy(APDU, cmd, 5);
  memcpy(APDU_DATA, (uint8_t *)send, send_len);
  data_len += send_len;
  if (!thd89_transmit(cmd, 5 + data_len, recv, recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_aes_128_decrypt(uint8_t mode, uint8_t *key, uint8_t *iv,
                           uint8_t *send, uint16_t send_len, uint8_t *recv,
                           uint16_t *recv_len) {
  uint8_t cmd[5] = {0x80, 0xE2, 0x00, 0x00, 0x00};
  uint16_t data_len;

  if (AES_ECB != mode && AES_CBC != mode) return secfalse;
  data_len = 0;

  // TODO
  if (AES_CBC == mode) {
    memcpy(APDU_DATA + data_len, iv, 16);
    data_len += 16;
  }
  memcpy(APDU_DATA + data_len, key, 16);
  data_len += 16;
  cmd[3] = mode;  // p2 is work mode
  // TODO
  memcpy(APDU, cmd, 5);
  memcpy(APDU_DATA, (uint8_t *)send, send_len);
  data_len += send_len;
  if (!thd89_transmit(cmd, 5 + data_len, recv, recv_len)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_getSessionCachedState(se_session_cached_status *status) {
  uint8_t state;
  uint16_t recv_len = 1;
  uint8_t cmd[5] = {0x80, 0xE7, 0x06, 0x00, 0x00};
  if (!thd89_transmit(cmd, 5, &state, &recv_len)) {
    return secfalse;
  }

  status->se_seed_status = state & 0x01;
  status->se_minisecret_status = state & 0x02;
  status->se_icarus_status = state & 0x04;
  return sectrue;
}

inline static bool session_generate_steps(uint8_t *passphrase, uint16_t len) {
// `seed` 'minisecret' or `icarus main secret`
#define TOTAL_PROCESSES 1000
#define SEED_PROCESS 200
#define MINI_PROCESS SEED_PROCESS
#define ICARUS_PROCESS (TOTAL_PROCESSES - SEED_PROCESS - MINI_PROCESS)

  // one thousandth precision
  int base = 0;

#define SESSION_GENERATE_STEP(type, precent)                      \
  do {                                                            \
    if ((precent) == 0) return true;                              \
    se_generate_session_t session = {0};                          \
    se_generate_state_t state =                                   \
        se_sessionBeginGenerate(passphrase, len, type, &session); \
    int step = 1;                                                 \
    while (state == STATE_GENERATING) {                           \
      int permil = base + step * (precent / 100);                 \
      if (ui_callback) {                                          \
        ui_callback(0, permil, "Generating session seed ...");    \
      }                                                           \
      step++;                                                     \
      state = se_sessionGenerating(&session);                     \
    }                                                             \
    if (state != STATE_COMPLETE) return false;                    \
    base += precent;                                              \
  } while (0)

  // generate `seed` 'minisecret' or `icarus main secret`
  SESSION_GENERATE_STEP(TYPE_SEED, SEED_PROCESS);
  SESSION_GENERATE_STEP(TYPE_MINI_SECRET, MINI_PROCESS);
  SESSION_GENERATE_STEP(TYPE_ICARUS_MAIN_SECRET, ICARUS_PROCESS);

  return true;
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

secbool se_gen_session_seed(const char *passphrase) {
  se_session_cached_status status = {0};
  if (!se_getSessionCachedState(&status)) {
    return secfalse;
  }

  if (status.se_seed_status) {
    return sectrue;
  }

  if (!session_generate_steps((uint8_t *)passphrase, strlen(passphrase))) {
    return secfalse;
  }
  return sectrue;
}

int se_ecdsa_sign_digest(const char *curve, const uint8_t *digest, uint8_t *sig,
                         uint8_t *pby,
                         int (*is_canonical)(uint8_t, uint8_t *)) {
  if (strcmp(curve, SECP256K1_NAME) == 0) {
    if (!_se_ecdsa_sign_digest(CURVE_SECP256K1, digest, sig, pby)) {
      return -1;
    }

  } else if (strcmp(curve, NIST256P1_NAME) == 0) {
    if (!_se_ecdsa_sign_digest(CURVE_NIST256P1, digest, sig, pby)) {
      return -1;
    }
  } else {
    return -1;
  }
  if (is_canonical && !is_canonical(*pby, sig)) return -1;
  return 0;
}

secbool se_ecdsa_ecdh(uint8_t curve, const uint8_t *publickey,
                      uint8_t *sessionkey) {
  uint8_t resp[128];
  uint16_t resp_len;
  if (!se_transmit(SE_CMD_ECC_EDDSA, EDDSA_INDEX_ECDH, (uint8_t *)publickey, 64,
                   resp, &resp_len, SE_PLAIN, curve)) {
    return secfalse;
  }
  memcpy(sessionkey, resp, resp_len);
  return sectrue;
}
#define se_secp256k1_ecdh(publickey, sessionkey) \
  se_ecdsa_ecdh(ECDH_SECP256K1, publickey, sessionkey)
#define se_nist256p1_ecdh(publickey, sessionkey) \
  se_ecdsa_ecdh(ECDH_NIST256P1, publickey, sessionkey)

bool se_25519_ecdh(uint8_t curve, const uint8_t *publickey,
                   uint8_t *sessionkey) {
  uint8_t resp[128];
  uint16_t resp_len;
  if (!se_transmit(SE_CMD_ECC_EDDSA, EDDSA_INDEX_ECDH, (uint8_t *)publickey, 64,
                   resp, &resp_len, SE_PLAIN, curve)) {
    return secfalse;
  }
  memcpy(sessionkey, resp, resp_len);
  return sectrue;
}
#define se_curve25519_ecdh(publickey, sessionkey) \
  se_25519_ecdh(ECDH_CURVE25519, publickey, sessionkey)

int se_get_shared_key(const char *curve, const uint8_t *peer_public_key,
                      uint8_t *session_key) {
  if (strcmp(curve, NIST256P1_NAME)) {
    if (!se_nist256p1_ecdh(peer_public_key + 1, session_key + 1)) return -1;
    return 0;
  } else if (strcmp(curve, SECP256K1_NAME)) {
    if (!se_secp256k1_ecdh(peer_public_key + 1, session_key + 1)) return -1;
    return 0;
  } else if (strcmp(curve, CURVE25519_NAME)) {
    if (!se_curve25519_ecdh(peer_public_key, session_key + 1)) return -1;
    return 0;
  }
  return -1;
}

secbool se_derive_tweak_private_keys(void) {
  uint8_t resp[256];
  uint16_t resp_len = 0xffff;

  if (!se_transmit(SE_CMD_ECC_EDDSA, EDDSA_INDEX_CHILDKEY, (uint8_t *)NULL, 0,
                   resp, &resp_len, SE_PLAIN, DERIVE_BIP86_TWEAK)) {
    return secfalse;
  }

  return sectrue;
}

int se_bip340_sign_digest(const uint8_t *digest, uint8_t sig[64]) {
  return se_schnoor_sign_plain(digest, 32, sig) ? 0 : -1;
}

int se_bip340_ecdh(const uint8_t *peer_public_key, uint8_t session_key[65]) {
  return se_secp256k1_ecdh(peer_public_key, session_key) ? 0 : -1;
}

secbool se_setCoinJoinAuthorization(const uint8_t *authorization,
                                    uint16_t len) {
  uint16_t recv_len = 0xff;
  if (!se_transmit(SE_CMD_WR_SESSION, 0x05, (uint8_t *)authorization, len, NULL,
                   &recv_len, SE_ENCRYPT, 0x00)) {
    return secfalse;
  }
  return sectrue;
}

secbool se_getCoinJoinAuthorization(uint8_t *authorization, uint16_t *len) {
  uint8_t cmd[5 + 16] = {0x80, 0xe7, 0x05, 0x01, 0x10};
  uint8_t recv_buf[0x100], ref_buf[0x100], rand_buf[0x10];
  uint16_t recv_len = 0xff;
  aes_decrypt_ctx aes_dec_ctx;

  // TODO. get se random 16 bytes
  random_buffer(rand_buf, 0x10);
  memcpy(cmd + 5, rand_buf, sizeof(rand_buf));
  if (!thd89_transmit(cmd, sizeof(cmd), recv_buf, &recv_len)) {
    return secfalse;
  }

  aes_decrypt_key128(se_session_key, &aes_dec_ctx);
  aes_ecb_decrypt(recv_buf, ref_buf, recv_len, &aes_dec_ctx);
  if (memcmp(ref_buf, rand_buf, sizeof(rand_buf)) != 0) {
    return secfalse;
  }
  // delete pad
  uint8_t i = 0, padLen = 0;
  for (i = 1; i < 0x11; i++) {
    if (ref_buf[recv_len - i] == 0x80) {
      for (padLen = 1; padLen < i; padLen++) {
        if (ref_buf[recv_len - padLen] != 0x00) {
          i = 0x11;
          break;
        }
      }
      break;
    }
  }
  if (i != 0x11) {
    recv_len = recv_len - i;
  }
  recv_len -= sizeof(rand_buf);

  memcpy(authorization, ref_buf + sizeof(rand_buf), recv_len);
  *len = recv_len;
  return sectrue;
}
