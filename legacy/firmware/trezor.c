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

#include "trezor.h"
#include "bitmaps.h"
#include "bl_check.h"
#include "buttons.h"
#include "common.h"
#include "compiler_traits.h"
#include "config.h"
#include "gettext.h"
#include "layout.h"
#include "layout2.h"
#include "memzero.h"
#include "oled.h"
#include "protect.h"
#include "rng.h"
#include "setup.h"
#include "timer.h"
#include "usb.h"
#include "util.h"
#if !EMULATOR
#if ONEKEY_MINI
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include "atca_api.h"
#include "atca_hal.h"
#include "device.h"
#include "device2.h"
#include "flash_enc.h"
#include "font_ex.h"
#include "se_hal.h"
#include "w25qxx.h"
#endif
#include <libopencm3/stm32/desig.h>
#include "ble.h"
#include "otp.h"
#include "sys.h"
#endif
#ifdef USE_SECP256K1_ZKP
#include "zkp_context.h"
#endif

#ifdef USE_SECP256K1_ZKP
void secp256k1_default_illegal_callback_fn(const char *str, void *data) {
  (void)data;
  __fatal_error(NULL, str, __FILE__, __LINE__, __func__);
  return;
}

void secp256k1_default_error_callback_fn(const char *str, void *data) {
  (void)data;
  __fatal_error(NULL, str, __FILE__, __LINE__, __func__);
  return;
}
#endif

/* Screen timeout */
uint32_t system_millis_lock_start = 0;

/* Busyscreen timeout */
uint32_t system_millis_busy_deadline = 0;

void check_lock_screen(void) {
  buttonUpdate();

  // wake from screensaver on any button
  if (layoutLast == layoutScreensaver && (button.NoUp || button.YesUp)) {
    layoutHome();
    return;
  }

  // button held for long enough (5 seconds)
  if ((layoutLast == layoutHomescreen || layoutLast == layoutBusyscreen) &&
      button.NoDown >= 114000 * 5) {
    layoutDialogAdapter(&bmp_icon_question, _("Cancel"), _("Lock Device"), NULL,
                        _("Do you really want to"), _("lock your Trezor?"),
                        NULL, NULL, NULL, NULL);

    // wait until NoButton is released
    usbTiny(1);
    do {
      waitAndProcessUSBRequests(5);
      buttonUpdate();
    } while (!button.NoUp);

    // wait for confirmation/cancellation of the dialog
    do {
      waitAndProcessUSBRequests(5);
      buttonUpdate();
    } while (!button.YesUp && !button.NoUp);
    usbTiny(0);

    if (button.YesUp) {
      // lock the screen
      config_lockDevice();
      layoutScreensaver();
    } else {
      // resume homescreen
      layoutHome();
    }
  }

  // if homescreen is shown for too long
  if (layoutLast == layoutHomescreen) {
    if ((timer_ms() - system_millis_lock_start) >=
        config_getAutoLockDelayMs()) {
      // lock the screen
      config_lockDevice();
      layoutScreensaver();
    }
  }
}

void check_busy_screen(void) {
  // Clear the busy screen once it expires.
  if (system_millis_busy_deadline != 0 &&
      system_millis_busy_deadline < timer_ms()) {
    system_millis_busy_deadline = 0;
    layoutHome();
  }
}

