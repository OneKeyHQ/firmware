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

#ifndef TREZORHAL_FLASH_H
#define TREZORHAL_FLASH_H

#include <stdint.h>
#include <stdlib.h>
#include "secbool.h"

// see docs/memory.md for more information
#if PRODUCTION_MODEL == 'H'

#define USE_EXTERN_FLASH 1
#if USE_EXTERN_FLASH
// 16 internal + 16 external code(2x64K) + 2 external storage(64K)
#define FLASH_SECTOR_COUNT (34)
#define FLASH_FIRMWARE_SECTOR_SIZE (128 * 1024)
#define FLASH_STORAGE_SECTOR_SIZE (64 * 1024)
#define FLASH_INNER_COUNT 16
#else
#define FLASH_SECTOR_COUNT 16
#endif
#elif TREZOR_MODEL == T
#define FLASH_SECTOR_COUNT 24
#elif TREZOR_MODEL == 1
#define FLASH_SECTOR_COUNT 12
#else
#error Unknown Trezor model
#endif

#if PRODUCTION_MODEL == 'H'
#define FLASH_SECTOR_BOARDLOADER_START 0
#define FLASH_SECTOR_BOARDLOADER_END 0
#else
#define FLASH_SECTOR_BOARDLOADER_START 0
//                                           1
#define FLASH_SECTOR_BOARDLOADER_END 2

//                                           3
#endif

#if PRODUCTION_MODEL == 'H'
#if USE_EXTERN_FLASH
#define FLASH_SECTOR_STORAGE_1 32
#define FLASH_SECTOR_STORAGE_2 33
#else
#define FLASH_SECTOR_STORAGE_1 2
#define FLASH_SECTOR_STORAGE_2 3
#endif
#elif TREZOR_MODEL == T
#define FLASH_SECTOR_STORAGE_1 4
#define FLASH_SECTOR_STORAGE_2 16
#elif TREZOR_MODEL == 1
#define FLASH_SECTOR_STORAGE_1 2
#define FLASH_SECTOR_STORAGE_2 3
#else
#error Unknown Trezor model
#endif

#if PRODUCTION_MODEL == 'H'
#define FLASH_SECTOR_BOOTLOADER_1 1
#define FLASH_SECTOR_BOOTLOADER_2 2

#if USE_EXTERN_FLASH
#define FLASH_SECTOR_FIRMWARE_START 3
#define FLASH_SECTOR_FIRMWARE_END 14
#else
#define FLASH_SECTOR_FIRMWARE_START 4
#define FLASH_SECTOR_FIRMWARE_END 14
#endif

#define FLASH_SECTOR_UNUSED_START 15
#define FLASH_SECTOR_UNUSED_END 15
#define FLASH_SECTOR_OTP_EMULATOR 15

#define FLASH_SECTOR_FIRMWARE_EXTRA_START 17
#define FLASH_SECTOR_FIRMWARE_EXTRA_END 32

#define BOOTLOADER_SECTORS_COUNT (2)
#define STORAGE_SECTORS_COUNT (2)
#define FIRMWARE_SECTORS_COUNT (12 + 16)
#else
#define FLASH_SECTOR_BOOTLOADER 5

#define FLASH_SECTOR_FIRMWARE_START 6
//                                           7
//                                           8
//                                           9
//                                          10
#define FLASH_SECTOR_FIRMWARE_END 11

#define FLASH_SECTOR_UNUSED_START 12
//                                          13
//                                          14
#define FLASH_SECTOR_UNUSED_END 15

#define FLASH_SECTOR_FIRMWARE_EXTRA_START 17
//                                          18
//                                          19
//                                          20
//                                          21
//                                          22
#define FLASH_SECTOR_FIRMWARE_EXTRA_END 23

#define BOOTLOADER_SECTORS_COUNT (1)
#define STORAGE_SECTORS_COUNT (2)
#define FIRMWARE_SECTORS_COUNT (6 + 7)
#endif

extern const uint8_t STORAGE_SECTORS[STORAGE_SECTORS_COUNT];
extern const uint8_t FIRMWARE_SECTORS[FIRMWARE_SECTORS_COUNT];

// note: FLASH_SR_RDERR is STM32F42xxx and STM32F43xxx specific (STM32F427)
// (reference RM0090 section 3.7.5)
#ifndef STM32F427xx
#define FLASH_SR_RDERR 0
#endif

#define FLASH_STATUS_ALL_FLAGS                                            \
  (FLASH_SR_RDERR | FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR | \
   FLASH_SR_WRPERR | FLASH_SR_SOP | FLASH_SR_EOP)

void flash_init(void);

secbool __wur flash_unlock_write(void);
secbool __wur flash_lock_write(void);

const void *flash_get_address(uint8_t sector, uint32_t offset, uint32_t size);

secbool __wur flash_erase_sectors(const uint8_t *sectors, int len,
                                  void (*progress)(int pos, int len));
static inline secbool flash_erase(uint8_t sector) {
  return flash_erase_sectors(&sector, 1, NULL);
}
secbool __wur flash_write_byte(uint8_t sector, uint32_t offset, uint8_t data);
secbool __wur flash_write_word(uint8_t sector, uint32_t offset, uint32_t data);
secbool __wur flash_write_words(uint8_t sector, uint32_t offset,
                                uint32_t data[8]);

#define FLASH_OTP_NUM_BLOCKS 16
#define FLASH_OTP_BLOCK_SIZE 32

typedef struct __attribute__((packed)) {
  uint8_t flag[32];
  uint8_t flash_otp[FLASH_OTP_NUM_BLOCKS][FLASH_OTP_BLOCK_SIZE];
} FlashLockedData;

// OTP blocks allocation
#define FLASH_OTP_BLOCK_BATCH 0
#define FLASH_OTP_BLOCK_BOOTLOADER_VERSION 1
#define FLASH_OTP_BLOCK_VENDOR_HEADER_LOCK 2
#define FLASH_OTP_BLOCK_RANDOMNESS 3

#define FLASH_OTP_BLOCK_608_SERIAL 8
#define FLASH_OTP_BLOCK_608_PROTECT_KEY 9
#define FLASH_OTP_BLOCK_608_INIT_PIN 10
#define FLASH_OTP_BLOCK_608_MIX_PIN 11

#define FLASH_OTP_DEVICE_SERIAL 12
#define FLASH_OTP_FACTORY_TEST 13
#define FLASH_OTP_RANDOM_KEY 14
#define FLASH_OTP_CPU_FIRMWARE_INFO 15

extern FlashLockedData *flash_otp_data;

void flash_otp_init(void);
secbool __wur flash_otp_read(uint8_t block, uint8_t offset, uint8_t *data,
                             uint8_t datalen);
secbool __wur flash_otp_write(uint8_t block, uint8_t offset,
                              const uint8_t *data, uint8_t datalen);
secbool __wur flash_otp_lock(uint8_t block);
secbool __wur flash_otp_is_locked(uint8_t block);

void flash_test(void);

#endif  // TREZORHAL_FLASH_H
