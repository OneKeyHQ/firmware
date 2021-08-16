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

#include "protect.h"
#include "buttons.h"
#include "common.h"
#include "config.h"
#include "debug.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "memory.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "oled.h"
#include "pinmatrix.h"
#include "prompt.h"
#include "rng.h"
#include "si2c.h"
#include "sys.h"
#include "usb.h"
#include "util.h"
#if ONEKEY_MINI
#include <libopencm3/stm32/timer.h>
#endif

#define MAX_WRONG_PINS 15

bool protectAbortedByCancel = false;
bool protectAbortedByInitialize = false;
bool protectAbortedByTimeout = false;

static uint8_t device_sleep_state = SLEEP_NONE;

#define goto_check(label)       \
  if (layoutLast == layoutHome) \
    return false;               \
  else                          \
    goto label;

bool protectButton(ButtonRequestType type, bool confirm_only) {
  ButtonRequest resp = {0};
  bool result = false;
  bool acked = false;
  bool timeout_flag = true;
#if DEBUG_LINK
  bool debug_decided = false;
#endif

  protectAbortedByTimeout = false;

  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = type;
  usbTiny(1);
  buttonUpdate();  // Clear button state
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

  timer_out_set(timer_out_oper, default_oper_time);
  if (g_bIsBixinAPP) acked = true;

  while (timer_out_get(timer_out_oper)) {
    usbPoll();

    // check for ButtonAck
    if (msg_tiny_id == MessageType_MessageType_ButtonAck) {
      msg_tiny_id = 0xFFFF;
      acked = true;
    }

    // button acked - check buttons
    if (acked) {
      usbSleep(5);
      buttonUpdate();
      if (button.YesUp) {
        timeout_flag = false;
        result = true;
        break;
      }
      if (!confirm_only && button.NoUp) {
        result = false;
        timeout_flag = false;
        break;
      }
    }

    // check for Cancel / Initialize
    protectAbortedByCancel = (msg_tiny_id == MessageType_MessageType_Cancel);
    protectAbortedByInitialize =
        (msg_tiny_id == MessageType_MessageType_Initialize);
    if (protectAbortedByCancel || protectAbortedByInitialize) {
      msg_tiny_id = 0xFFFF;
      timeout_flag = false;
      result = false;
      break;
    }

#if DEBUG_LINK
    // check DebugLink
    if (msg_tiny_id == MessageType_MessageType_DebugLinkDecision) {
      msg_tiny_id = 0xFFFF;
      DebugLinkDecision *dld = (DebugLinkDecision *)msg_tiny;
      result = dld->yes_no;
      debug_decided = true;
    }

    if (acked && debug_decided) {
      break;
    }

    if (msg_tiny_id == MessageType_MessageType_DebugLinkGetState) {
      msg_tiny_id = 0xFFFF;
      fsm_msgDebugLinkGetState((DebugLinkGetState *)msg_tiny);
    }
#endif
  }
  timer_out_set(timer_out_oper, 0);
  usbTiny(0);
  if (timeout_flag) protectAbortedByTimeout = true;

  return result;
}

bool protectButton_ex(ButtonRequestType type, bool confirm_only, bool requset,
                      uint32_t timeout_s) {
  ButtonRequest resp = {0};
  bool result = false;
  bool acked = false;
  bool timeout_flag = true;
#if DEBUG_LINK
  bool debug_decided = false;
#endif

  protectAbortedByTimeout = false;

  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = type;
  usbTiny(1);
  buttonUpdate();  // Clear button state
  if (requset) {
    msg_write(MessageType_MessageType_ButtonRequest, &resp);
  }
  if (timeout_s) {
    timer_out_set(timer_out_oper, timeout_s);
  }
  if (g_bIsBixinAPP) acked = true;
  acked = true;

  while (1) {
    if (timeout_s) {
      if (timer_out_get(timer_out_oper) == 0) break;
    }
    usbPoll();

    // check for ButtonAck
    if (msg_tiny_id == MessageType_MessageType_ButtonAck) {
      msg_tiny_id = 0xFFFF;
      acked = true;
    }

    // button acked - check buttons
    if (acked) {
      usbSleep(5);
      buttonUpdate();
      if (button.YesUp) {
        result = true;
        timeout_flag = false;
        break;
      }
      if (!confirm_only && button.NoUp) {
        timeout_flag = false;
        result = false;
        break;
      }
    }

    // check for Cancel / Initialize
    protectAbortedByCancel = (msg_tiny_id == MessageType_MessageType_Cancel);
    protectAbortedByInitialize =
        (msg_tiny_id == MessageType_MessageType_Initialize);
    if (protectAbortedByCancel || protectAbortedByInitialize) {
      msg_tiny_id = 0xFFFF;
      timeout_flag = false;
      result = false;
      break;
    }

#if DEBUG_LINK
    // check DebugLink
    if (msg_tiny_id == MessageType_MessageType_DebugLinkDecision) {
      msg_tiny_id = 0xFFFF;
      DebugLinkDecision *dld = (DebugLinkDecision *)msg_tiny;
      result = dld->yes_no;
      debug_decided = true;
    }

    if (acked && debug_decided) {
      break;
    }

    if (msg_tiny_id == MessageType_MessageType_DebugLinkGetState) {
      msg_tiny_id = 0xFFFF;
      fsm_msgDebugLinkGetState((DebugLinkGetState *)msg_tiny);
    }
#endif
  }
  timer_out_set(timer_out_oper, 0);
  usbTiny(0);
  if (timeout_flag) protectAbortedByTimeout = true;

  return result;
}

