/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2018 Pavol Rusnak <stick@satoshilabs.com>
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

#include <libopencm3/stm32/flash.h>
#include <stdint.h>
#include <string.h>
#include "bl_data.h"
#include "buttons.h"
#include "gettext.h"
#include "layout.h"
#include "memory.h"
#include "oled.h"
#include "util.h"

char bootloader_version[8] = "";

#if BOOTLOADER_QA
static int known_bootloader(int r, const uint8_t *hash) {
  if (r != 32) return 0;
  if (0 ==
      memcmp(hash,
             "\xc6\xbf\x87\x35\x34\x4d\x85\xdc\x58\xf5\xb9\x82\x4f\xe6\x28\xb4"
             "\xb2\xe0\xca\x30\x8a\xfb\xd0\x8d\x79\x61\xb6\x7b\x16\x73\x86\x08",
             32)) {
    memcpy(bootloader_version, "1.8.9", strlen("1.8.9"));
    return 1;  // 1.8.9 mini_boot_1010
  }
  if (0 ==
      memcmp(hash,
             "\x76\xee\xcb\x0a\xba\x6d\xa2\xb3\x93\x87\x38\xe1\x90\x7f\x86\x87"
             "\x66\x95\xa9\xb6\x75\x79\xae\x18\x94\x93\x57\x01\x9c\x93\xa7\xbd",
             32)) {
    memcpy(bootloader_version, "1.9.0", strlen("1.9.0"));
    return 1;  // 1.9.0
  }
  if (0 ==
      memcmp(hash,
             "\x1b\x5e\x73\xfa\x8a\xb6\x71\xc4\x2d\xb6\x91\xe7\x46\xe2\x13\xe7"
             "\x46\x68\x79\x91\x51\xbe\x6a\x52\xc5\x3e\xbc\x3c\xf8\x09\x72\x69",
             32)) {
    memcpy(bootloader_version, "1.9.1", strlen("1.9.1"));
    return 1;  // 1.9.0
  }
  if (0 ==
      memcmp(hash,
             "\x48\x35\xf1\x44\xbd\x04\x44\x91\xbe\xc6\xbd\xb7\x16\xfc\x0b\xcd"
             "\x2f\xc4\xd0\x43\x8f\x29\x29\xae\x9b\xf4\x10\xc4\xb5\x73\xe9\xcd",
             32)) {
    memcpy(bootloader_version, "1.9.2", strlen("1.9.2"));
    return 1;  // 1.9.2
  }
  // BEGIN AUTO-GENERATED QA BOOTLOADER ENTRIES (bl_check_qa.txt)
  if (0 ==
      memcmp(hash,
             "\xb9\xdb\x20\x23\x4a\xa7\xa9\xe5\x50\x89\x6f\xaa\x4a\xee\xdf\x6f"
             "\x9b\x67\x11\xdd\x12\xca\x08\x67\x84\x72\xd3\x2d\xda\x6f\x9d\xbe",
             32)) {
    memcpy(bootloader_version, "2.0.0", strlen("2.0.0"));
    return 1;  // 2.0.0 shipped with fw 2.11.0
  }
  // END AUTO-GENERATED QA BOOTLOADER ENTRIES (bl_check_qa.txt)
  memcpy(bootloader_version, "unknown", strlen("unknown"));
  return 0;
}
#endif

