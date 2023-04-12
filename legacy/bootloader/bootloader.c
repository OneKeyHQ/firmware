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

#include <string.h>

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "bootloader.h"
#include "buttons.h"
#include "compiler_traits.h"
#include "fw_signatures.h"
#include "layout.h"
#include "layout_boot.h"
#include "memory.h"
#include "oled.h"
#include "rng.h"
#include "setup.h"
#include "supervise.h"
#include "sys.h"
#include "usb.h"
#include "util.h"

#if ONEKEY_MINI
#include "atca_api.h"
#include "atca_hal.h"
#include "device.h"
#include "w25qxx.h"
#endif

void layoutFirmwareFingerprint(const uint8_t *hash) {
  char str[4][17] = {0};
  for (int i = 0; i < 4; i++) {
    data2hex(hash + i * 8, 8, str[i]);
  }
  layoutDialog(&bmp_icon_question, "Abort", "Continue", "Compare fingerprints",
               str[0], str[1], str[2], str[3], NULL, NULL);
}

bool get_button_response(void) {
  do {
    delay(100000);
    buttonUpdate();
  } while (!button.YesUp && !button.NoUp);
  return button.YesUp;
}

void show_halt(const char *line1, const char *line2) {
  layoutDialog(&bmp_icon_error, NULL, NULL, NULL, line1, line2, NULL,
               "Your device", "will be poweroff.", NULL);
  delay_ms(1000);
  shutdown();
}

void show_unplug(const char *line1, const char *line2) {
  layoutDialog(&bmp_icon_ok, NULL, NULL, NULL, line1, line2, NULL,
               "Your device", "will be poweroff.", NULL);
  delay_ms(1000);
}

#if !ONEKEY_MINI
static void show_unofficial_warning(const uint8_t *hash) {
// On production bootloader, show warning and wait for user
// to accept or reject it
// On non-production we only use unofficial firmwares,
// so just show hash for a while to see bootloader started
// but continue
#if PRODUCTION
  layoutDialog(&bmp_icon_warning, "Abort", "I'll take the risk", NULL,
               "WARNING!", NULL, "Unofficial firmware", "detected.", NULL,
               NULL);

  bool but = get_button_response();
  if (!but) {  // no button was pressed -> halt
    show_halt("Unofficial firmware", "aborted.");
  }

  layoutFirmwareFingerprint(hash);

  but = get_button_response();
  if (!but) {  // no button was pressed -> halt
    show_halt("Unofficial firmware", "aborted.");
  }

  // everything is OK, user pressed 2x Continue -> continue program
#else
  layoutFirmwareFingerprint(hash);
  delay(100000000);
#endif
}
#endif

static void __attribute__((noreturn)) load_app(int signed_firmware) {
  // zero out SRAM
  memset_reg(_ram_start, _ram_end, 0);

  jump_to_firmware((const vector_table_t *)FLASH_PTR(FLASH_APP_START),
                   signed_firmware);
}

static void bootloader_loop(void) { usbLoop(); }

#define DBGMCU_IDCODE 0xE0042000U
const char *cpu_info;

int main(void) {
  // grab "stay in bootloader" flag as soon as possible
  register uint32_t r11 __asm__("r11");
  volatile uint32_t stay_in_bootloader_flag = r11;

  static bool force_boot = false;
  if (memcmp((uint8_t *)(ST_RAM_END - 4), "boot", 4) == 0) {
    force_boot = true;
  }

  if (stay_in_bootloader_flag == STAY_IN_BOOTLOADER_FLAG) {
    force_boot = true;
  }

#ifndef APPVER
  setup();
#endif
  __stack_chk_guard = random32();  // this supports compiler provided
                                   // unpredictable stack protection checks
#ifndef APPVER
  oledInit();
#if ONEKEY_MINI
  oledClear();
  // fix starting flower screen
  oledDrawBitmap(52, 52, &bmp_boot_icon);
  oledRefresh();
  oledBackligthCtl(true);
#endif
#if ONEKEY_MINI
  device_init();
  w25qxx_init();
  atca_init();
#else
  sys_poweron();
  buttonsIrqInit();
  timer_init();
  register_timer("button", timer1s / 2, buttonsTimer);
#endif

  memory_protect();

  uint32_t idcode = *(uint32_t *)DBGMCU_IDCODE & 0xFFF;
  if (idcode == 0x411) {
    cpu_info = "STM32F2XX";
  } else if (idcode == 0x413) {
    cpu_info = "STM32F4XX";
  } else {
    cpu_info = "unknown-";
  }

#endif
  mpu_config_bootloader();
#ifndef APPVER

#if ONEKEY_MINI
  bool boot_button_pressed = (buttonRead() & BTN_PIN_FUNC) == 0;

  if (!boot_button_pressed && !force_boot) {
    update_from_spi_flash();
  }
#else
  bool boot_button_pressed = (buttonRead() & BTN_PIN_DOWN) == 0;
#endif

  if (firmware_present_new() && !boot_button_pressed && !force_boot) {
    oledClear();
    oledDrawBitmap(52, 52, &bmp_boot_icon);
    oledRefresh();
    const image_header *hdr =
        (const image_header *)FLASH_PTR(FLASH_FWHEADER_START);

    uint8_t fingerprint[32] = {0};
    int signed_firmware = signatures_match(hdr, fingerprint);
    if (SIG_OK != signed_firmware) {
      show_halt("Unofficial firmware", "aborted.");
    }

    if (SIG_OK != check_firmware_hashes(hdr)) {
      show_halt("Broken firmware", "detected.");
    }
    mpu_config_off();

    load_app(signed_firmware);
  }
#endif

  layoutBootHome();
  bootloader_loop();

  return 0;
}
