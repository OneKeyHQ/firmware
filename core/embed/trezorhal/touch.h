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

#ifndef TREZORHAL_TOUCH_H
#define TREZORHAL_TOUCH_H

#include <stdint.h>

#if PRODUCTION_MODEL== 'H'
#define TS_I2C_PORT GPIOB
#define TS_I2C_SCL_PIN GPIO_PIN_10
#define TS_I2C_SDA_PIN GPIO_PIN_11

#define TS_RESET_PORT GPIOC
#define TS_RESET_PIN GPIO_PIN_1

#define TS_INIT_PORT GPIOC
#define TS_INIT_PIN GPIO_PIN_0

#else
#define TS_I2C_PORT GPIOB
#define TS_I2C_SCL_PIN GPIO_PIN_6
#define TS_I2C_SDA_PIN GPIO_PIN_7

#define TS_RESET_PORT GPIOC
#define TS_RESET_PIN GPIO_PIN_5

#define TS_INIT_PORT GPIOC
#define TS_INIT_PIN GPIO_PIN_4
#endif

#define TOUCH_START (1U << 24)
#define TOUCH_MOVE (1U << 25)
#define TOUCH_END (1U << 26)

void touch_test(void);

void touch_init(void);
void touch_power_on(void);
void touch_power_off(void);
void touch_sensitivity(uint8_t value);
uint32_t touch_read(void);
uint32_t touch_click(void);
uint32_t touch_is_detected(void);
static inline uint16_t touch_unpack_x(uint32_t evt) {
  return (evt >> 12) & 0xFFF;
}
static inline uint16_t touch_unpack_y(uint32_t evt) {
  return (evt >> 0) & 0xFFF;
}
static inline uint32_t touch_pack_xy(uint16_t x, uint16_t y) {
  return ((x & 0xFFF) << 12) | (y & 0xFFF);
}

#endif
