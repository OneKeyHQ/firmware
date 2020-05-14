#ifndef __SE_CHIP_H__
#define __SE_CHIP_H__

#include <stdbool.h>
#include <stdint.h>

#define SE_CMD_GET_VERSION (0xE1)
#define SE_CMD_READ_SN (0x00F5010010)

bool se_device_init(uint8_t mode, const char *passphrase);
void se_get_seed(bool mode, const char *passphrase, uint8_t *seed);
bool se_ecdsa_get_pubkey(uint32_t *address, uint8_t count, uint8_t *pubkey);
bool se_set_value(const uint16_t key, const void *val_dest, uint16_t len);
bool se_get_value(const uint16_t key, void *val_dest, uint16_t max_len,
                  uint16_t *len);
bool se_delete_key(const uint16_t key);
void se_reset_storage(const uint16_t key);
bool se_get_sn(void *val_dest, uint16_t max_len, uint16_t *len);
bool se_get_version(void *val_dest, uint16_t max_len, uint16_t *len);
bool se_verify(void *message, uint16_t message_len, void *val_dest,
               uint16_t max_len, uint16_t *len);
bool se_backup(void *val_dest, uint16_t *len);
bool se_restore(void *val_src, uint16_t src_len);
#endif
