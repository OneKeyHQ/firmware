/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bignum.h"
#include "bitmaps.h"
#include "ble.h"
#include "buttons.h"
#include "chinese.h"
#include "common.h"
#include "config.h"
#include "crypto.h"
#include "font.h"
#include "gettext.h"
#include "layout2.h"
#include "memory.h"
#include "memzero.h"
#include "menu_core.h"
#include "menu_list.h"
#include "messages.h"
#include "nem2.h"
#include "oled.h"
#include "prompt.h"
#include "protect.h"
#include "qrcodegen.h"
#include "recovery.h"
#include "reset.h"
#include "se_chip.h"
#include "secp256k1.h"
#include "signing.h"
#include "sys.h"
#include "timer.h"
#include "util.h"

/* Display info timeout */
uint32_t system_millis_display_info_start = 0;

#if !EMULATOR
static volatile uint8_t charge_dis_timer_counter = 0;
static volatile uint8_t dis_hint_timer_counter = 0;
static uint8_t charge_dis_counter_bak = 0;
static uint8_t cur_level_dis = 0xff;
static uint8_t battery_old = 0xff;
static uint8_t dis_power_flag = 0;
static bool layout_refresh = false;
static bool hide_icon = false;
#endif

#if !EMULATOR
void chargeDisTimer(void) {
  charge_dis_timer_counter =
      charge_dis_timer_counter > 8 ? 0 : charge_dis_timer_counter + 1;

  if ((sys_usbState() == true) && (dis_hint_timer_counter <= 14)) {
    dis_hint_timer_counter++;
  }
}
#endif
#define LOCKTIME_TIMESTAMP_MIN_VALUE 500000000

const char *address_n_str(const uint32_t *address_n, size_t address_n_count,
                          bool address_is_account) {
  (void)address_is_account;
  if (address_n_count > 8) {
    return _("Unknown long path");
  }
  if (address_n_count == 0) {
    return _("Path: m");
  }

  //                  "Path: m"    /    i   '
  static char address_str[7 + 8 * (1 + 10 + 1) + 1];
  char *c = address_str + sizeof(address_str) - 1;

  *c = 0;
  c--;

  for (int n = (int)address_n_count - 1; n >= 0; n--) {
    uint32_t i = address_n[n];
    if (i & PATH_HARDENED) {
      *c = '\'';
      c--;
    }
    i = i & PATH_UNHARDEN_MASK;
    do {
      *c = '0' + (i % 10);
      c--;
      i /= 10;
    } while (i > 0);
    *c = '/';
    c--;
  }
  *c = 'm';

  return c;
}

// split longer string into 6 rows, rowlen chars each
const char **split_message(const uint8_t *msg, uint32_t len, uint32_t rowlen) {
  static char str[6][32 + 1];
  if (rowlen > 32) {
    rowlen = 32;
  }

  memzero(str, sizeof(str));
  for (int i = 0; i < 6; ++i) {
    size_t show_len = strnlen((char *)msg, MIN(rowlen, len));
    memcpy(str[i], (char *)msg, show_len);
    str[i][show_len] = '\0';
    msg += show_len;
    len -= show_len;
  }

  // if (len > 0) {
  //   str[3][rowlen - 1] = '.';
  //   str[3][rowlen - 2] = '.';
  //   str[3][rowlen - 3] = '.';
  //   str[3][rowlen - 4] = '.';
  //   str[3][rowlen - 5] = '.';
  // }
  static const char *ret[6] = {str[0], str[1], str[2], str[3], str[4], str[5]};
  return ret;
}

const char **split_message_hex(const uint8_t *msg, uint32_t len) {
  char hex[32 * 2 + 1] = {0};
  memzero(hex, sizeof(hex));
  uint32_t size = len;
  if (len > 32) {
    size = 32;
  }
  data2hex(msg, size, hex);
  if (len > 32) {
    hex[63] = '.';
    hex[62] = '.';
  }
  return split_message((const uint8_t *)hex, size * 2, 16);
}

void *layoutLast = NULL;

void layoutDialogSwipeWrapping(const BITMAP *icon, const char *btnNo,
                               const char *btnYes, const char *heading,
                               const char *description, const char *wrap_text) {
  const uint32_t row_len = 18;
  const char **str =
      split_message((const uint8_t *)wrap_text, strlen(wrap_text), row_len);
  layoutDialogSwipe(icon, btnNo, btnYes, NULL, heading, description, str[0],
                    str[1], str[2], str[3]);
}

const char **format_tx_message(const char *chain_name) {
  static char str[2][64 + 1];

  memzero(str, sizeof(str));
  snprintf(str[0], 65, "%s %s", chain_name, _("Transaction"));
  snprintf(str[1], 65, "%s%s %s", _("Do you want to sign this\n"), chain_name,
           _("transaction?"));

  static const char *ret[2] = {str[0], str[1]};
  return ret;
}

void layoutDialogSwipe(const BITMAP *icon, const char *btnNo,
                       const char *btnYes, const char *desc, const char *line1,
                       const char *line2, const char *line3, const char *line4,
                       const char *line5, const char *line6) {
  layoutDialogSwipeEx(icon, btnNo, btnYes, desc, line1, line2, line3, line4,
                      line5, line6, FONT_STANDARD);
}

void layoutDialogSwipeEx(const BITMAP *icon, const char *btnNo,
                         const char *btnYes, const char *desc,
                         const char *line1, const char *line2,
                         const char *line3, const char *line4,
                         const char *line5, const char *line6, uint8_t font) {
  layoutLast = layoutDialogSwipe;
  (void)font;
  layoutSwipe();
  layoutDialogAdapter(icon, btnNo, btnYes, desc, line1, line2, line3, line4,
                      line5, line6);
}

void layoutProgressSwipe(const char *desc, int permil) {
  if (layoutLast == layoutProgressSwipe) {
    oledClear_ex();
  } else {
    layoutLast = layoutProgressSwipe;
    layoutSwipe();
  }
  layoutProgressAdapter(desc, permil);
}

void layoutScreensaver(void) {
  if (system_millis_busy_deadline > timer_ms()) {
    // Busy screen overrides the screensaver.
    layoutBusyscreen();
  } else {
    layoutLast = layoutScreensaver;
    oledClear();
    oledRefresh();
  }
}

void layoutLabel(char *label) {
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 16, label, FONT_STANDARD);
}
#if !EMULATOR
void getBleDevInformation(void) {
  if (!ble_name_state()) {
    ble_request_info(BLE_CMD_BT_NAME);
    delay_ms(5);
  }
  if (!ble_ver_state()) {
    ble_request_info(BLE_CMD_VER);
    delay_ms(5);
  }
  if (!ble_battery_state()) {
    ble_request_info(BLE_CMD_BATTERY);
    delay_ms(5);
  }
}
void refreshNfcIcon(bool force_flag) {
  static bool nfc_status_old = false;

  if (sys_nfcState() == true) {
    if (force_flag || false == nfc_status_old) {
      nfc_status_old = true;
      oledDrawBitmap(OLED_WIDTH - 3 * LOGO_WIDTH - 18, 0, &bmp_nfc);
      layout_refresh = true;
    }
  } else if (true == nfc_status_old) {
    nfc_status_old = false;
    oledClearBitmap(OLED_WIDTH - 3 * LOGO_WIDTH - 18, 0, &bmp_nfc);
    layout_refresh = true;
  }
}
uint8_t refreshBleIcon(bool force_flag) {
  static bool ble_conn_status_old = false;
  static bool ble_icon_status_old = false;
  uint8_t ret = 0;

  if (ble_get_switch() == true) {
    if (sys_bleState() == true) {
      if (force_flag || false == ble_conn_status_old) {
        ble_conn_status_old = true;
        oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - 18, 0,
                       &bmp_status_ble_connect);
        layout_refresh = true;
      }
    } else if (force_flag || true == ble_icon_status_old) {
      if (ble_conn_status_old) {
        ble_conn_status_old = false;
        ret = 1;
      }
      oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - 18, 0, &bmp_status_ble);
      layout_refresh = true;
    }
    ble_icon_status_old = true;
  } else if (true == ble_icon_status_old) {
    if (ble_conn_status_old) {
      ble_conn_status_old = false;
      ret = 1;
    }
    ble_icon_status_old = false;
    oledClearBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - 18, 0, &bmp_status_ble);
    layout_refresh = true;
  }
  return ret;
}
void disLongPressBleTips(void) {
  if (change_ble_sta_flag == BUTTON_PRESS_BLE_ON) {
    oledClearPart();
    if (ui_language) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 20, "蓝牙已开启,",
                                  FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, "长按向上键 ^ 可关闭",
                                  FONT_STANDARD);
    } else {
      oledDrawStringCenter(60, 20, "Bluetooth enabled,press", FONT_STANDARD);
      oledDrawStringCenter(60, 30, "and hold up button ^", FONT_STANDARD);
      oledDrawStringCenter(60, 40, "to turn it off.", FONT_STANDARD);
    }

  } else if (change_ble_sta_flag == BUTTON_PRESS_BLE_OFF) {
    oledClearPart();
    if (ui_language) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 20, "蓝牙已关闭,",
                                  FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, "长按向上键 ^ 可开启",
                                  FONT_STANDARD);

    } else {
      oledDrawStringCenter(60, 20, (char *)" Bluetooth disabled,",
                           FONT_STANDARD);
      oledDrawStringCenter(60, 30, "press and hold up button^", FONT_STANDARD);
      oledDrawStringCenter(60, 40, "to turn it on.", FONT_STANDARD);
    }
  }
  if ((change_ble_sta_flag == BUTTON_PRESS_BLE_OFF) ||
      (change_ble_sta_flag == BUTTON_PRESS_BLE_ON)) {
    oledRefresh();
    waitButtonResponse(0, timer1s * 3);
    layoutRefreshSet(true);
  }
  change_ble_sta_flag = 0;
}
void disPcConnectTips(void) {
  if (ui_language) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 20, "数据传输模式只提供",
                                FONT_STANDARD);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, "基本电力,如需充电",
                                FONT_STANDARD);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 44, "建议使用充电头!",
                                FONT_STANDARD);

  } else {
    oledDrawStringCenter(60, 20, "Data Transfer Mode,", FONT_STANDARD);
    oledDrawStringCenter(60, 30, "use a charger if you ", FONT_STANDARD);
    oledDrawStringCenter(60, 40, "wanna faster charging.", FONT_STANDARD);
  }
}
void disPowerChargeTips(void) {
  if (ui_language) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 20, "使用5V-200mA",
                                FONT_STANDARD);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, "以上的充电头",
                                FONT_STANDARD);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 44, "可快速充电!",
                                FONT_STANDARD);

  } else {
    oledDrawStringCenter(60, 20, "Speed up charging with ", FONT_STANDARD);
    oledDrawStringCenter(60, 30, "5V and 200mA+ ", FONT_STANDARD);
    oledDrawStringCenter(60, 40, "charge heads!", FONT_STANDARD);
  }
}
void disUsbConnectTips(void) {
  oledClearPart();
  if (usb_connect_status == 1) {
    disPcConnectTips();
  } else {
    disPowerChargeTips();
  }
}
void refreshBatteryFlash(void) {
  if (charge_dis_counter_bak != charge_dis_timer_counter) {
    charge_dis_counter_bak = charge_dis_timer_counter;
    if (cur_level_dis == 0xff) {
      cur_level_dis = battery_cap;
    }
    disBatteryLevel(cur_level_dis);
    cur_level_dis = cur_level_dis >= 4 ? battery_cap : cur_level_dis + 1;
    layout_refresh = true;
  }
}
void refreshBatteryLevel(uint8_t force_flag) {
  if (battery_old != battery_cap || force_flag) {
    battery_old = battery_cap;
    cur_level_dis = battery_old;
    layout_refresh = true;
    disBatteryLevel(battery_old);
  }
}
void refreshUsbConnectTips(void) {
  if ((dis_power_flag == 0) && (dis_hint_timer_counter == 4)) {
    dis_power_flag = 1;
    disUsbConnectTips();
  }

  if ((dis_power_flag == 1) && (dis_hint_timer_counter == 8)) {
    dis_hint_timer_counter = 15;
    layoutRefreshSet(true);
  }
}
void disUsbConnectSomething(uint8_t force_flag) {
  static bool usb_status_old = false;
  if (sys_usbState() == false) {
    usb_connect_status = 0;
  }
  if (sys_usbState() == true) {
    refreshBatteryFlash();

    if (force_flag || false == usb_status_old) {
      usb_status_old = true;
      oledDrawBitmap(OLED_WIDTH - LOGO_WIDTH - 18, 0, &bmp_status_usb);
      layout_refresh = true;
    }
  } else if (true == usb_status_old) {
    usb_status_old = false;
    oledClearBitmap(OLED_WIDTH - LOGO_WIDTH - 18, 0, &bmp_status_usb);
    layout_refresh = true;
    cur_level_dis = battery_old;
    dis_power_flag = 0;
    dis_hint_timer_counter = 0;
    layoutRefreshSet(true);
  }
}

uint8_t layoutStatusLogoEx(bool need_fresh, bool force_fresh) {
  uint8_t ret = 0;
#if !EMULATOR
  if (hide_icon) return ret;
#endif
  getBleDevInformation();

  refreshNfcIcon(force_fresh);

  ret = refreshBleIcon(force_fresh);

  disLongPressBleTips();

  disUsbConnectSomething(force_fresh);

  refreshBatteryLevel(force_fresh);

  if (need_fresh) {
    if (layout_refresh) oledRefresh();
    layout_refresh = false;
  }

  return ret;
}

#endif

void layout_language_set(uint8_t key) {
  const char *lang[2] = {"en-US", "zh-CN"};
  static int index = 0;

  layoutItemsSelectAdapterEx(NULL, NULL, NULL, &bmp_bottom_right_arrow, NULL,
                             "Next", index + 1, 2, "Select Language", NULL,
                             index == 0 ? "English" : "简体中文", NULL, NULL,
                             index > 0 ? "English" : NULL, NULL, NULL,
                             index == 0 ? "简体中文" : NULL, NULL, NULL, false);

  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
        config_setLanguage(lang[index]);
      }
      break;
    case KEY_DOWN:
      if (index < 1) {
        index++;
        config_setLanguage(lang[index]);
      }
      break;
    case KEY_CONFIRM:
      config_setLanguage(lang[index]);
      return;
    default:
      return;
  }
}

static void layoutWelcome(void) {
  char desc[64] = "";
  int offset = 0;
  int len = 0;

  oledClear_ex();
  layoutHeader(_("Welcome to OneKey!"));
  len = oledStringWidthAdapter(_("Press"), FONT_STANDARD);
  memset(desc, 0, sizeof(desc));
  strcat(desc, _("Press"));
  strcat(desc, "       ");
  strcat(desc, _("key to go back"));
  // English
  if (desc[0] == 'P') {
    offset = 8;
    oledDrawBitmap(offset + len - 1, 19, &bmp_icon_exit);
    oledDrawBitmap(offset + len - 1, 33, &bmp_icon_enter);
  } else {
    offset = 20;
    oledDrawBitmap(offset + len - 1, 20, &bmp_icon_exit);
    oledDrawBitmap(offset + len - 1, 34, &bmp_icon_enter);
  }
  oledDrawStringAdapter(offset, 20, desc, FONT_STANDARD);

  memset(desc, 0, sizeof(desc));

  strcat(desc, _("Press"));
  strcat(desc, "       ");
  strcat(desc, _("key to continue"));
  oledDrawStringAdapter(offset, 34, desc, FONT_STANDARD);

  oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_arrow);
  oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                 &bmp_bottom_right_arrow);

  oledRefresh();
}

static int setup_wallet(uint8_t key) {
  static int index = 0;

  layoutItemsSelectAdapterEx(
      &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down,
      &bmp_bottom_left_arrow, &bmp_bottom_right_arrow, "Pre", "Next", index + 1,
      2, _("Set Up"), NULL,
      index == 0 ? _("Create New Wallet") : _("Import Wallet"), NULL, NULL,
      index > 0 ? _("Create New Wallet") : NULL, NULL, NULL,
      index == 0 ? _("Import Wallet") : NULL, NULL, NULL, false);

  switch (key) {
    case KEY_UP:
      if (index > 0) index--;
      break;
    case KEY_DOWN:
      if (index < 1) index++;
      break;
    case KEY_CONFIRM:
      break;
    default:
      break;
  }
  return index;
}

