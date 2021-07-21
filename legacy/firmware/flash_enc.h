#ifndef _FLASH_ENC_H_
#define _FLASH_ENC_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void flash_enc_init(void);
bool flash_write_enc(uint8_t *buffer, uint32_t address, uint32_t len);
bool flash_read_enc(uint8_t *buffer, uint32_t address, uint32_t len);

#endif
