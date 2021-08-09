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
  char atca_config_verson[8];
} DeviceConfig;

void device_init(void);
bool device_serial_set(void);
void device_set_factory_mode(bool mode);
bool device_is_factory_mode(void);
bool device_set_info(char *dev_serial);
bool device_get_serial(char **serial);
char *device_get_se_config_version(void);

#endif
