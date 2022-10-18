#ifndef __BLE_H__
#define __BLE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BLE_NAME_LEN 16

// BLE send command
#define BLE_CMD_ADV 0x01
#define BLE_CMD_CON_STA 0x02
#define BLE_CMD_PAIR_TX 0x03
#define BLE_CMD_PAIR_STA 0x04
#define BLE_CMD_FM_VER 0x05
#define BLE_CMD_PROTO_VER 0x06
#define BLE_CMD_BOOT_VER 0x07
#define BLE_CMD_PLUG_STA 0x08
#define BLE_CMD_EQ 0x09
#define BLE_CMD_RPESS 0x0A
#define BLE_CMD_PWR 0x0B

// ST send command
#define BLE_BT 0x81
#define BLE_BT_ON 0x01
#define BLE_BT_OFF 0x02
#define BLE_BT_DISCON 0x03
#define BLE_BT_STA 0x04
#define BLE_PWR 0x82
#define BLE_PWR_SYS_OFF 0x01
#define BLE_PWR_EMMC_OFF 0x02
#define BLE_PWR_EMMC_ON 0x03
#define BLE_PWR_EQ 0x04
#define BLE_PWR_CHARGING 0x05
#define BLE_VER 0x83
#define BLE_VER_ADV 0x01
#define BLE_VER_FW 0x02
#define BLE_VER_PROTO 0x03
#define BLE_VER_BOOT 0x04
#define BLE_REBOOT 0x84
#define BLE_REBOOT_SYS 0x01

bool ble_connect_state(void);
void ble_cmd_req(uint8_t cmd, uint8_t value);
void ble_uart_poll(void);

#if !EMULATOR
bool ble_is_enable(void);
bool ble_name_state(void);
bool ble_ver_state(void);
bool ble_battery_state(void);
bool ble_charging_state(void);
uint32_t ble_power_button_state(void);
char *ble_get_name(void);
char *ble_get_ver(void);
bool ble_switch_state(void);
void ble_set_switch(bool flag);
bool ble_get_switch(void);
void ble_get_dev_info(void);
#else
#define ble_name_state(...) false
#define ble_ver_state(...) false
#define ble_get_name(...) "OneKey814591011"
#define ble_get_ver(...) "1.0.1"
#define ble_switch_state(...) false
#define ble_set_switch(...)
#define ble_get_switch(...) false
#define change_ble_sta(...)
#endif

#endif
