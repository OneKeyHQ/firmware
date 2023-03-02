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

#if !BITCOIN_ONLY

static const char *slip44_extras(uint32_t coin_type) {
  if ((coin_type & 0x80000000) == 0) {
    return 0;
  }
  switch (coin_type & 0x7fffffff) {
    case 40:
      return "EXP";  // Expanse
    case 43:
      return "NEM";  // NEM
    case 60:
      return "ETH";  // Ethereum Mainnet
    case 61:
      return "ETC";  // Ethereum Classic Mainnet
    case 108:
      return "UBQ";  // UBIQ
    case 137:
      return "RSK";  // Rootstock Mainnet
    case 37310:
      return "tRSK";  // Rootstock Testnet
  }
  return 0;
}

#endif

#define BIP32_MAX_LAST_ELEMENT 1000000

static const char *address_n_str(const uint32_t *address_n,
                                 size_t address_n_count,
                                 bool address_is_account) {
  if (address_n_count > 8) {
    return _("Unknown long path");
  }
  if (address_n_count == 0) {
    return _("Path: m");
  }

  // known BIP44/49/84/86 path
  static char path[100];
  if (address_n_count == 5 &&
      (address_n[0] == (0x80000000 + 44) || address_n[0] == (0x80000000 + 49) ||
       address_n[0] == (0x80000000 + 84) ||
       address_n[0] == (0x80000000 + 86)) &&
      (address_n[1] & 0x80000000) && (address_n[2] & 0x80000000) &&
      (address_n[3] <= 1) && (address_n[4] <= BIP32_MAX_LAST_ELEMENT)) {
    bool taproot = (address_n[0] == (0x80000000 + 86));
    bool native_segwit = (address_n[0] == (0x80000000 + 84));
    bool p2sh_segwit = (address_n[0] == (0x80000000 + 49));
    bool legacy = false;
    const CoinInfo *coin = coinBySlip44(address_n[1]);
    const char *abbr = 0;
    if (taproot) {
      if (coin && coin->has_taproot && coin->bech32_prefix) {
        abbr = coin->coin_shortcut;
      }
    } else if (native_segwit) {
      if (coin && coin->has_segwit && coin->bech32_prefix) {
        abbr = coin->coin_shortcut;
      }
    } else if (p2sh_segwit) {
      if (coin && coin->has_segwit) {
        abbr = coin->coin_shortcut;
      }
    } else {
      if (coin) {
        if (coin->has_segwit) {
          legacy = true;
        }
        abbr = coin->coin_shortcut;
#if !BITCOIN_ONLY
      } else {
        abbr = slip44_extras(address_n[1]);
#endif
      }
    }
    const uint32_t accnum = address_is_account
                                ? ((address_n[4] & 0x7fffffff) + 1)
                                : (address_n[2] & 0x7fffffff) + 1;
    if (abbr && accnum < 100) {
      memzero(path, sizeof(path));
      strlcpy(path, abbr, sizeof(path));
      // account naming:
      // "Legacy", "Legacy SegWit", "SegWit" and "Taproot"
      // for BIP44/P2PKH, BIP49/P2SH-P2WPKH, BIP84/P2WPKH and BIP86/P2TR
      // respectively.
      // For non-segwit coins we use only BIP44 with no special naming.
      if (legacy) {
        strlcat(path, " Legacy", sizeof(path));
      } else if (p2sh_segwit) {
        strlcat(path, " L.SegWit", sizeof(path));
      } else if (native_segwit) {
        strlcat(path, " SegWit", sizeof(path));
      } else if (taproot) {
        strlcat(path, " Taproot", sizeof(path));
      }
      if (address_is_account) {
        strlcat(path, " address #", sizeof(path));
      } else {
        strlcat(path, " account #", sizeof(path));
      }
      char acc[3] = {0};
      memzero(acc, sizeof(acc));
      if (accnum < 10) {
        acc[0] = '0' + accnum;
      } else {
        acc[0] = '0' + (accnum / 10);
        acc[1] = '0' + (accnum % 10);
      }
      strlcat(path, acc, sizeof(path));
      return path;
    }
  }

  //                  "Path: m"    /    i   '
  static char address_str[7 + 8 * (1 + 10 + 1) + 1];
  char *c = address_str + sizeof(address_str) - 1;

  *c = 0;
  c--;

  for (int n = (int)address_n_count - 1; n >= 0; n--) {
    uint32_t i = address_n[n];
    if (i & 0x80000000) {
      *c = '\'';
      c--;
    }
    i = i & 0x7fffffff;
    do {
      *c = '0' + (i % 10);
      c--;
      i /= 10;
    } while (i > 0);
    *c = '/';
    c--;
  }
  *c = 'm';
  c--;
  *c = ' ';
  c--;
  *c = ':';
  c--;
  *c = 'h';
  c--;
  *c = 't';
  c--;
  *c = 'a';
  c--;
  *c = 'P';

  return c;
}

