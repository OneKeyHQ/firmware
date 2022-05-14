#include "ble.h"
#include "common.h"
#include "display.h"
#include "icon_install.h"
#include "sys.h"
#include "usart.h"

uint8_t battery_cap = 0xFF;

static usart_msg ble_usart_msg;
static bool get_ble_name = false;
static bool get_ble_ver = false;
static bool get_ble_battery = false;
static bool ble_connect = false;
static bool ble_switch = false;
static bool get_ble_switch = false;
static char ble_name[BLE_NAME_LEN + 1] = {0};
static char ble_ver[6] = {0};

static uint8_t calXor(uint8_t *buf, uint32_t len) {
  uint8_t tmp = 0;
  uint32_t i;
  for (i = 0; i < len; i++) {
    tmp ^= buf[i];
  }
  return tmp;
}

static void ble_cmd_packet(uint8_t *value, uint8_t value_len) {
  uint8_t cmd[64] = {0};
  cmd[0] = 0x5a;
  cmd[1] = 0xa5;
  cmd[2] = ((value_len + 1) >> 8) & 0xff;
  cmd[3] = (value_len + 1) & 0xff;
  memcpy(cmd + 4, value, value_len);
  cmd[value_len + 4] = calXor(cmd, value_len + 4);
  ble_usart_send(cmd, value_len + 5);
}

void ble_request_info(uint8_t type) {
  uint8_t cmd[64] = {0};
  cmd[0] = type;
  cmd[1] = 0x01;
  cmd[2] = 0x01;
  ble_cmd_packet(cmd, 3);
}

void ble_ctl_onoff(void) {
  uint8_t cmd[64] = {0};
  cmd[0] = BLE_CMD_ONOFF_BLE;
  cmd[1] = 0x01;
  cmd[2] = 0x03;
  ble_cmd_packet(cmd, 0x03);
}

void change_ble_sta(uint8_t mode) {
  uint8_t cmd[64] = {0};
  cmd[0] = BLE_CMD_ONOFF_BLE;
  cmd[1] = 0x01;
  cmd[2] = mode;
  if (ble_switch != mode) {
    ble_cmd_packet(cmd, 0x03);
    ble_switch = mode;
  }
}

bool ble_connect_state(void) { return ble_connect; }

bool ble_name_state(void) { return get_ble_name; }

bool ble_ver_state(void) { return get_ble_ver; }

bool ble_battery_state(void) { return get_ble_battery; }

bool ble_switch_state(void) { return get_ble_switch; }

char *ble_get_name(void) { return ble_name; }

char *ble_get_ver(void) { return ble_ver; }

void ble_set_switch(bool flag) { ble_switch = flag; }

bool ble_get_switch(void) { return ble_switch; }

void ble_reset(void) {}

void ble_uart_poll(void) {
  static uint8_t read_status = UARTSTATE_IDLE;
  static uint8_t buf[128] = {0};
  uint8_t passkey[7] = {0};
  static uint8_t index = 0;
  volatile uint8_t xor ;
  static bool need_refresh = false;

  if (ble_usart_read(buf + index, 1) != 1) {
    return;
  }

  index++;
  if (index > sizeof(buf)) {
    index = 0;
    read_status = UARTSTATE_IDLE;
    return;
  }

  if (read_status == UARTSTATE_IDLE) {
    if (index == 2 && buf[0] == 0x0B && buf[1] <= 100) {
      uint32_t percent = buf[1];
      if (percent == 99) {
        display_clear();
        display_text_center(DISPLAY_RESX / 2, 346, "BLE update success", -1,
                            FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
      } else {
        display_clear();
        display_text_center(DISPLAY_RESX / 2, 346, "Installing ble...", -1,
                            FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
        display_loader(10 * percent, false, -20, RGB16(0x4A, 0x90, 0xE2),
                       COLOR_WHITE, toi_icon_install, sizeof(toi_icon_install),
                       COLOR_BLACK);
      }
      index = 0;
      return;
    } else {
      if (index >= 2) {
        if ((buf[0] != 0x5A) && ((buf[1] != 0xA5))) {
          index = 0;
          return;
        }
        read_status = UARTSTATE_READ_LEN;
      }
    }
  } else if (read_status == UARTSTATE_READ_LEN) {
    if (index >= 4) {
      ble_usart_msg.len = (buf[2] << 8) + buf[3];
      if (ble_usart_msg.len == 0) {
        index = 0;
        read_status = UARTSTATE_IDLE;
        return;
      }
      read_status = UARTSTATE_READ_DATA;
    }
  } else if (read_status == UARTSTATE_READ_DATA) {
    if (index == ble_usart_msg.len + 4) {
      ble_usart_msg.xor = buf[ble_usart_msg.len + 3];
      xor = calXor(buf, ble_usart_msg.len + 3);
      if (xor != ble_usart_msg.xor) {
        index = 0;
        read_status = UARTSTATE_IDLE;
        return;
      }
      ble_usart_msg.cmd = buf[4];
      ble_usart_msg.cmd_len = buf[5];
      ble_usart_msg.cmd_vale = buf + 6;
      read_status = UARTSTATE_READ_FINISHED;
    }
  }

  if (read_status == UARTSTATE_READ_FINISHED) {
    index = 0;
    read_status = UARTSTATE_IDLE;

    switch (ble_usart_msg.cmd) {
      case BLE_CMD_CONNECT_STATE:
      case BLE_CMD_PAIR_STATE:
        if (ble_usart_msg.cmd_vale[0] == 0x01) {
          ble_connect = true;
        } else {
          ble_connect = false;
        }
        if (need_refresh) {
          need_refresh = false;
        }
        break;
      case BLE_CMD_PASSKEY:
        if (ble_usart_msg.cmd_len == 0x06) {
          memcpy(passkey, ble_usart_msg.cmd_vale, 6);
          display_text_center(DISPLAY_RESX / 2, 346, "Bluetooth passkey:", -1,
                              FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
          display_text_center(DISPLAY_RESX / 2, 370, (char *)passkey, -1,
                              FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
          need_refresh = true;
        }
        break;
      case BLE_CMD_BT_NAME:
        if (ble_usart_msg.cmd_len == BLE_NAME_LEN) {
          memcpy(ble_name, ble_usart_msg.cmd_vale, BLE_NAME_LEN);
          get_ble_name = true;
        }
        break;
      case BLE_CMD_BATTERY:
        get_ble_battery = true;
        if (ble_usart_msg.cmd_vale[0] <= 5)
          battery_cap = ble_usart_msg.cmd_vale[0];
        break;
      case BLE_CMD_VER:
        if (ble_usart_msg.cmd_len == 5) {
          memcpy(ble_ver, ble_usart_msg.cmd_vale, 5);
          get_ble_ver = true;
        }
        break;
      case BLE_CMD_ONOFF_BLE:
        get_ble_switch = true;
        if (ble_usart_msg.cmd_vale[0] == 0) {
          ble_switch = false;
        } else {
          ble_switch = true;
        }
        break;
      default:
        break;
    }
  }
}

void ble_get_dev_info(void) {
  if (!ble_name_state()) {
    ble_request_info(BLE_CMD_BT_NAME);
    hal_delay(5);
  }

  if (!ble_ver_state()) {
    ble_request_info(BLE_CMD_VER);
    hal_delay(5);
  }

  if (!ble_battery_state()) {
    ble_request_info(BLE_CMD_BATTERY);
    hal_delay(5);
  }
}
