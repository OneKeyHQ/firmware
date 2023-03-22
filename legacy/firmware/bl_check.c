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

char bootloader_version[8] = {0};

#if BOOTLOADER_QA
static int known_bootloader(int r, const uint8_t *hash) {
  if (r != 32) return 0;
  if (0 ==
      memcmp(hash,
             "\x1b\xd8\x1c\x0a\x82\x20\x43\x28\xe9\xaf\x7b\xb6\xb1\x6b\x45\x27"
             "\x81\x31\xcc\x76\x0a\xfa\x9f\xd0\x81\x6e\xc1\xe1\x12\x86\x49\x83",
             32))
    return 1;  // 1.3.2
  if (0 ==
      memcmp(hash,
             "\x19\x27\x59\xce\x15\x51\xc4\x21\x03\xcc\x9b\xe8\x4e\x06\x54\x24"
             "\x68\x8a\x06\xfa\xbb\xe6\xb0\x19\x18\x11\xcd\xbc\x19\x6c\x2f\x1b",
             32))
    return 1;  // 1.3.3
  if (0 ==
      memcmp(hash,
             "\x09\x34\x2f\x51\x56\x58\xc1\xe5\x95\x64\x1c\x4f\x8d\xf1\xce\x2b"
             "\x2b\x6a\x86\x39\x14\x35\x0a\x97\x0d\x6e\x4f\x37\xb1\xd7\xde\x65",
             32))
    return 1;  // 1.4.0
  if (0 ==
      memcmp(hash,
             "\x73\x37\xfa\x82\x2b\x62\x90\x50\x49\x3c\x6c\x39\x3a\x9f\x17\x41"
             "\x75\xe8\x1b\x39\xf3\xb7\x1b\xff\xab\xa2\xb9\x79\x7f\xf1\x0a\x2b",
             32))
    return 1;  // 1.5.0
  if (0 ==
      memcmp(hash,
             "\x5c\x8b\x5e\xfc\x8d\x8f\x78\xf3\xa0\x23\x9d\x5f\x90\x95\x4d\xe7"
             "\xe6\xb5\xb7\x09\xc9\x1c\x3a\xb2\xaa\x14\x6f\x0e\x26\x6e\x70\x60",
             32))
    return 1;  // 1.5.1
  if (0 ==
      memcmp(hash,
             "\xec\xbd\x06\xd3\x37\x25\xd5\xa6\xbe\xba\x2b\xe3\xde\xf4\x64\x7e"
             "\xef\x5d\x7b\xb9\x2b\x0c\x19\x8d\xd2\x89\x47\x3b\xad\xd3\x4c\x97",
             32))
    return 1;  // 1.6.0
  if (0 ==
      memcmp(hash,
             "\xec\xca\xff\x24\x34\xdf\x3b\x49\xef\x00\x0c\x0f\x07\x1f\xa5\x60"
             "\x18\x16\xfa\x19\x56\x0b\x23\xb4\x73\x52\x0e\x68\xc5\x2d\xe5\x7a",
             32))
    return 1;  // 1.6.1
  if (0 ==
      memcmp(hash,
             "\xe0\xc7\xfd\xb9\x1a\x14\xcb\x39\xd7\xc1\x43\xff\x70\xd2\x3a\x0b"
             "\xfb\x0a\xef\xb5\x49\xb6\x15\x0a\xa9\x09\xf8\x35\x0a\xa5\xeb\xa2",
             32))
    return 1;  // 1.8.0
  if (0 ==
      memcmp(hash,
             "\xf9\x9f\x49\xf8\xd0\xc3\xfa\x82\xd6\xad\x1e\xf4\xf2\xf2\xd7\x2d"
             "\x01\xa5\xdc\xa3\x6f\x11\xba\xb9\x13\xbd\xfe\xaf\x84\xb2\x6f\x4a",
             32))
    return 1;  // 1.10.0
  if (0 ==
      memcmp(hash,
             "\x8f\x83\x57\x70\x11\x75\xaf\x5c\x1a\xe5\xb9\x6f\x13\x42\x2f\x7f"
             "\x1a\x52\xed\x96\xcd\xa0\x18\x07\x63\x0e\x0d\x25\x6c\xd9\x18\x78",
             32))
    return 1;  // 1.11.0
  // BEGIN AUTO-GENERATED QA BOOTLOADER ENTRIES (bl_check_qa.txt)
  if (0 ==
      memcmp(hash,
             "\x21\xb9\x49\xf4\xf5\xfd\x9f\x3a\x7d\x63\x43\xd1\x07\x6f\x96\x0f"
             "\xb4\x54\x18\x19\x65\x31\xb9\xf2\x97\x4a\x68\xed\xe8\xdb\x2e\xa1",
             32))
    return 1;  // 1.12.0 shipped with fw 1.12.0
  if (0 ==
      memcmp(hash,
             "\xb4\xe5\x92\x44\x18\x5c\xe1\xcd\x6c\x8f\x59\x03\x10\x37\x02\x18"
             "\x50\x9c\x39\x32\x26\xf0\x07\x4b\x8c\xf6\xad\xed\xb3\xcd\x4d\x55",
             32))
    return 1;  // 1.12.1 shipped with fw 1.12.1
  // END AUTO-GENERATED QA BOOTLOADER ENTRIES (bl_check_qa.txt)
  return 0;
}
#endif