void onboarding(uint8_t key) {
  int l, pages = 5;
  static int index = 0;
  static int type = 0;
  layoutLast = onboarding;

  switch (index) {
    case 0:
      layout_language_set(key);
      break;
    case 1:
      layoutWelcome();
      break;
    case 2:
      layoutDialogAdapterEx(_("Quick Start"), &bmp_bottom_left_arrow, _("Back"),
                            &bmp_bottom_right_arrow, _("Next"),
                            _("Next, Follow the onscreen\ninstructions to set "
                              "up your\nOneKey Classic."),
                            NULL, NULL, NULL, NULL);
      break;
    // case 3:
    //   layoutDialogAdapterEx(
    //       _("Authenticity Check"), &bmp_bottom_left_arrow, _("Back"),
    //       &bmp_bottom_right_arrow, _("Next"),
    //       _("Want to check authenticity\nof this device? Go to the \nwebsite
    //       "
    //         "below for help:\nonekey.so/auth"),
    //       NULL, NULL, NULL, NULL);
    //   break;
    case 3:
      type = setup_wallet(key);
      break;
    case 4:
      if (0 == type) {
        // Create New Wallet
        reset_on_device();
      } else {
        // Import Wallet
        recovery_on_device();
      }
      if (config_isInitialized()) {
      done1:
        layoutDialogAdapterEx(_("Congratulations!"), NULL, NULL,
                              &bmp_bottom_right_arrow, _("Next"),
                              _("Wallet is ready! Download\nOneKey apps and "
                                "have fun\nwith your OneKey Classic."),
                              NULL, NULL, NULL, NULL);
        while (1) {
          key = protectWaitKey(0, 1);
          if (key == KEY_CONFIRM) {
            break;
          }
        }

      done2:
        layoutDialogAdapterEx(_("Download OneKey Apps"), &bmp_bottom_left_arrow,
                              _("Back"), &bmp_bottom_right_arrow, _("Next"),
                              _("Download OneKey apps at:\nonekey.so/download"),
                              NULL, NULL, NULL, NULL);
        l = oledStringWidthAdapter("onekey.so/download", FONT_STANDARD);
        if (ui_language == 0) {
          oledBox(0, 34, l, 34, true);
          oledRefresh();
        } else {
          oledBox(0, 36, l, 36, true);
          oledRefresh();
        }
        while (1) {
          key = protectWaitKey(0, 1);
          if (key == KEY_CONFIRM) {
            break;
          } else if (key == KEY_CANCEL) {
            goto done1;
          }
        }

        layoutDialogAdapterEx(_("Support"), &bmp_bottom_left_arrow, _("Back"),
                              &bmp_bottom_right_arrow, _("Next"),
                              _("Any questions? Visit Help\nCenter for "
                                "solutions:\nhelp.onekey.so"),
                              NULL, NULL, NULL, NULL);
        l = oledStringWidthAdapter("help.onekey.so", FONT_STANDARD);
        if (ui_language == 0) {
          oledBox(0, 45, l, 45, true);
          oledRefresh();
        } else {
          oledBox(0, 46, l, 46, true);
          oledRefresh();
        }
        while (1) {
          key = protectWaitKey(0, 1);
          if (key == KEY_CONFIRM) {
            break;
          } else if (key == KEY_CANCEL) {
            goto done2;
          }
        }

        layoutDialogAdapterEx(
            _("Done!"), NULL, NULL, &bmp_bottom_right_confirm, _("Next"),
            _("OneKey Classic is set up,\nyou will back to home\nscreen"), NULL,
            NULL, NULL, NULL);
        while (1) {
          key = protectWaitKey(0, 1);
          if (key == KEY_CONFIRM) {
            break;
          }
        }
        index = 0;
        layoutHome();
      } else {
        index = 3;
        layoutLast = onboarding;
        break;
      }
      break;
    default:
      break;
  }

  switch (key) {
    case KEY_UP:
      break;
    case KEY_DOWN:
      break;
    case KEY_CONFIRM:
      if (index < pages - 1) {
        index++;
      }
      break;
    case KEY_CANCEL:
      if (index > 0) {
        index--;
      }
      break;
    default:
      break;
  }
}

static void _layout_home(bool update_menu) {
  if (layoutLast == layoutHome || layoutLast == layoutScreensaver) {
    oledClear_ex();
  } else {
    layoutSwipe();
  }
  layoutLast = layoutHome;

  bool no_backup = false;
  bool unfinished_backup = false;
  bool needs_backup = false;
  bool backup_only = false;
  config_getNoBackup(&no_backup);
  config_getUnfinishedBackup(&unfinished_backup);
  config_getNeedsBackup(&needs_backup);
  backup_only = config_getMnemonicsImported();
  uint8_t homescreen[HOMESCREEN_SIZE] = {0};
  if (config_getHomescreen(homescreen, sizeof(homescreen))) {
    BITMAP b = {0};
    b.width = 128;
    b.height = 64;
    b.data = homescreen;
    oledDrawBitmap(0, 0, &b);
  } else {
    if (backup_only) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 20, _("Backup Mode"),
                                  FONT_STANDARD | FONT_DOUBLE);
      layoutFillBleName(5);
    } else {
      char label[MAX_LABEL_LEN + 1] = "";
      config_getLabel(label, sizeof(label));
      oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                     &bmp_bottom_right_arrow);
      if (session_isUnlocked() || !config_hasPin()) {
        oledDrawBitmap(52, 0, &bmp_onekey_logo);
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 26, label, FONT_STANDARD);
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 10,
                                    ble_get_name(), FONT_STANDARD);
      } else {
        oledDrawBitmap(128 / 2 - 4, 0, &bmp_status_locked);
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 19, label, FONT_STANDARD);
        if (no_backup) {
          oledBox(0, OLED_HEIGHT - 8, 127, 8, false);
          oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 9,
                                      _("SEEDLESS"), FONT_STANDARD);
        } else if (unfinished_backup) {
          oledBox(0, OLED_HEIGHT - 8, 127, 8, false);
          oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 9,
                                      _("BACKUP FAILED!"), FONT_STANDARD);
        } else if (needs_backup) {
          oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 9,
                                      _("Need Backup"), FONT_STANDARD);
        } else {
          oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 10,
                                      ble_get_name(), FONT_STANDARD);
        }
      }
    }
  }

  oledRefresh();

  bool initialized = config_isInitialized() | config_getMnemonicsImported();

  if (update_menu && initialized) {
    main_menu_init(initialized);
  }
  if (!initialized) {
    layoutLast = onboarding;
  }

  // Reset lock screen timeout
  system_millis_lock_start = timer_ms();
}

void layoutBusyscreen(void) {
  if (layoutLast == layoutBusyscreen || layoutLast == layoutScreensaver) {
    oledClear();
  } else {
    layoutSwipe();
  }
  layoutLast = layoutBusyscreen;

  layoutDialog(&bmp_icon_warning, NULL, NULL, NULL, _("Please wait"), NULL,
               _("Coinjoin in progress."), NULL, _("Do not disconnect"),
               _("your OneKey."));
}

void layoutHome(void) {
  // TODO
  // #if !EMULATOR
  static bool first_boot = true;
  if (first_boot && !config_isInitialized() && !se_isFactoryMode()) {
    first_boot = false;
    onboarding(KEY_UP);
  } else
  // #endif
  {
    _layout_home(true);
  }
}

void layoutHomeEx(void) { _layout_home(false); }

static void render_address_dialog(const CoinInfo *coin, const char *address,
                                  const char *line1, const char *line2,
                                  const char *extra_line) {
  if (coin && coin->cashaddr_prefix) {
    /* If this is a cashaddr address, remove the prefix from the
     * string presented to the user
     */
    int prefix_len = strlen(coin->cashaddr_prefix);
    if (strncmp(address, coin->cashaddr_prefix, prefix_len) == 0 &&
        address[prefix_len] == ':') {
      address += prefix_len + 1;
    }
  }
  int addrlen = strlen(address);
  int numlines = addrlen <= 42 ? 2 : 3;
  int linelen = (addrlen - 1) / numlines + 1;
  if (linelen > 21) {
    linelen = 21;
  }
  const char **str = split_message((const uint8_t *)address, addrlen, linelen);
  layoutLast = layoutDialogSwipe;
  layoutSwipe();
  oledClear_ex();
  oledDrawBitmap(0, 0, &bmp_icon_question);
  oledDrawStringAdapter(20, 0 * 9, line1, FONT_STANDARD);
  oledDrawStringAdapter(20, 1 * 9, line2, FONT_STANDARD);
  int left = linelen > 18 ? 0 : 20;
  oledDrawStringAdapter(left, 2 * 9, str[0], FONT_FIXED);
  oledDrawStringAdapter(left, 3 * 9, str[1], FONT_FIXED);
  oledDrawStringAdapter(left, 4 * 9, str[2], FONT_FIXED);
  oledDrawStringAdapter(left, 5 * 9, str[3], FONT_FIXED);
  if (!str[3][0]) {
    if (extra_line) {
      oledDrawStringAdapter(0, 5 * 9, extra_line, FONT_STANDARD);
    } else {
      oledHLine(OLED_HEIGHT - 13);
    }
  }
  layoutButtonNoAdapter(_("Cancel"), &bmp_btn_cancel);
  layoutButtonYesAdapter(_("Confirm"), &bmp_btn_confirm);
  oledRefresh();
}

static size_t format_coin_amount(uint64_t amount, const char *prefix,
                                 const CoinInfo *coin, AmountUnit amount_unit,
                                 char *output, size_t output_len) {
  // " " + (optional "m"/u") + shortcut + ending zero -> 16 should suffice
  char suffix[16];
  memzero(suffix, sizeof(suffix));
  suffix[0] = ' ';
  uint32_t decimals = coin->decimals;
  switch (amount_unit) {
    case AmountUnit_SATOSHI:
      decimals = 0;
      strlcpy(suffix + 1, "sat", sizeof(suffix) - 1);
      if (strcmp(coin->coin_shortcut, "BTC") != 0) {
        strlcpy(suffix + 4, " ", sizeof(suffix) - 4);
        strlcpy(suffix + 5, coin->coin_shortcut, sizeof(suffix) - 5);
      }
      break;
    case AmountUnit_MILLIBITCOIN:
      if (decimals >= 6) {
        decimals -= 6;
        suffix[1] = 'u';
        strlcpy(suffix + 2, coin->coin_shortcut, sizeof(suffix) - 2);
      } else {
        strlcpy(suffix + 1, coin->coin_shortcut, sizeof(suffix) - 1);
      }
      break;
    case AmountUnit_MICROBITCOIN:
      if (decimals >= 3) {
        decimals -= 3;
        suffix[1] = 'm';
        strlcpy(suffix + 2, coin->coin_shortcut, sizeof(suffix) - 2);
      } else {
        strlcpy(suffix + 1, coin->coin_shortcut, sizeof(suffix) - 1);
      }
      break;
    default:  // AmountUnit_BITCOIN
      strlcpy(suffix + 1, coin->coin_shortcut, sizeof(suffix) - 1);
      break;
  }
  return bn_format_amount(amount, prefix, suffix, decimals, output, output_len);
}

bool layoutConfirmOutput(const CoinInfo *coin, AmountUnit amount_unit,
                         const TxOutputType *out) {
  int index = 0;
  uint8_t key = KEY_NULL;
  uint8_t pages = 2;
  char title[32] = {0};
  char str_out[32 + 3] = {0};
  char desc[32] = {0};

  strcat(title, coin->coin_name);
  strcat(title, _("Transaction"));
  strcat(desc, _("Amount"));
  strcat(desc, ":");

  format_coin_amount(out->amount, NULL, coin, amount_unit, str_out,
                     sizeof(str_out) - 3);
  const char *address = out->address;
  const char *extra_line =
      (out->address_n_count > 0)
          ? address_n_str(out->address_n, out->address_n_count, false)
          : 0;
  if (coin && coin->cashaddr_prefix) {
    /* If this is a cashaddr address, remove the prefix from the
     * string presented to the user
     */
    int prefix_len = strlen(coin->cashaddr_prefix);
    if (strncmp(address, coin->cashaddr_prefix, prefix_len) == 0 &&
        address[prefix_len] == ':') {
      address += prefix_len + 1;
    }
  }
  if (extra_line) pages++;

refresh_menu:
  oledClear();
  layoutHeader(title);
  if (0 == index) {
    oledDrawStringAdapter(0, 13, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10, str_out, FONT_STANDARD);
  } else if (1 == index) {
    oledDrawStringAdapter(0, 13, _("Send to:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10, address, FONT_STANDARD);
  } else if (2 == index) {
    oledDrawStringAdapter(0, 13, _("Send to:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10, extra_line, FONT_STANDARD);
  }

  layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      goto refresh_menu;
    case KEY_DOWN:
      goto refresh_menu;
    case KEY_CONFIRM:
      if (index == pages - 1) {
        return true;
      }
      if (index < pages - 1) {
        index++;
      }
      goto refresh_menu;
    case KEY_CANCEL:
      return false;
    default:
      break;
  }

  return true;
}

void layoutConfirmOmni(const uint8_t *data, uint32_t size) {
  const char *desc = NULL;
  char str_out[32] = {0};
  uint32_t tx_type = 0, currency = 0;
  REVERSE32(*(const uint32_t *)(data + 4), tx_type);
  if (tx_type == 0x00000000 && size == 20) {  // OMNI simple send
    desc = _("Simple send of ");
    REVERSE32(*(const uint32_t *)(data + 8), currency);
    const char *suffix = " UNKN";
    bool divisible = false;
    switch (currency) {
      case 1:
        suffix = " OMNI";
        divisible = true;
        break;
      case 2:
        suffix = " tOMNI";
        divisible = true;
        break;
      case 3:
        suffix = " MAID";
        divisible = false;
        break;
      case 31:
        suffix = " USDT";
        divisible = true;
        break;
    }
    uint64_t amount_be = 0, amount = 0;
    memcpy(&amount_be, data + 12, sizeof(uint64_t));
    REVERSE64(amount_be, amount);
    bn_format_amount(amount, NULL, suffix, divisible ? 8 : 0, str_out,
                     sizeof(str_out));
  } else {
    desc = _("Unknown transaction");
    str_out[0] = 0;
  }
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Confirm OMNI Transaction:"), NULL, desc, NULL, str_out,
                    NULL);
}

bool is_valid_ascii(const uint8_t *data, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    if (data[i] < ' ' || data[i] > '~') {
      return false;
    }
  }
  return true;
}

void layoutConfirmOpReturn(const uint8_t *data, uint32_t size) {
  const char **str = NULL;
  if (!is_valid_ascii(data, size)) {
    str = split_message_hex(data, size);
  } else {
    str = split_message(data, size, 20);
  }
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Confirm OP_RETURN:"), str[0], str[1], str[2], str[3],
                    NULL);
}

static bool formatAmountDifference(const CoinInfo *coin, AmountUnit amount_unit,
                                   uint64_t amount1, uint64_t amount2,
                                   char *output, size_t output_length) {
  uint64_t abs_diff = 0;
  const char *sign = NULL;
  if (amount1 >= amount2) {
    abs_diff = amount1 - amount2;
  } else {
    abs_diff = amount2 - amount1;
    sign = "-";
  }

  return format_coin_amount(abs_diff, sign, coin, amount_unit, output,
                            output_length) != 0;
}

// Computes numer / denom and rounds to the nearest integer.
static uint64_t div_round(uint64_t numer, uint64_t denom) {
  return numer / denom + (2 * (numer % denom) >= denom);
}

static bool formatComputedFeeRate(uint64_t fee, uint64_t tx_weight,
                                  char *output, size_t output_length,
                                  bool segwit, bool parentheses) {
  // Convert transaction weight to virtual transaction size, which is defined
  // as tx_weight / 4 rounded up to the next integer.
  // https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki#transaction-size-calculations
  uint64_t tx_size = (tx_weight + 3) / 4;

  // Compute fee rate and modify it in place for the bn_format_amount()
  // function. We multiply by 100, because we want bn_format_amount() to display
  // two decimal digits.
  uint64_t fee_rate_multiplied = div_round(100 * fee, tx_size);

  size_t length =
      bn_format_amount(fee_rate_multiplied, parentheses ? "(" : NULL,
                       segwit ? " sat/vB" : " sat/B", 2, output, output_length);
  if (length == 0) {
    return false;
  }

  if (parentheses) {
    if (length + 2 > output_length) {
      return false;
    }
    output[length] = ')';
    output[length + 1] = '\0';
  }
  return true;
}

static bool formatFeeRate(uint64_t fee_per_kvbyte, char *output,
                          size_t output_length, bool segwit) {
  return formatComputedFeeRate(fee_per_kvbyte, 4000, output, output_length,
                               segwit, false);
}

void layoutConfirmTx(const CoinInfo *coin, AmountUnit amount_unit,
                     uint64_t total_in, uint64_t external_in,
                     uint64_t total_out, uint64_t change_out,
                     uint64_t tx_weight) {
  char str_out[32] = {0};
  formatAmountDifference(coin, amount_unit, total_in, change_out, str_out,
                         sizeof(str_out));

  if (external_in == 0) {
    char str_fee[32] = {0};
    formatAmountDifference(coin, amount_unit, total_in, total_out, str_fee,
                           sizeof(str_fee));

    char str_fee_rate[32] = {0};
    bool show_fee_rate = total_in >= total_out;

    if (show_fee_rate) {
      formatComputedFeeRate(total_in - total_out, tx_weight, str_fee_rate,
                            sizeof(str_fee_rate), coin->has_segwit, true);
    }

    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Confirm sending:"), str_out, _("including fee:"),
                      str_fee, show_fee_rate ? str_fee_rate : NULL, NULL);
  } else {
    char str_spend[32] = {0};
    formatAmountDifference(coin, amount_unit, total_in - external_in,
                           change_out, str_spend, sizeof(str_spend));
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("You are contributing:"), str_spend,
                      _("to the total amount:"), str_out, NULL, NULL);
  }
}

