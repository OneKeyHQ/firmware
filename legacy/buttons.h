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

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <libopencm3/stm32/gpio.h>
#include <stdbool.h>

#include "sys.h"
#include "timer.h"

struct buttonState {
  volatile bool YesUp;
  volatile int YesDown;
  volatile bool NoUp;
  volatile int NoDown;
  volatile bool UpUp;
  volatile int UpDown;
  volatile bool DownUp;
  volatile int DownDown;
};
#if !EMULATOR
extern uint8_t change_ble_sta_flag;
#endif
extern struct buttonState button;

uint16_t buttonRead(void);
void buttonUpdate(void);
bool hasbutton(void);
void buttonsIrqInit(void);
void buttonsTimer(void);
bool checkButtonOrTimeout(uint16_t btn, TimerOut type);
bool waitButtonResponse(uint16_t btn, uint32_t time_out);
uint8_t keyScan(void);
uint8_t waitKey(uint32_t time_out, uint8_t mode);

#define KEY_NULL 0
#define KEY_UP 'U'
#define KEY_DOWN 'D'
#define KEY_CONFIRM 'O'
#define KEY_CANCEL 'C'

#ifndef BTN_PORT
#define BTN_PORT GPIOC
#endif

#ifndef BTN_PIN_YES
#define BTN_PIN_YES GPIO2
#endif

#if ONEKEY_MINI

#ifndef BTN_PIN_NO
#define BTN_PIN_NO GPIO5
#endif

#ifndef BTN_PIN_UP
#define BTN_PIN_UP GPIO3
#endif

#ifndef BTN_PIN_DOWN
#define BTN_PIN_DOWN GPIO11
#endif

#ifndef BTN_PORT_NO
#define BTN_PORT_NO BTN_PORT
#endif

#ifndef BTN_PIN_FUNC
#define BTN_PIN_FUNC GPIO7
#endif

#else

#ifndef BTN_PIN_UP
#define BTN_PIN_UP GPIO3
#endif

#ifndef BTN_PIN_DOWN
#define BTN_PIN_DOWN GPIO5
#endif

#ifndef BTN_PORT_NO
#define BTN_PORT_NO BTN_POWER_PORT
#endif

#ifndef BTN_PIN_NO
#define BTN_PIN_NO BTN_POWER_PIN
#endif

#endif

#endif
