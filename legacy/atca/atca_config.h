#ifndef _ATCA_CONFIG_H_
#define _ATCA_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#define SLOT_DEFAULT_READKEY 15

#define SLOT_PRIMARY_PRIVATE_KEY 0
#define SLOT_IO_PROTECT_KEY 1
#define SLOT_USER_PIN 2
#define SLOT_PIN_ATTEMPT 3
#define SLOT_COUNTER_MATCH 4
#define SLOT_LAST_GOOD_COUNTER 5
#define SLOT_USER_SECRET 6
#define SLOT_USER_SATATE 7
#define SLOT_DEVICE_CERT 8

#define CONFIG_SLOT_PRI_SIGN_EXTERNAL 0x0001
#define CONFIG_SLOT_PRI_SIGN_INTERNAL 0x0002
#define CONFIG_SLOT_NO_MAC 0x0010
#define CONFIG_SLOT_LIMIT_USE 0x0020
#define CONFIG_SLOT_ENCRYPT_READ 0x0040
#define CONFIG_SLOT_ISSECRET 0x0080
#define CONFIG_SLOT_WRITE_ALWAYS 0x0000
#define CONFIG_SLOT_WRITE_PUBINVALID 0x1000
#define CONFIG_SLOT_WRITE_NEVER_TAGET 0x2000
#define CONFIG_SLOT_WRITE_ENCRYPT 0x4000
#define CONFIG_SLOT_WRITE_NEVER_PARENT 0x8000
#define CONFGI_SLOT_WRITE_KEY_POS 8

#define CONFIG_CHIP_OPTION_SELF_TEST 0x0001
#define CONFIG_CHIP_OPTION_IO_PROTECT 0x0002
#define CONFIG_CHIP_OPTION_KDF_PORT 0x0400
#define CONFIG_CHIP_OPTION_ECDH_PORT 0x0100
#define CONFIG_CHIP_OPTION_PROTECT_KEY_POS 12

#define COFIG_KEY_PRIVATE 0x00001
#define COFIG_KEY_PUBINFO 0x00002
#define CONFIG_KEY_TYPE_ECC 0x0010
#define CONFIG_KEY_TYPE_DATA 0x001C
#define CONFIG_KEY_LOCKABLE 0x0020
#define CONFIG_KEY_REQ_RANDOM 0x0040

#define CONFIG_KEY_REQ_AUTH 0x0080
#define CONFIG_KEY_AUTH_POS 8
#define CONFIG_KEY_PERSIST_DISABLE 0x1000

#define ATECC608_SN1_SIZE 4
#define ATECC608_SN2_SIZE 5
#define ATECC608_SN_SIZE (ATECC608_SN1_SIZE + ATECC608_SN2_SIZE)

typedef struct __attribute__((packed)) {
  uint8_t sn1[ATECC608_SN1_SIZE];
  uint8_t rev_num[4];
  uint8_t sn2[ATECC608_SN2_SIZE];
  uint8_t aes_enable;
  uint8_t i2c_enable;  // i2c swi
  uint8_t reserved_0;
  uint8_t i2c_address;  // gpio control for swi
  uint8_t reserved_1;
  uint8_t count_match;
  uint8_t chip_mode;
  uint16_t slot_config[16];
  uint8_t counter0[8];
  uint8_t counter1[8];
  uint8_t use_lock;
  uint8_t volatilekey_permission;
  uint16_t secure_boot;
  uint8_t kdfiv_loc;
  uint16_t kdfiv_str;
  uint8_t reserved_2[9];
  uint8_t user_extra;
  uint8_t user_extra_add;
  uint8_t lock_value;
  uint8_t lock_config;
  uint16_t slot_locked;
  uint16_t chip_options;
  uint8_t x509format[4];
  uint16_t key_config[16];
} ATCAConfiguration;

typedef struct __attribute__((packed)) {
  bool tempkey_nomac : 1;
  bool tempkey_genkey : 1;
  bool tempkey_gendig : 1;
  bool tempkey_source : 1;
  uint8_t tempkey_keyid : 4;
  bool tempkey_valid : 1;
  uint8_t auth_keyid : 4;
  bool auth_valid : 1;
  uint8_t res : 2;
  uint8_t rfu[2];
} ATCADeviceState;

#endif
