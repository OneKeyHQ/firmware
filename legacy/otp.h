/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2019 Pavol Rusnak <stick@satoshilabs.com>
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

#ifndef __OTP_H__
#define __OTP_H__

#include <stdbool.h>
#include <stdint.h>

#define FLASH_OTP_BASE 0x1FFF7800U
#define FLASH_OTP_LOCK_BASE 0x1FFF7A00U

#define FLASH_OTP_NUM_BLOCKS 16
#define FLASH_OTP_BLOCK_SIZE 32

#define FLASH_OTP_BLOCK_RANDOMNESS 3

#define FLASH_OTP_DEVICE_SERIAL 7

#define FLASH_OTP_BLOCK_608_SERIAL 8
#define FLASH_OTP_BLOCK_608_PROTECT_KEY 9
#define FLASH_OTP_BLOCK_608_INIT_PIN 10
#define FLASH_OTP_BLOCK_608_MIX_PIN 11

//#define FLASH_OTP_DEVICE_SERIAL 12 //deprecated,this zone may have been used
#define FLASH_OTP_FACTORY_TEST 13
#define FLASH_OTP_RANDOM_KEY 14
#define FLASH_OTP_CPU_FIRMWARE_INFO 15

bool flash_otp_is_locked(uint8_t block);
bool flash_otp_lock(uint8_t block);
bool flash_otp_read(uint8_t block, uint8_t offset, uint8_t *data,
                    uint8_t datalen);
bool flash_otp_write(uint8_t block, uint8_t offset, const uint8_t *data,
                     uint8_t datalen);
bool flash_otp_write_safe(uint8_t block, uint8_t offset, const uint8_t *data,
                          uint8_t datalen);

#endif
