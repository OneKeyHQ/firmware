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
#include <libopencm3/stm32/desig.h>
#include "ble.h"
#include "otp.h"
#include "sys.h"
#endif

/* Screen timeout */
uint32_t system_millis_lock_start = 0;

void check_lock_screen(void) {
  buttonUpdate();

  // wake from screensaver on any button
  if (layoutLast == layoutScreensaver && (button.NoUp || button.YesUp)) {
    layoutHome();
    return;
  }

  // button held for long enough (2 seconds)
  if (layoutLast == layoutHome && button.NoDown >= 285000 * 2) {
    layoutDialogAdapter(&bmp_icon_question, _("Cancel"), _("Lock Device"), NULL,
                        _("Do you really want to"), _("lock your Trezor?"),
                        NULL, NULL, NULL, NULL);

    // wait until NoButton is released
    usbTiny(1);
    do {
      usbSleep(5);
      buttonUpdate();
    } while (!button.NoUp);

    // wait for confirmation/cancellation of the dialog
    do {
      usbSleep(5);
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
  if (layoutLast == layoutHome) {
    if ((timer_ms() - system_millis_lock_start) >=
        config_getAutoLockDelayMs()) {
      // lock the screen
      config_lockDevice();
      layoutScreensaver();
    }
  }
}

#if !EMULATOR
extern volatile uint32_t system_millis;

void enter_sleep(void) {
  static int sleep_count = 0;
  bool unlocked = false;
  uint8_t oled_prev[OLED_BUFSIZE];
  void *layoutBack = NULL;
  bool ble_state_bak = ble_get_switch();
  sleep_count++;
  if (sleep_count == 1) {
    unlocked = session_isUnlocked();
    layoutBack = layoutLast;
    config_lockDevice();
  }
  oledBufferLoad(oled_prev);
  if (ble_state_bak) {
    change_ble_sta(false);
  }
  oledClear();
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 30, _("Sleep Mode"),
                              FONT_STANDARD);
  layoutButtonNoAdapter(_("Exit"), NULL);
  oledRefresh();
  svc_system_sleep();
  while (get_power_key_state()) {
  }
  timer_sleep_start_reset();
  if (unlocked) {
    if (sleep_count > 1) {
    } else {
      while (!protectPinOnDevice(false, false)) {
      }
    }
  }
  if (ble_state_bak) {
    change_ble_sta(true);
  }
  usbInit();
  oledBufferRestore(oled_prev);
  oledRefresh();
  sleep_count--;
  if (sleep_count == 0) {
    layoutLast = layoutBack;
  }
}

void auto_poweroff_timer(void) {
  if (config_getAutoLockDelayMs() == 0) return;
  if (timer_get_sleep_count() >= config_getAutoLockDelayMs()) {
    if (sys_nfcState() || sys_usbState()) {
      config_lockDevice();
      layoutScreensaver();
    } else {
      shutdown();
    }
  }
}
#endif

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
      flash_otp_write(FLASH_OTP_BLOCK_RANDOMNESS, 0, entropy,
                      FLASH_OTP_BLOCK_SIZE);
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

int main(void) {
#ifndef APPVER
  setup();
  __stack_chk_guard = random32();  // this supports compiler provided
                                   // unpredictable stack protection checks
  oledInit();
#else
  check_bootloader(true);
  setupApp();
  ble_reset();
#if !EMULATOR
  register_timer("button", timer1s / 2, buttonsTimer);
  register_timer("charge_dis", timer1s, chargeDisTimer);
  register_timer("poweroff", timer1s, auto_poweroff_timer);
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

#if DEBUG_LINK
  oledSetDebugLink(1);
#if !EMULATOR
  config_wipe();
#endif
#endif

  config_init();
  layoutHome();
  usbInit();

  for (;;) {
    usbPoll();
    layoutHomeInfo();
  }
  return 0;
}
