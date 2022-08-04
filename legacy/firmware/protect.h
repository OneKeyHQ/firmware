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

#ifndef __PROTECT_H__
#define __PROTECT_H__

#include <stdbool.h>
#include "messages-common.pb.h"
#include "secbool.h"

#define MAX_PASSPHRASE_LEN 50
#define MAX_PIN_TRIES 10

#define PIN_CANCELED_BY_BUTTON (void*)1

enum {
  SLEEP_NONE = 0x00,
  SLEEP_ENTER = 0x01,
  SLEEP_REENTER = 0X02,
  SLEEP_CANCEL_BY_BUTTON = 0x03,
  SLEEP_CANCEL_BY_USB = 0x04
};

bool protectButton(ButtonRequestType type, bool confirm_only);
secbool protectPinUiCallback(uint32_t wait, uint32_t progress,
                             const char* message);
bool protectPin(bool use_cached);
bool protectButton_ex(ButtonRequestType type, bool confirm_only, bool requset,
                      uint32_t timeout_s);
bool protectChangePin(bool removal);
bool protectChangeWipeCode(bool removal);
bool protectPassphrase(char* passphrase);
bool protectSeedPin(bool force_pin, bool setpin, bool update_pin);
uint8_t protectWaitKey(uint32_t time_out, uint8_t mode);
uint8_t blindsignWaitKey(void);
const char* protectInputPin(const char* text, uint8_t min_pin_len,
                            uint8_t max_pin_len, bool cancel_allowed);
bool protectPinOnDevice(bool use_cached, bool cancel_allowed);
bool protectChangePinOnDevice(bool is_prompt, bool set);
bool protectSelectMnemonicNumber(uint32_t* number);
bool protectPinCheck(bool retry);
bool protectPassphraseOnDevice(char* passphrase);

uint8_t protectButtonValue(ButtonRequestType type, bool confirm_only,
                           bool requset, uint32_t timeout_s);

#if !EMULATOR
void enter_sleep(void);
#endif

extern bool protectAbortedByCancel;
extern bool protectAbortedByInitialize;
extern bool protectAbortedByTimeout;
#endif