static void collect_hw_entropy(bool privileged) {
#if EMULATOR
  (void)privileged;
  memzero(HW_ENTROPY_DATA, HW_ENTROPY_LEN);
#else
  if (privileged) {
    desig_get_unique_id((uint32_t *)HW_ENTROPY_DATA);
    // set entropy in the OTP randomness block
    if (!flash_otp_is_locked(FLASH_OTP_BLOCK_RANDOMNESS)) {
      uint8_t entropy[FLASH_OTP_BLOCK_SIZE] = {0};
      random_buffer(entropy, FLASH_OTP_BLOCK_SIZE);
      ensure_ex(flash_otp_write_safe(FLASH_OTP_BLOCK_RANDOMNESS, 0, entropy,
                                     FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_BLOCK_RANDOMNESS);
    }
    // collect entropy from OTP randomness block
    flash_otp_read(FLASH_OTP_BLOCK_RANDOMNESS, 0, HW_ENTROPY_DATA + 12,
                   FLASH_OTP_BLOCK_SIZE);
  } else {
    // unprivileged mode => use fixed HW_ENTROPY
    memset(HW_ENTROPY_DATA, 0x3C, HW_ENTROPY_LEN);
  }
#endif
}

#if ONEKEY_MINI
void pwm_config(void) {
  uint32_t value = config_getBrightness();

  gpio_mode_setup(OLED_CTRL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, OLED_CTRL_PIN);
  gpio_set_output_options(OLED_CTRL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                          OLED_CTRL_PIN);
  gpio_set_af(OLED_CTRL_PORT, GPIO_AF2, OLED_CTRL_PIN);

  rcc_periph_clock_enable(RCC_TIM3);
  timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
  timer_set_oc_mode(TIM3, TIM_OC2, TIM_OCM_PWM1);
  timer_enable_oc_output(TIM3, TIM_OC2);
  timer_enable_break_main_output(TIM3);
  timer_set_oc_value(TIM3, TIM_OC2, value);
  timer_set_period(TIM3, 1000);
  timer_enable_counter(TIM3);
}
#endif

#define DBGMCU_IDCODE 0xE0042000U
const char *cpu_info;

int main(void) {
#ifndef APPVER
  setup();
  __stack_chk_guard = random32();  // this supports compiler provided
                                   // unpredictable stack protection checks
  oledInit();
#else
#if !FIRMWARE_QA
  check_and_replace_bootloader(true);
#endif
  setupApp();
  atca_init();
#if ONEKEY_MINI
  bool serial_set = false, font_set = false, cert_set = false;
  bool usb_init = false;
  uint32_t cert_len = 0;

  device_test();
  device_init();
  w25qxx_init();
  flash_enc_init();
  font_init();
  atca_config_check();

  uint32_t idcode = *(uint32_t *)DBGMCU_IDCODE & 0xFFF;
  if (idcode == 0x411) {
    cpu_info = "STM32F2XX";
  } else if (idcode == 0x413) {
    cpu_info = "STM32F4XX";
  } else {
    cpu_info = "unkown-";
  }

  do {
    if (!serial_set) {
      serial_set = device_serial_set();
    }
    if (!font_set) {
      font_set = font_imported();
    }
    if (!cert_set) {
      cert_set = se_get_certificate_len(&cert_len);
    }

    if (serial_set && font_set && cert_set && !usb_init) {
      device_set_factory_mode(false);
      break;
    } else {
      if (!usb_init) {
        usb_init = true;
        memset(config_uuid_str, '0', sizeof(config_uuid_str));
        device_set_factory_mode(true);
        usbInit();
      }
      usbPollFactory();
      layoutHomeFactory(serial_set, font_set, cert_set);
    }

  } while (1);

#endif
#if !EMULATOR && !ONEKEY_MINI
  ble_reset();
  register_timer("button", timer1s / 2, buttonsTimer);
  register_timer("charge_dis", timer1s, chargeDisTimer);
#endif
  __stack_chk_guard = random32();  // this supports compiler provided
                                   // unpredictable stack protection checks
#endif

  drbg_init();

  if (!is_mode_unprivileged()) {
    collect_hw_entropy(true);
    timer_init();
#ifdef APPVER
    // enable MPU (Memory Protection Unit)
    mpu_config_firmware();
#endif
  } else {
    collect_hw_entropy(false);
  }

#ifdef USE_SECP256K1_ZKP
  ensure(sectrue * (zkp_context_init() == 0), NULL);
#endif

#if DEBUG_LINK
#if !EMULATOR
  config_wipe();
#endif
#endif

  config_init();
#if ONEKEY_MINI
  pwm_config();
#endif
  layoutHome();
  usbInit();

  for (;;) {
#if EMULATOR
    waitAndProcessUSBRequests(10);
    layoutHomeInfo();
#else
    usbPoll();
    layoutHomeInfo();
#endif
    // check_lock_screen();
    check_busy_screen();
  }
  return 0;
}