void layoutConfirmReplacement(const char *description, uint8_t txid[32]) {
  const char **str = split_message_hex(txid, 32);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    description, str[0], str[1], str[2], str[3], NULL);
}

void layoutConfirmModifyOutput(const CoinInfo *coin, AmountUnit amount_unit,
                               TxOutputType *out, TxOutputType *orig_out,
                               int page) {
  if (page == 0) {
    render_address_dialog(coin, out->address, _("Modify amount for"),
                          _("address:"), NULL);
  } else {
    char *question = NULL;
    uint64_t amount_change = 0;
    if (orig_out->amount < out->amount) {
      question = _("Increase amount by:");
      amount_change = out->amount - orig_out->amount;
    } else {
      question = _("Decrease amount by:");
      amount_change = orig_out->amount - out->amount;
    }

    char str_amount_change[32] = {0};
    format_coin_amount(amount_change, NULL, coin, amount_unit,
                       str_amount_change, sizeof(str_amount_change));

    char str_amount_new[32] = {0};
    format_coin_amount(out->amount, NULL, coin, amount_unit, str_amount_new,
                       sizeof(str_amount_new));

    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      question, str_amount_change, NULL, _("New amount:"),
                      str_amount_new, NULL);
  }
}

void layoutConfirmModifyFee(const CoinInfo *coin, AmountUnit amount_unit,
                            uint64_t fee_old, uint64_t fee_new,
                            uint64_t tx_weight) {
  char str_fee_change[32] = {0};
  char str_fee_new[32] = {0};
  char *question = NULL;

  uint64_t fee_change = 0;
  if (fee_old < fee_new) {
    question = _("Increase your fee by:");
    fee_change = fee_new - fee_old;
  } else {
    question = _("Decrease your fee by:");
    fee_change = fee_old - fee_new;
  }
  format_coin_amount(fee_change, NULL, coin, amount_unit, str_fee_change,
                     sizeof(str_fee_change));

  format_coin_amount(fee_new, NULL, coin, amount_unit, str_fee_new,
                     sizeof(str_fee_new));

  char str_fee_rate[32] = {0};

  formatComputedFeeRate(fee_new, tx_weight, str_fee_rate, sizeof(str_fee_rate),
                        coin->has_segwit, true);

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    question, str_fee_change, _("Transaction fee:"),
                    str_fee_new, str_fee_rate, NULL);
}

void layoutFeeOverThreshold(const CoinInfo *coin, AmountUnit amount_unit,
                            uint64_t fee) {
  char str_fee[32] = {0};
  format_coin_amount(fee, NULL, coin, amount_unit, str_fee, sizeof(str_fee));
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Fee"), str_fee, _("is unexpectedly high."), NULL,
                    _("Send anyway?"), NULL);
}

void layoutFeeRateOverThreshold(const CoinInfo *coin, uint32_t fee_per_kvbyte) {
  char str_fee_rate[32] = {0};
  formatFeeRate(fee_per_kvbyte, str_fee_rate, sizeof(str_fee_rate),
                coin->has_segwit);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Fee rate"), str_fee_rate, _("is unexpectedly high."),
                    NULL, _("Proceed anyway?"), NULL);
}

void layoutChangeCountOverThreshold(uint32_t change_count) {
  char str_change[21] = {0};
  snprintf(str_change, sizeof(str_change), "There are %" PRIu32, change_count);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Warning!"), str_change, _("change-outputs."), NULL,
                    _("Continue?"), NULL);
}

void layoutConfirmUnverifiedExternalInputs(void) {
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Warning!"), _("The transaction"),
                    _("contains unverified"), _("external inputs."),
                    _("Continue?"), NULL);
}

void layoutConfirmNondefaultLockTime(uint32_t lock_time,
                                     bool lock_time_disabled) {
  if (lock_time_disabled) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Warning!"), _("Locktime is set but"),
                      _("will have no effect."), NULL, _("Continue?"), NULL);

  } else {
    char str_locktime[20] = {0};
    char *str_type = NULL;
    if (lock_time < LOCKTIME_TIMESTAMP_MIN_VALUE) {
      str_type = "blockheight:";
      snprintf(str_locktime, sizeof(str_locktime), "%" PRIu32, lock_time);
    } else {
      str_type = "timestamp (UTC):";
      time_t time = lock_time;
      const struct tm *tm = gmtime(&time);
      strftime(str_locktime, sizeof(str_locktime), "%F %T", tm);
    }

    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Locktime for this"), _("transaction is set to"),
                      _(str_type), str_locktime, _("Continue?"), NULL);
  }
}

void layoutAuthorizeCoinJoin(const CoinInfo *coin, uint64_t max_rounds,
                             uint32_t max_fee_per_kvbyte) {
  char str_max_rounds[32] = {0};
  char str_fee_rate[32] = {0};
  bn_format_amount(max_rounds, NULL, NULL, 0, str_max_rounds,
                   sizeof(str_max_rounds));
  formatFeeRate(max_fee_per_kvbyte, str_fee_rate, sizeof(str_fee_rate),
                coin->has_segwit);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"),
                    _("Authorize coinjoin"), _("Maximum rounds:"),
                    str_max_rounds, _("Maximum mining fee:"), str_fee_rate,
                    NULL, NULL);
}

void layoutConfirmCoinjoinAccess(void) {
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you want to allow"), _("access to your"),
                    _("coinjoin account?"), NULL, NULL, NULL);
}

void layoutVerifyAddress(const CoinInfo *coin, const char *address) {
  render_address_dialog(coin, address, _("Confirm address?"),
                        _("Message signed by:"), 0);
}

void layoutCipherKeyValue(bool encrypt, const char *key) {
  const char **str = split_message((const uint8_t *)key, strlen(key), 16);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"),
                    encrypt ? _("Encrypt value of this key?")
                            : _("Decrypt value of this key?"),
                    str[0], str[1], str[2], str[3], NULL, NULL);
}

void layoutEncryptMessage(const uint8_t *msg, uint32_t len, bool signing) {
  const char **str = split_message(msg, len, 16);
  layoutDialogSwipe(
      &bmp_icon_question, _("Cancel"), _("Confirm"),
      signing ? _("Encrypt+Sign message?") : _("Encrypt message?"), str[0],
      str[1], str[2], str[3], NULL, NULL);
}

void layoutDecryptMessage(const uint8_t *msg, uint32_t len,
                          const char *address) {
  const char **str = split_message(msg, len, 16);
  layoutDialogSwipe(
      &bmp_icon_info, NULL, _("OK"),
      address ? _("Decrypted signed message") : _("Decrypted message"), str[0],
      str[1], str[2], str[3], NULL, NULL);
}

void layoutResetWord(const char *word, int pass, int word_pos, bool last) {
  layoutLast = layoutResetWord;
  layoutSwipe();

  const char *btnYes = NULL;
  if (last) {
    if (pass == 1) {
      btnYes = _("Finish");
    } else {
      btnYes = _("Again");
    }
  } else {
    btnYes = _("Next");
  }

  const char *action = NULL;
  if (pass == 1) {
    action = _("Check the seed");
  } else {
    action = _("Write down");
  }

  char index_str[] = "##th word is:";
  char index_str_zh[] = "第##个:";
  char *index;
  if (ui_language == 0) {
    if (word_pos < 10) {
      index_str[0] = ' ';
    } else {
      index_str[0] = '0' + word_pos / 10;
    }
    index_str[1] = '0' + word_pos % 10;
    if (word_pos == 1 || word_pos == 21) {
      index_str[2] = 's';
      index_str[3] = 't';
    } else if (word_pos == 2 || word_pos == 22) {
      index_str[2] = 'n';
      index_str[3] = 'd';
    } else if (word_pos == 3 || word_pos == 23) {
      index_str[2] = 'r';
      index_str[3] = 'd';
    }
    index = index_str;
  } else {
    if (word_pos < 10) {
      index_str_zh[3] = ' ';
    } else {
      index_str_zh[3] = '0' + word_pos / 10;
    }
    index_str_zh[4] = '0' + word_pos % 10;
    index = index_str_zh;
  }

  int left = 0;
  oledClear_ex();
  oledDrawBitmap(0, 0, &bmp_icon_info);
  left = bmp_icon_info.width + 4;

  oledDrawStringAdapter(left, 0 * 9, action, FONT_STANDARD);
  oledDrawStringAdapter(left, 2 * 9, index, FONT_STANDARD);
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 4 * 9 - 3, word,
                              FONT_FIXED | FONT_DOUBLE);
  // 30 is the maximum pixels used for a pixel row in the BIP39 word "abstract"
  oledSCA(4 * 9 - 3 - 2, 4 * 9 - 3 + 15 + 2, 30);
  oledInvert(0, 4 * 9 - 3 - 2, OLED_WIDTH - 1, 4 * 9 - 3 + 15 + 2);
  layoutButtonYesAdapter(btnYes, &bmp_btn_confirm);
  oledRefresh();
}

static void drawScrollbar(int pages, int index) {
  int i, bar_start = 12, bar_end = 52;
  int bar_heght = 40 - 2 * (pages - 1);
  for (i = bar_start; i < bar_end; i += 2) {  // 40 pixel
    oledDrawPixel(OLED_WIDTH - 1, i);
  }
  for (i = bar_start + 2 * ((int)index);
       i < (bar_start + bar_heght + 2 * ((int)index)) - 1; i++) {
    oledDrawPixel(OLED_WIDTH - 1, i);
    oledDrawPixel(OLED_WIDTH - 2, i);
  }
}

#define QR_MAX_VERSION 9

uint8_t layoutAddress(const char *address, const char *desc, bool qrcode,
                      bool path, bool ignorecase, const uint32_t *address_n,
                      size_t address_n_count, bool address_is_account) {
  if (layoutLast != layoutAddress && layoutLast != layoutXPUBMultisig) {
    layoutSwipe();
  } else {
    oledClear_ex();
  }
  layoutLast = layoutAddress;
  uint8_t key = KEY_NULL;

  uint32_t addrlen = strlen(address);
  if (qrcode) {
    char address_upcase[addrlen + 1];
    memset(address_upcase, 0, sizeof(address_upcase));
    if (ignorecase) {
      for (uint32_t i = 0; i < addrlen + 1; i++) {
        address_upcase[i] = address[i] >= 'a' && address[i] <= 'z'
                                ? address[i] + 'A' - 'a'
                                : address[i];
      }
    }
    uint8_t codedata[qrcodegen_BUFFER_LEN_FOR_VERSION(11)] = {0};
    uint8_t tempdata[qrcodegen_BUFFER_LEN_FOR_VERSION(11)] = {0};

    int side = 0;
    if (qrcodegen_encodeText(ignorecase ? address_upcase : address, tempdata,
                             codedata, qrcodegen_Ecc_LOW, 11, 11,
                             qrcodegen_Mask_AUTO, true)) {
      side = qrcodegen_getSize(codedata);
    }

    oledInvert(33, 1, 95, 63);
    int offset = 32 - (side / 2);
    for (int i = 0; i < side; i++) {
      for (int j = 0; j < side; j++) {
        if (qrcodegen_getModule(codedata, i, j)) {
          oledClearPixel(32 + offset + i, offset + j);
        }
      }
    }
  } else if (path) {
    layoutHeader(desc);
    oledDrawStringAdapter(0, 13, _("Path:"), FONT_STANDARD);
    oledDrawString(
        0, 13 + 10,
        address_n_str(address_n, address_n_count, address_is_account),
        FONT_STANDARD);
  } else {
    uint32_t rowlen = 21;
    int index = 0, rowcount = addrlen / rowlen + 1;
    if (rowcount > 3) {
      const char **str =
          split_message((const uint8_t *)address, addrlen, rowlen);

    refresh_addr:
      oledClear_ex();
      layoutHeader(desc);

      if (0 == index) {
        oledDrawStringAdapter(0, 13, _("Address:"), FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[0], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[2], FONT_STANDARD);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else {
        oledDrawStringAdapter(0, 13, str[index - 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[index], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[index + 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[index + 2], FONT_STANDARD);
        if (index == rowcount - 3) {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
        } else {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
          oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_down);
        }
      }

      // scrollbar
      drawScrollbar(2, index);

      layoutButtonNoAdapter(NULL, &bmp_bottom_left_qrcode);
      layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);

      oledRefresh();
      key = protectWaitKey(0, 0);
      switch (key) {
        case KEY_UP:
          if (index > 0) {
            index--;
          }
          goto refresh_addr;
        case KEY_DOWN:
          if (index < rowcount - 3) {
            index++;
          }
          goto refresh_addr;
        case KEY_CONFIRM:
          if (index == rowcount - 3) {
            return KEY_CONFIRM;
          }
          index++;
          goto refresh_addr;
        case KEY_CANCEL:
          return KEY_CANCEL;
        default:
          break;
      }
      return KEY_NULL;
    } else {
      layoutHeader(desc);
      oledDrawStringAdapter(0, 13, _("Address:"), FONT_STANDARD);
      oledDrawString(0, 13 + 10, address, FONT_STANDARD);
    }
  }

  if (!qrcode) {
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_qrcode);
  } else {
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
  }

  if (path || qrcode) {
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  } else {
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

  return KEY_NULL;
}

void layoutQRCode(const char *index, const BITMAP *bmp_up,
                  const BITMAP *bmp_down, const char *title, const char *text) {
  int y = 0, h = OLED_HEIGHT - 1;

  uint8_t codedata[qrcodegen_BUFFER_LEN_FOR_VERSION(QR_MAX_VERSION)] = {0};
  uint8_t tempdata[qrcodegen_BUFFER_LEN_FOR_VERSION(QR_MAX_VERSION)] = {0};
  uint8_t times = 0;

  int side = 0;
  oledClear_ex();
  oledDrawStringAdapter(0, 0, index, FONT_STANDARD | FONT_FIXED);
  if (bmp_up) {
    oledDrawBitmap(60, y, bmp_up);
    y += 8;
    h -= 8;
  }
  if (bmp_down) {
    oledDrawBitmap(60, OLED_HEIGHT - 8, bmp_down);
    h -= 8;
  }
  if (title) {
    oledDrawStringCenterAdapter(64, y, title, FONT_STANDARD);
    y += 9;
    h -= 9;
  }
  if (qrcodegen_encodeText(text, tempdata, codedata, qrcodegen_Ecc_LOW,
                           qrcodegen_VERSION_MIN, QR_MAX_VERSION,
                           qrcodegen_Mask_AUTO, true)) {
    side = qrcodegen_getSize(codedata);
    times = h / side;
    int x = 64 - times * side / 2;
    y += (h - times * side) / 2;
    oledInvert(x - 1, y - 1, x + side * times, y + side * times);
    for (int i = 0; i < side; i++) {
      for (int j = 0; j < side; j++) {
        if (qrcodegen_getModule(codedata, i, j)) {
          oledBox(x + i * times, y + j * times, x + (i + 1) * times - 1,
                  y + (j + 1) * times - 1, false);
        }
      }
    }
  } else {
    layoutDialogAdapter(NULL, _("Cancel"), _("Confirm"), NULL,
                        _("Generate QR Code fail"), NULL, NULL, NULL, NULL,
                        NULL);
  }
  oledRefresh();
}

void layoutPublicKey(const uint8_t *pubkey) {
  char desc[16] = {0};
  strlcpy(desc, "Public Key: 00", sizeof(desc));
  if (pubkey[0] == 1) {
    /* ed25519 public key */
    // pass - leave 00
  } else {
    data2hex(pubkey, 1, desc + 12);
  }
  const char **str = split_message_hex(pubkey + 1, 32 * 2);
  layoutDialogSwipe(&bmp_icon_question, NULL, _("Continue"), NULL, desc, str[0],
                    str[1], str[2], str[3], NULL);
}

static void _layout_xpub(const char *xpub, const char *desc, int page) {
  // 21 characters per line, 4 lines, minus 3 chars for "..." = 81
  // skip 81 characters per page
  xpub += page * 81;
  const char **str = split_message((const uint8_t *)xpub, strlen(xpub), 21);
  oledDrawString(0, 0 * 9, desc, FONT_STANDARD);
  for (int i = 0; i < 4; i++) {
    oledDrawString(0, (i + 1) * 9 + 4, str[i], FONT_FIXED);
  }
}

bool layoutXPUB(const char *coin_name, const char *xpub,
                const uint32_t *address_n, size_t address_n_count) {
  bool result = false;
  int i, index = 0, sub_index = 0;
  uint8_t key = KEY_NULL;
  uint8_t max_index = 2, max_sub_index = 2;
  char title[64] = {0};
  const char **str = split_message((const uint8_t *)xpub, strlen(xpub), 20);
  if (strlen(xpub) < 80) {
    max_sub_index = 1;
  }
  strcat(title, coin_name);
  strcat(title, _(" Public Key"));

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_PublicKey;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

refresh_menu:
  if (layoutLast != layoutAddress && layoutLast != layoutXPUB) {
    layoutSwipe();
  } else {
    oledClear_ex();
  }
  layoutLast = layoutXPUB;
  if (index == 0) {
    layoutHeader(title);
    for (i = 0; i < 4; i++) {
      if ((i + sub_index * 4) < 6)
        oledDrawString(0, 13 + i * 10, str[i + sub_index * 4], FONT_STANDARD);
    }

    if (max_sub_index > 1 && sub_index == 0) {
      oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                     &bmp_bottom_middle_arrow_down);
    } else if (max_sub_index > 1) {
      oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                     &bmp_bottom_middle_arrow_up);
    }
    drawScrollbar(2, sub_index);

    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (max_index - 1 == index) {
    layoutHeader(title);
    oledDrawStringAdapter(0, 13, _("Path:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10,
                          address_n_str(address_n, address_n_count, false),
                          FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  }
  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (sub_index > 0) {
        sub_index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (sub_index < max_sub_index - 1) {
        sub_index++;
      }
      goto refresh_menu;
    case KEY_CONFIRM:
      if (max_index - 1 == index) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      goto refresh_menu;
    case KEY_CANCEL:
      if (0 == index || max_index == index) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    default:
      break;
  }

  return result;
}

void layoutXPUBMultisig(const char *xpub, int index, int page, bool ours) {
  if (layoutLast != layoutAddress && layoutLast != layoutXPUBMultisig) {
    layoutSwipe();
  } else {
    oledClear();
  }
  layoutLast = layoutXPUBMultisig;
  char desc[] = "XPUB #__ _/2 (________)";
  if (index + 1 >= 10) {
    desc[6] = '0' + (((index + 1) / 10) % 10);
    desc[7] = '0' + ((index + 1) % 10);
  } else {
    desc[6] = '0' + ((index + 1) % 10);
    desc[7] = ' ';
  }
  desc[9] = '1' + page;
  if (ours) {
    desc[14] = 'y';
    desc[15] = 'o';
    desc[16] = 'u';
    desc[17] = 'r';
    desc[18] = 's';
    desc[19] = ')';
    desc[20] = 0;
  } else {
    desc[14] = 'c';
    desc[15] = 'o';
    desc[16] = 's';
    desc[17] = 'i';
    desc[18] = 'g';
    desc[19] = 'n';
    desc[20] = 'e';
    desc[21] = 'r';
  }
  _layout_xpub(xpub, desc, page);
  layoutButtonNoAdapter(_("Next"), NULL);
  layoutButtonYesAdapter(_("Confirm"), &bmp_btn_confirm);
  oledRefresh();
}

void layoutSignIdentity(const IdentityType *identity, const char *challenge) {
  char row_proto[8 + 11 + 1] = {0};
  char row_hostport[64 + 6 + 1] = {0};
  char row_user[64 + 8 + 1] = {0};

  bool is_gpg = (strcmp(identity->proto, "gpg") == 0);

  if (identity->has_proto && identity->proto[0]) {
    if (strcmp(identity->proto, "https") == 0) {
      strlcpy(row_proto, _("Web sign in to:"), sizeof(row_proto));
    } else if (is_gpg) {
      strlcpy(row_proto, _("GPG sign for:"), sizeof(row_proto));
    } else {
      strlcpy(row_proto, identity->proto, sizeof(row_proto));
      char *p = row_proto;
      while (*p) {
        *p = toupper((int)*p);
        p++;
      }
      strlcat(row_proto, _(" login to:"), sizeof(row_proto));
    }
  } else {
    strlcpy(row_proto, _("Login to:"), sizeof(row_proto));
  }

  if (identity->has_host && identity->host[0]) {
    strlcpy(row_hostport, identity->host, sizeof(row_hostport));
    if (identity->has_port && identity->port[0]) {
      strlcat(row_hostport, ":", sizeof(row_hostport));
      strlcat(row_hostport, identity->port, sizeof(row_hostport));
    }
  } else {
    row_hostport[0] = 0;
  }

  if (identity->has_user && identity->user[0]) {
    strlcpy(row_user, _("user: "), sizeof(row_user));
    strlcat(row_user, identity->user, sizeof(row_user));
  } else {
    row_user[0] = 0;
  }

  if (is_gpg) {
    // Split "First Last <first@last.com>" into 2 lines:
    // "First Last"
    // "first@last.com"
    char *email_start = strchr(row_hostport, '<');
    if (email_start) {
      strlcpy(row_user, email_start + 1, sizeof(row_user));
      *email_start = 0;
      char *email_end = strchr(row_user, '>');
      if (email_end) {
        *email_end = 0;
      }
    }
  }

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"),
                    _("Do you want to sign in?"),
                    row_proto[0] ? row_proto : NULL,
                    row_hostport[0] ? row_hostport : NULL,
                    row_user[0] ? row_user : NULL, challenge, NULL, NULL);
}

void layoutDecryptIdentity(const IdentityType *identity) {
  char row_proto[8 + 11 + 1] = {0};
  char row_hostport[64 + 6 + 1] = {0};
  char row_user[64 + 8 + 1] = {0};

  if (identity->has_proto && identity->proto[0]) {
    strlcpy(row_proto, identity->proto, sizeof(row_proto));
    char *p = row_proto;
    while (*p) {
      *p = toupper((int)*p);
      p++;
    }
    strlcat(row_proto, _(" decrypt for:"), sizeof(row_proto));
  } else {
    strlcpy(row_proto, _("Decrypt for:"), sizeof(row_proto));
  }

  if (identity->has_host && identity->host[0]) {
    strlcpy(row_hostport, identity->host, sizeof(row_hostport));
    if (identity->has_port && identity->port[0]) {
      strlcat(row_hostport, ":", sizeof(row_hostport));
      strlcat(row_hostport, identity->port, sizeof(row_hostport));
    }
  } else {
    row_hostport[0] = 0;
  }

  if (identity->has_user && identity->user[0]) {
    strlcpy(row_user, _("user: "), sizeof(row_user));
    strlcat(row_user, identity->user, sizeof(row_user));
  } else {
    row_user[0] = 0;
  }

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"),
                    _("Do you want to decrypt?"),
                    row_proto[0] ? row_proto : NULL,
                    row_hostport[0] ? row_hostport : NULL,
                    row_user[0] ? row_user : NULL, NULL, NULL, NULL);
}

