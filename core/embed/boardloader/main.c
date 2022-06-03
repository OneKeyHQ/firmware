/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "common.h"
#include "compiler_traits.h"
#include "display.h"
#include "emmc.h"
#include "ff.h"
#include "flash.h"
#include "image.h"
#include "mipi_lcd.h"
#include "qspi_flash.h"
#include "rng.h"
#include "sdcard.h"
#include "sdram.h"
#include "touch.h"
#include "usb.h"

#include "lowlevel.h"
#include "version.h"

#include "memzero.h"

const uint8_t BOARDLOADER_KEY_M = 2;
const uint8_t BOARDLOADER_KEY_N = 3;
static const uint8_t * const BOARDLOADER_KEYS[] = {
#if PRODUCTION
    (const uint8_t
    *)"\x0e\xb9\x85\x6b\xe9\xba\x7e\x97\x2c\x7f\x34\xea\xc1\xed\x9b\x6f\xd0\xef\xd1\x72\xec\x00\xfa\xf0\xc5\x89\x75\x9d\xa4\xdd\xfb\xa0",
    (const uint8_t
    *)"\xac\x8a\xb4\x0b\x32\xc9\x86\x55\x79\x8f\xd5\xda\x5e\x19\x2b\xe2\x7a\x22\x30\x6e\xa0\x5c\x6d\x27\x7c\xdf\xf4\xa3\xf4\x12\x5c\xd8",
    (const uint8_t
    *)"\xce\x0f\xcd\x12\x54\x3e\xf5\x93\x6c\xf2\x80\x49\x82\x13\x67\x07\x86\x3d\x17\x29\x5f\xac\xed\x72\xaf\x17\x1d\x6e\x65\x13\xff\x06",
#else
    (const uint8_t
    *)"\xdb\x99\x5f\xe2\x51\x69\xd1\x41\xca\xb9\xbb\xba\x92\xba\xa0\x1f\x9f\x2e\x1e\xce\x7d\xf4\xcb\x2a\xc0\x51\x90\xf3\x7f\xcc\x1f\x9d",
    (const uint8_t
    *)"\x21\x52\xf8\xd1\x9b\x79\x1d\x24\x45\x32\x42\xe1\x5f\x2e\xab\x6c\xb7\xcf\xfa\x7b\x6a\x5e\xd3\x00\x97\x96\x0e\x06\x98\x81\xdb\x12",
    (const uint8_t
    *)"\x22\xfc\x29\x77\x92\xf0\xb6\xff\xc0\xbf\xcf\xdb\x7e\xdb\x0c\x0a\xa1\x4e\x02\x5a\x36\x5e\xc0\xe3\x42\xe8\x6e\x38\x29\xcb\x74\xb6",
#endif
};

