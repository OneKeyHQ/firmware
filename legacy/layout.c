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

#include "layout.h"
#include "ble.h"
#include "oled.h"
#include "prompt.h"
#include "usart.h"

#if !EMULATOR
#include "sys.h"
#include "timer.h"
#endif

static bool refresh_home = true;

bool layoutNeedRefresh(void) {
  if (refresh_home) {
    refresh_home = false;
    return true;
  }
  return false;
}
void layoutRefreshSet(bool refresh) { refresh_home = refresh; }

void layoutButtonNo(const char *btnNo, const BITMAP *icon) {
  int icon_width = 0;
  if (icon) {
    oledDrawBitmap(1, OLED_HEIGHT - 8, icon);
    icon_width = icon->width;
  }
  oledDrawString(3 + icon_width, OLED_HEIGHT - 8, btnNo, FONT_STANDARD);
  oledInvert(0, OLED_HEIGHT - 9,
             icon_width + oledStringWidth(btnNo, FONT_STANDARD) + 4,
             OLED_HEIGHT - 1);
}

void layoutButtonYes(const char *btnYes, const BITMAP *icon) {
  int icon_width = 0;
  if (icon) {
    oledDrawBitmap(OLED_WIDTH - 8 - 1, OLED_HEIGHT - 8, icon);
    icon_width = icon->width;
  }
  oledDrawStringRight(OLED_WIDTH - icon_width - 3, OLED_HEIGHT - 8, btnYes,
                      FONT_STANDARD);
  oledInvert(
      OLED_WIDTH - oledStringWidth(btnYes, FONT_STANDARD) - icon_width - 4,
      OLED_HEIGHT - 9, OLED_WIDTH - 1, OLED_HEIGHT - 1);
}

void layoutDialog(const BITMAP *icon, const char *btnNo, const char *btnYes,
                  const char *desc, const char *line1, const char *line2,
                  const char *line3, const char *line4, const char *line5,
                  const char *line6) {
  layoutDialogEx(icon, btnNo, btnYes, desc, line1, line2, line3, line4, line5,
                 line6, FONT_STANDARD);
}

inline void layoutDialogEx(const BITMAP *icon, const char *btnNo,
                           const char *btnYes, const char *desc,
                           const char *line1, const char *line2,
                           const char *line3, const char *line4,
                           const char *line5, const char *line6, uint8_t font) {
  int left = 0;
  oledClear();
  if (icon) {
    oledDrawBitmap(0, 0, icon);
    left = icon->width + 4;
  }
  if (line1) oledDrawString(left, 0 * 9, line1, font);
  if (line2) oledDrawString(left, 1 * 9, line2, font);
  if (line3) oledDrawString(left, 2 * 9, line3, font);
  if (line4) oledDrawString(left, 3 * 9, line4, font);
  if (desc) {
    oledDrawStringCenter(OLED_WIDTH / 2, OLED_HEIGHT - 2 * 9 - 1, desc,
                         FONT_STANDARD);
    if (btnYes || btnNo) {
      oledHLine(OLED_HEIGHT - 21);
    }
  } else {
    if (line5) oledDrawString(left, 4 * 9, line5, font);
    if (line6) oledDrawString(left, 5 * 9, line6, font);
    if (btnYes || btnNo) {
      oledHLine(OLED_HEIGHT - 13);
    }
  }
  if (btnNo) {
    layoutButtonNo(btnNo, &bmp_btn_cancel);
  }
  if (btnYes) {
    layoutButtonYes(btnYes, &bmp_btn_confirm);
  }
  oledRefresh();
}

