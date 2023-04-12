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
#include "ble.h"
#include "buttons.h"
#include "gettext.h"
#include "layout.h"
#include "memory.h"
#include "util.h"

char bootloader_version[8] = {0};

#if BOOTLOADER_QA
static int known_bootloader(int r, const uint8_t *hash) {
  if (r != 32) return 0;
  if (0 ==
      memcmp(hash,
             "\xe5\x83\x74\x1b\xb0\x53\xf2\x29\x29\xf5\x6c\x6f\xaf\xff\xea\xe9"
             "\xae\x96\x16\x67\xbf\xa5\xf2\x1b\x3d\x51\x8d\xc5\x52\x71\x59\x99",
             32)) {
    memcpy(bootloader_version, "1.8.3", strlen("1.8.3"));
    return 1;  // 1.8.3
  }

  if (0 ==
      memcmp(hash,
             "\xb5\x32\xd7\x5a\x3c\x38\x5d\x73\xba\x58\xb8\x29\x91\xe8\x36\xd1"
             "\x26\xea\xb4\x5b\xb3\x87\x10\x0b\xc6\xb4\xf7\x48\x05\xb0\x9f\xb0",
             32)) {
    memcpy(bootloader_version, "1.8.5", strlen("1.8.5"));
    return 1;  // 1.8.5
  }
  if (0 ==
      memcmp(hash,
             "\xbf\x2e\x53\xd3\xcb\x0b\x1a\xf2\x39\xc0\x74\xa4\x1e\x86\xf1\x47"
             "\xc4\x64\x95\xdf\x0d\x13\x9a\xd9\x56\x47\xc3\xf4\x6a\x66\x35\xa0",
             32)) {
    memcpy(bootloader_version, "1.8.5", strlen("1.8.5"));
    return 1;  // 1.8.5
  }
  if (0 ==
      memcmp(hash,
             "\xae\xdc\x2f\x7e\xb5\xbc\x6a\x22\x49\xed\x35\xc3\x1f\x9d\x1e\x8d"
             "\x55\x57\xa3\x0e\xc6\xd6\x36\xa6\xba\x9b\x3e\x92\xc9\x6d\x58\x29",
             32)) {
    memcpy(bootloader_version, "1.8.6", strlen("1.8.6"));
    return 1;  // 1.8.6
  }
  if (0 ==
      memcmp(hash,
             "\x28\xb1\xaa\x4b\x74\xd1\xdb\xda\xbe\xff\x5b\x24\x8b\xc0\x5f\x97"
             "\xca\x3a\x49\x07\x15\xfd\xa6\xca\x73\xc7\xf7\x6a\xb8\xa8\x49\xb9",
             32)) {
    memcpy(bootloader_version, "1.8.7", strlen("1.8.7"));
    return 1;  // 1.8.7
  }
  if (0 ==
      memcmp(hash,
             "\xa7\xb1\xed\xcc\x56\x94\x07\xe8\xaa\x4f\xfa\x60\x5a\xdb\xb9\xa8"
             "\xb2\x4c\x05\x4f\xe1\xac\xca\x56\xf0\x81\x5c\x9c\x02\xee\x71\xc6",
             32)) {
    memcpy(bootloader_version, "1.8.8", strlen("1.8.8"));
    return 1;  // 1.8.8
  }
  if (0 ==
      memcmp(hash,
             "\x93\x6a\xc4\x1c\xf0\x04\x85\xa0\xa2\xd0\x11\x9e\xc4\x64\x24\x59"
             "\xa5\x6a\x13\x84\xd2\x9e\x22\x2c\x42\x94\x82\x9a\x92\x40\xf8\xe2",
             32)) {
    memcpy(bootloader_version, "1.9.0", strlen("1.9.0"));
    return 1;  // 1.9.0
  }

  // BEGIN AUTO-GENERATED QA BOOTLOADER ENTRIES (bl_check_qa.txt)
  if (0 ==
      memcmp(hash,
             "\x8c\xce\xa2\x13\xba\x7b\x8e\x06\x62\x95\xd4\xac\x44\x6f\x71\x4f"
             "\xe5\x62\x93\x03\x06\xef\xd1\xe5\x2d\xad\x98\x38\x79\x6d\x57\xe2",
             32)) {
    memcpy(bootloader_version, "2.0.0", strlen("2.0.0"));
    return 1;  // 2.0.0 shipped with fw 3.0.0
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
             "\xe5\x83\x74\x1b\xb0\x53\xf2\x29\x29\xf5\x6c\x6f\xaf\xff\xea\xe9"
             "\xae\x96\x16\x67\xbf\xa5\xf2\x1b\x3d\x51\x8d\xc5\x52\x71\x59\x99",
             32)) {
    memcpy(bootloader_version, "1.8.3", strlen("1.8.3"));
    return 1;  // 1.8.3
  }

  if (0 ==
      memcmp(hash,
             "\xb5\x32\xd7\x5a\x3c\x38\x5d\x73\xba\x58\xb8\x29\x91\xe8\x36\xd1"
             "\x26\xea\xb4\x5b\xb3\x87\x10\x0b\xc6\xb4\xf7\x48\x05\xb0\x9f\xb0",
             32)) {
    memcpy(bootloader_version, "1.8.5", strlen("1.8.5"));
    return 1;  // 1.8.5
  }
  if (0 ==
      memcmp(hash,
             "\xbf\x2e\x53\xd3\xcb\x0b\x1a\xf2\x39\xc0\x74\xa4\x1e\x86\xf1\x47"
             "\xc4\x64\x95\xdf\x0d\x13\x9a\xd9\x56\x47\xc3\xf4\x6a\x66\x35\xa0",
             32)) {
    memcpy(bootloader_version, "1.8.5", strlen("1.8.5"));
    return 1;  // 1.8.5
  }
  if (0 ==
      memcmp(hash,
             "\xae\xdc\x2f\x7e\xb5\xbc\x6a\x22\x49\xed\x35\xc3\x1f\x9d\x1e\x8d"
             "\x55\x57\xa3\x0e\xc6\xd6\x36\xa6\xba\x9b\x3e\x92\xc9\x6d\x58\x29",
             32)) {
    memcpy(bootloader_version, "1.8.6", strlen("1.8.6"));
    return 1;  // 1.8.6
  }
  if (0 ==
      memcmp(hash,
             "\x28\xb1\xaa\x4b\x74\xd1\xdb\xda\xbe\xff\x5b\x24\x8b\xc0\x5f\x97"
             "\xca\x3a\x49\x07\x15\xfd\xa6\xca\x73\xc7\xf7\x6a\xb8\xa8\x49\xb9",
             32)) {
    memcpy(bootloader_version, "1.8.7", strlen("1.8.7"));
    return 1;  // 1.8.7
  }
  if (0 ==
      memcmp(hash,
             "\xa7\xb1\xed\xcc\x56\x94\x07\xe8\xaa\x4f\xfa\x60\x5a\xdb\xb9\xa8"
             "\xb2\x4c\x05\x4f\xe1\xac\xca\x56\xf0\x81\x5c\x9c\x02\xee\x71\xc6",
             32)) {
    memcpy(bootloader_version, "1.8.8", strlen("1.8.8"));
    return 1;  // 1.8.8
  }
  if (0 ==
      memcmp(hash,
             "\x93\x6a\xc4\x1c\xf0\x04\x85\xa0\xa2\xd0\x11\x9e\xc4\x64\x24\x59"
             "\xa5\x6a\x13\x84\xd2\x9e\x22\x2c\x42\x94\x82\x9a\x92\x40\xf8\xe2",
             32)) {
    memcpy(bootloader_version, "1.9.0", strlen("1.9.0"));
    return 1;  // 1.9.0
  }
  // BEGIN AUTO-GENERATED BOOTLOADER ENTRIES (bl_check.txt)
  if (0 ==
      memcmp(hash,
             "\xed\xf1\x9e\x2f\x39\x0c\xb5\xb0\xe7\x32\xb0\x9c\xc7\xb4\xb6\x11"
             "\xea\xf3\x26\x2a\xec\x8c\x21\x18\x30\x72\xe9\x00\xa5\x01\x1b\xdb",
             32)) {
    memcpy(bootloader_version, "2.0.0", strlen("2.0.0"));
    return 1;  // 2.0.0 shipped with fw 3.0.0
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

  if (sys_usbState() == false && battery_cap == 0xff) {
    layoutDialogCenterAdapterEx(NULL, NULL, NULL, NULL,
                                _("Get battery level..."), NULL, NULL, NULL);
    while (1) {
      if (battery_cap == 0xff) {
        ble_request_info(BLE_CMD_BATTERY);
      } else {
        break;
      }
      if (sys_usbState() == true) {
        break;
      }
      delay_ms(5);
    }
  }

  if (sys_usbState() == false && battery_cap < 2) {
    layoutDialogCenterAdapterEx(
        &bmp_icon_warning, NULL, &bmp_bottom_right_confirm, NULL,
        _("Low Battery!Use cable or"), _("Charge to 25% before"),
        _("updating the bootloader"), NULL);
    while (1) {
      uint8_t key = keyScan();
      if (key == KEY_CONFIRM) {
        return;
      }
      if (sys_usbState() == true) {
        break;
      }
    }
  }

  // ENABLE THIS AT YOUR OWN RISK
  // ATTEMPTING TO OVERWRITE BOOTLOADER WITH UNSIGNED FIRMWARE MAY BRICK
  // YOUR DEVICE.

  layoutDialogCenterAdapterEx(
      &bmp_icon_warning, &bmp_bottom_left_close, &bmp_bottom_right_confirm,
      NULL, _("DO NOT power off during"), _("update,or it may cause"),
      _("irreversible malfunction"), NULL);

  while (1) {
    uint8_t key = keyScan();
    if (key == KEY_CONFIRM) {
      break;
    } else if (key == KEY_CANCEL) {
      return;
    }
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