// clang-format off
static const uint8_t toi_icon_onekey[] = {
    // magic
    'T', 'O', 'I', 'f',
    // width (16-bit), height (16-bit)
    0x4a, 0x00, 0x4a, 0x00,
    // compressed data length (32-bit)
    0xe7, 0x00, 0x00, 0x00,
    // compressed data
    0xed, 0xd2, 0x49, 0x0e, 0xc3, 0x30, 0x0c, 0x43, 0x51, 0xde, 0xff, 0xd2, 0xec, 0xb6, 0x40, 0x11, 0x4f, 0xa2, 0x2c, 0x36, 0xd0, 0x5f, 0x66, 0x90, 0x5f, 0x02, 0x01, 0xbb, 0x71, 0x33, 0x24, 0xc6, 0x60, 0x5e, 0x1a, 0xb5, 0x8c, 0x09, 0xb9, 0x79, 0x22, 0x2e, 0x5e, 0xc8, 0x4f, 0xb4, 0xa7, 0xe2, 0xc5, 0xfc, 0x44, 0x6b, 0x2a, 0x16, 0xe4, 0x27, 0x1a, 0xab, 0x58, 0x98, 0x9f, 0xe8, 0x49, 0xe5, 0x67, 0xa2, 0x41, 0xfe, 0x26, 0x97, 0x0d, 0xf0, 0x36, 0xe9, 0xb6, 0x32, 0xbe, 0x03, 0xfa, 0xbf, 0xf4, 0x66, 0x13, 0xa9, 0xfa, 0x36, 0xdd, 0xb4, 0x36, 0xfd, 0xab, 0x89, 0x74, 0x33, 0x69, 0x6a, 0x53, 0x9b, 0xda, 0xd4, 0x75, 0x27, 0x1b, 0xe5, 0xb7, 0xdf, 0x0e, 0x32, 0x0c, 0xf2, 0x13, 0xd5, 0xa8, 0xb0, 0x50, 0xa5, 0x69, 0xe5, 0xfa, 0x4d, 0xd1, 0xfe, 0xdd, 0x0a, 0xd1, 0xf7, 0x13, 0x8e, 0xa6, 0x7b, 0xaa, 0x95, 0xf3, 0xda, 0xd4, 0x26, 0x17, 0xd3, 0xc9, 0x5b, 0xc0, 0xec, 0xc4, 0xf9, 0x13, 0xe3, 0xe9, 0xef, 0x30, 0xcd, 0xce, 0x8c, 0x88, 0x14, 0xa6, 0xdf, 0xf7, 0x47, 0xf7, 0xd6, 0x67, 0x47, 0x55, 0x4f, 0x45, 0xe6, 0x92, 0x19, 0xaa, 0xd8, 0x54, 0x52, 0xaf, 0x8a, 0xcf, 0x24, 0x95, 0x2e, 0xcd, 0x34, 0x86, 0xca, 0x99, 0x14, 0x9d, 0xa5, 0x49, 0xf7, 0xbf, 0xf3, 0x4c, 0xf5, 0x2a, 0xed, 0x66, 0xe6, 0x99, 0x6a, 0x55, 0x78, 0xcc, 0x4f, 0x54, 0xa5, 0xc2, 0x34, 0x3f, 0xd1, 0x6d, 0x15, 0x96, 0xf3, 0x13, 0xdd, 0x52, 0xe1, 0x20, 0x37, 0x4f, 0xa6, 0x0b, 0x82, 0xdc, 0x3c, 0x3a, 0x19, 0x12, 0xcb, 0xb7, 0x7c, 0x00,
};
// clang-format on

#if defined(STM32H747xx)

#include "stm32h7xx_hal.h"

static FATFS fs_instance;
PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},
    {0, 2},
};
uint32_t *sdcard_buf = (uint32_t *)0x24000000;

void fatfs_init(void) {
  FRESULT res;
  BYTE work[FF_MAX_SS];
  MKFS_PARM mk_para = {
      .fmt = FM_FAT32,
  };

  LBA_t plist[] = {
      BOOT_EMMC_BLOCKS,
      100};  // 1G sectors for 1st partition and left all for 2nd partition

  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    if (res == FR_NO_FILESYSTEM) {
      res = f_fdisk(0, plist, work); /* Divide physical drive 0 */
      if (res) {
        display_printf("f_fdisk error%d\n", res);
      }
      res = f_mkfs("0:", &mk_para, work, sizeof(work));
      if (res) {
        display_printf("f_mkfs 0 error%d\n", res);
      }
      res = f_mkfs("1:", &mk_para, work, sizeof(work));
      if (res) {
        display_printf("fatfs Format error");
      }
      f_setlabel("Onekey Data");
    } else {
      display_printf("mount err %d\n", res);
    }
  }
}

int fatfs_check_res(void) {
  FRESULT res;
  FIL fsrc;
  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    display_printf("fatfs mount error");
    return 0;
  }
  res = f_open(&fsrc, "/res/.ONEKEY_RESOURCE", FA_READ);
  if (res != FR_OK) {
    f_unmount("");
  }
  return res;
}

