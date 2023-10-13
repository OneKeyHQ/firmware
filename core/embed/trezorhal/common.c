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

#include STM32_HAL_H

#include <string.h>

#include "common.h"
#include "display.h"
#include "flash.h"
#include "rand.h"
#include "supervise.h"
#include "touch.h"

#if defined(STM32F427xx) || defined(STM32F405xx)
#include "stm32f4xx_ll_utils.h"
#elif defined(STM32H747xx)
#include "stm32h7xx_ll_utils.h"
#endif

#ifdef RGB16
#define COLOR_FATAL_ERROR RGB16(0x7F, 0x00, 0x00)
#else
#define COLOR_FATAL_ERROR COLOR_BLACK
#endif

// clang-format off
static const uint8_t toi_icon_warning[] = {
    // magic
    'T', 'O', 'I', 'f',
    // width (16-bit), height (16-bit)
    0x2e, 0x00, 0x28, 0x00,
    // compressed data length (32-bit)
    0x35, 0x01, 0x00, 0x00,
    // compressed data
    0xb5, 0xd2, 0xb1, 0x6d, 0xc3, 0x30, 0x10, 0x85, 0x61, 0x8e, 0xa0, 0x32, 0xe5, 0xad, 0xe0, 0x0d, 0x98, 0x15, 0x3c, 0x01, 0xa1, 0x11, 0xd4, 0xba, 0x11, 0x90, 0x52, 0x2b, 0xa4, 0x60, 0x91, 0xd2, 0x2b, 0x10, 0xb8, 0x05, 0x3c, 0x02, 0x01, 0x4d, 0xa0, 0x11, 0x88, 0x84, 0x45, 0x20, 0x89, 0x26, 0x7d, 0x47, 0xf2, 0xac, 0xd7, 0x09, 0xd0, 0x07, 0x42, 0xfc, 0x95, 0xe2, 0x3f, 0xa3, 0xf9, 0xb4, 0xea, 0x4d, 0x8f, 0x47, 0x8f, 0xef, 0x91, 0xa7, 0x35, 0xd8, 0x60, 0xa7, 0x55, 0x5e, 0x86, 0xbf, 0x33, 0x47, 0x7b, 0xd3, 0x83, 0x91, 0xb6, 0x7f, 0x20, 0xca, 0x71, 0x0b, 0x4a, 0x9f, 0xfa, 0x5f, 0x8e, 0xbb, 0x68, 0x49, 0xfb, 0x3e, 0x1f, 0x6d, 0x27, 0x28, 0x8f, 0xe6, 0x28, 0xc7, 0xc9, 0xb5, 0xe8, 0x31, 0xb5, 0x3d, 0xca, 0xdc, 0xe8, 0x4d, 0xa7, 0x72, 0xdc, 0x97, 0x96, 0x6b, 0x2f, 0x9d, 0x44, 0x8b, 0x7b, 0x7b, 0xe9, 0x7a, 0x5b, 0x04, 0x2c, 0xc9, 0xfd, 0x37, 0xfa, 0x30, 0xaf, 0x6c, 0x27, 0xd4, 0xde, 0xfe, 0xf6, 0xa8, 0x5f, 0x45, 0xda, 0xcb, 0xdb, 0xad, 0x2d, 0x9e, 0xdb, 0x53, 0x90, 0xb3, 0xdb, 0x5a, 0x04, 0xdc, 0x4e, 0xf6, 0x07, 0xe6, 0xed, 0x4d, 0x03, 0xf6, 0xb6, 0x37, 0xcc, 0x79, 0x3b, 0xd8, 0x6f, 0xe8, 0x6d, 0x2f, 0xff, 0xbf, 0x5b, 0x5a, 0xf4, 0x55, 0xb6, 0x6b, 0x6c, 0x8f, 0x63, 0x07, 0x3b, 0x32, 0x6b, 0x19, 0xcc, 0xf3, 0xa9, 0x29, 0x9b, 0xdb, 0xe2, 0xb9, 0x3d, 0x9e, 0xcd, 0x6b, 0x11, 0x30, 0xff, 0x2d, 0x65, 0x73, 0x5a, 0x4c, 0xdb, 0xe3, 0xda, 0xc1, 0xde, 0xe7, 0xd7, 0xf2, 0x45, 0x97, 0xbe, 0xa4, 0x6d, 0xaa, 0x45, 0x8f, 0x3d, 0xb6, 0xab, 0x6c, 0xaf, 0x6e, 0xd3, 0x5a, 0xba, 0xc5, 0xf2, 0xa9, 0xb9, 0xdb, 0x74, 0xbe, 0xc5, 0x9b, 0xee, 0x95, 0x4b, 0x2d, 0x02, 0x71, 0x66, 0xce, 0xff, 0x2e, 0xb5, 0x58, 0x6a, 0xaf, 0xd6, 0x7e, 0xbe, 0xd1, 0x2b, 0xf9, 0x05, 0xdf, 0x4e, 0x5b, 0xa4, 0x6f, 0xb1, 0xc6, 0x7e, 0x98, 0xba, 0xf6, 0x6a, 0xec, 0xbd, 0x45, 0x89, 0xf6, 0x4a, 0x2d, 0x2e, 0xe2, 0x72, 0xdc, 0x82, 0x74, 0x7b, 0xed, 0x03, 0xa4, 0xda, 0x6b, 0x9f, 0x53, 0xbf,
};
// clang-format on