const char *requestPin(PinMatrixRequestType type, const char *text,
                       const char **new_pin) {
  bool button_no = false;
  PinMatrixRequest resp = {0};
  *new_pin = NULL;
  memzero(&resp, sizeof(PinMatrixRequest));
  resp.has_type = true;
  resp.type = type;
  usbTiny(1);
  msg_write(MessageType_MessageType_PinMatrixRequest, &resp);
  pinmatrix_start(text);
  timer_out_set(timer_out_oper, default_oper_time);
  while (timer_out_get(timer_out_oper)) {
    usbPoll();
    buttonUpdate();
    if (msg_tiny_id == MessageType_MessageType_PinMatrixAck) {
      timer_out_set(timer_out_oper, 0);
      msg_tiny_id = 0xFFFF;
      PinMatrixAck *pma = (PinMatrixAck *)msg_tiny;
      usbTiny(0);
      if (pma->has_new_pin) {
        if (sectrue ==
            pinmatrix_done(false, pma->new_pin))  // convert via pinmatrix
          *new_pin = pma->new_pin;
      }
      if (sectrue == pinmatrix_done(true, pma->pin))  // convert via pinmatrix
        return pma->pin;
      else
        return 0;
    } else if (msg_tiny_id == MessageType_MessageType_BixinPinInputOnDevice) {
      uint8_t min_pin_len = MIN_PIN_LEN;
      if (PinMatrixRequestType_PinMatrixRequestType_NewFirst == type ||
          PinMatrixRequestType_PinMatrixRequestType_NewSecond == type) {
        min_pin_len = DEFAULT_PIN_LEN;
      }
      msg_tiny_id = 0xFFFF;
      usbTiny(0);
      return protectInputPin(text, min_pin_len, MAX_PIN_LEN, true);
    }
    if (button.NoUp) {
      timer_out_set(timer_out_oper, 0);
      button_no = true;
      msg_tiny_id = MessageType_MessageType_Cancel;
    }
    // check for Cancel / Initialize
    protectAbortedByCancel = (msg_tiny_id == MessageType_MessageType_Cancel);
    protectAbortedByInitialize =
        (msg_tiny_id == MessageType_MessageType_Initialize);
    if (protectAbortedByCancel || protectAbortedByInitialize) {
      pinmatrix_done(true, 0);
      msg_tiny_id = 0xFFFF;
      usbTiny(0);
      if (button_no)
        return PIN_CANCELED_BY_BUTTON;
      else
        return 0;
    }
#if DEBUG_LINK
    if (msg_tiny_id == MessageType_MessageType_DebugLinkGetState) {
      msg_tiny_id = 0xFFFF;
      fsm_msgDebugLinkGetState((DebugLinkGetState *)msg_tiny);
    }
#endif
  }
  // time out
  msg_tiny_id = 0xFFFF;
  usbTiny(0);
  return PIN_CANCELED_BY_BUTTON;
}

