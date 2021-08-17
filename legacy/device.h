#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
  uint8_t st_id[12];
  uint8_t random_key[32];
} DeviceInfomation;

void device_set_factory_mode(bool mode);
bool device_is_factory_mode(void);
void device_init(void);
bool device_serial_set(void);
bool device_set_serial(char *dev_serial);
bool device_cpu_firmware_set(void);
bool device_set_cpu_firmware(char *cpu_info, char *firmware_ver);
bool device_get_cpu_firmware(char **cpu_info, char **firmware_ver);
bool device_get_serial(char **serial);
char *device_get_se_config_version(void);
bool device_get_NFT_voucher(uint8_t voucher[32]);

#endif
