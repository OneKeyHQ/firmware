#include "ble.h"
#include "common.h"
#include "display.h"
#include "sys.h"
#include "usart.h"

uint8_t battery_cap = 0xFF;
uint8_t dev_pwr_sta = 0;

static usart_msg ble_usart_msg;
static bool get_ble_name = false;
static bool get_ble_ver = false;
static bool get_ble_proto_ver = false;
static bool get_ble_boot_ver = false;
static bool get_ble_battery = false;
static bool ble_connect = false;
static bool ble_switch = false;
static bool get_ble_switch = false;
static char ble_name[BLE_NAME_LEN + 1] = {0};
static char ble_ver[6] = {0};
static char ble_proto_ver[16 + 1] = {0};
static char ble_boot_ver[6] = {0};
static uint8_t dev_press_sta = 0;
static uint8_t dev_pwr = 0;

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

void ble_cmd_req(uint8_t cmd, uint8_t value) {
  uint8_t buf[64] = {0};
  buf[0] = cmd;
  buf[1] = value;
  ble_cmd_packet(buf, 2);
  hal_delay(10);
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

extern trans_fifo uart_fifo_in;

void ble_uart_poll(void) {
  uint32_t total_len, len;

  uint8_t passkey[7] = {0};
  uint8_t buf[64] = {0};

  total_len = fifo_lockdata_len(&uart_fifo_in);
  if (total_len < 5) {
    return;
  }

  fifo_read_peek(&uart_fifo_in, buf, 4);

  len = (buf[2] << 8) + buf[3];

  fifo_read_lock(&uart_fifo_in, buf, len + 3);

  ble_usart_msg.cmd = buf[4];
  ble_usart_msg.cmd_vale = buf + 5;

  switch (ble_usart_msg.cmd) {
    case BLE_CMD_ADV:
      memcpy(ble_name, ble_usart_msg.cmd_vale, BLE_NAME_LEN);
      get_ble_name = true;
      break;
    case BLE_CMD_CON_STA:
      if (ble_usart_msg.cmd_vale[0] == 0x01) {
        ble_connect = true;
      } else if (ble_usart_msg.cmd_vale[0] == 0x02) {
        ble_connect = false;
      } else if (ble_usart_msg.cmd_vale[0] == 0x03) {
        ble_switch = true;
      } else if (ble_usart_msg.cmd_vale[0] == 0x04) {
        ble_switch = false;
      }
      break;
    case BLE_CMD_PAIR_TX:
      memcpy(passkey, ble_usart_msg.cmd_vale, 6);
      display_text_center(DISPLAY_RESX / 2, 346, "Bluetooth passkey:", -1,
                          FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
      display_text_center(DISPLAY_RESX / 2, 370, (char *)passkey, -1,
                          FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
      break;
    case BLE_CMD_PAIR_STA:
      if (ble_usart_msg.cmd_vale[0] == 0x01) {
        ble_connect = true;
      } else {
        ble_connect = false;
      }
      break;
    case BLE_CMD_FM_VER:
      memcpy(ble_ver, ble_usart_msg.cmd_vale, 5);
      get_ble_ver = true;
      break;
    case BLE_CMD_PROTO_VER:
      memcpy(ble_proto_ver, ble_usart_msg.cmd_vale, 16);
      get_ble_proto_ver = true;
      break;
    case BLE_CMD_BOOT_VER:
      memcpy(ble_boot_ver, ble_usart_msg.cmd_vale, 5);
      get_ble_boot_ver = true;
      break;
    case BLE_CMD_PLUG_STA:
      dev_pwr_sta = ble_usart_msg.cmd_vale[0];
      break;
    case BLE_CMD_EQ:
      get_ble_battery = true;
      battery_cap = ble_usart_msg.cmd_vale[0];
      break;
    case BLE_CMD_RPESS:
      dev_press_sta = ble_usart_msg.cmd_vale[0];
      break;
    case BLE_CMD_PWR:
      dev_pwr = ble_usart_msg.cmd_vale[0];
      break;
    default:
      break;
  }
}

void ble_get_dev_info(void) {
  if (!ble_name_state()) {
    ble_cmd_req(BLE_VER, BLE_VER_ADV);
    hal_delay(5);
  }

  if (!ble_ver_state()) {
    ble_cmd_req(BLE_VER, BLE_VER_FW);
    hal_delay(5);
  }

  if (!ble_battery_state()) {
    ble_cmd_req(BLE_PWR, BLE_PWR_EQ);
    hal_delay(5);
  }
}