// split longer string into 4 rows, rowlen chars each
const char **split_message(const uint8_t *msg, uint32_t len, uint32_t rowlen) {
  static char str[4][32 + 1];
  if (rowlen > 32) {
    rowlen = 32;
  }

  memzero(str, sizeof(str));
  for (int i = 0; i < 4; ++i) {
    size_t show_len = strnlen((char *)msg, MIN(rowlen, len));
    memcpy(str[i], (char *)msg, show_len);
    str[i][show_len] = '\0';
    msg += show_len;
    len -= show_len;
  }

  if (len > 0) {
    str[3][rowlen - 1] = '.';
    str[3][rowlen - 2] = '.';
    str[3][rowlen - 3] = '.';
  }
  static const char *ret[4] = {str[0], str[1], str[2], str[3]};
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

void *layoutLast = layoutHome;

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
  layoutLast = layoutScreensaver;
  oledClear();
  oledRefresh();
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
        oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - 18, 0, &bmp_blecon);
        layout_refresh = true;
      }
    } else if (force_flag || true == ble_icon_status_old) {
      if (ble_conn_status_old) {
        ble_conn_status_old = false;
        ret = 1;
      }
      oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - 18, 0, &bmp_ble);
      layout_refresh = true;
    }
    ble_icon_status_old = true;
  } else if (true == ble_icon_status_old) {
    if (ble_conn_status_old) {
      ble_conn_status_old = false;
      ret = 1;
    }
    ble_icon_status_old = false;
    oledClearBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - 18, 0, &bmp_ble);
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
      oledDrawBitmap(OLED_WIDTH - LOGO_WIDTH - 18, 0, &bmp_usb);
      layout_refresh = true;
    }
  } else if (true == usb_status_old) {
    usb_status_old = false;
    oledClearBitmap(OLED_WIDTH - LOGO_WIDTH - 18, 0, &bmp_usb);
    layout_refresh = true;
    cur_level_dis = battery_old;
    dis_power_flag = 0;
    dis_hint_timer_counter = 0;
    layoutRefreshSet(true);
  }
}

uint8_t layoutStatusLogoEx(bool need_fresh, bool force_fresh) {
  uint8_t ret = 0;

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
  layoutLast = layout_language_set;
  static int index = 0;

  switch (key) {
    case KEY_UP:
      if (index > 0) index--;
      break;
    case KEY_DOWN:
      if (index < 1) index++;
      break;
    case KEY_CONFIRM:
      config_setLanguage(lang[index]);
      layoutHome();
      return;
    default:
      return;
  }

  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                           NULL, index == 0 ? "Okay" : "确认", index + 1, 2,
                           NULL, NULL, index == 0 ? "English" : "繁体中文",
                           index > 0 ? "English" : NULL,
                           index == 0 ? "繁体中文" : NULL);
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
      oledDrawBitmap(56, 8, &bmp_home_logo);
      char desc[64] = "";
      char label[MAX_LABEL_LEN + 1] = "";
      config_getLabel(label, sizeof(label));
      strcat(desc, label);
      if (ble_name_state() == true) {
        strcat(desc, "(");
        strcat(desc, ble_get_name());
        strcat(desc, ")");
      }
      if (!config_isInitialized()) {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, desc, FONT_STANDARD);
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 10,
                                    _("Not Actived"), FONT_STANDARD);
      } else {
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
        }
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, desc, FONT_STANDARD);
      }
      if (session_isUnlocked() || !config_hasPin()) {
        oledDrawStringCenterAdapter(
            OLED_WIDTH / 2, 42, _("Press any key to continue"), FONT_STANDARD);
      } else {
        oledDrawStringCenterAdapter(OLED_WIDTH / 2, 42,
                                    _("Enter PIN to unlock"), FONT_STANDARD);
      }
    }
  }

  oledRefresh();

  bool initialized = config_isInitialized() | config_getMnemonicsImported();

  if (update_menu) main_menu_init(initialized);

  // Reset lock screen timeout
  system_millis_lock_start = timer_ms();
}