secbool protectPinUiCallback(uint32_t wait, uint32_t progress,
                             const char *message) {
#if ONEKEY_MINI
  (void)wait;
  (void)progress;
  (void)message;
#else
  const struct font_desc *font = find_cur_font();
  int y = 9;
  // Convert wait to secstr string.
  char secstrbuf[32] = "";
  uint2str(wait, secstrbuf);

  strcat(secstrbuf, _("second"));

  if (ui_language == 0) {
    if (wait > 1) strcat(secstrbuf, "s");
  }

  oledClear_ex();

  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 0, _(message), FONT_STANDARD);
  y += font->pixel + 1;
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, _("Please wait"),
                              FONT_STANDARD);
  y += font->pixel + 1;
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, secstrbuf, FONT_STANDARD);
  y += font->pixel + 1;
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, y, _("to continue ..."),
                              FONT_STANDARD);

  // progressbar
  oledFrame(0, OLED_HEIGHT - 8, OLED_WIDTH - 1, OLED_HEIGHT - 1);
  oledBox(1, OLED_HEIGHT - 7, OLED_WIDTH - 2, OLED_HEIGHT - 2, 0);
  progress = progress * (OLED_WIDTH - 4) / 1000;
  if (progress > OLED_WIDTH - 4) {
    progress = OLED_WIDTH - 4;
  }
  oledBox(2, OLED_HEIGHT - 6, 1 + progress, OLED_HEIGHT - 3, 1);
  oledRefresh();
#endif

  // Check for Cancel / Initialize.
  protectAbortedByCancel = (msg_tiny_id == MessageType_MessageType_Cancel);
  protectAbortedByInitialize =
      (msg_tiny_id == MessageType_MessageType_Initialize);
  if (protectAbortedByCancel || protectAbortedByInitialize) {
    msg_tiny_id = 0xFFFF;
    usbTiny(0);
    fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
    return sectrue;
  }

  return secfalse;
}

bool protectPin(bool use_cached) {
  const char *newpin = NULL;
  if (use_cached && session_isUnlocked()) {
    return true;
  }

  const char *pin = "";
  if (config_hasPin()) {
    pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                     _("Please enter current PIN"), &newpin);
    if (!pin) {
      fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;
  }

  bool ret = config_unlock(pin);
  if (!ret) {
    fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
    protectPinCheck(false);
  }
  return ret;
}

bool protectChangePin(bool removal) {
  static CONFIDENTIAL char old_pin[MAX_PIN_LEN + 1] = "";
  static CONFIDENTIAL char new_pin[MAX_PIN_LEN + 1] = "";
  const char *pin = NULL;
  const char *newpin = NULL;
  bool need_new_pin = true;

  if (config_hasPin()) {
#if ONEKEY_MINI
    pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                     _("Verify PIN"), &newpin);
#else
    pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                     _("Please enter current PIN"), &newpin);
#endif

    if (pin == NULL) {
      fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;

    // If removing, defer the check to config_changePin().
    if (!removal) {
      usbTiny(1);
      bool ret = config_unlock(pin);
      usbTiny(0);
      if (ret == false) {
        fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
        protectPinCheck(false);
        return false;
      }
    }
    strlcpy(old_pin, pin, sizeof(old_pin));
    if (g_bIsBixinAPP) {
      need_new_pin = false;
      if (newpin == NULL) {
        newpin = protectInputPin(_("Please enter new PIN"), DEFAULT_PIN_LEN,
                                 MAX_PIN_LEN, true);
      }
    }
  }

  if (!removal) {
    if (!g_bIsBixinAPP || need_new_pin) {
#if ONEKEY_MINI
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_NewFirst,
                       _("Set new PIN"), &newpin);
#else
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_NewFirst,
                       _("Please enter new PIN"), &newpin);