#if PRODUCTION

#if 0
static int known_bootloader(int r, const uint8_t *hash) {
  if (r != 32) return 0;
  if (0 ==
      memcmp(hash,
             "\xbf\x72\xe2\x5e\x2c\x2f\xc1\xba\x57\x04\x50\xfa\xdf\xb6\x6f\xaa"
             "\x5a\x71\x6d\xcd\xc0\x33\x35\x88\x55\x7b\x77\x54\x0a\xb8\x7e\x98",
             32))
    return 1;  // 1.2.0a
  if (0 ==
      memcmp(hash,
             "\x77\xb8\xe2\xf2\x5f\xaa\x8e\x8c\x7d\x9f\x5b\x32\x3b\x27\xce\x05"
             "\x6c\xa3\xdb\xc2\x3f\x56\xc3\x7e\xe3\x3f\x97\x7c\xa6\xeb\x4d\x3e",
             32))
    return 1;  // 1.2.0b
  if (0 ==
      memcmp(hash,
             "\xc4\xc3\x25\x39\xb4\xa0\x25\xa8\xe7\x53\xa4\xc4\x62\x64\x28\x59"
             "\x11\xa4\x5f\xcb\x14\xf4\x71\x81\x79\xe7\x11\xb1\xce\x99\x05\x24",
             32))
    return 1;  // 1.2.5
  if (0 ==
      memcmp(hash,
             "\x42\x59\x66\x94\xa0\xf2\x9d\x1e\xc2\x35\x71\x29\x2d\x54\x39\xd8"
             "\x2f\xa1\x8c\x07\x37\xcb\x10\x7e\x98\xf6\x1e\xf5\x93\x4d\xe7\x16",
             32))
    return 1;  // 1.3.0a
  if (0 ==
      memcmp(hash,
             "\x3a\xcf\x2e\x51\x0b\x0f\xe1\x56\xb5\x58\xbb\xf7\x9c\x7e\x48\x5e"
             "\xb0\x26\xe5\xe0\x8c\xb4\x4d\x15\x2d\x44\xd6\x4e\x0c\x6a\x41\x37",
             32))
    return 1;  // 1.3.0b
  if (0 ==
      memcmp(hash,
             "\x15\x85\x21\x5b\xc6\xe5\x5a\x34\x07\xa8\xb3\xee\xe2\x79\x03\x4e"
             "\x95\xb9\xc4\x34\x00\x33\xe1\xb6\xae\x16\x0c\xe6\x61\x19\x67\x15",
             32))
    return 1;  // 1.3.1
  if (0 ==
      memcmp(hash,
             "\x76\x51\xb7\xca\xba\x5a\xae\x0c\xc1\xc6\x5c\x83\x04\xf7\x60\x39"
             "\x6f\x77\x60\x6c\xd3\x99\x0c\x99\x15\x98\xf0\xe2\x2a\x81\xe0\x07",
             32))
    return 1;  // 1.3.2
  // note to those verifying these values: bootloader versions above this
  // comment are aligned/padded to 32KiB with trailing 0xFF bytes and versions
  // below are padded with 0x00.
  //
  // for more info, refer to "make -C bootloader align" and
  // "firmware/bl_data.py".
  if (0 ==
      memcmp(hash,
             "\x8c\xe8\xd7\x9e\xdf\x43\x0c\x03\x42\x64\x68\x6c\xa9\xb1\xd7\x8d"
             "\x26\xed\xb2\xac\xab\x71\x39\xbe\x8f\x98\x5c\x2a\x3c\x6c\xae\x11",
             32))
    return 1;  // 1.3.3
  if (0 ==
      memcmp(hash,
             "\x63\x30\xfc\xec\x16\x72\xfa\xd3\x0b\x42\x1b\x60\xf7\x4f\x83\x9a"
             "\x39\x39\x33\x45\x65\xcb\x70\x3b\x2b\xd7\x18\x2e\xa2\xdd\xa0\x19",
             32))
    return 1;  // 1.4.0 shipped with fw 1.6.1
  if (0 ==
      memcmp(hash,
             "\xaf\xb4\xcf\x7a\x4a\x57\x96\x10\x0e\xd5\x41\x6b\x75\x12\x1b\xc7"
             "\x10\x08\xc2\xa2\xfd\x54\x49\xbd\x8f\x63\xcc\x22\xa6\xa7\xd6\x80",
             32))
    return 1;  // 1.5.0 shipped with fw 1.6.2
  if (0 ==
      memcmp(hash,
             "\x51\x12\x90\xa8\x72\x3f\xaf\xe7\x34\x15\x25\x9d\x25\x96\x76\x54"
             "\x06\x32\x5c\xe2\x4b\x4b\x80\x03\x2c\x0b\x70\xb0\x5d\x98\x46\xe9",
             32))
    return 1;  // 1.5.1 shipped with fw 1.6.3
  if (0 ==
      memcmp(hash,
             "\x3e\xc4\xbd\xd5\x77\xea\x0c\x36\xc7\xba\xb7\xb9\xa3\x5b\x87\x17"
             "\xb3\xf1\xfc\x2f\x80\x9e\x69\x0c\x8a\xbe\x5b\x05\xfb\xc2\x43\xc6",
             32))
    return 1;  // 1.6.0 shipped with fw 1.7.0
  if (0 ==
      memcmp(hash,
             "\x8e\x83\x02\x3f\x0d\x4f\x82\x4f\x64\x71\x20\x75\x2b\x6c\x71\x6f"
             "\x55\xd7\x95\x70\x66\x8f\xd4\x90\x65\xd5\xb7\x97\x6e\x7a\x6e\x19",
             32))
    return 1;  // 1.6.0 shipped with fw 1.7.1 and 1.7.2
  if (0 ==
      memcmp(hash,
             "\xa2\x36\x6e\x77\xde\x8e\xfd\xfd\xc9\x99\xf4\x72\x20\xc0\x16\xe3"
             "\x3f\x6d\x24\x24\xe2\x45\x90\x79\x11\x7a\x90\xb3\xa8\x88\xba\xdd",
             32))
    return 1;  // 1.6.1 shipped with fw 1.7.3
  if (0 ==
      memcmp(hash,
             "\xf7\xfa\x16\x5b\xe6\xd7\x80\xf3\xe1\xaf\x00\xab\xc0\x7d\xf8\xb3"
             "\x07\x6b\xcd\xad\x72\xd7\x0d\xa2\x2a\x63\xd8\x89\x6b\x63\x91\xd8",
             32))
    return 1;  // 1.8.0 shipped with fw 1.8.0 and 1.8.1
  if (0 ==
      memcmp(hash,
             "\x74\x47\xa4\x17\x17\x02\x2e\x3e\xb3\x20\x11\xb0\x0b\x2a\x68\xeb"
             "\xb9\xc7\xf6\x03\xcd\xc7\x30\xe7\x30\x78\x50\xa3\xf4\xd6\x2a\x5c",
             32))
    return 1;  // 1.10.0 shipped with fw 1.10.0
  if (0 ==
      memcmp(hash,
             "\xfa\x12\xa4\x4f\xa0\x5f\xd1\xd2\x05\x39\x35\x8b\x54\xf3\x01\xce"
             "\xe4\xc3\x21\x9c\x9f\x1b\xb3\xa5\x77\x2f\xfd\x60\x9a\xf9\xe8\xe2",
             32))
    return 1;  // 1.11.0 shipped with fw 1.11.1
  // BEGIN AUTO-GENERATED BOOTLOADER ENTRIES (bl_check.txt)
  if (0 ==
      memcmp(hash,
             "\xe9\xec\x8f\xa2\xfe\xfa\xd2\xb3\xb6\xb7\xc4\xab\x76\x69\x1a\x33"
             "\x61\xb3\xee\xfb\x8a\x0d\xda\xa3\x4d\x7e\x45\xb6\x3b\x3d\x56\x64",
             32))
    return 1;  // 1.12.0 shipped with fw 1.12.0
  if (0 ==
      memcmp(hash,
             "\x94\xf1\xc9\x0d\xb2\x8d\xb1\xf8\xce\x5d\xca\x96\x69\x76\x34\x36"
             "\x58\xf5\xda\xde\xe8\x38\x34\x98\x7c\x8b\x04\x9c\x49\xd1\xed\xd0",
             32))
    return 1;  // 1.12.1 shipped with fw 1.12.1
  if (0 ==
      memcmp(hash,
             "\x38\x83\x85\xb2\x55\xf3\x80\x07\x70\xb2\xc4\x47\x65\x8c\x4d\xd0"
             "\xa7\x43\xa7\x80\xbf\x3a\xcb\xe8\xc6\xe1\xc8\x1f\x8d\xfd\x75\x90",
             32))
    return 1;  // 1.9.0 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\x7e\xa1\xd0\x17\x19\x15\x64\xc0\xc4\x06\x10\xc1\x5b\xa8\x5a\x2c"
             "\xc5\x00\x2b\xa8\xac\x57\x78\x72\x8c\x2c\x11\x28\x87\xfa\xf3\x78",
             32))
    return 1;  // 1.10.0 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\xa3\xdb\x14\x14\xf9\x26\x0e\x45\x27\x80\x5c\x20\xb9\xe9\xb7\xbb"
             "\xec\x1f\x2d\xd0\x9b\x72\x0a\x5e\xf2\xc3\x12\x44\xbe\xb8\xe4\x81",
             32))
    return 1;  // 1.10.0 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\x2b\x1e\xff\xf5\x1a\xee\x3c\x6d\x4d\x95\x31\x20\xc7\x27\xd4\x54"
             "\x03\x42\x4f\xf5\xb1\x6b\x86\x15\x50\x8e\xc1\xd7\x37\x36\x58\x3a",
             32))
    return 1;  // 1.10.0 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\x98\x68\x2e\xc8\x0b\x56\xf1\x4a\x93\x0a\x2f\xf0\xd1\xe6\x27\xf0"
             "\x67\x74\x10\x81\x45\x05\xba\xca\x8b\x82\xea\xe7\x3f\x8d\xcc\x77",
             32))
    return 1;  // 1.10.1 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\x15\xe6\xfb\x17\x03\x8a\xdf\x67\x00\x4e\x37\x0e\xe7\xcd\x76\xe5"
             "\x3c\x40\xac\x8d\xbe\x75\xa8\xcf\xa5\xfa\x88\x6a\x9d\xd4\xa4\xa0",
             32))
    return 1;  // 1.10.2 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\xe9\xd1\x86\xb6\x91\x7c\xcc\x1e\x66\x9b\x52\xd4\xb4\x86\xbb\x39"
             "\x88\x5c\xaf\x37\x19\x15\xa1\xaa\xc0\x59\xe7\x5a\x9e\xe9\x4b\x9c",
             32))
    return 1;  // 1.10.1 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\x93\x01\x16\xfc\x1b\x4d\xd8\x98\x73\x1d\xf7\xa4\x62\xca\x5e\xc0"
             "\x27\x4e\x8c\x51\x87\xa0\x05\xf4\x1a\x1e\xdd\xb8\x77\x9f\x7b\x25",
             32))
    return 1;  // 1.10.2 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\xd7\x74\x32\xdb\xdd\xed\xb2\x76\x8f\xa0\x99\x87\x40\xe7\xf5\xf8"
             "\xc7\x77\xe8\x76\x10\x8c\x8f\x02\x85\xe9\xc8\x8e\x16\xba\x31\xef",
             32))
    return 1;  // 2.0.0 shipped with fw 1.99.99
  if (0 ==
      memcmp(hash,
             "\x9d\x0c\x16\x94\xc2\x22\x24\x58\x1f\x0d\x54\x92\xf9\x87\x71\xa8"
             "\x05\x3b\x6f\xdd\xe8\xc8\xaf\xef\x3c\xc1\xe8\x02\x98\x4c\x32\x98",
             32))
    return 1;  // 1.10.0 shipped with fw 1.99.99
  // END AUTO-GENERATED BOOTLOADER ENTRIES (bl_check.txt)
  return 0;
}
#endif

static int onekey_known_bootloader(int r, const uint8_t *hash) {
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
    return 1;  // 1.8.8
  }

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

  if (!onekey_known_bootloader(r, hash)) {
    layoutDialog(&bmp_icon_error, NULL, NULL, NULL, _("Unknown bootloader"),
                 _("detected."), NULL, _("Shutdown your OneKey"),
                 _("contact our support."), NULL);
    delay_ms(1000);
    // shutdown();
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
               _("broken."), NULL, _("Unplug your Trezor"),
               _("contact our support."), NULL);
  shutdown();
#endif
  // prevent compiler warning when PRODUCTION==0
  (void)shutdown_on_replace;
}