void layoutHome(void) {
#if !EMULATOR
  static bool first_boot = true;
  if (first_boot && !config_isInitialized() && !se_isFactoryMode()) {
    first_boot = false;
    layout_language_set(KEY_UP);
  } else
#endif
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
      strlcpy(suffix + 1, "sat ", sizeof(suffix) - 1);
      strlcpy(suffix + 5, coin->coin_shortcut, sizeof(suffix) - 5);
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
  return bn_format_uint64(amount, prefix, suffix, decimals, 0, false, output,
                          output_len);
}

void layoutConfirmOutput(const CoinInfo *coin, AmountUnit amount_unit,
                         const TxOutputType *out) {
  char str_out[32 + 3] = {0};
  format_coin_amount(out->amount, NULL, coin, amount_unit, str_out,
                     sizeof(str_out) - 3);
  strlcat(str_out, " to", sizeof(str_out));
  const char *address = out->address;
  const char *extra_line =
      (out->address_n_count > 0)
          ? address_n_str(out->address_n, out->address_n_count, false)
          : 0;
  render_address_dialog(coin, address, _("Confirm sending"), str_out,
                        extra_line);
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
    bn_format_uint64(amount, NULL, suffix, divisible ? 8 : 0, 0, false, str_out,
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

void layoutConfirmTx(const CoinInfo *coin, AmountUnit amount_unit,
                     uint64_t total_in, uint64_t total_out,
                     uint64_t change_out) {
  char str_out[32] = {0};
  formatAmountDifference(coin, amount_unit, total_in, change_out, str_out,
                         sizeof(str_out));

  char str_fee[32] = {0};
  formatAmountDifference(coin, amount_unit, total_in, total_out, str_fee,
                         sizeof(str_fee));

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Really send"), str_out, _("from your wallet?"),
                    _("Fee included:"), str_fee, NULL);
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
                            uint64_t fee_old, uint64_t fee_new) {
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

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    question, str_fee_change, NULL, _("Transaction fee:"),
                    str_fee_new, NULL);
}

void layoutFeeOverThreshold(const CoinInfo *coin, AmountUnit amount_unit,
                            uint64_t fee) {
  char str_fee[32] = {0};
  format_coin_amount(fee, NULL, coin, amount_unit, str_fee, sizeof(str_fee));
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Fee"), str_fee, _("is unexpectedly high."), NULL,
                    _("Send anyway?"), NULL);
}