#endif
    } else {
      if (newpin == NULL) {
        fsm_sendFailure(FailureType_Failure_PinExpected, NULL);
        layoutHome();
        return false;
      }
      pin = newpin;
    }
    if (pin == PIN_CANCELED_BY_BUTTON) {
      return false;
    } else if (pin == NULL || pin[0] == '\0') {
      memzero(old_pin, sizeof(old_pin));
      fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
      return false;
    }

    strlcpy(new_pin, pin, sizeof(new_pin));

    if (!g_bIsBixinAPP) {
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_NewSecond,
                       _("Please re-enter new PIN"), &newpin);
      if (pin == NULL) {
        memzero(old_pin, sizeof(old_pin));
        memzero(new_pin, sizeof(new_pin));
        fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
        return false;
      } else if (pin == PIN_CANCELED_BY_BUTTON)
        return false;
      if (strncmp(new_pin, pin, sizeof(new_pin)) != 0) {
        memzero(old_pin, sizeof(old_pin));
        memzero(new_pin, sizeof(new_pin));
        fsm_sendFailure(FailureType_Failure_PinMismatch, NULL);
        return false;
      }
    } else {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Please confirm PIN"), NULL, new_pin, NULL, NULL,
                        NULL);
      if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
        i2c_set_wait(false);
        fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
        layoutHome();
        return false;
      }
    }
  }

  bool ret = config_changePin(old_pin, new_pin);
  memzero(old_pin, sizeof(old_pin));
  memzero(new_pin, sizeof(new_pin));
  if (ret == false) {
    i2c_set_wait(false);
    if (removal) {
      fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
    } else {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("The new PIN must be different from your wipe code."));
    }
  }
  return ret;
}

bool protectChangeWipeCode(bool removal) {
  static CONFIDENTIAL char pin[MAX_PIN_LEN + 1] = "";
  static CONFIDENTIAL char wipe_code[MAX_PIN_LEN + 1] = "";
  const char *input = NULL;
  const char *newpin = NULL;

  if (config_hasPin()) {
    input = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                       _("Please enter your PIN:"), &newpin);
    if (input == NULL) {
      fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;

    // If removing, defer the check to config_changeWipeCode().
    if (!removal) {
      usbTiny(1);
      bool ret = config_unlock(input);
      usbTiny(0);
      if (ret == false) {
        fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
        return false;
      }
    }

    strlcpy(pin, input, sizeof(pin));
  }

  if (!removal) {
    input = requestPin(PinMatrixRequestType_PinMatrixRequestType_WipeCodeFirst,
                       _("Enter new wipe code:"), &newpin);
    if (input == NULL) {
      memzero(pin, sizeof(pin));
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;
    if (strncmp(pin, input, sizeof(pin)) == 0) {
      memzero(pin, sizeof(pin));
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("The wipe code must be different from your PIN."));
      return false;
    }
    strlcpy(wipe_code, input, sizeof(wipe_code));

    input = requestPin(PinMatrixRequestType_PinMatrixRequestType_WipeCodeSecond,
                       _("Re-enter new wipe code:"), &newpin);
    if (input == NULL) {
      memzero(pin, sizeof(pin));
      memzero(wipe_code, sizeof(wipe_code));
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;

    if (strncmp(wipe_code, input, sizeof(wipe_code)) != 0) {
      memzero(pin, sizeof(pin));
      memzero(wipe_code, sizeof(wipe_code));
      fsm_sendFailure(FailureType_Failure_WipeCodeMismatch, NULL);
      return false;
    }
  }

  bool ret = config_changeWipeCode(pin, wipe_code);
  memzero(pin, sizeof(pin));
  memzero(wipe_code, sizeof(wipe_code));
  if (ret == false) {
    fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
  }
  return ret;
}

bool protectPassphrase(char *passphrase) {
  memzero(passphrase, MAX_PASSPHRASE_LEN + 1);
  bool passphrase_protection = false;
  config_getPassphraseProtection(&passphrase_protection);
  if (!passphrase_protection) {
    // passphrase already set to empty by memzero above
    return true;
  }

  PassphraseRequest resp = {0};
  memzero(&resp, sizeof(PassphraseRequest));
  usbTiny(1);
  msg_write(MessageType_MessageType_PassphraseRequest, &resp);

  if (!g_bIsBixinAPP) {
    layoutDialogSwipe(&bmp_icon_info, NULL, NULL, NULL, _("Please enter your"),
                      _("passphrase using"), _("the computer's"),
                      _("keyboard."), NULL, NULL);
  }

  bool result = false;
  timer_out_set(timer_out_oper, default_oper_time);
  while (timer_out_get(timer_out_oper)) {
    usbPoll();
    buttonUpdate();
    if (msg_tiny_id == MessageType_MessageType_PassphraseAck) {
      msg_tiny_id = 0xFFFF;
      PassphraseAck *ppa = (PassphraseAck *)msg_tiny;
      if (ppa->has_on_device && ppa->on_device == true) {
        fsm_sendFailure(FailureType_Failure_DataError,
                        _("This firmware is incapable of passphrase entry on "
                          "the device."));
        result = false;
        break;
      }
      if (!ppa->has_passphrase) {
        fsm_sendFailure(FailureType_Failure_DataError,
                        _("No passphrase provided. Use empty string to set an "
                          "empty passphrase."));
        result = false;
        break;
      }
      strlcpy(passphrase, ppa->passphrase, sizeof(ppa->passphrase));
      result = true;
      break;
    }
    if (button.NoUp) {
      result = false;
      break;
    }
    if (button.YesUp) {
      result = true;
      break;
    }
    // check for Cancel / Initialize
    protectAbortedByCancel = (msg_tiny_id == MessageType_MessageType_Cancel);
    protectAbortedByInitialize =
        (msg_tiny_id == MessageType_MessageType_Initialize);
    if (protectAbortedByCancel || protectAbortedByInitialize) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      result = false;
      msg_tiny_id = 0xFFFF;
      break;
    }
  }
  usbTiny(0);
  layoutHome();
  return result;
}

