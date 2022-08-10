#ifndef _SE_ATCA_H_
#define _SE_ATCA_H_

#include <stdbool.h>
#include <stdint.h>

#define se_sync_session_key(void) NULL
#define se_setNeedsBackup(needs_backup) true

char *se_get_version(void);
bool se_get_sn(char **serial);

bool se_setSeedStrength(uint32_t strength);
bool se_getSeedStrength(uint32_t *strength);
bool se_importSeed(uint8_t *seed);
bool se_export_seed(uint8_t *seed);
void se_get_status(void);
bool se_hasPin(void);
bool se_verifyPin(const char *pin);
bool se_changePin(const char *old_pin, const char *new_pin);
bool se_isInitialized(void);
bool se_is_wiping(void);
void se_set_wiping(bool flag);
void se_reset_state(void);
void se_reset_storage(void);
uint32_t se_pinFailedCounter(void);
bool se_device_init(uint8_t mode, const char *passphrase);

bool se_get_pubkey(uint8_t pubkey[64]);
bool se_write_certificate(const uint8_t *cert, uint32_t cert_len);
bool se_get_certificate_len(uint32_t *cert_len);
bool se_read_certificate(uint8_t *cert, uint32_t *cert_len);
bool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature);
void se_init(void);

#endif
