#ifndef __BLE_H__
#define __BLE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BLE_NAME_LEN 0x12
#define BLE_ADV_OFF 0x00
#define BLE_ADV_ON 0x01
#define BLE_ADV_ON_TEMP 0x05
#define BLE_ADV_OFF_TEMP 0x06

#define BUTTON_PRESS_BLE_ON 0x01
#define BUTTON_PRESS_BLE_OFF 0x02

enum {
  BLE_CMD_CONNECT_STATE = 0x01,
  BLE_CMD_PAIR_STATE = 0x02,
  BLE_CMD_PASSKEY = 0x03,
  BLE_CMD_BT_NAME = 0x04,
  BLE_CMD_BATTERY = 0x05,
  BLE_CMD_VER = 0x06,
  BLE_CMD_ONOFF_BLE = 0x07,
  BLE_CMD_DFU_STA = 0x0A
};

bool ble_connect_state(void);
void ble_request_info(uint8_t type);
void ble_ctl_onoff(void);
void ble_reset(void);
void ble_uart_poll(void);

#if !EMULATOR
bool ble_is_enable(void);
bool ble_name_state(void);
bool ble_ver_state(void);
bool ble_battery_state(void);
char *ble_get_name(void);
char *ble_get_ver(void);
bool ble_switch_state(void);
void ble_set_switch(bool flag);
bool ble_get_switch(void);
void change_ble_sta(uint8_t mode);
#else
#define ble_name_state(...) false
#define ble_ver_state(...) false
#define ble_get_name(...) "BixinKEY814591011"
#define ble_get_ver(...) "1.0.1"
#define ble_switch_state(...) false
#define ble_set_switch(...)
#define ble_get_switch(...) false
#define change_ble_sta(...)
#endif

#endif