bool protectSeedPin(bool force_pin, bool setpin, bool update_pin) {
  static CONFIDENTIAL char old_pin[MAX_PIN_LEN + 1] = "";
  static CONFIDENTIAL char new_pin[MAX_PIN_LEN + 1] = "";
  const char *pin = NULL;
  const char *newpin = NULL;

  if (update_pin) {
    bool ret = config_changePin(old_pin, new_pin);
    memzero(old_pin, sizeof(old_pin));
    memzero(new_pin, sizeof(new_pin));
    if (ret == false) {
      i2c_set_wait(false);
      fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
      return false;
    }
  } else {
    if (config_hasPin()) {
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                       _("Please enter current PIN"), &newpin);
      if (!pin) {
        fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
        return false;
      } else if (pin == PIN_CANCELED_BY_BUTTON)
        return false;

      bool ret = config_unlock(pin);
      if (!ret) {
        fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
        protectPinCheck(false);
        return false;
      }
    } else {
      if (force_pin) {
        pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_NewFirst,
                         _("Please enter new PIN"), &newpin);
        if (pin == PIN_CANCELED_BY_BUTTON) {
          return false;
        } else if (pin == NULL || pin[0] == '\0') {
          fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
          return false;
        }

        layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                          _("Please confirm PIN"), NULL, NULL, pin, NULL, NULL);

        if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall,
                           false)) {
          i2c_set_wait(false);
          fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
          layoutHome();
          return false;
        }
        strlcpy(new_pin, pin, sizeof(new_pin));
        if (setpin) {
          bool ret = config_changePin(old_pin, new_pin);
          memzero(old_pin, sizeof(old_pin));
          memzero(new_pin, sizeof(new_pin));
          if (ret == false) {
            i2c_set_wait(false);
            fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
            return false;
          }
        }
      } else {
        pin = "";
      }
    }

#if !ONEKEY_MINI
    if (!config_setSeedPin(pin)) {
      fsm_sendFailure(FailureType_Failure_PinMismatch, NULL);
      return false;
    }
#endif
  }
  return true;
}

uint8_t protectWaitKey(uint32_t time_out, uint8_t mode) {
  uint8_t key = KEY_NULL;

  protectAbortedByInitialize = false;
  usbTiny(1);
  timer_out_set(timer_out_oper, time_out);
  while (1) {
    layoutEnterSleep();
    usbPoll();
    if (time_out > 0 && timer_out_get(timer_out_oper) == 0) break;
    protectAbortedByInitialize =
        (msg_tiny_id == MessageType_MessageType_Initialize);
    if (protectAbortedByInitialize) {
      msg_tiny_id = 0xFFFF;
      break;
    }
    key = keyScan();
    if (key != KEY_NULL) {
      if (device_sleep_state) device_sleep_state = SLEEP_CANCEL_BY_BUTTON;
      if (mode == 0) {
        break;
      } else {
        if (key == KEY_CONFIRM || key == KEY_CANCEL) break;
      }
    }
    if (device_sleep_state == SLEEP_REENTER) {
      break;
    }
  }
  usbTiny(0);
  if (protectAbortedByInitialize) {
    if (device_sleep_state) device_sleep_state = SLEEP_CANCEL_BY_USB;
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
  }

  return key;
}