#if U2F_ENABLED

void layoutU2FDialog(const char *verb, const char *appname) {
  layoutDialogAdapter(&bmp_webauthn, _("Reject"), verb, NULL, verb,
                      _("U2F security key?"), NULL, appname, NULL, NULL);
}

#endif

void layoutShowPassphrase(const char *passphrase) {
  if (layoutLast != layoutShowPassphrase) {
    layoutSwipe();
  } else {
    oledClear();
  }

  int l;
  char index_str[16] = "";

  layoutHeader(_("Use this passphrase?"));

  // index
  uint2str(strlen(passphrase), index_str);
  strcat(index_str + strlen(index_str), "/");
  uint2str(50, index_str + strlen(index_str));
  l = oledStringWidthAdapter(index_str, FONT_SMALL);
  oledDrawStringAdapter(OLED_WIDTH / 2 - l / 2, OLED_HEIGHT - 8, index_str,
                        FONT_SMALL);

  oledDrawStringAdapter(0, 13, passphrase, FONT_STANDARD);

  oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_close);
  oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                 &bmp_bottom_right_confirm);

  oledRefresh();
}

#if !BITCOIN_ONLY

void layoutNEMDialog(const BITMAP *icon, const char *btnNo, const char *btnYes,
                     const char *desc, const char *line1, const char *address) {
  static char first_third[NEM_ADDRESS_SIZE / 3 + 1];
  strlcpy(first_third, address, sizeof(first_third));

  static char second_third[NEM_ADDRESS_SIZE / 3 + 1];
  strlcpy(second_third, &address[NEM_ADDRESS_SIZE / 3], sizeof(second_third));

  const char *third_third = &address[NEM_ADDRESS_SIZE * 2 / 3];

  layoutDialogSwipe(icon, btnNo, btnYes, desc, line1, first_third, second_third,
                    third_third, NULL, NULL);
}

void layoutNEMTransferXEM(const char *desc, uint64_t quantity,
                          const bignum256 *multiplier, uint64_t fee) {
  char str_out[32] = {0}, str_fee[32] = {0};

  nem_mosaicFormatAmount(NEM_MOSAIC_DEFINITION_XEM, quantity, multiplier,
                         str_out, sizeof(str_out));
  nem_mosaicFormatAmount(NEM_MOSAIC_DEFINITION_XEM, fee, NULL, str_fee,
                         sizeof(str_fee));

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Next"), desc,
                    _("Confirm transfer of"), str_out, _("and network fee of"),
                    str_fee, NULL, NULL);
}

void layoutNEMNetworkFee(const char *desc, bool confirm, const char *fee1_desc,
                         uint64_t fee1, const char *fee2_desc, uint64_t fee2) {
  char str_fee1[32] = {0}, str_fee2[32] = {0};

  nem_mosaicFormatAmount(NEM_MOSAIC_DEFINITION_XEM, fee1, NULL, str_fee1,
                         sizeof(str_fee1));

  if (fee2_desc) {
    nem_mosaicFormatAmount(NEM_MOSAIC_DEFINITION_XEM, fee2, NULL, str_fee2,
                           sizeof(str_fee2));
  }

  layoutDialogSwipe(
      &bmp_icon_question, _("Cancel"), confirm ? _("Confirm") : _("Next"), desc,
      fee1_desc, str_fee1, fee2_desc, fee2_desc ? str_fee2 : NULL, NULL, NULL);
}

void layoutNEMTransferMosaic(const NEMMosaicDefinition *definition,
                             uint64_t quantity, const bignum256 *multiplier,
                             uint8_t network) {
  char str_out[32] = {0}, str_levy[32] = {0};

  nem_mosaicFormatAmount(definition, quantity, multiplier, str_out,
                         sizeof(str_out));

  if (definition->has_levy) {
    nem_mosaicFormatLevy(definition, quantity, multiplier, network, str_levy,
                         sizeof(str_levy));
  }

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Next"),
                    definition->has_name ? definition->name : _("Mosaic"),
                    _("Confirm transfer of"), str_out,
                    definition->has_levy ? _("and levy of") : NULL,
                    definition->has_levy ? str_levy : NULL, NULL, NULL);
}

void layoutNEMTransferUnknownMosaic(const char *namespace, const char *mosaic,
                                    uint64_t quantity,
                                    const bignum256 *multiplier) {
  char mosaic_name[32] = {0};
  nem_mosaicFormatName(namespace, mosaic, mosaic_name, sizeof(mosaic_name));

  char str_out[32] = {0};
  nem_mosaicFormatAmount(NULL, quantity, multiplier, str_out, sizeof(str_out));

  char *decimal = strchr(str_out, '.');
  if (decimal != NULL) {
    *decimal = '\0';
  }

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("I take the risk"),
                    _("Unknown Mosaic"), _("Confirm transfer of"), str_out,
                    _("raw units of"), mosaic_name, NULL, NULL);
}

void layoutNEMTransferPayload(const uint8_t *payload, size_t length,
                              bool encrypted) {
  if (length >= 1 && payload[0] == 0xFE) {
    char encoded[(length - 1) * 2 + 1];
    memset(encoded, 0, sizeof(encoded));

    data2hex(&payload[1], length - 1, encoded);

    const char **str =
        split_message((uint8_t *)encoded, sizeof(encoded) - 1, 16);
    layoutDialogSwipe(
        &bmp_icon_question, _("Cancel"), _("Next"),
        encrypted ? _("Encrypted hex data") : _("Unencrypted hex data"), str[0],
        str[1], str[2], str[3], NULL, NULL);
  } else {
    const char **str = split_message(payload, length, 16);
    layoutDialogSwipe(
        &bmp_icon_question, _("Cancel"), _("Next"),
        encrypted ? _("Encrypted message") : _("Unencrypted message"), str[0],
        str[1], str[2], str[3], NULL, NULL);
  }
}

void layoutNEMMosaicDescription(const char *description) {
  const char **str =
      split_message((uint8_t *)description, strlen(description), 16);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Next"),
                    _("Mosaic Description"), str[0], str[1], str[2], str[3],
                    NULL, NULL);
}

void layoutNEMLevy(const NEMMosaicDefinition *definition, uint8_t network) {
  const NEMMosaicDefinition *mosaic = NULL;
  if (nem_mosaicMatches(definition, definition->levy_namespace,
                        definition->levy_mosaic, network)) {
    mosaic = definition;
  } else {
    mosaic = nem_mosaicByName(definition->levy_namespace,
                              definition->levy_mosaic, network);
  }

  char mosaic_name[32] = {0};
  if (mosaic == NULL) {
    nem_mosaicFormatName(definition->levy_namespace, definition->levy_mosaic,
                         mosaic_name, sizeof(mosaic_name));
  }

  char str_out[32] = {0};

  switch (definition->levy) {
    case NEMMosaicLevy_MosaicLevy_Percentile:
      bn_format_amount(definition->fee, NULL, NULL, 0, str_out,
                       sizeof(str_out));

      layoutDialogSwipe(
          &bmp_icon_question, _("Cancel"), _("Next"), _("Percentile Levy"),
          _("Raw levy value is"), str_out, _("in"),
          mosaic ? (mosaic == definition ? _("the same mosaic") : mosaic->name)
                 : mosaic_name,
          NULL, NULL);
      break;

    case NEMMosaicLevy_MosaicLevy_Absolute:
    default:
      nem_mosaicFormatAmount(mosaic, definition->fee, NULL, str_out,
                             sizeof(str_out));
      layoutDialogSwipe(
          &bmp_icon_question, _("Cancel"), _("Next"), _("Absolute Levy"),
          _("Levy is"), str_out,
          mosaic ? (mosaic == definition ? _("in the same mosaic") : NULL)
                 : _("in raw units of"),
          mosaic ? NULL : mosaic_name, NULL, NULL);
      break;
  }
}

#endif

static inline bool is_slip18(const uint32_t *address_n,
                             size_t address_n_count) {
  // m / 10018' / [0-9]'
  return address_n_count == 2 && address_n[0] == (PATH_HARDENED + 10018) &&
         (address_n[1] & PATH_HARDENED) &&
         (address_n[1] & PATH_UNHARDEN_MASK) <= 9;
}

void layoutCosiSign(const uint32_t *address_n, size_t address_n_count,
                    const uint8_t *data, uint32_t len) {
  char *desc = _("CoSi sign message?");
  char desc_buf[32] = {0};
  if (is_slip18(address_n, address_n_count)) {
    strlcpy(desc_buf, _("CoSi sign index #?"), sizeof(desc_buf));
    desc_buf[16] = '0' + (address_n[1] & PATH_UNHARDEN_MASK);
    desc = desc_buf;
  }
  char str[4][17] = {0};
  if (len == 32) {
    data2hex(data, 8, str[0]);
    data2hex(data + 8, 8, str[1]);
    data2hex(data + 16, 8, str[2]);
    data2hex(data + 24, 8, str[3]);
  } else {
    strlcpy(str[0], "Data", sizeof(str[0]));
    strlcpy(str[1], "of", sizeof(str[1]));
    strlcpy(str[2], "unsupported", sizeof(str[2]));
    strlcpy(str[3], "length", sizeof(str[3]));
  }
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), desc, str[0],
                    str[1], str[2], str[3], NULL, NULL);
}

void layoutHomeInfo(void) {
  uint8_t key = KEY_NULL;
  key = keyScan();
  if (layoutLast == onboarding) {
    onboarding(key);
  } else {
    layoutEnterSleep();
    if (layoutNeedRefresh()) {
      layoutHome();
    }
    if (layoutLast == layoutHome) {
#if !EMULATOR
      refreshUsbConnectTips();
#endif
      if (key == KEY_UP || key == KEY_DOWN || key == KEY_CONFIRM) {
        if (protectPinOnDevice(true, true)) {
          menu_run(KEY_NULL, 0);
        } else {
          layoutHome();
        }
      }
    } else if (layoutLast == menu_run) {
      menu_run(key, 0);
    }

    // wake from screensaver on any button
    if (layoutLast == layoutScreensaver &&
        (button.NoUp || button.YesUp || button.UpUp || button.DownUp)) {
      layoutHome();
      return;
    }
    if (layoutLast != layoutHome && layoutLast != layoutScreensaver) {
      if (button.NoUp) {
        recovery_abort();
        signing_abort();
      }
    }
  }
}

void layoutDialogSwipeCenterAdapter(const BITMAP *icon, const BITMAP *bmp_no,
                                    const char *btnNo, const BITMAP *bmp_yes,
                                    const char *btnYes, const char *desc,
                                    const char *line1, const char *line2,
                                    const char *line3, const char *line4,
                                    const char *line5, const char *line6) {
  layoutLast = layoutDialogSwipe;
  layoutSwipe();
  layoutDialogCenterAdapter(icon, bmp_no, btnNo, bmp_yes, btnYes, desc, line1,
                            line2, line3, line4, line5, line6);
}