PCB_VERSION pcb_version;
volatile uint32_t system_reset = 0;

// from util.s
extern void shutdown_privileged(void);

void shutdown(void) {
#ifdef USE_SVC_SHUTDOWN
  svc_shutdown();
#else
  // It won't work properly unless called from the privileged mode
  shutdown_privileged();
#endif
}

void restart(void) { svc_reset_system(); }

void reboot_to_board(void) {
  *STAY_IN_FLAG_ADDR = STAY_IN_BOARDLOADER_FLAG;
  SCB_CleanDCache();
  svc_reset_system();
}

void reboot_to_boot(void) {
  *STAY_IN_FLAG_ADDR = STAY_IN_BOOTLOADER_FLAG;
  SCB_CleanDCache();
  svc_reset_system();
}

void __attribute__((noreturn))
__fatal_error(const char *expr, const char *msg, const char *file, int line,
              const char *func) {
  display_orientation(0);
  display_backlight(255);
  display_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  display_print_color(RGB16(0x69, 0x69, 0x69), COLOR_BLACK);
  display_printf("\nFATAL ERROR:\n");
  if (expr) {
    display_printf("expr: %s\n", expr);
  }
  if (msg) {
    display_printf("msg : %s\n", msg);
  }
  if (file) {
    display_printf("file: %s:%d\n", file, line);
  }
  if (func) {
    display_printf("func: %s\n", func);
  }
#ifdef SCM_REVISION
  const uint8_t *rev = (const uint8_t *)SCM_REVISION;
  display_printf("rev : %02x%02x%02x%02x%02x\n", rev[0], rev[1], rev[2], rev[3],
                 rev[4]);
#endif
#ifdef BUILD_ID
  const uint8_t *id = (const uint8_t *)BUILD_ID;
  display_printf("build id: %s\n", id);
#endif
  display_printf("\n\n");
  display_image(9, 50, 46, 40, toi_icon_warning + 12,
                sizeof(toi_icon_warning) - 12);
  display_text(8, 140, "System problem detected.", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_BLACK);
  display_text(8, 784, "Tap to restart ...", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_BLACK);
  // shutdown();
  while (!touch_click()) {
  }
  restart();
  for (;;)
    ;
}

void __attribute__((noreturn))
error_shutdown(const char *line1, const char *line2, const char *line3,
               const char *line4) {
  display_orientation(0);
#ifdef TREZOR_FONT_NORMAL_ENABLE
  uint16_t font_color = RGB16(0x69, 0x69, 0x69);
  display_clear();
  display_image(9, 50, 46, 40, toi_icon_warning + 12,
                sizeof(toi_icon_warning) - 12);
  display_text(8, 140, "System problem detected.", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_BLACK);
  display_text(8, 784, "Tap to restart ...", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_BLACK);

  int y = 720;

  if (line4) {
    display_text(8, y, line4, -1, FONT_NORMAL, font_color, COLOR_BLACK);
    y -= 32;
  }
  if (line3) {
    display_text(8, y, line3, -1, FONT_NORMAL, font_color, COLOR_BLACK);
    y -= 32;
  }
  if (line2) {
    display_text(8, y, line2, -1, FONT_NORMAL, font_color, COLOR_BLACK);
    y -= 32;
  }
  if (line1) {
    display_text(8, y, line1, -1, FONT_NORMAL, font_color, COLOR_BLACK);
    y -= 32;
  }

#else
  display_print_color(COLOR_WHITE, COLOR_FATAL_ERROR);
  if (line1) {
    display_printf("%s\n", line1);
  }
  if (line2) {
    display_printf("%s\n", line2);
  }
  if (line3) {
    display_printf("%s\n", line3);
  }
  if (line4) {
    display_printf("%s\n", line4);
  }
  display_printf("\nPlease unplug the device.\n");
#endif
  display_backlight(255);
  // shutdown();
  while (!touch_click()) {
  }
  restart();
  for (;;)
    ;
}

