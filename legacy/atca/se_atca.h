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
bool se_hasPin(void);
bool se_verifyPin(const char *pin);
bool se_changePin(const char *old_pin, const char *new_pin);
bool se_isInitialized(void);
void se_reset_storage(void);
uint32_t se_pinFailedCounter(void);
bool se_device_init(uint8_t mode, const char *passphrase);

#endif
