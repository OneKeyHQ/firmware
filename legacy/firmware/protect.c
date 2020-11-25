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
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "memory.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "oled.h"
#include "pinmatrix.h"
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
  // Convert wait to secstr string.
  char secstrbuf[] = "________0 seconds";
  char *secstr = secstrbuf + 9;
  uint32_t secs = wait;
  do {
    secstr--;
    *secstr = (secs % 10) + '0';
    secs /= 10;
  } while (secs > 0 && secstr >= secstrbuf);
  if (wait == 1) {
    // Change "seconds" to "second".
    secstrbuf[16] = 0;
  }
  oledClear();
  if (ui_language) {
    memset(secstrbuf + 10, 0x00, sizeof(secstrbuf) - 10);
    memcpy(secstrbuf + 10, "秒", strlen("秒"));
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 10, message, FONT_STANDARD);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 10 + 13, "请等待",
                                FONT_STANDARD);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 10 + 26, secstr, FONT_STANDARD);

  } else {
    oledDrawStringCenter(OLED_WIDTH / 2, 1 * 9, message, FONT_STANDARD);
    oledDrawStringCenter(OLED_WIDTH / 2, 2 * 9, _("Please wait"),
                         FONT_STANDARD);
    oledDrawStringCenter(OLED_WIDTH / 2, 3 * 9, secstr, FONT_STANDARD);
    oledDrawStringCenter(OLED_WIDTH / 2, 4 * 9, _("to continue ..."),
                         FONT_STANDARD);
  }

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
                     ui_prompt_current_pin[ui_language], &newpin);
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
    if (!g_bIsBixinAPP) {
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                       ui_prompt_current_pin[ui_language], &newpin);
    } else {
      pin = requestPin(PinMatrixRequestType_PinMatrixRequestType_Current,
                       ui_prompt_input_pin[ui_language], &newpin);
      need_new_pin = false;
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
                       ui_prompt_new_pin[ui_language], &newpin);
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
                       ui_prompt_new_pin_ack[ui_language], &newpin);
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
      if (ui_language) {
        layoutDialogSwipeAdapter(&bmp_icon_question, "取消", "确认", NULL,
                                 "请确认PIN码", NULL, new_pin, NULL);
      } else {
        layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                          _("Please confirm PIN"), NULL, NULL, new_pin, NULL,
                          NULL);
      }
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
    if (ui_language) {
      layoutDialogSwipeAdapter(&bmp_icon_info, NULL, NULL, NULL, "请输入密语",
                               NULL, NULL, NULL);
    } else {
      layoutDialogSwipe(&bmp_icon_info, NULL, NULL, NULL,
                        _("Please enter your"), _("passphrase using"),
                        _("the computer's"), _("keyboard."), NULL, NULL);
    }
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
                       ui_prompt_current_pin[ui_language], &newpin);
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
                         ui_prompt_input_pin[ui_language], &newpin);
        if (pin == PIN_CANCELED_BY_BUTTON) {
          return false;
        } else if (pin == NULL || pin[0] == '\0') {
          fsm_sendFailure(FailureType_Failure_PinCancelled, NULL);
          return false;
        }

        if (ui_language) {
          layoutDialogSwipeAdapter(&bmp_icon_question, "取消", "确认", NULL,
                                   "请确认PIN码", NULL, pin, NULL);
        } else {
          layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                            _("Please confirm PIN"), NULL, NULL, pin, NULL,
                            NULL);
        }
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