void error_reset(const char *line1, const char *line2, const char *line3,
                 const char *line4) {
  display_orientation(0);
  display_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  display_print_color(RGB16(0x69, 0x69, 0x69), COLOR_BLACK);
  if (line1) {
    display_printf("%s\n", line1);
  }
  if (line2) {
    display_printf("%s\n", line2);
  }
  if (line3) {
    display_printf("%s\n", line3);
  }
  if (line4) {
    display_printf("%s\n", line4);
  }

  display_backlight(255);
  display_printf("\n\n");
  display_image(9, 50, 46, 40, toi_icon_warning + 12,
                sizeof(toi_icon_warning) - 12);
  display_text(8, 140, "System problem detected.", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_BLACK);
  display_text(8, 784, "It will be restart 5s later.", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_BLACK);

  hal_delay(5000);
  restart();
}

void error_pin_max(void) {
  display_orientation(0);
  display_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  display_print_color(RGB16(0x69, 0x69, 0x69), COLOR_BLACK);

  display_backlight(255);
  display_printf("\n\n");
  display_image(9, 50, 46, 40, toi_icon_warning + 12,
                sizeof(toi_icon_warning) - 12);
  display_text(8, 140, "Too many wrong PIN attempts.Storage", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_BLACK);
  display_text(8, 164, "has been wiped.", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_BLACK);

  display_text(8, 784, "It will be restart 5s later.", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_BLACK);

  hal_delay(5000);
  restart();
}

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func,
                   const char *expr) {
  __fatal_error(expr, "assert failed", file, line, func);
}
#endif

void hal_delay(uint32_t ms) { HAL_Delay(ms); }
uint32_t hal_ticks_ms() { return HAL_GetTick(); }

// reference RM0090 section 35.12.1 Figure 413
#define USB_OTG_HS_DATA_FIFO_RAM (USB_OTG_HS_PERIPH_BASE + 0x20000U)
#define USB_OTG_HS_DATA_FIFO_SIZE (4096U)

void clear_otg_hs_memory(void) {
  // use the HAL version due to section 2.1.6 of STM32F42xx Errata sheet
  __HAL_RCC_USB_OTG_HS_CLK_ENABLE();  // enable USB_OTG_HS peripheral clock so
                                      // that the peripheral memory is
                                      // accessible
  memset_reg(
      (volatile void *)USB_OTG_HS_DATA_FIFO_RAM,
      (volatile void *)(USB_OTG_HS_DATA_FIFO_RAM + USB_OTG_HS_DATA_FIFO_SIZE),
      0);
  __HAL_RCC_USB_OTG_HS_CLK_DISABLE();  // disable USB OTG_HS peripheral clock as
                                       // the peripheral is not needed right now
}

uint32_t __stack_chk_guard = 0;

void __attribute__((noreturn)) __stack_chk_fail(void) {
  error_shutdown("Internal error", "(SS)", NULL, NULL);
}

uint8_t HW_ENTROPY_DATA[HW_ENTROPY_LEN];

void collect_hw_entropy(void) {
  // collect entropy from UUID
  uint32_t w = LL_GetUID_Word0();
  memcpy(HW_ENTROPY_DATA, &w, 4);
  w = LL_GetUID_Word1();
  memcpy(HW_ENTROPY_DATA + 4, &w, 4);
  w = LL_GetUID_Word2();
  memcpy(HW_ENTROPY_DATA + 8, &w, 4);
  // set entropy in the OTP randomness block
  if (secfalse == flash_otp_is_locked(FLASH_OTP_BLOCK_RANDOMNESS)) {
    uint8_t entropy[FLASH_OTP_BLOCK_SIZE];
    random_buffer(entropy, FLASH_OTP_BLOCK_SIZE);
    ensure(flash_otp_write(FLASH_OTP_BLOCK_RANDOMNESS, 0, entropy,
                           FLASH_OTP_BLOCK_SIZE),
           NULL);
    ensure(flash_otp_lock(FLASH_OTP_BLOCK_RANDOMNESS), NULL);
  }
  // collect entropy from OTP randomness block
  ensure(flash_otp_read(FLASH_OTP_BLOCK_RANDOMNESS, 0, HW_ENTROPY_DATA + 12,
                        FLASH_OTP_BLOCK_SIZE),
         NULL);
}

bool check_all_ones(const void *data, int len) {
  if (!data) return false;
  uint8_t result = 0xff;
  const uint8_t *ptr = (const uint8_t *)data;

  for (; len; len--, ptr++) {
    result &= *ptr;
    if (result != 0xff) break;
  }

  return (result == 0xff);
}

bool check_all_zeros(const void *data, int len) {
  if (!data) return false;
  uint8_t result = 0x0;
  const uint8_t *ptr = (const uint8_t *)data;

  for (; len; len--, ptr++) {
    result |= *ptr;
    if (result) break;
  }

  return (result == 0x00);
}

// this function resets settings changed in one layer (bootloader/firmware),
// which might be incompatible with the other layers older versions,
// where this setting might be unknown
void ensure_compatible_settings(void) {
#ifdef TREZOR_MODEL_T
  display_set_big_endian();
#endif
}