void layoutConfirmAutoLockDelay(uint32_t delay_ms) {
  char line[sizeof("after 4294967296 minutes?")] = {0};

  const char *unit = _("second");
  uint32_t num = delay_ms / 1000U;

  if (delay_ms >= 60 * 60 * 1000) {
    unit = _("hour");
    num /= 60 * 60U;
  } else if (delay_ms >= 60 * 1000) {
    unit = _("minute");
    num /= 60U;
  }

  strlcpy(line, _("after "), sizeof(line));
  size_t off = strlen(line);
  bn_format_amount(num, NULL, NULL, 0, &line[off], sizeof(line) - off);
  strlcat(line, " ", sizeof(line));
  strlcat(line, unit, sizeof(line));
  if (num > 1 && ui_language == 0) {
    strlcat(line, "s", sizeof(line));
  }
  strlcat(line, "?", sizeof(line));
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you really want to"), _("auto-lock your device"),
                    line, NULL, NULL, NULL);
}

void layoutConfirmSafetyChecks(SafetyCheckLevel safety_ckeck_level) {
  if (safety_ckeck_level == SafetyCheckLevel_Strict) {
    // Disallow unsafe actions. This is the default.
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("enforce strict safety"),
                      _("checks?"), _("(Recommended.)"), NULL, NULL);
  } else if (safety_ckeck_level == SafetyCheckLevel_PromptTemporarily) {
    // Ask user before unsafe action. Reverts to Strict after reboot.
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("be temporarily able"),
                      _("to approve some"), _("actions which might"),
                      _("be unsafe?"), NULL);
  }
}

void layoutConfirmHash(const BITMAP *icon, const char *description,
                       const uint8_t *hash, uint32_t len) {
  const char **str = split_message_hex(hash, len);

  layoutSwipe();
  oledClear();
  oledDrawBitmap(0, 0, icon);
  oledDrawString(20, 0 * 9, description, FONT_STANDARD);
  oledDrawString(20, 1 * 9, str[0], FONT_FIXED);
  oledDrawString(20, 2 * 9, str[1], FONT_FIXED);
  oledDrawString(20, 3 * 9, str[2], FONT_FIXED);
  oledDrawString(20, 4 * 9, str[3], FONT_FIXED);
  oledHLine(OLED_HEIGHT - 13);

  layoutButtonNo(_("Cancel"), &bmp_btn_cancel);
  layoutButtonYes(_("Confirm"), &bmp_btn_confirm);
  oledRefresh();
}

void layoutConfirmOwnershipProof(void) {
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you want to"), _("create a proof of"),
                    _("ownership?"), NULL, NULL, NULL);
}

// layout chinese
void layoutButtonNoAdapter(const char *btnNo, const BITMAP *icon) {
  const struct font_desc *font = find_cur_font();
  int icon_width = 0;
  if (!btnNo) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, icon);
    return;
  }
  if (icon) {
    oledDrawBitmap(1, OLED_HEIGHT - 8 - 1, icon);
    icon_width = icon->width;
  }
  oledDrawStringAdapter(3 + icon_width, OLED_HEIGHT - (font->pixel + 1), btnNo,
                        FONT_STANDARD);
  oledInvert(0, OLED_HEIGHT - (font->pixel + 2),
             icon_width + oledStringWidthAdapter(btnNo, FONT_STANDARD) + 4,
             OLED_HEIGHT);
}

void layoutButtonYesAdapter(const char *btnYes, const BITMAP *icon) {
  const struct font_desc *font = find_cur_font();
  int icon_width = 0;
  if (!btnYes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, icon);
    return;
  }
  if (icon) {
    oledDrawBitmap(OLED_WIDTH - 8 - 1, OLED_HEIGHT - 8 - 1, icon);
    icon_width = icon->width;
  }
  oledDrawStringRightAdapter(OLED_WIDTH - icon_width - 3,
                             OLED_HEIGHT - (font->pixel + 1), btnYes,
                             FONT_STANDARD);
  oledInvert(OLED_WIDTH - oledStringWidthAdapter(btnYes, FONT_STANDARD) -
                 icon_width - 4,
             OLED_HEIGHT - (font->pixel + 2), OLED_WIDTH, OLED_HEIGHT);
}

static void _layoutDialogAdapter(const BITMAP *icon, const BITMAP *bmp_no,
                                 const char *btnNo, const BITMAP *bmp_yes,
                                 const char *btnYes, const char *desc,
                                 const char *line1, const char *line2,
                                 const char *line3, const char *line4,
                                 const char *line5, const char *line6,
                                 bool spilt) {
  int left = 0;
  const struct font_desc *font = find_cur_font();

  oledClear_ex();
  if (icon) {
    oledDrawBitmap(0, 0, icon);
    left = icon->width + 4;
  }
  if (line1) {
    oledDrawStringAdapter(left, 0 * (font->pixel + 1), line1, FONT_STANDARD);
  }
  if (line2) {
    oledDrawStringAdapter(left, 1 * (font->pixel + 1), line2, FONT_STANDARD);
  }
  if (line3) {
    if (line2 &&
        (oledStringWidthAdapter(line2, FONT_STANDARD) > (OLED_WIDTH - left))) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * (font->pixel + 1), line3,
                                  FONT_STANDARD);
    } else {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * (font->pixel + 1), line3,
                                  FONT_STANDARD);
    }
  }
  if (line4) {
    oledDrawStringAdapter(0, 3 * (font->pixel + 1), line4, FONT_STANDARD);
  }

  if (desc) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2,
                                OLED_HEIGHT - 2 * (font->pixel + 1) - 1, desc,
                                FONT_STANDARD);
    if (btnYes || btnNo) {
      if (spilt) {
        oledHLine(OLED_HEIGHT - 2 * (font->pixel + 1) - 3);
      }
    }
  } else {
    if (line5) {
      oledDrawStringAdapter(0, 4 * (font->pixel + 1), line5, FONT_STANDARD);
    }
    if (line6) {
      oledDrawStringAdapter(0, 5 * (font->pixel + 1), line6, FONT_STANDARD);
    }
    if (btnYes || btnNo) {
      if (spilt) {
        oledHLine(OLED_HEIGHT - (font->pixel + 4));
      }
    }
  }
  if (btnNo) {
    layoutButtonNoAdapter(btnNo, bmp_no);
  }
  if (btnYes) {
    layoutButtonYesAdapter(btnYes, bmp_yes);
  }
  oledRefresh();
}

void layoutDialogAdapter(const BITMAP *icon, const char *btnNo,
                         const char *btnYes, const char *desc,
                         const char *line1, const char *line2,
                         const char *line3, const char *line4,
                         const char *line5, const char *line6) {
  _layoutDialogAdapter(icon, &bmp_btn_cancel, btnNo, &bmp_btn_confirm, btnYes,
                       desc, line1, line2, line3, line4, line5, line6, true);
}

void layoutDialogAdapter_ex(const BITMAP *icon, const BITMAP *bmp_no,
                            const char *btnNo, const BITMAP *bmp_yes,
                            const char *btnYes, const char *desc,
                            const char *line1, const char *line2,
                            const char *line3, const char *line4,
                            const char *line5, const char *line6) {
  _layoutDialogAdapter(icon, bmp_no, btnNo, bmp_yes, btnYes, desc, line1, line2,
                       line3, line4, line5, line6, false);
}

void layoutDialogCenterAdapter(const BITMAP *icon, const BITMAP *bmp_no,
                               const char *btnNo, const BITMAP *bmp_yes,
                               const char *btnYes, const char *desc,
                               const char *line1, const char *line2,
                               const char *line3, const char *line4,
                               const char *line5, const char *line6) {
  const struct font_desc *font = find_cur_font();

  oledClear_ex();
  if (icon) {
    oledDrawBitmap(56, 2, icon);
  } else {
    if (line1) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 0 * (font->pixel + 1), line1,
                                  FONT_STANDARD);
    }
    if (line2) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 1 * (font->pixel + 1), line2,
                                  FONT_STANDARD);
    }
    if (line3) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * (font->pixel + 1), line3,
                                  FONT_STANDARD);
    }
  }
  if (line4) {
    if (icon && (ui_language == 1)) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * (font->pixel + 1) + 1,
                                  line4, FONT_STANDARD);
    } else if (icon && (ui_language == 0)) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * (font->pixel + 1) + 3,
                                  line4, FONT_STANDARD);
    } else {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * (font->pixel + 1), line4,
                                  FONT_STANDARD);
    }
  }

  if (desc) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 2 * (font->pixel),
                                desc, FONT_STANDARD);
    if (btnYes || btnNo) {
      oledHLine(OLED_HEIGHT - 2 * (font->pixel) - 1);
    }

  } else {
    if (line5) {
      if (icon && (ui_language == 1)) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * (font->pixel + 1) + 1,
                                    line5, FONT_STANDARD);
      } else if (icon && (ui_language == 0)) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * (font->pixel + 1) + 3,
                                    line5, FONT_STANDARD);
      } else {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 4 * (font->pixel + 1) + 1,
                                    line5, FONT_STANDARD);
      }
    }
    if (line6) {
      if (icon && (ui_language == 1)) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 4 * (font->pixel + 1) + 1,
                                    line6, FONT_STANDARD);
      } else if (icon && (ui_language == 0)) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 4 * (font->pixel + 1) + 3,
                                    line6, FONT_STANDARD);
      } else {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 5 * (font->pixel + 1) + 1,
                                    line6, FONT_STANDARD);
      }
    }
  }
  if (btnNo || bmp_no) {
    layoutButtonNoAdapter(btnNo, bmp_no);
  }
  if (btnYes) {
    layoutButtonYesAdapter(btnYes, bmp_yes);
  } else if (bmp_yes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, bmp_yes);
  }

  oledRefresh();
}

void layoutDialogAdapterEx(const char *title, const BITMAP *bmp_no,
                           const char *btnNo, const BITMAP *bmp_yes,
                           const char *btnYes, const char *desc,
                           const char *line1, const char *line2,
                           const char *line3, const char *line4) {
  const struct font_desc *font = find_cur_font();
  int i, len, lines = 0, y = 0;

  oledClear_ex();
  if (title) {
    y = 14;
    if (ui_language) y--;
    layoutHeader(title);
  }

  if (desc) {
    lines = 1;
    len = strlen(desc);
    for (i = 0; i < len; i++) {
      if (desc[i] == '\n') lines++;
    }
    if (lines <= 3) {
      y = 17;
    }
    oledDrawStringCenterAdapter(0, y, desc, FONT_STANDARD);
  } else {
    if (line1) lines++;
    if (line2) lines++;
    if (line3) lines++;
    if (line4) lines++;
    if (lines <= 3) {
      // y = 17; TODO
    }
    if (line1) {
      oledDrawStringAdapter(0, y + 0 * (font->pixel + 1), line1, FONT_STANDARD);
    }
    if (line2) {
      oledDrawStringCenterAdapter(0, y + 1 * (font->pixel + 1), line2,
                                  FONT_STANDARD);
    }
    if (line3) {
      oledDrawStringCenterAdapter(0, y + 2 * (font->pixel + 1), line3,
                                  FONT_STANDARD);
    }

    if (line4) {
      oledDrawStringCenterAdapter(0, y + 3 * (font->pixel + 1), line4,
                                  FONT_STANDARD);
    }
  }

  if (btnNo || bmp_no) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, bmp_no);
  }
  if (btnYes || bmp_yes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, bmp_yes);
  }
  oledRefresh();
}

void layoutProgressAdapter(const char *desc, int permil) {
  oledClear_ex();
  layoutProgressPercent(permil / 10);
  // progressbar
  oledFrame(0, OLED_HEIGHT - 8, OLED_WIDTH - 1, OLED_HEIGHT - 1);
  oledBox(1, OLED_HEIGHT - 7, OLED_WIDTH - 2, OLED_HEIGHT - 2, 0);
  permil = permil * (OLED_WIDTH - 4) / 1000;
  if (permil < 0) {
    permil = 0;
  }
  if (permil > OLED_WIDTH - 4) {
    permil = OLED_WIDTH - 4;
  }
  oledBox(2, OLED_HEIGHT - 6, 1 + permil, OLED_HEIGHT - 3, 1);
  // text
  oledBox(0, OLED_HEIGHT - 16, OLED_WIDTH - 1, OLED_HEIGHT - 16 + 7, 0);
  if (desc) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 24, desc,
                                FONT_STANDARD);
  }
  oledRefresh();
}

void _layout_iterm_select(int x, int y, const BITMAP *bmp, const char *text,
                          uint8_t font, bool vert) {
  int l = 0;
  int y0 = font & FONT_DOUBLE ? 8 : 0;
  oledBox(x - 4, y - 8, x + 4, y + 16 + y0, false);
  oledDrawBitmap(x - 4, y - 7, &bmp_arrow_up_w5);
  l = oledStringWidth(text, font);
  if (bmp) {
    oledDrawBitmap(x - 4, y + 1, bmp);
  } else {
    oledDrawStringAdapter(x - l / 2, y, text, font);
    if (vert) {
      oledInvert(x - l / 2 - 1, y - 1, x + l / 2, y + 8);
      oledClearPixel(x - l / 2 - 1, y - 1);
      oledClearPixel(x - l / 2 - 1, y + 8);
      oledClearPixel(x + l / 2, y - 1);
      oledClearPixel(x + l / 2, y + 8);
    }
  }

  oledDrawBitmap(x - 4, y + 11 + y0, &bmp_arrow_down_w5);
  oledRefresh();
}

void layoutItemsSelect(int x, int y, const char *text, uint8_t font) {
  _layout_iterm_select(x, y, NULL, text, font, false);
}

void layoutItemsSelect_ex(int x, int y, const char *text, uint8_t font,
                          bool vert) {
  _layout_iterm_select(x, y, NULL, text, font, vert);
}

void layoutBmpSelect(int x, int y, const BITMAP *bmp) {
  _layout_iterm_select(x, y, bmp, NULL, FONT_STANDARD, false);
}

void layoutInputPin(uint8_t pos, const char *text, int index,
                    bool cancel_allowed) {
  int l, y = 9;
  char pin_show[9] = "_________";
  char table[][2] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", " "};
  char buf[2] = {0};
  int x = 6;

  layoutLast = layoutInputPin;
  for (uint8_t i = 0; i < pos; i++) {
    pin_show[i] = '*';
  }
  oledClear_ex();
  layoutHeader(text);
  y += 18;

  for (uint32_t i = 0; i < sizeof(pin_show); i++) {
    buf[0] = pin_show[i];
    l = oledStringWidth(buf, FONT_STANDARD);
    if (i < pos) {
      oledDrawBitmap(x + 13 * i + 7 - l / 2, y + 1, &bmp_pin_filled);
    } else {
      oledDrawStringAdapter(x + 13 * i + 7 - l / 2, y, buf, FONT_STANDARD);
    }
  }

  if (index > 0 && index < 10) {
    layoutItemsSelect(x + 13 * pos + 7, y, table[index], FONT_STANDARD);
  } else {
    layoutBmpSelect(x + 13 * pos + 7, y, &bmp_input_submit);
  }

  if (pos == 0) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_close);
  } else if (pos != 0 || cancel_allowed) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_delete);
  }

  if (pos < MAX_PIN_LEN - 1) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                   &bmp_bottom_right_arrow);
  } else {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                   &bmp_bottom_right_confirm);
  }

  oledRefresh();
}

void layoutInputWord(const char *text, uint8_t prefix_len, const char *prefix,
                     const char *letter) {
  int l, y = 9;
  char word_show[8] = "________";
  char buf[2] = {0};
  int x = 25;

  for (uint8_t i = 0; i < prefix_len; i++) {
    word_show[i] = prefix[i];
  }
  oledClear_ex();
  layoutHeader(text);
  y += 18;
  for (uint32_t i = 0; i < sizeof(word_show); i++) {
    buf[0] = word_show[i];
    l = oledStringWidth(buf, FONT_STANDARD);
    oledDrawStringAdapter(x + 9 * i + 7 - l / 2, y, buf, FONT_STANDARD);
  }

  layoutItemsSelect(x + 9 * prefix_len + 7, y, letter, FONT_STANDARD);
  oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_arrow);
  oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                 &bmp_bottom_right_arrow);

  oledRefresh();
}
static char *input[4] = {"abc", "ABC", "123", "=/<"};
static char *input1[4] = {"ab", "AB", "01", "=<"};
static char *inputTitle[4] = {"Switch Input (Lowercase)",
                              "Switch Input (Uppercase)",
                              "Switch Input (Number)", "Switch Input (Symbol)"};