static uint32_t check_sdcard(void) {
  FRESULT res;

  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    return 0;
  }
  uint64_t cap = emmc_get_capacity_in_bytes();
  if (cap < 1024 * 1024) {
    return 0;
  }

  memzero(sdcard_buf, IMAGE_HEADER_SIZE);

  FIL fsrc;
  UINT num_of_read = 0;

  res = f_open(&fsrc, "/boot/bootloader.bin", FA_READ);
  if (res != FR_OK) {
    return 0;
  }
  res = f_read(&fsrc, sdcard_buf, IMAGE_HEADER_SIZE, &num_of_read);
  if ((num_of_read != IMAGE_HEADER_SIZE) || (res != FR_OK)) {
    f_close(&fsrc);
    return 0;
  }
  f_close(&fsrc);

  image_header hdr_old;
  image_header hdr_new;
  secbool new_present = secfalse, old_present = secfalse;

  old_present = load_image_header(
      (const uint8_t *)BOOTLOADER_START, BOOTLOADER_IMAGE_MAGIC,
      BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
      BOARDLOADER_KEYS, &hdr_old);

  new_present =
      load_image_header((const uint8_t *)sdcard_buf, BOOTLOADER_IMAGE_MAGIC,
                        BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M,
                        BOARDLOADER_KEY_N, BOARDLOADER_KEYS, &hdr_new);
  if (sectrue == new_present && secfalse == old_present) {
    return hdr_new.codelen;
  } else if (sectrue == new_present && sectrue == old_present) {
    if (memcmp(&hdr_new.version, &hdr_old.version, 4) > 0) {
      return hdr_new.codelen;
    }
  }
  return 0;
}

static void progress_callback(int pos, int len) { display_printf("."); }

static secbool copy_sdcard(uint32_t code_len) {
  display_backlight(255);

  display_printf("OneKey Boardloader\n");
  display_printf("==================\n\n");

  display_printf("new version bootloader found\n\n");
  display_printf("applying bootloader in 5 seconds\n\n");
  display_printf("touch screen if you want to abort\n\n");

  uint32_t touched = 0;
  for (int i = 5; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);

    touched = touch_is_detected() | touch_read();
    if (touched) {
      display_printf("\n\ncanceled, aborting\n");
      return secfalse;
    }
  }

  display_printf("\n\nerasing flash:\n\n");

  // erase all flash (except boardloader)
  static const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER_1,
      FLASH_SECTOR_BOOTLOADER_2,
  };

  if (sectrue !=
      flash_erase_sectors(sectors, sizeof(sectors), progress_callback)) {
    display_printf(" failed\n");
    return secfalse;
  }
  display_printf(" done\n\n");

  ensure(flash_unlock_write(), NULL);

  // copy bootloader from SD card to Flash
  display_printf("copying new bootloader from SD card\n\n");

  memzero(sdcard_buf, EMMC_BLOCK_SIZE);

  FIL fsrc;
  FRESULT res;
  UINT num_of_read;
  res = f_open(&fsrc, "/boot/bootloader.bin", FA_READ);
  if (res != FR_OK) {
    return secfalse;
  }
  int blocks = (IMAGE_HEADER_SIZE + code_len) / EMMC_BLOCK_SIZE;
  int percent = 0, percent_bak = 0;
  for (int i = 0; i < blocks; i++) {
    percent = (i * 100) / blocks;
    if (percent != percent_bak) {
      percent_bak = percent;
      display_printf("%d ", percent);
    }

    f_lseek(&fsrc, i * EMMC_BLOCK_SIZE);
    res = f_read(&fsrc, sdcard_buf, EMMC_BLOCK_SIZE, &num_of_read);
    if ((num_of_read != EMMC_BLOCK_SIZE) || (res != FR_OK)) {
      f_close(&fsrc);
      return secfalse;
    }
    if (i * EMMC_BLOCK_SIZE < FLASH_FIRMWARE_SECTOR_SIZE) {
      for (int j = 0; j < EMMC_BLOCK_SIZE / (sizeof(uint32_t) * 8); j++) {
        ensure(
            flash_write_words(FLASH_SECTOR_BOOTLOADER_1,
                              i * EMMC_BLOCK_SIZE + j * (sizeof(uint32_t) * 8),
                              (uint32_t *)&sdcard_buf[8 * j]),
            NULL);
      }
    } else {
      for (int j = 0; j < EMMC_BLOCK_SIZE / (sizeof(uint32_t) * 8); j++) {
        ensure(flash_write_words(
                   FLASH_SECTOR_BOOTLOADER_2,
                   (i - FLASH_FIRMWARE_SECTOR_SIZE / EMMC_BLOCK_SIZE) *
                           EMMC_BLOCK_SIZE +
                       j * (sizeof(uint32_t) * 8),
                   (uint32_t *)&sdcard_buf[8 * j]),
               NULL);
      }
    }
  }
  f_close(&fsrc);
  ensure(flash_lock_write(), NULL);

  display_printf("\ndone\n\n");
  display_printf("Device will be restart in 3 seconds\n");

  for (int i = 3; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);
  }
  HAL_NVIC_SystemReset();
  return sectrue;
}

