#ifndef __BLE_H__
#define __BLE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BLE_NAME_LEN 0x12

bool ble_connect_state(void);
void ble_request_name(void);
void ble_ctl_onoff(void);
void ble_reset(void);
void ble_uart_poll(void);

#if !EMULATOR
bool ble_name_state(void);
bool ble_ver_state(void);
char *ble_get_name(void);
char *ble_get_ver(void);
#else
#define ble_name_state(...) false
#define ble_ver_state(...) false
#define ble_get_name(...) "BixinKEY814591011"
#define ble_get_ver(...) "1.0.1"
#endif

#endif
