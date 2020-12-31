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
#include "si2c.h"
#include "sys.h"
#include "usb.h"
#include "util.h"

#define MAX_WRONG_PINS 15

bool protectAbortedByCancel = false;
bool protectAbortedByInitialize = false;
bool protectAbortedByTimeout = false;

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
      return protectInputPin(text, 6);
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
                     _("PIN REF Table"), &newpin);
    if (!pin) {
      fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;
  }

  bool ret = config_unlock(pin);
  if (!ret) {
    fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
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
    pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                     _("PIN REF Table"), &newpin);
    if (g_bIsBixinAPP) {
      need_new_pin = false;
      if (newpin == NULL) {
        newpin = protectInputPin(_("Please enter new PIN"), 6);
      }
    }
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
        return false;
      }
    }

    strlcpy(old_pin, pin, sizeof(old_pin));
  }

  if (!removal) {
    if (!g_bIsBixinAPP || need_new_pin) {
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_NewFirst,
                       _("PIN REF Table"), &newpin);
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
                       _("PIN REF Table"), &newpin);
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
                       _("PIN REF Table"), &newpin);
      if (!pin) {
        fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
        return false;
      } else if (pin == PIN_CANCELED_BY_BUTTON)
        return false;

      bool ret = config_unlock(pin);
      if (!ret) {
        fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
        return false;
      }
    } else {
      if (force_pin) {
        pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_NewFirst,
                         _("PIN REF Table"), &newpin);
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

    if (!config_setSeedPin(pin)) {
      fsm_sendFailure(FailureType_Failure_PinMismatch, NULL);
      return false;
    }
  }
  return true;
}

uint8_t protectWaitKey(uint32_t time_out, uint8_t mode) {
  uint8_t key = KEY_NULL;
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
      if (mode == 0) {
        break;
      } else {
        if (key == KEY_CONFIRM || key == KEY_CANCEL) break;
      }
    }
  }
  usbTiny(0);
  if (protectAbortedByInitialize)
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
  return key;
}

const char *protectInputPin(const char *text, uint8_t pin_len) {
  uint8_t key = KEY_NULL;
  uint8_t counter = 0;
  char value[2] = "1";
  static char pin[10] = "";

  memzero(pin, sizeof(pin));
refresh_menu:
  layoutInputPin(counter, text, value);
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (value[0] > '1')
        value[0]--;
      else
        value[0] = '9';
      goto refresh_menu;
    case KEY_DOWN:
      if (value[0] < '9')
        value[0]++;
      else
        value[0] = '1';
      goto refresh_menu;
    case KEY_CONFIRM:
      (void)pin;
      pin[counter++] = value[0];
      if (counter == pin_len) return pin;
      value[0] = '1';
      goto refresh_menu;
    case KEY_CANCEL:
      break;
    default:
      break;
  }
  return NULL;
}

bool protectPinOnDevice(bool use_cached) {
  //   static bool input_pin = false;  // void recursive
  if (use_cached && session_isUnlocked()) {
    return true;
  }
  //   if (input_pin) return true;
  const char *pin = "";
input:
  if (config_hasPin()) {
    // input_pin = true;
    pin = protectInputPin(_("Please enter currnet PIN"), 6);
    // input_pin = false;

    if (!pin) {
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;
  }

  bool ret = config_unlock(pin);
  if (ret == false) {
    if (protectPinCheck()) {
      goto input;
    } else
      return false;
  }
  return ret;
}

bool protectChangePinOnDevice(void) {
  static CONFIDENTIAL char old_pin[MAX_PIN_LEN + 1] = "";
  static CONFIDENTIAL char new_pin[MAX_PIN_LEN + 1] = "";
  const char *pin = NULL;

  if (config_hasPin()) {
  input:
    pin = protectInputPin(_("Please enter currnet PIN"), 6);

    if (pin == NULL) {
      return false;
    } else if (pin == PIN_CANCELED_BY_BUTTON)
      return false;

    bool ret = config_unlock(pin);
    if (ret == false) {
      if (protectPinCheck()) {
        goto input;
      } else
        return false;
    }

    strlcpy(old_pin, pin, sizeof(old_pin));
  }
retry:
  pin = protectInputPin(_("Please enter new PIN"), 6);
  if (pin == PIN_CANCELED_BY_BUTTON) {
    return false;
  } else if (pin == NULL || pin[0] == '\0') {
    memzero(old_pin, sizeof(old_pin));
    return false;
  }
  strlcpy(new_pin, pin, sizeof(new_pin));

  pin = protectInputPin(_("Please re-enter new PIN"), 6);
  if (pin == NULL) {
    memzero(old_pin, sizeof(old_pin));
    memzero(new_pin, sizeof(new_pin));
    return false;
  } else if (pin == PIN_CANCELED_BY_BUTTON)
    return false;
  if (strncmp(new_pin, pin, sizeof(new_pin)) != 0) {
    memzero(old_pin, sizeof(old_pin));
    memzero(new_pin, sizeof(new_pin));
    layoutDialogSwipeCenterAdapter(NULL, NULL, &bmp_btn_retry, _("Retry"), NULL,
                                   NULL, NULL, _("Inconsistent PIN code"),
                                   _("Please try again"), NULL, NULL);
    while (1) {
      uint8_t key = protectWaitKey(0, 1);
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
    layoutDialogSwipeCenterAdapter(NULL, NULL, &bmp_btn_confirm, _("Done"),
                                   NULL, NULL, NULL, _("PIN code change"),
                                   _("successfully"), NULL, NULL);
    protectWaitKey(0, 1);
  }
  layoutHome();
  return ret;
}

bool protectSelectMnemonicNumber(uint32_t *number) {
  uint8_t key = KEY_NULL;
  uint32_t index = 0;
  uint32_t num_s[3] = {12, 18, 24};
  char *numbers[3] = {"12", "18", "24"};

  layoutDialogSwipeCenterAdapter(
      &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
      _("Please select the"), _("number of Mnemonic"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

refresh_menu:
  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                           NULL, _("Okay"), index + 1, 3, NULL, NULL,
                           numbers[index],
                           index > 0 ? numbers[index - 1] : NULL,
                           index < 2 ? numbers[index + 1] : NULL);

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

bool protectPinCheck(void) {
  uint8_t key = KEY_NULL;
  char desc1[64] = "";
  char desc2[64] = "";

  uint32_t fails = config_getPinFails();
  if (fails == 1) {
    layoutDialogCenterAdapter(NULL, NULL, &bmp_btn_retry, _("Retry"), NULL,
                              NULL, NULL, _("Incorrect PIN entered"),
                              "You still have 5 times", "to try", NULL);
  } else if (fails > 1 && fails < 5) {
    strcat(desc1, _("Wrong PIN for "));
    uint2str(fails, desc1 + strlen(desc1));
    strcat(desc1, _(" times"));
    strcat(desc2, _("you still have "));
    uint2str(5 - fails, desc2 + strlen(desc2));
    strcat(desc2, _(" times"));
    layoutDialogCenterAdapter(NULL, NULL, &bmp_btn_retry, _("Retry"), NULL,
                              NULL, NULL, desc1, desc2, "to try", NULL);
  } else {
    layoutDialogCenterAdapter(NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                              _("Device will be erased"), NULL, NULL, NULL);
    protectWaitKey(timer1s * 2, 0);
    config_wipe();
    return false;
  }
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      return true;
    } else if (key == KEY_NULL) {
      return false;
    }
  }
}