void layoutInputMethod(uint8_t index) {
  layoutItemsSelectAdapterEx(
      &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down,
      &bmp_bottom_left_close, &bmp_bottom_right_confirm, _("Cancel"),
      _("Confirm"), index + 1, 4, _(inputTitle[index]), input[index],
      input[index], NULL, NULL, index > 0 ? input[index - 1] : NULL,
      index > 1 ? input[index - 2] : NULL, index > 2 ? input[index - 3] : NULL,
      index < 4 - 1 ? input[index + 1] : NULL,
      index < 4 - 2 ? input[index + 2] : NULL,
      index < 4 - 3 ? input[index + 3] : NULL, true);
}

void layoutInputPassphrase(const char *text, uint8_t prefix_len,
                           const char *prefix, uint8_t char_index,
                           uint8_t input_type) {
  int l, y = 10;
  char word_show[9] = "_________";
  char buf[2] = {0};
  char index_str[16] = "";
  uint8_t location = 0;

  int x = 6;

  if (prefix_len < 9) {
    memcpy(word_show, prefix, prefix_len);
  } else {
    memcpy(word_show, prefix + prefix_len - 8, 8);
  }

  oledClear_ex();

  layoutHeader(text);

  // index
  uint2str(prefix_len + 1, index_str);
  strcat(index_str + strlen(index_str), "/");
  uint2str(50, index_str + strlen(index_str));
  l = oledStringWidthAdapter(index_str, FONT_SMALL);
  oledDrawStringAdapter(OLED_WIDTH / 2 - l / 2, OLED_HEIGHT - 8, index_str,
                        FONT_SMALL);

  y += 18;
  if (prefix_len < 9) {
    for (uint32_t i = 0; i < sizeof(word_show); i++) {
      buf[0] = word_show[i];
      l = oledStringWidth(buf, FONT_STANDARD);
      oledDrawStringAdapter(x + 13 * i + 7 - l / 2, y, buf, FONT_STANDARD);
    }
  } else {
    oledDrawStringAdapter(0, y, "..", FONT_STANDARD);
    for (uint32_t i = 0; i < sizeof(word_show); i++) {
      buf[0] = word_show[i];
      l = oledStringWidth(buf, FONT_STANDARD);
      oledDrawStringAdapter(x + 13 * i + 7 - l / 2, y, buf, FONT_STANDARD);
    }
  }

  location = prefix_len > 8 ? 8 : prefix_len;
  if (char_index == 0) {
    layoutItemsSelect_ex(x + 13 * location + 7, y, input1[input_type],
                         FONT_STANDARD, true);
    // layoutButtonYesAdapter(_("Switch"), &bmp_btn_switch);
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                   &bmp_bottom_right_change);
  } else if (char_index == 0xFF) {
    layoutBmpSelect(x + 13 * location + 7, y, &bmp_btn_confirm);
    // layoutButtonYesAdapter(_("Submit"), &bmp_btn_confirm);
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                   &bmp_bottom_right_confirm);
  } else {
    buf[0] = char_index;
    layoutItemsSelect_ex(x + 13 * location + 7, y, buf, FONT_STANDARD, false);
    if (prefix_len == (MAX_PASSPHRASE_LEN - 1)) {
      // layoutButtonYesAdapter(_("Submit"), &bmp_btn_confirm);
      oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                     &bmp_bottom_right_confirm);
    } else {
      // layoutButtonYesAdapter(_("Enter"), &bmp_btn_confirm);
      oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                     &bmp_bottom_right_confirm);
    }
  }

  if (prefix_len == 0) {
    // layoutButtonNoAdapter(_("Quit"), &bmp_btn_back);
    oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_close);
  } else {
    // layoutButtonNoAdapter(_("Back"), &bmp_btn_back);
    oledDrawBitmap(1, OLED_HEIGHT - 11, &bmp_bottom_left_delete);
  }

  oledRefresh();
}

void layoutItemsSelectAdapter(const BITMAP *bmp_up, const BITMAP *bmp_down,
                              const BITMAP *bmp_no, const BITMAP *bmp_yes,
                              const char *btnNo, const char *btnYes,
                              uint32_t index, uint32_t count, const char *title,
                              const char *prefex, const char *current,
                              const char *previous, const char *next) {
  int x, l, y, y1;
  int step = 3;
  char index_str[16] = "";
  const struct font_desc *cur_font = find_cur_font();

  y = 0;
  l = 0;

  oledClear_ex();
  if (title) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 0, title, FONT_STANDARD);
    y += cur_font->pixel + 1;
    y++;
    oledHLine(y);
    y += 2;
    y1 = 34;
  } else {
    y1 = 28;
  }

  if (index > 0) {
    uint2str(index, index_str);
    strcat(index_str + strlen(index_str), "/");
    uint2str(count, index_str + strlen(index_str));
    oledDrawStringAdapter(0, 0, index_str, FONT_STANDARD | FONT_FIXED);
  }
  if (previous) {
    oledDrawBitmap(60, y, bmp_up);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, y1 - cur_font->pixel - step,
                                previous, FONT_STANDARD);
  }

  if (prefex) {
    char buf[64] = "";
    strcat(buf, prefex);
    strcat(buf, "   ");
    strcat(buf, current);
    l = oledStringWidthAdapter(buf, FONT_STANDARD);
    x = (OLED_WIDTH - l) / 2;
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, y1, buf, FONT_STANDARD);
  } else {
    l = oledStringWidthAdapter(current, FONT_STANDARD);
    x = (OLED_WIDTH - l) / 2;
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, y1, current, FONT_STANDARD);
  }

  oledInvert(x - 2, y1 - 1, x + l + 1, y1 + cur_font->pixel);
  oledClearPixel(x - 2, y1 - 1);
  oledClearPixel(x - 2, y1 + cur_font->pixel);
  oledClearPixel(x + l + 1, y1 - 1);
  oledClearPixel(x + l + 1, y1 + cur_font->pixel);

  if (next) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, y1 + cur_font->pixel + step,
                                next, FONT_STANDARD);
    oledDrawBitmap(60, OLED_HEIGHT - 8, bmp_down);
  }
  if (btnNo) {
    layoutButtonNoAdapter(btnNo, bmp_no);
  }
  if (btnYes) {
    layoutButtonYesAdapter(btnYes, bmp_yes);
  }

  oledRefresh();
}

void layoutHeader(const char *title) {
#if !EMULATOR
  hide_icon = true;
#endif
  oledBox(0, 0, OLED_WIDTH, 10, false);
  if (0 == ui_language) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2, _(title), FONT_STANDARD);
  } else {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 1, _(title), FONT_STANDARD);
  }
  oledInvert(0, 0, OLED_WIDTH, 10);

  oledBox(0, 0, 2, 2, false);
  oledBox(1, 1, 2, 2, true);
  oledBox(0, 10 - 2, 2, 10, false);
  oledBox(1, 10 - 2, 2, 10 - 1, true);

  oledBox(OLED_WIDTH - 3, 0, OLED_WIDTH - 1, 2, false);
  oledBox(OLED_WIDTH - 3, 1, OLED_WIDTH - 2, 2, true);
  oledBox(OLED_WIDTH - 3, 10 - 2, OLED_WIDTH - 1, 10, false);
  oledBox(OLED_WIDTH - 3, 10 - 3, OLED_WIDTH - 2, 10 - 1, true);
}

void layoutItemsSelectAdapterEx(
    const BITMAP *bmp_up, const BITMAP *bmp_down, const BITMAP *bmp_no,
    const BITMAP *bmp_yes, const char *btnNo, const char *btnYes,
    uint32_t index, uint32_t count, const char *title, const char *input_desc,
    const char *current, const char *name2, const char *param,
    const char *previous, const char *pre_previous,
    const char *pre_pre_previous, const char *next, const char *next_next,
    const char *next_next_next, bool show_index) {
  (void)btnNo;
  (void)btnYes;
  int x, l, y;
  int step = 2;
  char index_str[16] = "";
  const struct font_desc *cur_font = find_cur_font();
  int item_height = 9;
  x = 1;
  y = 1;
  l = 0;
  oledClear_ex();
  if (title) {
    layoutHeader(title);
    step = 4;
    y = 18;
  } else {
#if !EMULATOR
    hide_icon = false;
#endif
    if (cur_font->pixel > 8) {
      y = 9;
    } else {
      y = 10;
    }
  }

  if (count > 4 || (title && count > 3)) {
    // scrollbar
    drawScrollbar(count, index - 1);

    if (pre_previous && (index == count)) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, pre_previous,
                                  FONT_STANDARD);
      y += cur_font->pixel + step;
    }
    if (previous) {
      if (bmp_up) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8, bmp_up);
      }
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, previous, FONT_STANDARD);
      y += cur_font->pixel + step;
    }

    l = oledStringWidthAdapter(input_desc, FONT_STANDARD);
    int x1 = (OLED_WIDTH - l) / 2;
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, input_desc, FONT_STANDARD);

    if (cur_font->pixel > 8) {
      oledInvert(x1 - 4, y - 2, x1 + l + 1, y + item_height + 1);
    } else {
      if (l % 2) {
        oledInvert(x1 - 2, y - 2, x1 + l + 2, y + item_height - 1);
      } else {
        oledInvert(x1 - 3, y - 2, x1 + l + 1, y + item_height - 1);
      }
    }
    y += cur_font->pixel + step;

    if (next) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, next, FONT_STANDARD);
      y += cur_font->pixel + step;
      if (bmp_down) {
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
      }
    }

    if (next_next && (index == 1)) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, next_next, FONT_STANDARD);
      y += cur_font->pixel + step;
    }
  } else {
    if (count == 1) y = 27;
    if (count == 2) y = 21;
    if (count == 3) y = 15;
    // if(0 == ui_language) y++;
    if (pre_pre_previous) {
      if (title) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, pre_pre_previous,
                                    FONT_STANDARD);
      } else {
        oledDrawStringAdapter(x, y, pre_pre_previous, FONT_STANDARD);
      }
      y += item_height + step;
    }
    if (pre_previous) {
      if (title) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, pre_previous,
                                    FONT_STANDARD);
      } else {
        oledDrawStringAdapter(x, y, pre_previous, FONT_STANDARD);
      }
      y += item_height + step;
    }
    if (previous) {
      if (bmp_up) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8, bmp_up);
      }
      if (title) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, previous, FONT_STANDARD);
      } else {
        oledDrawStringAdapter(x, y, previous, FONT_STANDARD);
      }
      y += item_height + step;
    }
    if (title) {
      l = oledStringWidthAdapter(current, FONT_STANDARD);
      int x1 = (OLED_WIDTH - l) / 2;

      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, current, FONT_STANDARD);
      if (cur_font->pixel > 8) {
        oledInvert(x1 - 3, y - 1, x1 + l + 1, y + item_height);
      } else {
        if (is_valid_ascii((const uint8_t *)current, strlen(current))) {
          if (l % 2) {
            oledInvert(x1 - 2, y - 2, x1 + l + 2, y + item_height - 1);
          } else {
            oledInvert(x1 - 3, y - 2, x1 + l + 1, y + item_height - 1);
          }
        } else {
          oledInvert(x1 - 3, y - 2, x1 + l, y + item_height);
        }
      }
      y += item_height + step;
    } else {
      oledDrawStringAdapter(x, y, current, FONT_STANDARD);
      if (param) {
        l = oledStringWidthAdapter(param, FONT_STANDARD);
        oledDrawStringAdapter(OLED_WIDTH - l - 1, y, param, FONT_STANDARD);
      }
      if (name2) {
        l = oledStringWidthAdapter(param, FONT_STANDARD);
        oledDrawStringAdapter(OLED_WIDTH - l - 1, y, param, FONT_STANDARD);
      }
      if (cur_font->pixel > 8) {
        oledInvert(0, y - 1, OLED_WIDTH, y + item_height);
      } else {
        oledInvert(0, y - 2, OLED_WIDTH, y + item_height - 1);
      }
      y += item_height + step;
    }

    if (next) {
      if (title) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, next, FONT_STANDARD);
      } else {
        oledDrawStringAdapter(x, y, next, FONT_STANDARD);
      }
      y += item_height + step;
      if (bmp_down) {
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
      }
    }

    if (next_next) {
      if (title) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, next_next,
                                    FONT_STANDARD);
      } else {
        oledDrawStringAdapter(x, y, next_next, FONT_STANDARD);
      }
      y += item_height + step;
    }
    if (next_next_next) {
      if (title) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, next_next_next,
                                    FONT_STANDARD);
      } else {
        oledDrawStringAdapter(x, y, next_next_next, FONT_STANDARD);
      }
      y += item_height + step;
    }
  }

  if (show_index) {
    uint2str(index, index_str);
    strcat(index_str + strlen(index_str), "/");
    uint2str(count, index_str + strlen(index_str));
    l = oledStringWidthAdapter(index_str, FONT_SMALL);
    oledDrawStringAdapter(OLED_WIDTH / 2 - l / 2, OLED_HEIGHT - 8, index_str,
                          FONT_SMALL);
  }

  if (bmp_no) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, bmp_no);
  }
  if (bmp_yes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, bmp_yes);
  }
  oledRefresh();
}

void layoutItemsSelectAdapterWords(
    const BITMAP *bmp_up, const BITMAP *bmp_down, const BITMAP *bmp_no,
    const BITMAP *bmp_yes, const char *btnNo, const char *btnYes,
    uint32_t index, uint32_t count, const char *title, const char *input_desc,
    const char *current, const char *previous, const char *pre_previous,
    const char *pre_pre_previous, const char *next, const char *next_next,
    const char *next_next_next, bool show_index, bool is_select) {
  (void)btnNo;
  (void)btnYes;
  int x, l, y, p = 0;
  char index_str[16] = "";
  int item_height = 11;
  y = 1;
  l = 0;
  if (ui_language == 1) p = 1;
  oledClear_ex();
  if (title) {
    layoutHeader(title);
    y = 18;
  } else {
#if !EMULATOR
    hide_icon = false;
#endif
    y = 11;
  }

  if (count > 4 || (title && count > 3)) {
    // scrollbar
    drawScrollbar(count, index - 1);

    if (pre_previous && (index == count)) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, pre_previous,
                                  FONT_STANDARD);
      y += item_height;
    }
    if (previous) {
      oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8, bmp_up);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, previous,
                                  FONT_STANDARD);
      y += item_height;
    }

    l = oledStringWidthAdapter(input_desc, FONT_STANDARD);
    x = (OLED_WIDTH - l) / 2;
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, input_desc,
                                FONT_STANDARD);
    if (l % 2) {
      oledInvert(x - 2, y - 2, x + l + 2, y + item_height - 3);
    } else {
      oledInvert(x - 3, y - 2, x + l + 1, y + item_height - 3);
    }
    y += item_height;

    if (next) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, next, FONT_STANDARD);
      y += item_height;
      oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
    }

    if (next_next && (index == 1)) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, next_next,
                                  FONT_STANDARD);
      y += item_height;
    }

  } else {
    if (count == 1) y = 27;
    if (count == 2) y = 21;
    if (count == 3) y = 18;
    if (pre_pre_previous) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, pre_pre_previous,
                                  FONT_STANDARD);
      y += item_height;
    }
    if (pre_previous) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, pre_previous,
                                  FONT_STANDARD);
      y += item_height;
    }
    if (previous) {
      oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8, bmp_up);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, previous,
                                  FONT_STANDARD);
      y += item_height;
    }

    l = oledStringWidthAdapter(current, FONT_STANDARD);
    x = (OLED_WIDTH - l) / 2;
    if (is_select) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, current,
                                  FONT_STANDARD);
      if (l % 2) {
        oledInvert(x - 2, y - 2, x + l + 2, y + item_height - 3);
      } else {
        oledInvert(x - 3, y - 2, x + l + 1, y + item_height - 3);
      }
    } else {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, current,
                                  FONT_STANDARD | FONT_DOUBLE);
    }
    y += item_height;

    if (next) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, next, FONT_STANDARD);
      y += item_height;
      oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
    }
    if (next_next) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, next_next,
                                  FONT_STANDARD);
      y += item_height;
      oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
    }
    if (next_next_next) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, y - p, next_next_next,
                                  FONT_STANDARD);
      y += item_height;
      oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
    }
  }

  if (show_index) {
    uint2str(index, index_str);
    strcat(index_str + strlen(index_str), "/");
    uint2str(count, index_str + strlen(index_str));
    l = oledStringWidthAdapter(index_str, FONT_SMALL);
    oledDrawStringAdapter(OLED_WIDTH / 2 - l / 2, OLED_HEIGHT - 8, index_str,
                          FONT_SMALL);
  }

  if (bmp_no) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, bmp_no);
  }
  if (bmp_yes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, bmp_yes);
  }

  oledRefresh();
}