#if PRODUCTION
static int known_bootloader(int r, const uint8_t *hash) {
  if (r != 32) return 0;
  if (0 ==
      memcmp(hash,
             "\xc6\xbf\x87\x35\x34\x4d\x85\xdc\x58\xf5\xb9\x82\x4f\xe6\x28\xb4"
             "\xb2\xe0\xca\x30\x8a\xfb\xd0\x8d\x79\x61\xb6\x7b\x16\x73\x86\x08",
             32)) {
    memcpy(bootloader_version, "1.8.9", strlen("1.8.9"));
    return 1;  // 1.8.9 mini_boot_1010
  }
  if (0 ==
      memcmp(hash,
             "\x76\xee\xcb\x0a\xba\x6d\xa2\xb3\x93\x87\x38\xe1\x90\x7f\x86\x87"
             "\x66\x95\xa9\xb6\x75\x79\xae\x18\x94\x93\x57\x01\x9c\x93\xa7\xbd",
             32)) {
    memcpy(bootloader_version, "1.9.0", strlen("1.9.0"));
    return 1;  // 1.9.0
  }
  if (0 ==
      memcmp(hash,
             "\x1b\x5e\x73\xfa\x8a\xb6\x71\xc4\x2d\xb6\x91\xe7\x46\xe2\x13\xe7"
             "\x46\x68\x79\x91\x51\xbe\x6a\x52\xc5\x3e\xbc\x3c\xf8\x09\x72\x69",
             32)) {
    memcpy(bootloader_version, "1.9.1", strlen("1.9.1"));
    return 1;  // 1.9.0
  }
  if (0 ==
      memcmp(hash,
             "\x48\x35\xf1\x44\xbd\x04\x44\x91\xbe\xc6\xbd\xb7\x16\xfc\x0b\xcd"
             "\x2f\xc4\xd0\x43\x8f\x29\x29\xae\x9b\xf4\x10\xc4\xb5\x73\xe9\xcd",
             32)) {
    memcpy(bootloader_version, "1.9.2", strlen("1.9.2"));
    return 1;  // 1.9.2
  }
  // BEGIN AUTO-GENERATED BOOTLOADER ENTRIES (bl_check.txt)
  if (0 ==
      memcmp(hash,
             "\xd3\x27\xbb\x1a\x44\xa9\x21\xe7\xd6\xae\x87\xda\x3c\xb6\xcc\x84"
             "\xdb\x7a\x7b\xce\x94\x3f\x1d\xd7\x01\xbd\x8c\x81\xaf\xd9\x74\xef",
             32)) {
    memcpy(bootloader_version, "2.0.0", strlen("2.0.0"));
    return 1;  // 2.0.0 shipped with fw 2.11.0
  }
  // END AUTO-GENERATED BOOTLOADER ENTRIES (bl_check.txt)
  memcpy(bootloader_version, "unknown", strlen("unknown"));
  return 0;
}

#endif

/**
 * If bootloader is older and known, replace with newer bootloader.
 * If bootloader is unknown, halt with error message.
 *
 * @param shutdown_on_replace: if true, shuts down device instead of return
 */
void check_and_replace_bootloader(bool shutdown_on_replace) {
#if PRODUCTION || BOOTLOADER_QA
  uint8_t hash[32] = {0};
  int r = memory_bootloader_hash(hash);

  if (!known_bootloader(r, hash)) {
    layoutDialog(&bmp_icon_error, NULL, NULL, NULL, _("Unknown bootloader"),
                 _("detected."), NULL, _("Shutdown your OneKey"),
                 _("contact our support."), NULL);
    delay_ms(1000);
    shutdown();
  }

  if (is_mode_unprivileged()) {
    return;
  }

  if (r == 32 && 0 == memcmp(hash, bl_hash, 32)) {
    // all OK -> done
    return;
  }

  // ENABLE THIS AT YOUR OWN RISK
  // ATTEMPTING TO OVERWRITE BOOTLOADER WITH UNSIGNED FIRMWARE MAY BRICK
  // YOUR DEVICE.

  layoutDialog2(&bmp_icon_warning, NULL, NULL, NULL, NULL,
                _("DO NOT power off during"), _("update,or it may cause"),
                _("irreversible malfunction"), NULL, NULL);

  char delay_str[4] = "3s";
  for (int i = 2; i >= 0; i--) {
    oledclearLine(15);
    oledRefresh();
    delay_str[0] = '1' + i;
    oledDrawStringCenter(OLED_WIDTH / 2, 120, delay_str, FONT_STANDARD);
    oledRefresh();
    delay_ms(1000);
  }

  // unlock sectors
  memory_write_unlock();

  for (int tries = 0; tries < 10; tries++) {
    // replace bootloader
    flash_wait_for_last_operation();
    flash_clear_status_flags();
    flash_unlock();
    for (int i = FLASH_BOOT_SECTOR_FIRST; i <= FLASH_BOOT_SECTOR_LAST; i++) {
      flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
    }
    for (int i = 0; i < FLASH_BOOT_LEN / 4; i++) {
      const uint32_t *w = (const uint32_t *)(bl_data + i * 4);
      flash_program_word(FLASH_BOOT_START + i * 4, *w);
    }
    flash_wait_for_last_operation();
    flash_lock();
    // check whether the write was OK
    r = memory_bootloader_hash(hash);
    if (r == 32 && 0 == memcmp(hash, bl_hash, 32)) {
      if (shutdown_on_replace) {
        // OK -> show info and halt
        layoutDialog(&bmp_icon_info, NULL, NULL, NULL, _("Update finished"),
                     _("successfully."), NULL, _("Please reconnect"),
                     _("the device."), NULL);
        shutdown();
      }
      return;
    }
  }
  // show info and halt
  layoutDialog(&bmp_icon_error, NULL, NULL, NULL, _("Bootloader update"),
               _("broken."), NULL, _("Unplug your OneKey"),
               _("contact our support."), NULL);
  delay_ms(1000);
  shutdown();
#endif
  // prevent compiler warning when PRODUCTION==0
  (void)shutdown_on_replace;
}