const char *protectInputPin(const char *text, uint8_t min_pin_len,
                            uint8_t max_pin_len, bool cancel_allowed) {
  uint8_t key = KEY_NULL;
  uint8_t counter = 0;
  int index = 0, max_index = 0;
  bool update = true, first_num = false;
  static char pin[10] = "";

  memzero(pin, sizeof(pin));

refresh_menu:
  if (update) {
    update = false;
    first_num = false;
    if (counter >= min_pin_len) {
      max_index = 10;
    } else {
      max_index = 9;
    }
    if (counter >= DEFAULT_PIN_LEN) {
      index = 10;
      first_num = true;
    } else {
      do {
        index = random_uniform(10);
      } while (index == 0);
    }
  } else if (first_num) {
    first_num = false;
    do {
      index = random_uniform(10);
    } while (index == 0);
  }

  layoutInputPin(counter, text, index, cancel_allowed);
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 1)
        index--;
      else
        index = max_index;
      goto refresh_menu;
    case KEY_DOWN:
      if (index < max_index)
        index++;
      else
        index = 1;
      goto refresh_menu;
    case KEY_CONFIRM:
      (void)pin;
      if (index == 10) {
        return pin;
      } else {
        pin[counter++] = index + '0';
        if (counter == max_pin_len) return pin;
        update = true;
        goto refresh_menu;
      }
    case KEY_CANCEL:
      if (counter) {
        counter--;
        pin[counter] = 0;
        update = true;
        goto refresh_menu;
      }
      break;
    default:
      break;
  }
  return NULL;
}

