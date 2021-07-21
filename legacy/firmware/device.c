#include "device.h"
#include "otp.h"
#include "util.h"

#include "atca_api.h"
#include "buttons.h"
#include "chinese.h"
#include "layout2.h"
#include "memory.h"
#include "oled.h"
#include "w25qxx.h"

static DeviceSerialNo device_serial_obj;
static DeviceSerialNo *device_serial = &device_serial_obj;
static bool serial_set = false;
static bool factory_mode = false;

void device_init(void) {
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    serial_set = false;
    return;
  }
  memcpy(&device_serial_obj,
         FLASH_PTR(FLASH_OTP_BASE +
                   FLASH_OTP_DEVICE_SERIAL * FLASH_OTP_BLOCK_SIZE),
         sizeof(DeviceSerialNo));
  serial_set = true;
  return;
}

bool device_serial_set(void) { return serial_set; }

void device_set_factory_mode(bool mode) { factory_mode = mode; }

bool device_is_factory_mode(void) { return factory_mode; }

bool device_set_info(DeviceSerialNo *dev_serial) {
  uint8_t buffer[FLASH_OTP_BLOCK_SIZE] = {0};

  if (serial_set) {
    return false;
  }
  // check serial_nome
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    if (check_all_ones(device_serial->product, FLASH_OTP_BLOCK_SIZE)) {
      memcpy(buffer, dev_serial, sizeof(DeviceSerialNo));
      flash_otp_write(FLASH_OTP_DEVICE_SERIAL, 0, buffer, FLASH_OTP_BLOCK_SIZE);
      flash_otp_lock(FLASH_OTP_DEVICE_SERIAL);
      device_init();
      return true;
    }
  }
  return false;
}

bool device_get_serial(char *serial) {
  if (!serial_set) {
    return false;
  }
  memcpy(serial, &device_serial, sizeof(device_serial));
  return true;
}

void device_test(void) {
  uint8_t key = KEY_NULL;
  uint8_t key_flag = 0x00;
  char key_value[] = {"上 下 左 右"};
  ATCAConfiguration atca_config = {0};

  if (flash_otp_is_locked(FLASH_OTP_FACTORY_TEST)) {
    return;
  }

  // LCD TEST
  layoutDialogCenterAdapter(NULL, NULL, "否", NULL, "是", NULL, NULL, NULL,
                            "确认屏幕显示正常", NULL, NULL, NULL);

  key = waitKey(0, 0);
  if (key != KEY_CONFIRM) {
    layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              NULL, "屏幕测试失败", NULL, NULL);
    while (1) {
    }
  }

  // BUTTON TEST
  layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                            "按下所有按键", key_value, NULL, NULL);
  while (1) {
    key = waitKey(0, 0);
    switch (key) {
      case KEY_CONFIRM:
        key_flag |= 0X01;
        memset(key_value + 4 * 3, 0x20, 3);
        break;
      case KEY_CANCEL:
        key_flag |= 0X02;
        memset(key_value + 4 * 2, 0x20, 3);
        break;
      case KEY_DOWN:
        key_flag |= 0X04;
        memset(key_value + 4 * 1, 0x20, 3);
        break;
      case KEY_UP:
        key_flag |= 0X08;
        memset(key_value, 0x20, 3);
        break;
      default:
        break;
    }
    if (key != KEY_NULL) {
      layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                "按下所有按键", key_value, NULL, NULL);
    }
    if (key_flag == 0x0f) {
      break;
    }
  }

  // SE 1-wire
  layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                            "SE通讯测试...", NULL, NULL, NULL);
  if (atca_get_config(&atca_config)) {
    layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              "SE通讯测试失败", NULL, NULL, NULL);
    while (1) {
    }
  }

  // SPI-FLASH
  layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                            "SPI-FLASH通讯测试...", NULL, NULL, NULL);
  if (!w25qxx_init()) {
    layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              "SPI-FLASH通讯测试失败", NULL, NULL, NULL);
    while (1) {
    }
  }

  flash_otp_lock(FLASH_OTP_FACTORY_TEST);
}
