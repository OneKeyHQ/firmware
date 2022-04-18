#ifndef _ATCA_API_H_
#define _ATCA_API_H_

#include "atca_config.h"
#include "atca_status.h"

#define ATCA_CONFIG_VERSION "0.0.1"

typedef struct __attribute__((packed)) {
  bool pin_set;
  bool initialized;
  uint32_t strength;
  bool wiping;
  uint8_t rfu[25];
} ATCAUserState;

typedef struct __attribute__((packed)) {
  uint8_t serial[16];
  uint8_t rfu[16];
  uint8_t protect_key[32];
  uint8_t init_pin[32];
  uint8_t hash_mix[32];
} ATCAPairingInfo;

ATCA_STATUS atca_get_config(ATCAConfiguration *config);
ATCA_STATUS atca_mac_slot(uint16_t key_id, uint8_t *slot_key);
ATCA_STATUS atca_sha_hmac(const uint8_t *data, size_t data_size,
                          uint16_t key_slot, uint8_t *digest);
void atca_pair_unlock(void);
void atca_config_init(void);
void atca_read_slot_data(uint16_t key_id, uint8_t data[32]);
void atca_update_counter(void);
uint32_t atca_get_failed_counter(void);
ATCA_STATUS atca_write_enc(uint16_t key_id, uint8_t block, uint8_t *data,
                           uint8_t *enc_key, uint16_t enc_key_id);
ATCA_STATUS atca_read_enc(uint16_t key_id, uint8_t block, uint8_t *data,
                          uint8_t *enc_key, uint16_t enc_key_id);

bool atca_cal_pubkey(uint8_t pubkey[64]);
bool atca_write_certificate(const uint8_t *cert, uint32_t cert_len);
bool atca_get_certificate_len(uint32_t *cert_len);
bool atca_read_certificate(uint8_t *cert, uint32_t *cert_len);
void atca_test(void);
#endif