bool protectPinOnDevice(bool use_cached, bool cancel_allowed) {
  //   static bool input_pin = false;  // void recursive
  if (use_cached && session_isUnlocked()) {
    return true;
  }
  //   if (input_pin) return true;
  const char *pin = "";
input:
  if (config_hasPin()) {
    // input_pin = true;
#if ONEKEY_MINI
    pin = protectInputPin(_("Enter PIN code"), MIN_PIN_LEN, MAX_PIN_LEN,
                          cancel_allowed);
#else
    pin = protectInputPin(_("Please enter current PIN"), MIN_PIN_LEN,
                          MAX_PIN_LEN, cancel_allowed);
#endif
    // input_pin = false;
    if (!pin) {
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;
  }

  bool ret = config_unlock(pin);
  if (ret == false) {
    if (protectPinCheck(true)) {
      goto input;
    } else
      return false;
  }
  return ret;
}

bool protectChangePinOnDevice(bool is_prompt, bool set) {
  static CONFIDENTIAL char old_pin[MAX_PIN_LEN + 1] = "";
  static CONFIDENTIAL char new_pin[MAX_PIN_LEN + 1] = "";
  const char *pin = NULL;
  bool is_change = false;
  uint8_t key;

pin_set:
  if (config_hasPin()) {
    is_change = true;
  input:
#if ONEKEY_MINI
    pin = protectInputPin(_("Verify PIN"), MIN_PIN_LEN, MAX_PIN_LEN, true);
#else
    pin = protectInputPin(_("Please enter current PIN"), MIN_PIN_LEN,
                          MAX_PIN_LEN, true);
#endif

    if (pin == NULL) {
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;

    bool ret = config_unlock(pin);
    if (ret == false) {
      if (protectPinCheck(true)) {
        goto input;
      } else
        return false;
    }

    strlcpy(old_pin, pin, sizeof(old_pin));
  } else {
#if ONEKEY_MINI
    layoutDialogSwipeCenterAdapterEx(
        NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
        NULL, NULL, NULL,
        _("Please set your PIN.\nPIN is used to unlock\nyour device. "
          "Please\nkeep it safe."),
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
    layoutDialogSwipeCenterAdapter(
        NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
        NULL, NULL, _("Please set the PIN"), NULL, NULL);
#endif
    key = protectWaitKey(0, 1);
    if (key != KEY_CONFIRM) {
      return false;
    }
  }
retry:
#if ONEKEY_MINI
  pin = protectInputPin(_("Set new PIN"), DEFAULT_PIN_LEN, MAX_PIN_LEN, true);
#else
  pin = protectInputPin(_("Please enter new PIN"), DEFAULT_PIN_LEN, MAX_PIN_LEN,
                        true);
#endif
  if (pin == PIN_CANCELED_BY_BUTTON) {
    return false;
  } else if (pin == NULL || pin[0] == '\0') {
    memzero(old_pin, sizeof(old_pin));
    if (set) {
      goto_check(pin_set);
    }
    return false;
  }
  strlcpy(new_pin, pin, sizeof(new_pin));

#if ONEKEY_MINI
  pin =
      protectInputPin(_("Enter PIN again"), DEFAULT_PIN_LEN, MAX_PIN_LEN, true);
#else
  pin = protectInputPin(_("Please re-enter new PIN"), DEFAULT_PIN_LEN,
                        MAX_PIN_LEN, true);
#endif
  if (pin == NULL) {
    memzero(old_pin, sizeof(old_pin));
    memzero(new_pin, sizeof(new_pin));
    if (set) {
      goto_check(retry);
    }
    return false;
  } else if (pin == PIN_CANCELED_BY_BUTTON)
    return false;
  if (strncmp(new_pin, pin, sizeof(new_pin)) != 0) {
    memzero(old_pin, sizeof(old_pin));
    memzero(new_pin, sizeof(new_pin));
#if ONEKEY_MINI
    setRgbBitmap(true);
    layoutDialogSwipeCenterAdapter(&bmp_icon_forbid, NULL, NULL, &bmp_btn_retry,
                                   _("RETRY"), NULL, NULL, NULL, NULL, NULL,
                                   NULL, _("PINs do not match,\ntry again"));
#else
    layoutDialogSwipeCenterAdapter(
        &bmp_icon_error, NULL, NULL, &bmp_btn_retry, _("Retry"), NULL, NULL,
        NULL, NULL, _("Inconsistent PIN code"), _("Please try again"), NULL);
#endif
    while (1) {
      key = protectWaitKey(0, 1);
#if ONEKEY_MINI
      setRgbBitmap(false);
#endif
      if (key == KEY_CONFIRM) {
        goto retry;
      } else if (key == KEY_NULL) {
        return false;
      }
    }
  }

  bool ret = config_changePin(old_pin, new_pin);
  memzero(old_pin, sizeof(old_pin));
  memzero(new_pin, sizeof(new_pin));
  if (ret == false) {
  } else {
    if (is_prompt) {
      layoutDialogSwipeCenterAdapter(
          &bmp_icon_ok, NULL, NULL, &bmp_btn_confirm, _("Done"), NULL, NULL,
          NULL, NULL, is_change ? _("PIN code change") : _("PIN code set"),
          _("successfully"), NULL);
      protectWaitKey(0, 1);
      layoutHome();
    }
  }
  return ret;
}

bool protectSelectMnemonicNumber(uint32_t *number) {
  uint8_t key = KEY_NULL;
#if ONEKEY_MINI
  char desc[64] = "";
  uint32_t index = 1;
  strcat(desc, _("Select the number of\nrecovery phrase word"));
#else
  uint32_t index = 0;
#endif

  uint32_t num_s[3] = {12, 18, 24};
  char *numbers[3] = {"12", "18", "24"};

#if !ONEKEY_MINI
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      NULL, _("Please select the"), _("number of Mnemonic"), NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }
#endif

refresh_menu:
#if ONEKEY_MINI
  layoutItemsSelectAdapterLeft(
      &bmp_btn_up, &bmp_btn_down, NULL, NULL, NULL, NULL, index + 1, 3, NULL,
      desc, NULL, numbers[index], index > 0 ? numbers[index - 1] : NULL,
      index < 2 ? numbers[index + 1] : NULL);
#else
  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                           NULL, _("Okay"), index + 1, 3, NULL, NULL,
                           numbers[index],
                           index > 0 ? numbers[index - 1] : NULL,
                           index < 2 ? numbers[index + 1] : NULL);
#endif

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_DOWN:
      if (index < 2) index++;
      goto refresh_menu;
    case KEY_CONFIRM:
      *number = num_s[index];
      return true;
    case KEY_CANCEL:
      goto refresh_menu;
    default:
      return false;
  }
}