void layoutWords(const char *title, const BITMAP *bmp_up,
                 const BITMAP *bmp_down, const BITMAP *bmp_no,
                 const BITMAP *bmp_yes, uint32_t index, uint32_t count,
                 const char *word1, const char *word2, const char *word3,
                 const char *word4, const char *word5, const char *word6) {
  char desc[32] = {0};

  oledClear_ex();
  layoutHeader(title);

  // scrollbar
  drawScrollbar(count, index - 1);

  // word1
  memzero(desc, 32);
  uint2str((index - 1) * 6 + 1, desc);
  strcat(desc, ".  ");
  strcat(desc, word1);
  oledDrawString(0, 19, desc, FONT_STANDARD);

  // word2
  memzero(desc, 32);
  uint2str((index - 1) * 6 + 2, desc);
  strcat(desc, ".  ");
  strcat(desc, word2);
  oledDrawString(0, 29, desc, FONT_STANDARD);

  // word3
  memzero(desc, 32);
  uint2str((index - 1) * 6 + 3, desc);
  strcat(desc, ".  ");
  strcat(desc, word3);
  oledDrawString(0, 39, desc, FONT_STANDARD);

  // word4
  memzero(desc, 32);
  uint2str((index - 1) * 6 + 4, desc);
  strcat(desc, ".  ");
  strcat(desc, word4);
  oledDrawString(64, 19, desc, FONT_STANDARD);

  // word5
  memzero(desc, 32);
  uint2str((index - 1) * 6 + 5, desc);
  strcat(desc, ".  ");
  strcat(desc, word5);
  oledDrawString(64, 29, desc, FONT_STANDARD);

  // word6
  memzero(desc, 32);
  uint2str((index - 1) * 6 + 6, desc);
  strcat(desc, ".  ");
  strcat(desc, word6);
  oledDrawString(64, 39, desc, FONT_STANDARD);

  if (bmp_down && (index != count)) {
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8, bmp_down);
  }
  if (bmp_up && (index != 1)) {
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8, bmp_up);
  }

  if (bmp_no) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, bmp_no);
  }
  if (bmp_yes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, bmp_yes);
  }
  oledRefresh();
}

#define DEVICE_INFO_PAGE_NUM 5
extern char bootloader_version[8];

void layouKeyValue(int y, const char *desc, const char *value) {
  oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
  oledDrawStringRightAdapter(OLED_WIDTH - 1, y, value, FONT_STANDARD);
}

