#ifndef __SE_CHIP_H__
#define __SE_CHIP_H__

#include <stdbool.h>
#include <stdint.h>

#define SESSION_KEYLEN (16)

// session key addr
#define SESSION_FALG (0x55AA55AA)

//
#define SESSION_FALG_INDEX (0x80)
#define SESSION_ADDR_INDEX (0x81)

#define LITTLE_REVERSE32(w, x)                                       \
  {                                                                  \
    uint32_t tmp = (w);                                              \
    tmp = (tmp >> 16) | (tmp << 16);                                 \
    (x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
  }

#define MI2C_OK 0xAAAAAAAAU
#define MI2C_ERROR 0x00000000U

#define MI2C_ENCRYPT 0x00
#define MI2C_PLAIN 0x80

#define GET_SESTORE_DATA (0x00)
#define SET_SESTORE_DATA (0x01)
#define DELETE_SESTORE_DATA (0x02)
#define DEVICEINIT_DATA (0x03)

#define CURVE_NIST256P1 (0x40)

#define MI2C_CMD_WR_PIN (0xE1)
#define MI2C_CMD_AES (0xE2)
#define MI2C_CMD_ECC_EDDSA (0xE3)

// ecc ed2519 index
#define ECC_INDEX_GITPUBKEY (0x00)
#define ECC_INDEX_SIGN (0x01)
#define ECC_INDEX_VERIFY (0x02)
#define EDDSA_INDEX_GITPUBKEY (0x03)
#define EDDSA_INDEX_SIGN (0x04)
#define EDDSA_INDEX_VERIFY (0x05)
#define EDDSA_INDEX_CHILDKEY (0x06)
#define EDDSA_INDEX_U2FKEY (0x07)

#define SE_EXPORT_SEED (0x24)

// mnemonic index
#define MNEMONIC_INDEX_TOSEED (26)

#define SE_CMD_GET_VERSION (0xE1)

extern uint8_t g_ucSessionKey[SESSION_KEYLEN];

#if !EMULATOR

extern void se_sync_session_key(void);
extern uint32_t se_transmit(uint8_t ucCmd, uint8_t ucIndex,
                            uint8_t *pucSendData, uint16_t usSendLen,
                            uint8_t *pucRevData, uint16_t *pusRevLen,
                            uint8_t ucMode, uint8_t ucWRFlag);
extern uint32_t se_transmit_plain(uint8_t *pucSendData, uint16_t usSendLen,
                                  uint8_t *pucRevData, uint16_t *pusRevLen);

bool se_device_init(uint8_t mode, const char *passphrase);
void se_get_seed(bool mode, const char *passphrase, uint8_t *seed);
bool se_ecdsa_get_pubkey(uint32_t *address, uint8_t count, uint8_t *pubkey);
bool se_set_value(const uint16_t key, const void *val_dest, uint16_t len);
bool se_get_value(const uint16_t key, void *val_dest, uint16_t max_len,
                  uint16_t *len);
bool se_delete_key(const uint16_t key);
void se_reset_storage(void);
bool se_get_sn(char **serial);
char *se_get_version(void);
bool se_verify(void *message, uint16_t message_len, uint16_t max_len,
               void *cert_val, uint16_t *cert_len, void *signature_val,
               uint16_t *signature_len);
bool se_backup(void *val_dest, uint16_t *len);
bool se_restore(void *val_src, uint16_t src_len);
bool se_st_seed_en(const uint16_t key, void *plain_data, uint16_t plain_len,
                   void *cipher_data, uint16_t *cipher_len);
bool se_st_seed_de(const uint16_t key, void *cipher_data, uint16_t cipher_len,
                   void *plain_data, uint16_t *plain_len);
bool st_backup_entory_to_se(const uint16_t key, uint8_t *seed,
                            uint8_t seed_len);
bool st_restore_entory_from_se(const uint16_t key, uint8_t *seed,
                               uint8_t *seed_len);

bool se_isInitialized(void);
bool se_hasPin(void);
bool se_setPin(uint32_t pin);
bool se_verifyPin(uint32_t pin);
bool se_changePin(uint32_t oldpin, uint32_t newpin);
uint32_t se_pinFailedCounter(void);
bool se_setSeedStrength(uint32_t strength);
bool se_getSeedStrength(uint32_t *strength);
bool se_getNeedsBackup(bool *needs_backup);
bool se_setNeedsBackup(bool needs_backup);
bool se_export_seed(uint8_t *seed);
bool se_importSeed(uint8_t *seed);
bool se_isFactoryMode(void);

#else
#define se_transmit(...) 0
#define se_get_sn(...) false
#define se_get_version(...) "1.1.0.0"
#define se_backup(...) false
#define se_restore(...) false
#define se_verify(...) false
#define se_device_init(...) false
#define se_st_seed_en(...) false
#define se_st_seed_de(...) false
#define se_set_value(...) false
#define st_backup_entory_to_se(...) false
#define st_restore_entory_from_se(...) false
#define se_reset_storage(...)
#define se_isInitialized(...) false
#define se_hasPin(...) false
#define se_setPin(...) false
#define se_verifyPin(...) false
#define se_changePin(...) false
#define se_pinFailedCounter(...) 0
#define se_setSeedStrength(...) false
#define se_getSeedStrength(...) false
#define se_getNeedsBackup(...) false
#define se_setNeedsBackup(...) false
#define se_export_seed(...) false
#define se_importSeed(...) false
#define se_isFactoryMode(...) false
#endif
#endif