int main(void) {
  volatile uint32_t stay_in_boardloader_flag = *STAY_IN_FLAG_ADDR;

  reset_flags_reset();

  periph_init();

  /* Enable the CPU Cache */
  cpu_cache_enable();

  system_clock_config();

  rng_init();

  flash_option_bytes_init();

  clear_otg_hs_memory();

  flash_otp_init();

  gpio_init();

  sdram_init();

  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();

  mpu_config();

  lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  display_clear();
  display_image((DISPLAY_RESX - 74) / 2, (DISPLAY_RESY - 74) / 2, 74, 74,
                toi_icon_onekey + 12, sizeof(toi_icon_onekey) - 12);

#if !PRODUCTION
  display_text_center(DISPLAY_RESX / 2, DISPLAY_RESX / 2, "TEST VERSION", -1,
                      FONT_NORMAL, COLOR_RED, COLOR_BLACK);
  hal_delay(1000);
#endif

  touch_init();
  emmc_init();
  fatfs_init();

  bool stay_in_msc = false;
  uint32_t touched = 0;

  if (stay_in_boardloader_flag == STAY_IN_BOARDLOADER_FLAG) {
    stay_in_msc = true;
    *STAY_IN_FLAG_ADDR = 0;
  } else if (fatfs_check_res() != 0) {
    stay_in_msc = true;
  }

  if (!stay_in_msc) {
    for (int i = 0; i < 1000; i++) {
      if (touch_num_detected() > 1) {
        touched = 1;
        break;
      }
      hal_delay(1);
    }
  }

  if (touched || stay_in_msc) {
    display_printf("OneKey Boardloader\n");
    display_printf("USB Mass Storage Mode\n");
    display_printf("=====================\n\n");
    usb_msc_init();
    while (1)
      ;
  }

  uint32_t code_len = 0;
  code_len = check_sdcard();
  if (code_len) {
    return copy_sdcard(code_len) == sectrue ? 0 : 3;
  }

  image_header hdr;

  ensure(load_image_header((const uint8_t *)BOOTLOADER_START,
                           BOOTLOADER_IMAGE_MAGIC, BOOTLOADER_IMAGE_MAXSIZE,
                           BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
                           BOARDLOADER_KEYS, &hdr),
         "invalid bootloader header");

  const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER_1,
      FLASH_SECTOR_BOOTLOADER_2,
  };
  ensure(
      check_image_contents(&hdr, IMAGE_HEADER_SIZE, sectors, sizeof(sectors)),
      "invalid bootloader hash");

  jump_to(BOOTLOADER_START + IMAGE_HEADER_SIZE);

  return 0;
}

#else

// we use SRAM as SD card read buffer (because DMA can't access the CCMRAM)
extern uint32_t sram_start[];
#define sdcard_buf sram_start

static uint32_t check_sdcard(void) {
  if (sectrue != sdcard_power_on()) {
    return 0;
  }

  uint64_t cap = sdcard_get_capacity_in_bytes();
  if (cap < 1024 * 1024) {
    sdcard_power_off();
    return 0;
  }

  memzero(sdcard_buf, IMAGE_HEADER_SIZE);

  const secbool read_status =
      sdcard_read_blocks(sdcard_buf, 0, IMAGE_HEADER_SIZE / SDCARD_BLOCK_SIZE);

  sdcard_power_off();

  image_header hdr;

  if ((sectrue == read_status) &&
      (sectrue ==
       load_image_header((const uint8_t *)sdcard_buf, BOOTLOADER_IMAGE_MAGIC,
                         BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M,
                         BOARDLOADER_KEY_N, BOARDLOADER_KEYS, &hdr))) {
    return hdr.codelen;
  } else {
    return 0;
  }
}

