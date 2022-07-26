#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define PRODUCT_STRING "OneKey Touch"
#define SE_NAME "ATECC608"

typedef struct __attribute__((packed)) {
  char product[2];
  char hardware[2];
  char color;
  char factory[2];
  char utc[10];
  char serial[7];
} DeviceSerialNo;

typedef struct __attribute__((packed)) {
  char serial[32];
  char cpu_info[16];
  char pre_firmware[16];
  char se_config[8];
  uint32_t st_id[3];
  bool random_key_init;
  uint8_t random_key[32];
  bool vendor_key_init;
  uint8_t vendor_key[32];
} DeviceInfomation;

void device_set_factory_mode(bool mode);
bool device_is_factory_mode(void);
void device_para_init(void);
bool device_serial_set(void);
bool device_set_serial(char *dev_serial);
bool device_cpu_firmware_set(void);
bool device_set_cpu_firmware(char *cpu_info, char *firmware_ver);
bool device_get_cpu_firmware(char **cpu_info, char **firmware_ver);
bool device_get_serial(char **serial);
char *device_get_se_config_version(void);
void device_get_enc_key(uint8_t key[32]);
bool device_vendor_key_is_set(void);
bool device_set_vendor_key(uint8_t key[32]);
bool device_get_vendor_key(uint8_t key[32]);

void device_test(void);

#endif