void layoutChangeCountOverThreshold(uint32_t change_count) {
  char str_change[21] = {0};
  snprintf(str_change, sizeof(str_change), "There are %" PRIu32, change_count);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Warning!"), str_change, _("change-outputs."), NULL,
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

#define QR_MAX_VERSION 9

void layoutAddress(const char *address, const char *desc, bool qrcode,
                   bool ignorecase, const uint32_t *address_n,
                   size_t address_n_count, bool address_is_account) {
  if (layoutLast != layoutAddress && layoutLast != layoutXPUBMultisig) {
    layoutSwipe();
  } else {
    oledClear_ex();
  }
  layoutLast = layoutAddress;

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
    uint8_t codedata[qrcodegen_BUFFER_LEN_FOR_VERSION(QR_MAX_VERSION)] = {0};
    uint8_t tempdata[qrcodegen_BUFFER_LEN_FOR_VERSION(QR_MAX_VERSION)] = {0};

    int side = 0;
    if (qrcodegen_encodeText(ignorecase ? address_upcase : address, tempdata,
                             codedata, qrcodegen_Ecc_LOW, qrcodegen_VERSION_MIN,
                             QR_MAX_VERSION, qrcodegen_Mask_AUTO, true)) {
      side = qrcodegen_getSize(codedata);
    }

    oledInvert(0, 0, 63, 63);
    if (side > 0 && side <= 29) {
      int offset = 32 - side;
      for (int i = 0; i < side; i++) {
        for (int j = 0; j < side; j++) {
          if (qrcodegen_getModule(codedata, i, j)) {
            oledBox(offset + i * 2, offset + j * 2, offset + 1 + i * 2,
                    offset + 1 + j * 2, false);
          }
        }
      }
    } else if (side > 0 && side <= 60) {
      int offset = 32 - (side / 2);
      for (int i = 0; i < side; i++) {
        for (int j = 0; j < side; j++) {
          if (qrcodegen_getModule(codedata, i, j)) {
            oledClearPixel(offset + i, offset + j);
          }
        }
      }
    }
  } else {
    uint32_t i = 0;
    if (desc) {
      oledDrawStringAdapter(0, 0 * 9, desc, FONT_STANDARD);
    }
    if (addrlen > 10) {  // don't split short addresses
      uint32_t rowcount = 4;
      if (addrlen <= 42) {
        rowcount = 2;
      } else if (addrlen <= 63) {
        rowcount = 3;
      }

      uint32_t rowlen = (addrlen - 1) / rowcount + 1;
      const char **str =
          split_message((const uint8_t *)address, addrlen, rowlen);

      for (i = 0; i < 4; i++) {
        oledDrawString(0, (i + 1) * 9, str[i], FONT_FIXED);
      }
    } else {
      oledDrawString(0, (0 + 1) * 9 + 4, address, FONT_FIXED);
    }
    oledDrawString(
        0, (i + 1) * 9,
        address_n_str(address_n, address_n_count, address_is_account),
        FONT_STANDARD);
  }

  if (!qrcode) {
    layoutButtonNoAdapter(_("QR Code"), NULL);
  }

  layoutButtonYesAdapter(_("Confirm"), &bmp_btn_confirm);
  oledRefresh();
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

void layoutXPUB(const char *xpub, int page) {
  if (layoutLast != layoutAddress && layoutLast != layoutXPUB) {
    layoutSwipe();
  } else {
    oledClear_ex();
  }
  layoutLast = layoutXPUB;
  char desc[] = "XPUB _/2";
  desc[5] = '1' + page;
  _layout_xpub(xpub, desc, page);
  layoutButtonNoAdapter(_("Cancel"), &bmp_btn_cancel);
  layoutButtonYesAdapter(_("Confirm"), &bmp_btn_confirm);
  oledRefresh();
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
  layoutDialogAdapter(&bmp_webauthn, NULL, verb, NULL, verb,
                      _("U2F security key?"), NULL, appname, NULL, NULL);
}

#endif

void layoutShowPassphrase(const char *passphrase) {
  if (layoutLast != layoutShowPassphrase) {
    layoutSwipe();
  } else {
    oledClear();
  }
  const char **str =
      split_message((const uint8_t *)passphrase, strlen(passphrase), 21);
  for (int i = 0; i < 3; i++) {
    oledDrawString(0, (i + 1) * 9, str[i], FONT_FIXED);
  }
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 2 * 9 - 1,
                              _("Use this passphrase?"), FONT_STANDARD);
  oledHLine(OLED_HEIGHT - 21);
  layoutButtonNoAdapter(_("Cancel"), &bmp_btn_cancel);
  layoutButtonYesAdapter(_("Confirm"), &bmp_btn_confirm);
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
      bn_format_uint64(definition->fee, NULL, NULL, 0, 0, false, str_out,
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
  return address_n_count == 2 && address_n[0] == (0x80000000 + 10018) &&
         (address_n[1] & 0x80000000) && (address_n[1] & 0x7FFFFFFF) <= 9;
}

void layoutCosiCommitSign(const uint32_t *address_n, size_t address_n_count,
                          const uint8_t *data, uint32_t len, bool final_sign) {
  char *desc = final_sign ? _("CoSi sign message?") : _("CoSi commit message?");
  char desc_buf[32] = {0};
  if (is_slip18(address_n, address_n_count)) {
    if (final_sign) {
      strlcpy(desc_buf, _("CoSi sign index #?"), sizeof(desc_buf));
      desc_buf[16] = '0' + (address_n[1] & 0x7FFFFFFF);
    } else {
      strlcpy(desc_buf, _("CoSi commit index #?"), sizeof(desc_buf));
      desc_buf[18] = '0' + (address_n[1] & 0x7FFFFFFF);
    }
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
  if (layoutLast == layout_language_set) {
    layout_language_set(key);
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
  bn_format_uint64(num, NULL, NULL, 0, 0, false, &line[off],
                   sizeof(line) - off);
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

// layout chinese
void layoutButtonNoAdapter(const char *btnNo, const BITMAP *icon) {
  const struct font_desc *font = find_cur_font();
  int icon_width = 0;
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
    oledDrawBitmap(56, 9, icon);
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
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * (font->pixel + 1), line4,
                                FONT_STANDARD);
  }

  if (desc) {
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, OLED_HEIGHT - 2 * (font->pixel),
                                desc, FONT_STANDARD);
    if (btnYes || btnNo) {
      oledHLine(OLED_HEIGHT - 2 * (font->pixel) - 1);
    }

  } else {
    if (line5) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 4 * (font->pixel + 1) + 1,
                                  line5, FONT_STANDARD);
    }
    if (line6) {
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 5 * (font->pixel + 1) + 1,
                                  line6, FONT_STANDARD);
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
  oledDrawBitmap(x - 4, y - 9, &bmp_btn_up);
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

  oledDrawBitmap(x - 4, y + 11 + y0, &bmp_btn_down);
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
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, text, FONT_STANDARD);
  y += 18;

  for (uint32_t i = 0; i < sizeof(pin_show); i++) {
    buf[0] = pin_show[i];
    l = oledStringWidth(buf, FONT_STANDARD);
    oledDrawStringAdapter(x + 13 * i + 7 - l / 2, y, buf, FONT_STANDARD);
  }

  if (index > 0 && index < 10) {
    layoutItemsSelect(x + 13 * pos + 7, y, table[index], FONT_STANDARD);
  } else {
    layoutBmpSelect(x + 13 * pos + 7, y, &bmp_btn_confirm);
  }

  if (pos != 0 || cancel_allowed)
    layoutButtonNoAdapter(_("Prev"), &bmp_btn_back);
  if (pos < MAX_PIN_LEN - 1) {
    layoutButtonYesAdapter(_("Confirm"), &bmp_btn_forward);
  } else {
    layoutButtonYesAdapter(_("Done"), &bmp_btn_confirm);
  }

  oledRefresh();
}