static void progress_callback(int pos, int len) { display_printf("."); }

static secbool copy_sdcard(void) {
  display_backlight(255);

  display_printf("Trezor Boardloader\n");
  display_printf("==================\n\n");

  display_printf("bootloader found on the SD card\n\n");
  display_printf("applying bootloader in 10 seconds\n\n");
  display_printf("unplug now if you want to abort\n\n");

  uint32_t codelen;

  for (int i = 10; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);
    codelen = check_sdcard();
    if (0 == codelen) {
      display_printf("\n\nno SD card, aborting\n");
      return secfalse;
    }
  }

  display_printf("\n\nerasing flash:\n\n");

  // erase all flash (except boardloader)
  static const uint8_t sectors[] = {
      FLASH_SECTOR_STORAGE_1,
      FLASH_SECTOR_STORAGE_2,
      3,
      FLASH_SECTOR_BOOTLOADER,
      FLASH_SECTOR_FIRMWARE_START,
      7,
      8,
      9,
      10,
      FLASH_SECTOR_FIRMWARE_END,
      FLASH_SECTOR_UNUSED_START,
      13,
      14,
      FLASH_SECTOR_UNUSED_END,
      FLASH_SECTOR_FIRMWARE_EXTRA_START,
      18,
      19,
      20,
      21,
      22,
      FLASH_SECTOR_FIRMWARE_EXTRA_END,
  };
  if (sectrue !=
      flash_erase_sectors(sectors, sizeof(sectors), progress_callback)) {
    display_printf(" failed\n");
    return secfalse;
  }
  display_printf(" done\n\n");

  ensure(flash_unlock_write(), NULL);

  // copy bootloader from SD card to Flash
  display_printf("copying new bootloader from SD card\n\n");

  ensure(sdcard_power_on(), NULL);

  memzero(sdcard_buf, SDCARD_BLOCK_SIZE);

  for (int i = 0; i < (IMAGE_HEADER_SIZE + codelen) / SDCARD_BLOCK_SIZE; i++) {
    ensure(sdcard_read_blocks(sdcard_buf, i, 1), NULL);
    for (int j = 0; j < SDCARD_BLOCK_SIZE / sizeof(uint32_t); j++) {
      ensure(flash_write_word(FLASH_SECTOR_BOOTLOADER,
                              i * SDCARD_BLOCK_SIZE + j * sizeof(uint32_t),
                              sdcard_buf[j]),
             NULL);
    }
  }

  sdcard_power_off();
  ensure(flash_lock_write(), NULL);

  display_printf("\ndone\n\n");
  display_printf("Unplug the device and remove the SD card\n");

  return sectrue;
}

int main(void) {
  reset_flags_reset();

  // need the systick timer running before many HAL operations.
  // want the PVD enabled before flash operations too.
  periph_init();

  if (sectrue != flash_configure_option_bytes()) {
    // display is not initialized so don't call ensure
    const secbool r =
        flash_erase_sectors(STORAGE_SECTORS, STORAGE_SECTORS_COUNT, NULL);
    (void)r;
    return 2;
  }

  clear_otg_hs_memory();

  display_init();
  sdcard_init();

  if (check_sdcard()) {
    return copy_sdcard() == sectrue ? 0 : 3;
  }

  image_header hdr;

  ensure(load_image_header((const uint8_t *)BOOTLOADER_START,
                           BOOTLOADER_IMAGE_MAGIC, BOOTLOADER_IMAGE_MAXSIZE,
                           BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
                           BOARDLOADER_KEYS, &hdr),
         "invalid bootloader header");

  const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER,
  };
  ensure(check_image_contents(&hdr, IMAGE_HEADER_SIZE, sectors, 1),
         "invalid bootloader hash");

  jump_to(BOOTLOADER_START + IMAGE_HEADER_SIZE);

  return 0;
}

#endif
