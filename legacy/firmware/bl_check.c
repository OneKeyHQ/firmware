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
#include "gettext.h"
#include "layout.h"
#include "memory.h"
#include "util.h"

char bootloader_version[8] = "1.8.9";

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
             "\x33\xe1\x0a\x96\x18\xea\x9c\xd7\x15\x61\x7b\xf5\xc3\x13\x8a\x41"
             "\x89\x34\x96\x97\x59\x72\x1d\x56\x92\xc9\x02\xe7\x96\xa5\xfe\x00",
             32)) {
    memcpy(bootloader_version, "1.10.0", strlen("1.10.0"));
    return 1;  // 1.10.0 shipped with fw 2.11.0
  }
  // END AUTO-GENERATED QA BOOTLOADER ENTRIES (bl_check_qa.txt)

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
             "\x8b\xfb\xb1\xfb\xd6\x41\x21\x9a\xaf\x5e\xf4\x4e\x8b\x6f\x2b\xf9"
             "\xd1\x4b\x21\x56\x4e\x95\xe2\xad\x9d\xd1\x23\x70\x3c\x5f\x16\xd4",
             32)) {
    memcpy(bootloader_version, "1.10.0", strlen("1.10.0"));
    return 1;  // 1.10.0 shipped with fw 2.11.0
  }
  // END AUTO-GENERATED BOOTLOADER ENTRIES (bl_check.txt)
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

  layoutDialog(&bmp_icon_warning, NULL, NULL, NULL, _("Updating bootloader"),
               NULL, NULL, _("DO NOT UNPLUG"), _("YOUR ONEKEY!"), NULL);

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
  shutdown();
#endif
  // prevent compiler warning when PRODUCTION==0
  (void)shutdown_on_replace;
}