void layoutInputWord(const char *text, uint8_t prefix_len, const char *prefix,
                     const char *letter) {
  int l, y = 9;
  char word_show[8] = "________";
  char buf[2] = {0};

  int x = 12;

  for (uint8_t i = 0; i < prefix_len; i++) {
    word_show[i] = prefix[i];
  }
  oledClear_ex();
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, text, FONT_STANDARD);
  y += 18;
  for (uint32_t i = 0; i < sizeof(word_show); i++) {
    buf[0] = word_show[i];
    l = oledStringWidth(buf, FONT_STANDARD);
    oledDrawStringAdapter(x + 13 * i + 7 - l / 2, y, buf, FONT_STANDARD);
  }

  layoutItemsSelect(x + 13 * prefix_len + 7, y, letter, FONT_STANDARD);
  layoutButtonNoAdapter(_("Prev"), &bmp_btn_back);
  layoutButtonYesAdapter(_("Confirm"), &bmp_btn_confirm);

  oledRefresh();
}
static char *input[4] = {"abc", "ABC", "123", "=/<"};
static char *input1[4] = {"ab", "AB", "01", "=<"};

void layoutInputMethod(uint8_t index) {
  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                           NULL, _("OK"), index + 1, 4, NULL, NULL,
                           input[index], index > 0 ? input[index - 1] : NULL,
                           index < 3 ? input[index + 1] : NULL);
}

