#ifndef _THD89_BOOT_H_
#define _THD89_BOOT_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define THD89_STATE_BOOT 0x00
#define THD89_STATE_NOT_ACTIVATED 0x33
#define THD89_STATE_APP 0x55

bool se_get_state(uint8_t *state);
bool se_back_to_boot(void);
bool se_active_app(void);
bool se_update(uint8_t step, uint8_t *data, uint16_t data_len);
bool se_back_to_boot_progress(void);
bool se_update_firmware(uint8_t *data, uint32_t data_len,
                        void (*ui_callback)(int progress));
bool se_active_app_progress(void);
bool se_verify_firmware(uint8_t *header, uint32_t header_len);
bool se_check_firmware(void);

#endif