bool protectPinCheck(bool retry) {
#if !EMULATOR
  uint8_t key = KEY_NULL;
  char desc1[64] = "";
  char desc2[64] = "";

  uint32_t fails = config_getPinFails();
  if (fails == 1) {
    layoutDialogCenterAdapter(
        &bmp_icon_error, NULL, NULL, retry ? &bmp_btn_retry : &bmp_btn_confirm,
        retry ? _("Retry") : _("Okay"), NULL, NULL, NULL, NULL,
        _("PIN invalid"), _("You still have 9 times"), _("to try"));
  } else if (fails > 1 && fails < 10) {
    strcat(desc1, _("Wrong PIN for "));
    uint2str(fails, desc1 + strlen(desc1));
    strcat(desc1, _("times"));
    strcat(desc2, _("you still have "));
    uint2str(10 - fails, desc2 + strlen(desc2));
    strcat(desc2, _("chances"));
    layoutDialogCenterAdapter(&bmp_icon_error, NULL, NULL,
                              retry ? &bmp_btn_retry : &bmp_btn_confirm,
                              retry ? _("Retry") : _("Okay"), NULL, NULL, NULL,
                              NULL, desc1, desc2, _("to try"));
  } else {
    layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              _("Device reset in progress"), NULL, NULL, NULL);
    protectWaitKey(timer1s * 1, 0);

    uint8_t ui_language_bak = ui_language;

    config_wipe();
    if (ui_language_bak) {
      ui_language = ui_language_bak;
    }
    layoutDialogSwipeCenterAdapter(
        &bmp_icon_info, NULL, NULL, &bmp_btn_confirm, _("Confirm"), NULL, NULL,
        NULL, NULL, _("Device has been reset"), _("Please reboot"), NULL);
    protectWaitKey(0, 0);

    svc_system_reset();
  }
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      return true;
    } else if (key == KEY_NULL) {
      return false;
    }
  }

#endif
  return false;
}

#if !EMULATOR

void enter_sleep(void) {
#if ONEKEY_MINI
  static bool backup = false;
#endif
  static int sleep_count = 0;
  uint8_t key = KEY_NULL;
  bool unlocked = false;
  uint8_t oled_prev[OLED_BUFSIZE];
  void *layoutBack = NULL;
  sleep_count++;
  if (sleep_count == 1) {
    unlocked = session_isUnlocked();
    layoutBack = layoutLast;
    oledBufferLoad(oled_prev);
#if ONEKEY_MINI
    if (isRgbBitmap()) {
      setRgbBitmap(false);
      backup = true;
    }
#endif
    config_lockDevice();
  }

#if ONEKEY_MINI
  // close back light
  timer_disable_oc_output(TIM3, TIM_OC2);
#endif

  layoutScreensaver();
  if (sleep_count == 1) {
  sleep_loop:
    device_sleep_state = SLEEP_ENTER;
    key = protectWaitKey(0, 0);
    if (device_sleep_state == SLEEP_REENTER) {
      goto sleep_loop;
    }
    if (key == KEY_NULL) {
      device_sleep_state = SLEEP_NONE;
      sleep_count = 0;
      return;
    }
    if (unlocked) {
      layoutHomeEx();
#if ONEKEY_MINI
      // open back light
      timer_enable_oc_output(TIM3, TIM_OC2);
#endif
      key = protectWaitKey(0, 0);
      if (device_sleep_state == SLEEP_REENTER) {
        goto sleep_loop;
      }
      if (key == KEY_NULL) {
        device_sleep_state = SLEEP_NONE;
        sleep_count = 0;
        return;
      }
    input_pin:
      if (!protectPinOnDevice(false, false)) {
        if (device_sleep_state == SLEEP_REENTER) {
          goto sleep_loop;
        }
        if (device_sleep_state == SLEEP_CANCEL_BY_BUTTON) {
          device_sleep_state = SLEEP_ENTER;
          goto input_pin;
        }
        if (device_sleep_state == SLEEP_CANCEL_BY_USB) {
          device_sleep_state = SLEEP_NONE;
          layoutHome();
          sleep_count = 0;
          return;
        }
      }
    }
  }
  if (sleep_count > 1) {
    device_sleep_state = SLEEP_REENTER;
  }
  sleep_count--;
  if (sleep_count == 0) {
    layoutLast = layoutBack;
    oledBufferRestore(oled_prev);
#if ONEKEY_MINI
    if (backup) {
      setRgbBitmap(true);
      backup = false;
    }
#endif
    oledRefresh();
#if ONEKEY_MINI
    // open back light
    timer_enable_oc_output(TIM3, TIM_OC2);
#endif
    device_sleep_state = SLEEP_NONE;
    return;
  }
}
#endif