void layoutInputPassphrase(const char *text, uint8_t prefix_len,
                           const char *prefix, uint8_t char_index,
                           uint8_t input_type) {
  int l, y = 10;
  char word_show[9] = "_________";
  char buf[2] = {0};
  char index_str[16] = "";
  uint8_t location = 0;

  int x = 6, icon_x = 80;

  if (prefix_len < 9) {
    memcpy(word_show, prefix, prefix_len);
  } else {
    memcpy(word_show, prefix + prefix_len - 8, 8);
  }

  oledClear_ex();

  // index
  uint2str(prefix_len + 1, index_str);
  strcat(index_str + strlen(index_str), "/");
  uint2str(50, index_str + strlen(index_str));
  oledDrawStringAdapter(1, 1, index_str, FONT_SMALL);

  // input type
  oledDrawString(icon_x, 1, input[input_type], FONT_SMALL);

  l = oledStringWidth(input[input_type], FONT_SMALL);

  oledInvert(icon_x - 2, 0, icon_x + l, 6);
  oledClearPixel(icon_x - 2, 0);
  oledClearPixel(icon_x - 2, 6);
  oledClearPixel(icon_x + l, 0);
  oledClearPixel(icon_x + l, 6);
  oledRefresh();

  // title
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, text, FONT_STANDARD);

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
    layoutButtonYesAdapter(_("Switch"), &bmp_btn_switch);
  } else if (char_index == 0xFF) {
    layoutBmpSelect(x + 13 * location + 7, y, &bmp_btn_confirm);
    layoutButtonYesAdapter(_("Submit"), &bmp_btn_confirm);
  } else {
    buf[0] = char_index;
    layoutItemsSelect_ex(x + 13 * location + 7, y, buf, FONT_STANDARD, false);
    if (prefix_len == (MAX_PASSPHRASE_LEN - 1)) {
      layoutButtonYesAdapter(_("Submit"), &bmp_btn_confirm);
    } else {
      layoutButtonYesAdapter(_("Enter"), &bmp_btn_confirm);
    }
  }

  if (prefix_len == 0) {
    layoutButtonNoAdapter(_("Quit"), &bmp_btn_back);
  } else {
    layoutButtonNoAdapter(_("Back"), &bmp_btn_back);
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

#define DEVICE_INFO_PAGE_NUM 9
extern char bootloader_version[8];

void layouKeyValue(int y, const char *desc, const char *value) {
  oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
  oledDrawStringRightAdapter(OLED_WIDTH - 1, y, value, FONT_STANDARD);
}

void layoutDeviceParameters(int num) {
  (void)num;
  const struct font_desc *font = find_cur_font();
  char *se_version = NULL;
  char *se_sn = NULL;
  int y = 0;
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
  oledDrawStringAdapter(1, 1, index_str, FONT_SMALL);

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
    case 5:
      oledDrawStringAdapter(0, y, _("BUILD ID:"), FONT_STANDARD);
      y += font->pixel + 1;

      // split build id
      char build_id1[64] = {0};
      char build_id2[64] = {0};

      for (int i = 0; i < BUILD_ID_MAX_LEN; i++) {
        build_id1[i] = BUILD_ID[i];
        if (oledStringWidthAdapter(build_id1, FONT_STANDARD) > OLED_WIDTH) {
          build_id1[i] = 0;
          strcat(build_id2, BUILD_ID + i);
          break;
        }
      }

      oledDrawStringAdapter(0, y, build_id1, FONT_STANDARD);
      y += font->pixel + 1;
      oledDrawStringAdapter(0, y, build_id2, FONT_STANDARD);
      break;
    case 6:
      oledDrawBitmap((OLED_WIDTH - bmp_Icon_fc.width) / 2, 4, &bmp_Icon_fc);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 40, "FCC ID:2AV5MBIXINKEY1",
                                  FONT_STANDARD);
      break;
    case 7:
      oledDrawBitmap((OLED_WIDTH - bmp_Icon_bc.width) / 2, 12, &bmp_Icon_bc);
      break;
    case 8:
      oledDrawBitmap(20, 12, &bmp_Icon_ce);
      oledDrawBitmap(72, 12, &bmp_Icon_weee);
      break;
    default:
      break;
  }
  layoutButtonNoAdapter(NULL, &bmp_button_back);

  // scrollbar
  for (int i = 0; i < OLED_HEIGHT; i += 2) {
    oledDrawPixel(OLED_WIDTH - 1, i);
  }
  oledBox(OLED_WIDTH - 2, index * 7, OLED_WIDTH, (index + 1) * 7, true);

  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (index < DEVICE_INFO_PAGE_NUM - 1) {
        index++;
      }
      goto refresh_menu;
    case KEY_CONFIRM:
      goto refresh_menu;
    case KEY_CANCEL:
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