static void layoutTitle(const char *title) {
  oledBox(0, 0, OLED_WIDTH, 10, false);
  oledDrawStringCenter(OLED_WIDTH / 2, 2, title, FONT_STANDARD);

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

void layoutDialogCenterAdapterEx(const BITMAP *icon, const BITMAP *bmp_no,
                                 const BITMAP *bmp_yes, const char *title,
                                 const char *line1, const char *line2,
                                 const char *line3, const char *line4) {
  int y = 0;
  oledClear();
  if (icon) {
    y = 21;
    oledDrawBitmap(56, 2, icon);
  } else if (title) {
    y = 13;
    layoutTitle(title);
  }

  if (line1)
    oledDrawStringCenter(OLED_WIDTH / 2, y + (0 * 10), line1, FONT_STANDARD);
  if (line2)
    oledDrawStringCenter(OLED_WIDTH / 2, y + (1 * 10), line2, FONT_STANDARD);
  if (line3)
    oledDrawStringCenter(OLED_WIDTH / 2, y + (2 * 10), line3, FONT_STANDARD);
  if (line4)
    oledDrawStringCenter(OLED_WIDTH / 2, y + (3 * 10), line4, FONT_STANDARD);

  if (bmp_no) {
    oledDrawBitmap(1, OLED_HEIGHT - 11, bmp_no);
  }
  if (bmp_yes) {
    oledDrawBitmap(OLED_WIDTH - 16 - 1, OLED_HEIGHT - 11, bmp_yes);
  }
  oledRefresh();
}

void layoutinfoCenter(const char *line1, const char *line2, const char *line3,
                      const char *line4, const char *line5, const char *line6) {
  oledClear();

  if (line1) oledDrawStringCenter(OLED_WIDTH / 2, 1 * 9, line1, FONT_STANDARD);
  if (line2) oledDrawStringCenter(OLED_WIDTH / 2, 2 * 9, line2, FONT_STANDARD);
  if (line3) oledDrawStringCenter(OLED_WIDTH / 2, 3 * 9, line3, FONT_STANDARD);
  if (line4) oledDrawStringCenter(OLED_WIDTH / 2, 4 * 9, line4, FONT_STANDARD);
  if (line5) oledDrawStringCenter(OLED_WIDTH / 2, 5 * 9, line5, FONT_STANDARD);
  if (line6) oledDrawStringCenter(OLED_WIDTH / 2, 6 * 9, line6, FONT_STANDARD);

  oledRefresh();
}

void layoutProgressUpdate(bool refresh) {
  static uint8_t step = 0;
  switch (step) {
    case 0:
      oledDrawBitmap(40, 0, &bmp_gears0);
      break;
    case 1:
      oledDrawBitmap(40, 0, &bmp_gears1);
      break;
    case 2:
      oledDrawBitmap(40, 0, &bmp_gears2);
      break;
    case 3:
      oledDrawBitmap(40, 0, &bmp_gears3);
      break;
  }
  step = (step + 1) % 4;
  if (refresh) {
    oledRefresh();
  }
}

void layoutProgressPercent(int permil) {
  char percent_asc[5] = {0};
  int i = 0;
  if (permil < 10) {
    percent_asc[i++] = permil + 0x30;
  } else if (permil < 100) {
    percent_asc[i++] = permil / 10 + 0x30;
    percent_asc[i++] = permil % 10 + 0x30;
  } else {
    permil = 100;
    percent_asc[i++] = permil / 100 + 0x30;
    percent_asc[i++] = permil % 100 / 10 + 0x30;
    percent_asc[i++] = permil % 10 + 0x30;
  }
  percent_asc[i] = '%';
  oledDrawStringCenter(60, 20, percent_asc, FONT_STANDARD);
}

void layoutProgress(const char *desc, int permil) {
  oledClear();
  oledDrawStringCenter(OLED_WIDTH / 2, OLED_HEIGHT / 2 - 6, desc,
                       FONT_STANDARD);

  // progressbar
  oledBox(2, OLED_HEIGHT - 13, OLED_WIDTH - 2, OLED_HEIGHT - 2, 0);
  oledBox(5, OLED_HEIGHT - 13, OLED_WIDTH - 3, OLED_HEIGHT - 13, 1);
  oledBox(5, OLED_HEIGHT - 3, OLED_WIDTH - 3, OLED_HEIGHT - 3, 1);

  permil = permil * (OLED_WIDTH - 4) / 1000;
  if (permil > OLED_WIDTH - 4) {
    permil = OLED_WIDTH - 4;
  }
  oledBox(2, OLED_HEIGHT - 10, 2, OLED_HEIGHT - 6, 1);
  oledBox(3, OLED_HEIGHT - 12, 4, OLED_HEIGHT - 4, 1);
  if (permil > 3) {
    oledBox(5, OLED_HEIGHT - 12, permil + 1, OLED_HEIGHT - 4, 1);
  }

  oledBox(OLED_WIDTH - 5, OLED_HEIGHT - 13, OLED_WIDTH - 3, OLED_HEIGHT - 3, 0);

  oledBox(OLED_WIDTH - 5, OLED_HEIGHT - 12, OLED_WIDTH - 4, OLED_HEIGHT - 11,
          1);
  oledClearPixel(OLED_WIDTH - 5, OLED_HEIGHT - 11);
  oledBox(OLED_WIDTH - 3, OLED_HEIGHT - 10, OLED_WIDTH - 3, OLED_HEIGHT - 6, 1);
  oledBox(OLED_WIDTH - 5, OLED_HEIGHT - 5, OLED_WIDTH - 4, OLED_HEIGHT - 4, 1);
  oledClearPixel(OLED_WIDTH - 5, OLED_HEIGHT - 5);

  if (permil >= OLED_WIDTH - 6) {
    oledBox(OLED_WIDTH - 5, OLED_HEIGHT - 11, OLED_WIDTH - 5, OLED_HEIGHT - 5,
            1);
    if (permil > OLED_WIDTH - 6) {
      oledBox(OLED_WIDTH - 4, OLED_HEIGHT - 11, OLED_WIDTH - 4, OLED_HEIGHT - 5,
              1);
    }
  }
  oledRefresh();
}

#if !EMULATOR
void disBatteryLevel(uint8_t cur_level) {
  switch (cur_level) {
    case 0:
      oledDrawBitmap(OLED_WIDTH - 16, 0, &bmp_status_battery_0);
      break;
    case 1:
      oledDrawBitmap(OLED_WIDTH - 16, 0, &bmp_status_battery_1);
      break;
    case 2:
      oledDrawBitmap(OLED_WIDTH - 16, 0, &bmp_status_battery_2);
      break;
    case 3:
      oledDrawBitmap(OLED_WIDTH - 16, 0, &bmp_status_battery_3);
      break;
    case 4:
      oledDrawBitmap(OLED_WIDTH - 16, 0, &bmp_status_battery_4);
      break;
    default:
      oledClearBitmap(OLED_WIDTH - 16, 0, &bmp_status_battery_4);
      break;
  }
}

uint8_t layoutStatusLogo(bool force_fresh) {
  static bool nfc_status_bak = false;
  static bool ble_status_bak = false;
  static bool ble_adv_status_bak = false;
  static bool usb_status_bak = false;
  static uint8_t battery_bak = 0xff;
  uint8_t pad = 16;
  bool refresh = false;
  uint8_t ret = 0;

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

  if (sys_nfcState() == true) {
    if (force_fresh || false == nfc_status_bak) {
      nfc_status_bak = true;
      oledDrawBitmap(OLED_WIDTH - 3 * LOGO_WIDTH - pad, 0, &bmp_nfc);
      refresh = true;
    }
  } else if (true == nfc_status_bak) {
    nfc_status_bak = false;
    oledClearBitmap(OLED_WIDTH - 3 * LOGO_WIDTH - pad, 0, &bmp_nfc);
    refresh = true;
  }
  if (sys_bleState() == true) {
    if (force_fresh || false == ble_status_bak) {
      ble_status_bak = true;
      oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - pad, 0,
                     &bmp_status_ble_connect);
      refresh = true;
    }
  } else if (true == ble_status_bak) {
    ble_status_bak = false;
    oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - pad, 0, &bmp_status_ble);
    refresh = true;
    ret = 1;
  } else if (ble_get_switch() == true) {
    if (force_fresh || false == ble_adv_status_bak) {
      ble_adv_status_bak = true;
      oledDrawBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - pad, 0, &bmp_status_ble);
      refresh = true;
    }
  } else if (true == ble_adv_status_bak) {
    ble_adv_status_bak = false;
    oledClearBitmap(OLED_WIDTH - 2 * LOGO_WIDTH - pad, 0, &bmp_status_ble);
    refresh = true;
    ret = 1;
  }
  if (sys_usbState() == false) {
    usb_connect_status = 0;
  }
  if (sys_usbState() == true) {
    if (force_fresh || false == usb_status_bak) {
      usb_status_bak = true;
      oledDrawBitmap(OLED_WIDTH - LOGO_WIDTH - pad, 0, &bmp_status_usb);
      refresh = true;
    }
  } else if (true == usb_status_bak) {
    usb_status_bak = false;
    oledClearBitmap(OLED_WIDTH - LOGO_WIDTH - pad, 0, &bmp_status_usb);
    refresh = true;
  }

  if (battery_bak != battery_cap || force_fresh) {
    battery_bak = battery_cap;
    refresh = true;
    disBatteryLevel(battery_bak);
  }
  if (refresh) oledRefresh();
  return ret;
}

void layoutBlePasskey(uint8_t *passkey) {
  oledClear();
  layoutTitle("Bluetooth Pair");
  oledDrawStringCenter(60, 20, (char *)passkey, FONT_DOUBLE);
  oledDrawStringCenter(60, 40, "Enter pair code on device", FONT_STANDARD);
  oledRefresh();
}

#endif

void layoutFillBleName(uint8_t line) {
  if (line < (OLED_HEIGHT / 8)) {
    if (ble_name_state() == true) {
      oledDrawString(50, 38, ble_get_name(), FONT_STANDARD);
    }
  }
}

void layoutFillBleVersion(uint8_t line) {
  if (line < (OLED_HEIGHT / 8)) {
    if (ble_ver_state())
      oledDrawStringCenter(64, line * 8, ble_get_ver(), FONT_STANDARD);
  }
}

extern void shutdown(void);

void layoutError(const char *line1, const char *line2) {
  layoutDialogCenterAdapterEx(&bmp_icon_error, NULL, NULL, NULL, line1, line2,
                              "Your device", "will reset.");
  shutdown();
}