bool layoutEraseDevice(void) {
  int index = 0, sub_index = 0, pages = 3, subpages = 2;
  uint8_t key = KEY_NULL;
  char desc[128] = {0};

  oledClear_ex();
  layoutHeader(_("Reset Device"));

refresh_menu:
  oledClear_ex();
  switch (index) {
    case 0:
      sub_index = 0;
      layoutDialogAdapterEx(_("Reset Device"), &bmp_bottom_left_arrow,
                            _("Back"), &bmp_bottom_right_arrow, _("Next"),
                            _("Reset device to factory \ndefault."), NULL, NULL,
                            NULL, NULL);
      break;
    case 1:
      if ((sub_index == 0) && (0 == ui_language)) {
        layoutDialogAdapterEx(
            _("WARNING! (1/2)"), &bmp_bottom_left_arrow, _("Back"),
            &bmp_bottom_right_arrow_off, _("Next"),
            _("This will permanently \nerase all data stored on\nSecure "
              "Element (SE) and \ninternal storage,"),
            NULL, NULL, NULL, NULL);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else if ((sub_index == 1) && (0 == ui_language)) {
        layoutDialogAdapterEx(_("WARNING! (1/2)"), &bmp_bottom_left_arrow,
                              _("Back"), &bmp_bottom_right_arrow, _("Next"),
                              _("including private keys and settings."), NULL,
                              NULL, NULL, NULL);
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        memset(desc, 0, 128);
        strcat(desc,
               _("This will permanently \nerase all data stored on\nSecure "
                 "Element (SE) and \ninternal storage,"));
        strcat(desc, _("including private keys and settings."));
        layoutDialogAdapterEx(_("WARNING! (1/2)"), &bmp_bottom_left_arrow,
                              _("Back"), &bmp_bottom_right_arrow, _("Next"),
                              desc, NULL, NULL, NULL, NULL);
      }
      break;
    case 2:
      if ((sub_index == 0) && (0 == ui_language)) {
        layoutDialogAdapterEx(_("WARNING! (2/2)"), &bmp_bottom_left_arrow,
                              _("Back"), &bmp_bottom_right_arrow_off, _("Next"),
                              _("Recovery Phrase is the \nonly way to restore "
                                "the \nprivate keys that own the\nassets."),
                              NULL, NULL, NULL, NULL);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else if ((sub_index == 1) && (0 == ui_language)) {
        layoutDialogAdapterEx(
            _("WARNING! (2/2)"), &bmp_bottom_left_arrow, _("Back"),
            &bmp_bottom_right_arrow, _("Next"),
            _("Make sure you still have a backup of current wallet."), NULL,
            NULL, NULL, NULL);
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        memset(desc, 0, 128);
        strcat(desc, _("Recovery Phrase is the \nonly way to restore "
                       "the \nprivate keys that own the\nassets."));
        strcat(desc, _("Make sure you still have a backup of current wallet."));
        layoutDialogAdapterEx(_("WARNING! (2/2)"), &bmp_bottom_left_arrow,
                              _("Back"), &bmp_bottom_right_arrow, _("Next"),
                              desc, NULL, NULL, NULL, NULL);
      }
      break;
    default:
      break;
  }

  // scrollbar
  if ((index == 1 || index == 2) && (0 == ui_language)) {
    drawScrollbar(2, sub_index);
  }

  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (sub_index > 0) {
        sub_index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (sub_index < subpages - 1) {
        sub_index++;
      }
      goto refresh_menu;
    case KEY_CONFIRM:
      if ((0 == ui_language) && (((index == 1) && (sub_index == 0)) ||
                                 ((index == 2) && (sub_index == 0)))) {
        sub_index++;
        goto refresh_menu;
      }
      sub_index = 0;
      if (index == pages - 1) return true;
      if (index < pages - 1) {
        index++;
      }
      goto refresh_menu;
    case KEY_CANCEL:
      sub_index = 0;
      if (index == 0) return false;
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    default:
      break;
  }
  return false;
}

void layoutDeviceParameters(int num) {
  (void)num;
  const struct font_desc *font = find_cur_font();
  char *se_version = NULL;
  char *se_sn = NULL;
  int y = 0, l;
  int index = 0;
  uint8_t key = KEY_NULL;
  char index_str[16] = "";

refresh_menu:
  y = 9;
  oledClear_ex();

  // index
  uint2str(index + 1, index_str);
  strcat(index_str + strlen(index_str), "/");
  uint2str(DEVICE_INFO_PAGE_NUM, index_str + strlen(index_str));
  l = oledStringWidthAdapter(index_str, FONT_SMALL);
  oledDrawStringAdapter(OLED_WIDTH / 2 - l / 2, OLED_HEIGHT - 8, index_str,
                        FONT_SMALL);

  switch (index) {
    case 0:

      oledDrawStringAdapter(0, y, _("MODEL:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, "OneKey Classic", FONT_STANDARD);
      y += font->pixel + 4;

      oledDrawStringAdapter(0, y, _("BLUETOOTH NAME:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, ble_get_name(), FONT_STANDARD);

      break;
    case 1:

      oledDrawStringAdapter(0, y, _("FIRMWARE VERSION:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, ONEKEY_VERSION, FONT_STANDARD);
      y += font->pixel + 4;

      oledDrawStringAdapter(0, y, _("BLUETOOTH VERSION:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, ble_get_ver(), FONT_STANDARD);
      break;

    case 2:
      se_version = se_get_version();
      oledDrawStringAdapter(0, y, _("SE VERSION:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, se_version, FONT_STANDARD);
      y += font->pixel + 4;

#if !EMULATOR
      oledDrawStringAdapter(0, y, _("BOOTLOADER:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, bootloader_version, FONT_STANDARD);
      y += font->pixel + 1;
#endif
      break;

    case 3:
      oledDrawStringAdapter(0, y, _("SERIAL NUMBER:"), FONT_STANDARD);
      y += font->pixel + 1;

      se_get_sn(&se_sn);
      oledDrawStringAdapter(0, y, se_sn, FONT_STANDARD);

      y += font->pixel + 4;
      oledDrawStringAdapter(0, y, _("BUILD ID:"), FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, BUILD_ID + strlen(BUILD_ID) - 7,
                            FONT_STANDARD);
      break;
    case 4:
      oledDrawStringAdapter(0, y, _("DEVICE ID:"), FONT_STANDARD);
      y += font->pixel + 1;

      // split uuid
      char uuid1[32] = {0};
      char uuid2[32] = {0};

      for (int i = 0; i < 2 * UUID_SIZE; i++) {
        uuid1[i] = config_uuid_str[i];
        if (oledStringWidthAdapter(uuid1, FONT_STANDARD) > OLED_WIDTH) {
          uuid1[i] = 0;
          strcat(uuid2, config_uuid_str + i);
          break;
        }
      }

      oledDrawStringAdapter(0, y, uuid1, FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, uuid2, FONT_STANDARD);
      break;
      break;
    default:
      break;
  }

  if (index == 0) {
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_down);
  } else if (index == 4) {
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_up);
  } else {
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_down);
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_up);
  }
  oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                 &bmp_bottom_right_confirm);

  drawScrollbar(5, index);

  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (index < 4) {
        index++;
      }
      goto refresh_menu;
    case KEY_CONFIRM:
      return;
    case KEY_CANCEL:
      goto refresh_menu;
    default:
      return;
  }
}

void layoutAboutCertifications(int num) {
  (void)num;
  int l;
  int index = 0;
  uint8_t key = KEY_NULL;
  char index_str[16] = "";

refresh_menu:
  oledClear_ex();

  // index
  uint2str(index + 1, index_str);
  strcat(index_str + strlen(index_str), "/");
  uint2str(3, index_str + strlen(index_str));
  l = oledStringWidthAdapter(index_str, FONT_SMALL);
  oledDrawStringAdapter(OLED_WIDTH / 2 - l / 2, OLED_HEIGHT - 8, index_str,
                        FONT_SMALL);

  switch (index) {
    case 0:
      oledDrawBitmap((OLED_WIDTH - bmp_Icon_fc.width) / 2, 4, &bmp_Icon_fc);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 40, "FCC ID:2AV5MBIXINKEY1",
                                  FONT_STANDARD);
      break;
    case 1:
      oledDrawBitmap((OLED_WIDTH - bmp_Icon_bc.width) / 2, 12, &bmp_Icon_bc);
      break;
    case 2:
      oledDrawBitmap(20, 12, &bmp_Icon_ce);
      oledDrawBitmap(72, 12, &bmp_Icon_weee);
      break;
    default:
      break;
  }

  if (index == 0) {
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_down);
  } else if (index == 2) {
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_up);
  } else {
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_down);
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_up);
  }
  oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11,
                 &bmp_bottom_right_confirm);

  // scrollbar
  drawScrollbar(3, index);

  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (index < 2) {
        index++;
      }
      goto refresh_menu;
    case KEY_CONFIRM:
      return;
    case KEY_CANCEL:
      goto refresh_menu;
    default:
      return;
  }
}

void layoutEnterSleep(void) {
#if !EMULATOR
  static uint32_t system_millis_logo_refresh = 0;

  if (config_getSleepDelayMs() > 0) {
    if (timer_get_sleep_count() >= config_getSleepDelayMs()) {
      enter_sleep();
    }
  }
  if (layoutLast != layoutScreensaver) {
    // 1000 ms refresh
    if ((timer_ms() - system_millis_logo_refresh) >= 1000) {
      layoutStatusLogoEx(true, false);
      system_millis_logo_refresh = timer_ms();
    }
  }

#else
  if ((timer_ms() - system_millis_lock_start) >= config_getAutoLockDelayMs()) {
    config_lockDevice();
    layoutScreensaver();
  }
#endif
}

void layoutScroollbarButtonYesAdapter(const char *btnYes, const BITMAP *icon) {
  const struct font_desc *font = find_cur_font();
  int icon_width = 0;
  if (icon) {
    oledDrawBitmap(OLED_WIDTH - 8 - 4, OLED_HEIGHT - 8 - 1, icon);
    icon_width = icon->width;
  }
  oledDrawStringRightAdapter(OLED_WIDTH - icon_width - 6,
                             OLED_HEIGHT - (font->pixel + 1), btnYes,
                             FONT_STANDARD);
  oledInvert(OLED_WIDTH - oledStringWidthAdapter(btnYes, FONT_STANDARD) -
                 icon_width - 7,
             OLED_HEIGHT - (font->pixel + 2), OLED_WIDTH - 4, OLED_HEIGHT);
}

bool layoutTransactionSign(const char *chain_name, bool token_transfer,
                           const char *amount, const char *to_str,
                           const char *signer, const char *recipient,
                           const char *token_id, const uint8_t *data,
                           uint16_t len, const char *key1, const char *value1,
                           const char *key2, const char *value2,
                           const char *key3, const char *value3,
                           const char *key4, const char *value4) {
  bool result = false;
  int index = 0, sub_index = 0;
  int i, y = 0, bar_heght, bar_start = 12, bar_end = 52;
  uint8_t key = KEY_NULL;
  uint8_t max_index = 4;
  char desc[64] = {0};
  char title[32] = {0};
  char title_data[32] = {0};
  char lines[21] = {0};
  int data_rowcount = len % 10 ? len / 10 + 1 : len / 10;
  uint32_t rowlen = 21;
  const char **str;
  uint32_t tokenid_len = strlen(token_id);
  int token_id_rowcount = tokenid_len / rowlen + 1;
  const char **tx_msg = format_tx_message(chain_name);
  if (token_transfer && token_id == NULL) {
    strcat(title, _("Token Transfer"));
  } else {
    strcat(title, chain_name);
    strcat(title, " ");
    strcat(title, _("Transaction"));
  }
  strcat(title_data, _("View Data"));
  strcat(title_data, "(");
  uint2str(len, title_data + strlen(title_data));
  strcat(title_data, ")");

  if (len > 0) max_index++;
  if (key1) max_index++;
  if (key2) max_index++;
  if (key3) max_index++;
  if (key4) max_index++;
  if (token_transfer && token_id) max_index += 2;  // nft transfer

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

refresh_menu:
  layoutSwipe();
  oledClear();
  y = 13;
  if (0 == index) {
    sub_index = 0;
    layoutHeader(title);
    memset(desc, 0, 64);
    strcat(desc, _("Amount"));
    strcat(desc, ":");
    oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, amount, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (1 == index && token_transfer &&
             token_id) {  // nft contract address
    sub_index = 0;
    layoutHeader(title);
    oledDrawStringAdapter(0, y, _("Token Contract:"), FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, to_str, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (2 == index && token_transfer && token_id) {  // nft token id
    layoutHeader(title);

    if (strlen(token_id) > 3) {
      str = split_message((const uint8_t *)token_id, tokenid_len, rowlen);
      if (0 == sub_index) {
        oledDrawStringAdapter(0, 13, _("Token ID:"), FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[0], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[2], FONT_STANDARD);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else {
        oledDrawStringAdapter(0, 13, str[sub_index - 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[sub_index], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[sub_index + 1],
                              FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[sub_index + 2],
                              FONT_STANDARD);
        if (sub_index == token_id_rowcount - 3) {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
        } else {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
          oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_down);
        }
      }

      // scrollbar
      drawScrollbar(2, sub_index);

    } else {
      oledDrawStringAdapter(0, y, _("Token ID:"), FONT_STANDARD);
      oledDrawStringAdapter(0, y + 10, token_id, FONT_STANDARD);
    }

    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (3 == index && token_transfer && token_id) {  // nft recipient
    sub_index = 0;
    layoutHeader(title);
    oledDrawStringAdapter(0, y, _("Send to:"), FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, recipient, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if ((1 == index && token_id == NULL)) {  // To
    sub_index = 0;
    layoutHeader(title);
    oledDrawStringAdapter(0, y, _("Send to:"), FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, to_str, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if ((2 == index && token_id == NULL) ||
             (4 == index && token_id)) {  // From
    sub_index = 0;
    layoutHeader(title);
    memset(desc, 0, 64);
    strcat(desc, _("From"));
    strcat(desc, ":");
    oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, signer, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if ((3 == index && len > 0 && token_id == NULL) ||
             (5 == index && len > 0 && token_id)) {  // data
    layoutHeader(title_data);
    if (data_rowcount > 4) {
      data2hexaddr(data + 10 * (sub_index), 10, lines);
      oledDrawStringAdapter(0, 13, lines, FONT_STANDARD);
      data2hexaddr(data + 10 * (sub_index + 1), 10, lines);
      oledDrawStringAdapter(0, 13 + 1 * 10, lines, FONT_STANDARD);
      data2hexaddr(data + 10 * (sub_index + 2), 10, lines);
      oledDrawStringAdapter(0, 13 + 2 * 10, lines, FONT_STANDARD);
      if (sub_index == data_rowcount - 4) {
        if (len % 10) {
          data2hexaddr(data + 10 * (sub_index + 3), len % 10, lines);
        } else {
          data2hexaddr(data + 10 * (sub_index + 3), 10, lines);
        }
      } else {
        data2hexaddr(data + 10 * (sub_index + 3), 10, lines);
      }
      oledDrawStringAdapter(0, 13 + 3 * 10, lines, FONT_STANDARD);

      // scrollbar
      bar_heght = 40 - 2 * (data_rowcount - 5);
      if (bar_heght < 6) bar_heght = 6;
      for (i = bar_start; i < bar_end; i += 2) {  // 40 pixel
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      if (sub_index <= 18) {
        for (i = bar_start + 2 * ((int)sub_index);
             i < (bar_start + bar_heght + 2 * ((int)sub_index - 1)) - 1; i++) {
          oledDrawPixel(OLED_WIDTH - 1, i);
          oledDrawPixel(OLED_WIDTH - 2, i);
        }
      } else {
        for (i = bar_start + 2 * 18;
             i < (bar_start + bar_heght + 2 * (18 - 1)) - 1; i++) {
          oledDrawPixel(OLED_WIDTH - 1, i);
          oledDrawPixel(OLED_WIDTH - 2, i);
        }
      }

      if (sub_index == 0) {
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else if (sub_index == data_rowcount - 4) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      }
    } else {
      char buf[90] = {0};
      data2hexaddr(data, len, buf);
      oledDrawStringAdapter(0, 13, buf, FONT_STANDARD);
    }

    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_next);
  } else if (max_index - 1 == index) {
    sub_index = 0;
    layoutHeader(_("Sign Transaction"));
    oledDrawStringAdapter(0, y, tx_msg[1], FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  } else {  // key*
    sub_index = 0;
    layoutHeader(title);
    if ((3 == index && len == 0 && !token_id) ||
        (4 == index && len > 0 && !token_id) || (5 == index && token_id)) {
      oledDrawStringAdapter(0, y, key1, FONT_STANDARD);
      oledDrawStringAdapter(0, y + 10, value1, FONT_STANDARD);
    } else if ((4 == index && len == 0 && !token_id) ||
               (5 == index && len > 0 && !token_id) ||
               (6 == index && token_id)) {
      oledDrawStringAdapter(0, y, key2, FONT_STANDARD);
      oledDrawStringAdapter(0, y + 10, value2, FONT_STANDARD);
    } else if ((5 == index && len == 0 && !token_id) ||
               (6 == index && len > 0 && !token_id) ||
               (7 == index && token_id)) {
      oledDrawStringAdapter(0, y, key3, FONT_STANDARD);
      oledDrawStringAdapter(0, y + 10, value3, FONT_STANDARD);
    } else if ((6 == index && len == 0 && !token_id) ||
               (7 == index && len > 0 && !token_id) ||
               (8 == index && token_id)) {
      oledDrawStringAdapter(0, y, key4, FONT_STANDARD);
      oledDrawStringAdapter(0, y + 10, value4, FONT_STANDARD);
    }
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (sub_index > 0) {
        sub_index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (len > 0 && index == 3 && sub_index < data_rowcount - 4) {
        sub_index++;
      }
      if (token_transfer && token_id && index == 2 &&
          sub_index < token_id_rowcount - 3) {  // token_id
        sub_index++;
      }
      goto refresh_menu;
    case KEY_CONFIRM:
      if (index == max_index - 1) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      goto refresh_menu;
    case KEY_CANCEL:
      if ((0 == index) || (index == max_index - 1)) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    default:
      break;
  }

  return result;
}

bool layoutBlindSign(const char *chain_name, bool is_contract,
                     const char *contract_address, const char *address,
                     const uint8_t *data, uint16_t len, const char *key1,
                     const char *value1, const char *key2, const char *value2,
                     const char *key3, const char *value3) {
  bool result = false;
  int index = 0, sub_index = 0;
  int i, bar_heght, bar_start = 12, bar_end = 52;
  uint8_t max_index = 4;
  uint8_t key = KEY_NULL;
  char title_data[32] = {0};
  char desc[64] = {0};
  char lines[21] = {0};
  uint32_t rowlen = 21;
  const char **str;
  uint32_t addrlen = strlen(address);
  int address_rowcount = addrlen / rowlen + 1;
  int data_rowcount = len % 10 ? len / 10 + 1 : len / 10;
  const char **tx_msg = format_tx_message(chain_name);
  if (key1) max_index++;
  if (key2) max_index++;
  if (key3) max_index++;

  strcat(title_data, _("View Data"));
  strcat(title_data, "(");
  uint2str(len, title_data + strlen(title_data));
  strcat(title_data, ")");

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

refresh_layout:
  layoutSwipe();
  oledClear();

  if (0 == index) {
    sub_index = 0;
    layoutHeader(tx_msg[0]);
    if (is_contract) {
      oledDrawStringAdapter(0, 13, _("Contract:"), FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, contract_address, FONT_STANDARD);
    } else {
      oledDrawStringAdapter(0, 13, _("Format:"), FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, _("Unknown"), FONT_STANDARD);
    }
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (1 == index) {
    layoutHeader(tx_msg[0]);
    if (address_rowcount > 3) {
      str = split_message((const uint8_t *)address, addrlen, rowlen);
      if (0 == sub_index) {
        oledDrawStringAdapter(0, 13, _("Signer:"), FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[0], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[2], FONT_STANDARD);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else {
        oledDrawStringAdapter(0, 13, str[sub_index - 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[sub_index], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[sub_index + 1],
                              FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[sub_index + 2],
                              FONT_STANDARD);
        if (sub_index == address_rowcount - 3) {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
        } else {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
          oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_down);
        }
      }

      // scrollbar
      drawScrollbar(2, sub_index);

    } else {
      oledDrawStringAdapter(0, 13, _("Signer:"), FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, address, FONT_STANDARD);
    }

    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (2 == index) {
    layoutHeader(title_data);
    if (data_rowcount > 4) {
      data2hexaddr(data + 10 * (sub_index), 10, lines);
      oledDrawStringAdapter(0, 13, lines, FONT_STANDARD);
      data2hexaddr(data + 10 * (sub_index + 1), 10, lines);
      oledDrawStringAdapter(0, 13 + 1 * 10, lines, FONT_STANDARD);
      data2hexaddr(data + 10 * (sub_index + 2), 10, lines);
      oledDrawStringAdapter(0, 13 + 2 * 10, lines, FONT_STANDARD);
      if (sub_index == data_rowcount - 4) {
        if (len % 10) {
          memset(lines, 0, 21);
          data2hexaddr(data + 10 * (sub_index + 3), len % 10, lines);
        } else {
          data2hexaddr(data + 10 * (sub_index + 3), 10, lines);
        }
      } else {
        data2hexaddr(data + 10 * (sub_index + 3), 10, lines);
      }
      oledDrawStringAdapter(0, 13 + 3 * 10, lines, FONT_STANDARD);

      // scrollbar
      bar_heght = 40 - 2 * (data_rowcount - 5);
      if (bar_heght < 6) bar_heght = 6;
      for (i = bar_start; i < bar_end; i += 2) {  // 40 pixel
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      if (sub_index <= 18) {
        for (i = bar_start + 2 * ((int)sub_index);
             i < (bar_start + bar_heght + 2 * ((int)sub_index - 1)) - 1; i++) {
          oledDrawPixel(OLED_WIDTH - 1, i);
          oledDrawPixel(OLED_WIDTH - 2, i);
        }
      } else {
        for (i = bar_start + 2 * 18;
             i < (bar_start + bar_heght + 2 * (18 - 1)) - 1; i++) {
          oledDrawPixel(OLED_WIDTH - 1, i);
          oledDrawPixel(OLED_WIDTH - 2, i);
        }
      }

      if (sub_index == 0) {
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else if (sub_index == data_rowcount - 4) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      }
    } else {
      char buf[90] = {0};
      data2hexaddr(data, len, buf);
      oledDrawStringAdapter(0, 13, buf, FONT_STANDARD);
    }

    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_next);
  } else if (max_index - 1 == index) {
    sub_index = 0;
    layoutHeader(_("Sign Transaction"));
    oledDrawStringAdapter(0, 13, tx_msg[1], FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  } else {
    layoutHeader(tx_msg[0]);
    if (3 == index) {
      memset(desc, 0, 64);
      strcat(desc, _(key1));
      strcat(desc, ":");
      oledDrawStringAdapter(0, 13, desc, FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, value1, FONT_STANDARD);
    } else if (4 == index) {
      memset(desc, 0, 64);
      strcat(desc, _(key2));
      strcat(desc, ":");
      oledDrawStringAdapter(0, 13, desc, FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, value2, FONT_STANDARD);
    } else if (5 == index) {
      memset(desc, 0, 64);
      strcat(desc, _(key3));
      strcat(desc, ":");
      oledDrawStringAdapter(0, 13, desc, FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, value3, FONT_STANDARD);
    }
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (sub_index > 0) {
        sub_index--;
      }
      goto refresh_layout;
    case KEY_DOWN:
      if (index == 1 && sub_index < address_rowcount - 3) {
        sub_index++;
      }
      if (index == 2 && sub_index < data_rowcount - 4) {
        sub_index++;
      }
      goto refresh_layout;
    case KEY_CONFIRM:
      if (index == max_index - 1) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      goto refresh_layout;
    case KEY_CANCEL:
      if (0 == index || index == max_index - 1) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_layout;
    default:
      break;
  }

  return result;
}

bool layoutSignMessage(const char *chain_name, bool verify, const char *signer,
                       const uint8_t *data, uint16_t len, bool is_ascii) {
  bool result = false;
  int index = 0, sub_index = 0, data_rowcount;
  int i, bar_heght, bar_start = 12, bar_end = 52;
  uint8_t max_index = 3;
  uint8_t key = KEY_NULL;
  char title[32] = {0};
  char title_tx[64] = {0};
  char lines[21] = {0};
  uint32_t rowlen = 21;
  const char **str;
  uint32_t addrlen = strlen(signer);
  int address_rowcount = addrlen / rowlen + 1;
  if (!is_ascii) {
    data_rowcount = len % 10 ? len / 10 + 1 : len / 10;
  } else {
    data_rowcount = len % 20 ? len / 20 + 1 : len / 20;
  }

  if (verify) {
    strcat(title, _("Confirm Address"));
    strcat(title_tx, _("Do you want to verify this message?"));
  } else {
    strcat(title, chain_name);
    strcat(title, _("Message"));
    snprintf(title_tx, 64, "%s%s %s?", _("Do you want to sign this\n"),
             chain_name, _("message"));
  }

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

refresh_layout:
  layoutSwipe();
  oledClear();

  if (0 == index) {
    layoutHeader(title);
    if (address_rowcount > 3) {
      str = split_message((const uint8_t *)signer, addrlen, rowlen);
      if (0 == sub_index) {
        oledDrawStringAdapter(0, 13, _("Signed by:"), FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[0], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[2], FONT_STANDARD);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else {
        oledDrawStringAdapter(0, 13, str[sub_index - 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[sub_index], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[sub_index + 1],
                              FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[sub_index + 2],
                              FONT_STANDARD);
        if (sub_index == address_rowcount - 3) {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
        } else {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
          oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_down);
        }
      }

      // scrollbar
      drawScrollbar(2, sub_index);
    } else {
      oledDrawStringAdapter(0, 13, _("Signer:"), FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, signer, FONT_STANDARD);
    }

    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (1 == index) {
    if (verify) {
      layoutHeader(_("Verified message"));
    } else {
      layoutHeader(title);
    }
    if (data_rowcount > 4) {
      if (is_ascii) {
        memcpy(lines, data + 20 * (sub_index), 20);
        oledDrawStringAdapter(0, 13, lines, FONT_STANDARD);
        memcpy(lines, data + 20 * (sub_index + 1), 20);
        oledDrawStringAdapter(0, 13 + 1 * 10, lines, FONT_STANDARD);
        memcpy(lines, data + 20 * (sub_index + 2), 20);
        oledDrawStringAdapter(0, 13 + 2 * 10, lines, FONT_STANDARD);
        if (sub_index >= data_rowcount - 4) {
          if (len % 20) {
            memset(lines, 0, 21);
            memcpy(lines, data + 20 * (sub_index + 3), len % 20);
          } else {
            memcpy(lines, data + 20 * (sub_index + 3), 20);
          }
        } else {
          memcpy(lines, data + 20 * (sub_index + 3), 20);
        }
        oledDrawStringAdapter(0, 13 + 3 * 10, lines, FONT_STANDARD);
      } else {
        data2hexaddr(data + 10 * (sub_index), 10, lines);
        oledDrawStringAdapter(0, 13, lines, FONT_STANDARD);
        data2hexaddr(data + 10 * (sub_index + 1), 10, lines);
        oledDrawStringAdapter(0, 13 + 1 * 10, lines, FONT_STANDARD);
        data2hexaddr(data + 10 * (sub_index + 2), 10, lines);
        oledDrawStringAdapter(0, 13 + 2 * 10, lines, FONT_STANDARD);
        if (sub_index >= data_rowcount - 4) {
          if (len % 10) {
            data2hexaddr(data + 10 * (sub_index + 3), len % 10, lines);
          } else {
            data2hexaddr(data + 10 * (sub_index + 3), 10, lines);
          }
        } else {
          data2hexaddr(data + 10 * (sub_index + 3), 10, lines);
        }
        oledDrawStringAdapter(0, 13 + 3 * 10, lines, FONT_STANDARD);
      }

      // scrollbar
      bar_heght = 40 - 2 * (data_rowcount - 5);
      if (bar_heght < 6) bar_heght = 6;
      for (i = bar_start; i < bar_end; i += 2) {  // 40 pixel
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      if (sub_index <= 18) {
        for (i = bar_start + 2 * ((int)sub_index);
             i < (bar_start + bar_heght + 2 * ((int)sub_index - 1)) - 1; i++) {
          oledDrawPixel(OLED_WIDTH - 1, i);
          oledDrawPixel(OLED_WIDTH - 2, i);
        }
      } else {
        for (i = bar_start + 2 * 18;
             i < (bar_start + bar_heght + 2 * (18 - 1)) - 1; i++) {
          oledDrawPixel(OLED_WIDTH - 1, i);
          oledDrawPixel(OLED_WIDTH - 2, i);
        }
      }

      if (sub_index == 0) {
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else if (sub_index == data_rowcount - 4) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      }
      layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
      layoutButtonYesAdapter(NULL, &bmp_bottom_right_next);
    } else {
      if (is_ascii) {
        oledDrawStringAdapter(0, 13, (char *)data, FONT_STANDARD);
      } else {
        char buf[90] = {0};
        data2hexaddr(data, len, buf);
        oledDrawStringAdapter(0, 13, buf, FONT_STANDARD);
      }
      layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
      layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
    }
  } else {
    if (verify) {
      if (is_ascii)
        layoutHeader(_("Verified message"));
      else
        layoutHeader(_("Verified binary message?"));
    } else {
      if (is_ascii)
        layoutHeader(_("Sign Message"));
      else
        layoutHeader(_("Sign Binary Message?"));
    }
    oledDrawStringAdapter(0, 13, title_tx, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  }
  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (sub_index > 0) {
        sub_index--;
      }
      goto refresh_layout;
    case KEY_DOWN:
      if (index == 0 && sub_index < address_rowcount - 3) {
        sub_index++;
      }
      if (index == 1 && sub_index < data_rowcount - 4) {
        sub_index++;
      }
      goto refresh_layout;
    case KEY_CONFIRM:
      if (index == max_index - 1) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      sub_index = 0;
      goto refresh_layout;
    case KEY_CANCEL:
      if (0 == index || index == max_index - 1) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_layout;
    default:
      break;
  }

  return result;
}

bool layoutSignHash(const char *chain_name, bool verify, const char *signer,
                    const char *domain_hash, const char *message_hash,
                    const char *warning) {
  bool result = false;
  int index = 0;
  uint8_t max_index = 5;
  uint8_t key = KEY_NULL;
  char title[32] = {0};
  char title_tx[64] = {0};
  if (!message_hash) max_index--;

  if (verify) {
    strcat(title, _("Confirm Address"));
    strcat(title_tx, _("Do you want to verify this message?"));
  } else {
    strcat(title, chain_name);
    strcat(title, _("Message"));
    snprintf(title_tx, 64, "%s%s %s?", _("Do you want to sign this\n"),
             chain_name, _("message"));
  }

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

refresh_layout:
  layoutSwipe();
  oledClear();

  if (0 == index) {
    layoutHeader(title);
    oledDrawStringAdapter(0, 13, warning, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (1 == index) {
    layoutHeader(title);
    oledDrawStringAdapter(0, 13, _("Signer:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10, signer, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (2 == index) {
    layoutHeader(_("Domain Hash:"));
    oledDrawStringAdapter(0, 13, domain_hash, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (3 == index && message_hash) {
    layoutHeader(_("Message Hash:"));
    oledDrawStringAdapter(0, 13, message_hash, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else {
    layoutHeader(title);
    oledDrawStringAdapter(0, 13, title_tx, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      goto refresh_layout;
    case KEY_DOWN:
      goto refresh_layout;
    case KEY_CONFIRM:
      if (index == max_index - 1) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      goto refresh_layout;
    case KEY_CANCEL:
      if (0 == index || index == max_index - 1) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_layout;
    default:
      break;
  }

  return result;
}