bool layoutBlindSign(char *address) {
  const struct font_desc *font = find_cur_font();
  bool result = false;
  int index = 0;
  int y = 0;
  uint8_t key = KEY_NULL;
  const char **str =
      split_message((const uint8_t *)address, strlen(address), 17);
  int lines = (strlen(address) / 17) + 1;
  if (lines > 4) lines = 4;

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

refresh_menu:
  layoutSwipe();
  oledClear();
  y = 0;
  switch (index) {
    case 0:
      oledDrawStringAdapter(0, y, _("SENDER:"), FONT_STANDARD);
      y += font->pixel + 5;
      for (int i = 0; i < lines; i++) {
        oledDrawString(0, y, str[i], FONT_STANDARD);
        y += font->pixel + 1;
      }

      // scrollbar
      for (int i = 0; i < OLED_HEIGHT; i += 3) {
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      for (int i = 0; i < OLED_HEIGHT / 3; i++) {
        oledDrawPixel(OLED_WIDTH - 1, i);
        oledDrawPixel(OLED_WIDTH - 2, i);
      }
      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, OLED_HEIGHT - 8,
                     &bmp_btn_down);
      break;
    case 1:
      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, 0, &bmp_btn_up);
      y += bmp_btn_up.height + 1;
      oledDrawStringAdapter(0, y, _("FORMAT:"), FONT_STANDARD);
      y += font->pixel + 5;
      oledDrawStringAdapter(0, y, _("Unknown"), FONT_STANDARD);
      // scrollbar
      for (int i = 0; i < OLED_HEIGHT; i += 3) {
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      for (int i = OLED_HEIGHT / 3; i < 2 * OLED_HEIGHT / 3; i++) {
        oledDrawPixel(OLED_WIDTH - 1, i);
        oledDrawPixel(OLED_WIDTH - 2, i);
      }
      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, OLED_HEIGHT - 8,
                     &bmp_btn_down);
      break;
    case 2:
      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, 0, &bmp_btn_up);
      y += bmp_btn_up.height + 1;
      oledDrawStringAdapter(0, y, _("CONFIRM SIGNING:"), FONT_STANDARD);
      y += font->pixel + 5;
      oledDrawStringAdapter(0, y, _("Transaction data cannot be decoded"),
                            FONT_STANDARD);
      if (0 == ui_language) {
        y += 2 * font->pixel + 3;
      } else {
        y += font->pixel + 1;
      }
      oledDrawStringAdapter(0, y, _("Sign at you own risk"), FONT_STANDARD);

      // scrollbar
      for (int i = 0; i < OLED_HEIGHT; i += 3) {
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      for (int i = 2 * OLED_HEIGHT / 3; i < OLED_HEIGHT; i++) {
        oledDrawPixel(OLED_WIDTH - 1, i);
        oledDrawPixel(OLED_WIDTH - 2, i);
      }

      layoutButtonNoAdapter(_("CANCEL"), &bmp_btn_cancel);
      layoutScroollbarButtonYesAdapter(_("APPROVE"), &bmp_btn_confirm);
      break;
    default:
      break;
  }
  oledRefresh();

scan_key:
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
        goto refresh_menu;
      } else {
        goto scan_key;
      }
    case KEY_DOWN:
      if (index < 2 /* 3 pages */) {
        index++;
        goto refresh_menu;
      } else {
        goto scan_key;
      }

    case KEY_CONFIRM:
      if (2 == index) {
        result = true;
        break;
      }
      goto scan_key;
    case KEY_CANCEL:
      if (2 == index) {
        result = false;
        break;
      }
      goto scan_key;
    default:
      break;
  }

  return result;
}
