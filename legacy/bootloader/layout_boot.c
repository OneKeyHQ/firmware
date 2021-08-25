#include "layout_boot.h"
#include "prompt.h"

#if ONEKEY_MINI
#include "../atca/se_atca.h"
#include "buttons.h"
#include "device.h"
#include "memory.h"
#include "signatures.h"
#include "util.h"
#include "w25qxx.h"

#define FONT_HEIGHT 8

void layoutBootDevParam(uint8_t index) {
  char *serial;
  int y = 0;
  char desc[33] = "";
  uint8_t jedec_id;

  y = 9;
  oledClear();

  switch (index) {
    case 0:
      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, 0, &bmp_btn_up);

      oledDrawString(0, y, "SERIAL:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      if (device_get_serial(&serial)) {
        oledDrawString(0, y, serial, FONT_STANDARD);
      }

      y += FONT_HEIGHT + 1;
      y += FONT_HEIGHT + 1;
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, "MODAL NAME:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, PRODUCT_STRING, FONT_STANDARD);

      memset(desc, 0, 33);
      const image_header *hdr =
          (const image_header *)FLASH_PTR(FLASH_FWHEADER_START);
      uint2str(hdr->onekey_version, desc);
      y += FONT_HEIGHT + 1;
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, "FIRMWARE:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, desc, FONT_STANDARD);

      y += FONT_HEIGHT + 1;
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, "SE:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, SE_NAME, FONT_STANDARD);

      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, OLED_HEIGHT - 8,
                     &bmp_btn_down);
      break;
    case 1:
      oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, 0, &bmp_btn_up);

      memset(desc, 0, 33);
      if (device_get_cpu_firmware(&cpu, &firmware)) {
        strcat(desc, cpu);
        strcat(desc, "-");
        strcat(desc, firmware); ;
      }
      oledDrawString(0, y, "ST:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, desc, FONT_STANDARD);

      jedec_id = (w25qxx_read_id() >> 16) & 0xff;
      y += FONT_HEIGHT + 1;
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, "FLASH:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      memset(desc, 0, 33);
      if (jedec_id == MF_ID_WB) {
        strcat(desc, "WB-");
      } else if (jedec_id == MF_ID_GD) {
        strcat(desc, "GD-");
      } else {
        // Unknown Manufacturer
        data2hex(&jedec_id, 1, desc);
        strcat(desc, "-");
      }
      strncat(desc, w25qxx_get_desc(), 3);
      oledDrawString(0, y, desc, FONT_STANDARD);

      y += FONT_HEIGHT + 1;
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y, "BOOTLOADER:", FONT_STANDARD);
      y += FONT_HEIGHT + 1;
      oledDrawString(0, y,
                     VERSTR(VERSION_MAJOR) "." VERSTR(VERSION_MINOR) "." VERSTR(
                         VERSION_PATCH),
                     FONT_STANDARD);
      break;
    default:
      break;
  }

  oledRefresh();
}

void layoutBootHome(void) {
  uint8_t key = KEY_NULL;
  static uint8_t index = 0;
  static bool menu = false;
  char buf[64] = "";

  if (layoutNeedRefresh()) {
    strcat(buf, "BOOTLOADER ");
    strcat(buf, VERSTR(VERSION_MAJOR) "." VERSTR(VERSION_MINOR) "." VERSTR(
                    VERSION_PATCH));

    oledClear();
    oledDrawStringCenter(63, 20, PRODUCT_STRING, FONT_STANDARD);
    oledDrawStringCenter(63, 45, buf, FONT_STANDARD);
    oledDrawBitmap(OLED_WIDTH / 2, OLED_HEIGHT - 9, &bmp_btn_down);
    oledRefresh();
    oledBackligthCtl(true);
    index = 0;
    menu = false;
  }
  key = keyScan();
  if (!menu) {
    if (key == KEY_DOWN) {
      layoutBootDevParam(index);
      menu = true;
    }
  } else {
    if (key == KEY_UP || key == KEY_CONFIRM) {
      if (index > 0) {
        index--;
        layoutBootDevParam(index);
      } else {
        layoutRefreshSet(true);
      }
    } else if (key == KEY_DOWN) {
      if (index < 1) {
        index++;
        layoutBootDevParam(index);
      } else {
        layoutRefreshSet(true);
      }
    } else if (key == KEY_CANCEL) {
      layoutRefreshSet(true);
    }
  }
}
#else
#define PRODUCT_STRING "Onekey"

void layoutBootHome(void) {
  if (layoutNeedRefresh()) {
    oledClear();
    oledDrawBitmap(30, 20, &bmp_home_logo);
    oledDrawStringCenter(85, 20, PRODUCT_STRING, FONT_STANDARD);
    oledDrawStringCenter(85, 30, "Bootloader", FONT_STANDARD);
    oledDrawStringCenter(85, 40,
                         VERSTR(VERSION_MAJOR) "." VERSTR(
                             VERSION_MINOR) "." VERSTR(VERSION_PATCH),
                         FONT_STANDARD);
    layoutFillBleName(7);
    oledRefresh();
  }

  static uint32_t system_millis_logo_refresh = 0;
  // 1000 ms refresh
  if ((timer_ms() - system_millis_logo_refresh) >= 1000) {
#if !EMULATOR
    layoutStatusLogo(true);
    system_millis_logo_refresh = timer_ms();
#endif
  }
}
#endif
