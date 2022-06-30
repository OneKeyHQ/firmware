#include "device2.h"
#include "atca_api.h"
#include "buttons.h"
#include "chinese.h"
#include "common.h"
#include "layout2.h"
#include "otp.h"
#include "util.h"
#include "w25qxx.h"

void device_test(void) {
  uint8_t key = KEY_NULL;
  uint8_t key_flag = 0x00;
  ATCAConfiguration atca_config = {0};
  bool retest = false;

  bool up_key = (buttonRead() & BTN_PIN_UP) == 0;
  bool down_key = (buttonRead() & BTN_PIN_DOWN) == 0;

  if (up_key && down_key) {
    retest = true;
  }
  if (!retest) {
    if (flash_otp_is_locked(FLASH_OTP_FACTORY_TEST)) {
      return;
    }
  }

  // LCD TEST
  layoutDialogCenterAdapter(NULL, NULL, "否", NULL, "是", NULL, NULL, NULL,
                            "确认屏幕显示正常", NULL, NULL, NULL);
  oledFrame(0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1);
  oledRefresh();

  // wait for release
  if (retest) {
    while (1) {
      if ((buttonRead() & BTN_PIN_UP) && (buttonRead() & BTN_PIN_DOWN)) {
        break;
      }
    }
  }

  key = waitKey(0, 0);
  if (key != KEY_CONFIRM) {
    layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              NULL, "屏幕测试失败", NULL, NULL);
    while (1) {
    }
  }

  // BUTTON TEST
  oledClear();
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 60, "按键测试", FONT_STANDARD);
  oledDrawBitmap(60, 9, &bmp_button_up);
  oledDrawBitmap(60, 110, &bmp_button_down);
  oledDrawBitmap(9, 60, &bmp_button_back);
  oledDrawBitmap(110, 60, &bmp_button_forward);
  oledDrawBitmap(110, 0, &bmp_arrow_up);
  oledRefresh();

  while (1) {
    key = waitKey(0, 0);
    switch (key) {
      case KEY_CONFIRM:
        key_flag |= 0X01;
        oledClearBitmap(110, 60, &bmp_button_forward);
        break;
      case KEY_CANCEL:
        key_flag |= 0X02;
        oledClearBitmap(9, 60, &bmp_button_back);
        break;
      case KEY_DOWN:
        key_flag |= 0X04;
        oledClearBitmap(60, 110, &bmp_button_down);
        break;
      case KEY_UP:
        key_flag |= 0X08;
        oledClearBitmap(60, 9, &bmp_button_up);
        break;
      case KEY_TOP:
        key_flag |= 0X10;
        oledClearBitmap(110, 0, &bmp_arrow_up);
        break;
      default:
        break;
    }
    if (key != KEY_NULL) {
      oledRefresh();
    }
    if (key_flag == 0x1f) {
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

  if (!retest) {
    // save ATECC608 configuration version
    char atca_version[8] = {0};
    strlcpy(atca_version, ATCA_CONFIG_VERSION, sizeof(atca_version));
    ensure_ex(
        flash_otp_write_safe(FLASH_OTP_FACTORY_TEST, 0, (uint8_t *)atca_version,
                             sizeof(atca_version)),
        true, NULL);

    flash_otp_lock(FLASH_OTP_FACTORY_TEST);
  }
}
