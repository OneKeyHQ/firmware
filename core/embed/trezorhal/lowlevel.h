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

#ifndef __TREZORHAL_LOWLEVEL_H__
#define __TREZORHAL_LOWLEVEL_H__

#include "secbool.h"

uint32_t flash_wait_and_clear_status_flags(void);
secbool flash_check_option_bytes(void);
void flash_lock_option_bytes(void);
void flash_unlock_option_bytes(void);
uint32_t flash_set_option_bytes(void);
secbool flash_configure_option_bytes(void);
void periph_init(void);
secbool reset_flags_check(void);
void reset_flags_reset(void);

#if defined(STM32H747xx)

#define SDRAM_DEVICE_ADDR 0xD0000000U
#define SDRAM_DEVICE_SIZE 0x2000000U

void cpu_cache_enable(void);
void mpu_config(void);
void system_clock_config(void);
void flash_option_bytes_init(void);

void led_init(void);
void led_on(void);
void led_off(void);
#endif

#endif  // __TREZORHAL_LOWLEVEL_H__
