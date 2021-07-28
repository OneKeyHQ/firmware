#include "device2.h"
#include "atca_api.h"
#include "buttons.h"
#include "chinese.h"
#include "layout2.h"
#include "otp.h"
#include "util.h"
#include "w25qxx.h"

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

  // save ATECC608 configuration version
  char atca_version[8] = {0};
  strlcpy(atca_version, ATCA_CONFIG_VERSION, sizeof(atca_version));
  flash_otp_write(FLASH_OTP_FACTORY_TEST, 0, (uint8_t *)atca_version,
                  sizeof(atca_version));

  flash_otp_lock(FLASH_OTP_FACTORY_TEST);
}
