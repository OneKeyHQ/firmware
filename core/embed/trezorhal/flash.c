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

#include STM32_HAL_H

#include <string.h>

#include "common.h"
#include "flash.h"

// see docs/memory.md for more information
#if defined(STM32H747xx)

#include "qspi_flash.h"

#define FLASH_TYPEPROGRAM_WORD FLASH_TYPEPROGRAM_FLASHWORD

FlashLockedData *flash_otp_data = (FlashLockedData *)0x081E0000;

#if 0
static const uint32_t FLASH_SECTOR_TABLE[FLASH_SECTOR_COUNT + 2] = {
    [0] = 0x08000000,   // - 0x0801FFFF | 128 KiB
    [1] = 0x08020000,   // - 0x08007FFF | 128 KiB
    [2] = 0x08040000,   // - 0x0800BFFF | 128 KiB
    [3] = 0x08060000,   // - 0x0800FFFF | 128 KiB
    [4] = 0x08080000,   // - 0x0801FFFF | 128 KiB
    [5] = 0x080A0000,   // - 0x0803FFFF | 128 KiB
    [6] = 0x080C0000,   // - 0x0805FFFF | 128 KiB
    [7] = 0x080E0000,   // - 0x0807FFFF | 128 KiB
    [8] = 0x08100000,   // - 0x0809FFFF | 128 KiB
    [9] = 0x08120000,   // - 0x080BFFFF | 128 KiB
    [10] = 0x08140000,  // - 0x080DFFFF | 128 KiB
    [11] = 0x08160000,  // - 0x080FFFFF | 128 KiB
    [12] = 0x08180000,  // - 0x08103FFF | 128 KiB
    [13] = 0x081A0000,  // - 0x08107FFF | 128 KiB
    [14] = 0x081C0000,  // - 0x0810BFFF | 128 KiB
    [15] = 0x081E0000,  // - 0x0810FFFF | 128 KiB
#if USE_EXTERN_FLASH
    [16] = 0x08200000,               // last element - not a valid sector
    [17] = QSPI_FLASH_BASE_ADDRESS,  // 128 KiB
    [18] = QSPI_FLASH_BASE_ADDRESS + 0x20000,                   // 128 KiB
    [19] = QSPI_FLASH_BASE_ADDRESS + 0x40000,                   // 128 KiB
    [20] = QSPI_FLASH_BASE_ADDRESS + 0x60000,                   // 128 KiB
    [21] = QSPI_FLASH_BASE_ADDRESS + 0x80000,                   // 128 KiB
    [22] = QSPI_FLASH_BASE_ADDRESS + 0xA0000,                   // 128 KiB
    [23] = QSPI_FLASH_BASE_ADDRESS + 0xC0000,                   // 128 KiB
    [24] = QSPI_FLASH_BASE_ADDRESS + 0xE0000,                   // 128 KiB
    [25] = QSPI_FLASH_BASE_ADDRESS + 0x100000,                  // 128 KiB
    [26] = QSPI_FLASH_BASE_ADDRESS + 0x120000,                  // 128 KiB
    [27] = QSPI_FLASH_BASE_ADDRESS + 0x140000,                  // 128 KiB
    [28] = QSPI_FLASH_BASE_ADDRESS + 0x160000,                  // 128 KiB
    [29] = QSPI_FLASH_BASE_ADDRESS + 0x180000,                  // 128 KiB
    [30] = QSPI_FLASH_BASE_ADDRESS + 0x1A0000,                  // 128 KiB
    [31] = QSPI_FLASH_BASE_ADDRESS + 0x1C0000,                  // 128 KiB
    [32] = QSPI_FLASH_BASE_ADDRESS + 0x1E0000,                  // 128 KiB
    [33] = QSPI_FLASH_BASE_ADDRESS + QSPI_FLASH_STORAG_OFFSET,  // 64 KiB
    [34] = QSPI_FLASH_BASE_ADDRESS + QSPI_FLASH_STORAG_OFFSET +
           64 * 1024,  //  64 KiB storage sector 2
    [35] = QSPI_FLASH_BASE_ADDRESS + QSPI_FLASH_STORAG_OFFSET + 128 * 1024,
#else
    [16] = 0x08200000,  // last element - not a valid sector
#endif

};
#endif

const uint8_t FIRMWARE_SECTORS[FIRMWARE_SECTORS_COUNT] = {
    FLASH_SECTOR_FIRMWARE_START,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    FLASH_SECTOR_FIRMWARE_END,
    FLASH_SECTOR_FIRMWARE_EXTRA_START,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    FLASH_SECTOR_FIRMWARE_EXTRA_END};
#else
static const uint32_t FLASH_SECTOR_TABLE[FLASH_SECTOR_COUNT + 1] = {
    [0] = 0x08000000,   // - 0x08003FFF |  16 KiB
    [1] = 0x08004000,   // - 0x08007FFF |  16 KiB
    [2] = 0x08008000,   // - 0x0800BFFF |  16 KiB
    [3] = 0x0800C000,   // - 0x0800FFFF |  16 KiB
    [4] = 0x08010000,   // - 0x0801FFFF |  64 KiB
    [5] = 0x08020000,   // - 0x0803FFFF | 128 KiB
    [6] = 0x08040000,   // - 0x0805FFFF | 128 KiB
    [7] = 0x08060000,   // - 0x0807FFFF | 128 KiB
    [8] = 0x08080000,   // - 0x0809FFFF | 128 KiB
    [9] = 0x080A0000,   // - 0x080BFFFF | 128 KiB
    [10] = 0x080C0000,  // - 0x080DFFFF | 128 KiB
    [11] = 0x080E0000,  // - 0x080FFFFF | 128 KiB
#if defined TREZOR_MODEL_T || defined TREZOR_MODEL_R
    [12] = 0x08100000,  // - 0x08103FFF |  16 KiB
    [13] = 0x08104000,  // - 0x08107FFF |  16 KiB
    [14] = 0x08108000,  // - 0x0810BFFF |  16 KiB
    [15] = 0x0810C000,  // - 0x0810FFFF |  16 KiB
    [16] = 0x08110000,  // - 0x0811FFFF |  64 KiB
    [17] = 0x08120000,  // - 0x0813FFFF | 128 KiB
    [18] = 0x08140000,  // - 0x0815FFFF | 128 KiB
    [19] = 0x08160000,  // - 0x0817FFFF | 128 KiB
    [20] = 0x08180000,  // - 0x0819FFFF | 128 KiB
    [21] = 0x081A0000,  // - 0x081BFFFF | 128 KiB
    [22] = 0x081C0000,  // - 0x081DFFFF | 128 KiB
    [23] = 0x081E0000,  // - 0x081FFFFF | 128 KiB
    [24] = 0x08200000,  // last element - not a valid sector
#elif defined TREZOR_MODEL_1
    [12] = 0x08100000,  // last element - not a valid sector
#else
#error Unknown Trezor model
#endif
};

const uint8_t FIRMWARE_SECTORS[FIRMWARE_SECTORS_COUNT] = {
    FLASH_SECTOR_FIRMWARE_START,
    7,
    8,
    9,
    10,
    FLASH_SECTOR_FIRMWARE_END,
    FLASH_SECTOR_FIRMWARE_EXTRA_START,
    18,
    19,
    20,
    21,
    22,
    FLASH_SECTOR_FIRMWARE_EXTRA_END,
};
#endif

const uint8_t STORAGE_SECTORS[STORAGE_SECTORS_COUNT] = {
    FLASH_SECTOR_STORAGE_1,
    FLASH_SECTOR_STORAGE_2,
};

secbool flash_unlock_write(void) {
  HAL_FLASH_Unlock();
#if defined(STM32H747xx)
  SCB_DisableDCache();
#else
  FLASH->SR |= FLASH_STATUS_ALL_FLAGS;  // clear all status flags
#endif
  return sectrue;
}

secbool flash_lock_write(void) {
  HAL_FLASH_Lock();
#if defined(STM32H747xx)
  SCB_EnableDCache();
#endif
  return sectrue;
}

#if USE_EXTERN_FLASH

const void *flash_get_address(uint8_t sector, uint32_t offset, uint32_t size) {
  if (sector >= FLASH_SECTOR_COUNT) {
    return NULL;
  }
  if (sector < FLASH_INNER_COUNT) {
    if (offset + size > FLASH_FIRMWARE_SECTOR_SIZE) {
      return NULL;
    }
    return (const void *)(0x08000000 + sector * FLASH_FIRMWARE_SECTOR_SIZE +
                          offset);
  } else if (sector >= FLASH_SECTOR_FIRMWARE_EXTRA_START &&
             sector <= FLASH_SECTOR_FIRMWARE_EXTRA_END) {
    if (offset + size > FLASH_FIRMWARE_SECTOR_SIZE) {
      return NULL;
    }
    return (const void *)(QSPI_FLASH_BASE_ADDRESS +
                          (sector - FLASH_SECTOR_FIRMWARE_EXTRA_START) *
                              FLASH_FIRMWARE_SECTOR_SIZE +
                          offset);
  } else if (sector >= FLASH_SECTOR_STORAGE_1 &&
             sector <= FLASH_SECTOR_STORAGE_2) {
    if (offset + size > FLASH_STORAGE_SECTOR_SIZE) {
      return NULL;
    }
    return (const void *)(QSPI_FLASH_BASE_ADDRESS + QSPI_FLASH_STORAG_OFFSET +
                          (sector - FLASH_SECTOR_STORAGE_1) *
                              FLASH_STORAGE_SECTOR_SIZE +
                          offset);
  }
  return NULL;
}

secbool flash_erase_sectors(const uint8_t *sectors, int len,
                            void (*progress)(int pos, int len)) {
  if (progress) {
    progress(0, len);
  }
  for (int i = 0; i < len; i++) {
    if (sectors[i] >= FLASH_SECTOR_FIRMWARE_EXTRA_START &&
        sectors[i] <= FLASH_SECTOR_FIRMWARE_EXTRA_END) {
      qspi_flash_erase_block_64k(
          (sectors[i] - FLASH_SECTOR_FIRMWARE_EXTRA_START) * 2 *
          QSPI_SECTOR_SIZE);
      qspi_flash_erase_block_64k(
          (sectors[i] - FLASH_SECTOR_FIRMWARE_EXTRA_START) * 2 *
              QSPI_SECTOR_SIZE +
          QSPI_SECTOR_SIZE);
    } else if (sectors[i] >= FLASH_SECTOR_STORAGE_1 &&
               sectors[i] <= FLASH_SECTOR_STORAGE_2) {
      qspi_flash_erase_block_64k((sectors[i] - FLASH_SECTOR_STORAGE_1) *
                                     QSPI_SECTOR_SIZE +
                                 QSPI_FLASH_STORAG_OFFSET);
    } else {
      ensure(flash_unlock_write(), NULL);
      FLASH_EraseInitTypeDef EraseInitStruct;
      EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
      EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
      EraseInitStruct.NbSectors = 1;

      if (sectors[i] < 8) {
        EraseInitStruct.Banks = FLASH_BANK_1;
        EraseInitStruct.Sector = sectors[i];
      } else {
        EraseInitStruct.Banks = FLASH_BANK_2;
        EraseInitStruct.Sector = sectors[i] - 8;
      }

      uint32_t SectorError;
      if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        ensure(flash_lock_write(), NULL);
        return secfalse;
      }
      ensure(flash_lock_write(), NULL);
    }

    // check whether the sector was really deleted (contains only 0xFF)
    const uint32_t addr_start = (uint32_t)flash_get_address(sectors[i], 0, 0);
    uint32_t addr_end;
    if (sectors[i] < FLASH_SECTOR_STORAGE_1) {
      addr_end = addr_start + FLASH_FIRMWARE_SECTOR_SIZE;
    } else {
      addr_end = addr_start + FLASH_STORAGE_SECTOR_SIZE;
    }
    for (uint32_t addr = addr_start; addr < addr_end; addr += 4) {
      if (*((const uint32_t *)addr) != 0xFFFFFFFF) {
        return secfalse;
      }
    }
    if (progress) {
      progress(i + 1, len);
    }
  }

  return sectrue;
}
secbool flash_write_byte(uint8_t sector, uint32_t offset, uint8_t data) {
  uint32_t address = (uint32_t)flash_get_address(sector, offset, 1);
  if (address == 0) {
    return secfalse;
  }
  if (address < QSPI_FLASH_BASE_ADDRESS) {
    return secfalse;
  }
  if (data != (data & *((const uint8_t *)address))) {
    return secfalse;
  }
  // display_printf("flash write byte\n");
  if (HAL_OK != qspi_flash_write_buffer_unsafe(
                    (uint8_t *)&data, address - QSPI_FLASH_BASE_ADDRESS, 1)) {
    return secfalse;
  }
  if (data != *((const uint8_t *)address)) {
    return secfalse;
  }
  return sectrue;
}

secbool flash_write_word(uint8_t sector, uint32_t offset, uint32_t data) {
  uint32_t address = (uint32_t)flash_get_address(sector, offset, 4);
  if (address == 0) {
    return secfalse;
  }
  if (address < QSPI_FLASH_BASE_ADDRESS) {
    return secfalse;
  }
  if (offset % sizeof(uint32_t)) {  // we write only at 4-byte boundary
    return secfalse;
  }
  if (data != (data & *((const uint32_t *)address))) {
    return secfalse;
  }
  if (HAL_OK != qspi_flash_write_buffer_unsafe(
                    (uint8_t *)&data, address - QSPI_FLASH_BASE_ADDRESS, 4)) {
    return secfalse;
  }
  if (data != *((const uint32_t *)address)) {
    return secfalse;
  }
  return sectrue;
}

secbool flash_write_words(uint8_t sector, uint32_t offset, uint32_t data[8]) {
  uint32_t flash_word[8];
  int retry = -1;

  uint32_t address = (uint32_t)flash_get_address(sector, offset, 4);
  if (address == 0) {
    return secfalse;
  }
  if (address % 32) {  // we write only at 4-byte boundary
    return secfalse;
  }
  if (offset % 32) {  // we write only at 4-byte boundary
    return secfalse;
  }
  for (int i = 0; i < 8; i++) {
    if (flash_word[i] !=
        (flash_word[i] & *((const uint32_t *)(address + 4 * i)))) {
      return secfalse;
    }
  }
rewrite:
  retry++;
  if (retry == 3) {
    return secfalse;
  }
  memcpy(flash_word, data, 32);

  if (sector >= FLASH_SECTOR_BOOTLOADER_1 &&
      sector <= FLASH_SECTOR_OTP_EMULATOR) {
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address,
                                    (uint32_t)&flash_word)) {
      goto rewrite;
    }
  } else {
    if (HAL_OK !=
        qspi_flash_write_buffer_unsafe((uint8_t *)&flash_word,
                                       address - QSPI_FLASH_BASE_ADDRESS, 32)) {
      goto rewrite;
    }
  }

  for (int i = 0; i < 8; i++) {
    if (flash_word[i] != *((const uint32_t *)(address + 4 * i))) {
      goto rewrite;
    }
  }
  return sectrue;
}

uint32_t flash_sector_size(uint8_t sector) {
  if (sector >= FLASH_SECTOR_COUNT) {
    return 0;
  }
  if (sector < FLASH_INNER_COUNT) {
    return FLASH_FIRMWARE_SECTOR_SIZE;
  } else if (sector >= FLASH_SECTOR_FIRMWARE_EXTRA_START &&
             sector <= FLASH_SECTOR_FIRMWARE_EXTRA_END) {
    return FLASH_FIRMWARE_SECTOR_SIZE;
  } else if (sector >= FLASH_SECTOR_STORAGE_1 &&
             sector <= FLASH_SECTOR_STORAGE_2) {
    return FLASH_STORAGE_SECTOR_SIZE;
  }
  return 0;
}

bool flash_check_ecc_fault() {
  if ((FLASH->SR1 & (FLASH_FLAG_SNECCERR_BANK1 | FLASH_FLAG_DBECCERR_BANK1)) !=
          0U ||
      (FLASH->SR2 & (FLASH_FLAG_SNECCERR_BANK2 | FLASH_FLAG_DBECCERR_BANK2) &
       0x7FFFFFFFU) != 0U) {
    return true;
  }
  return false;
}

bool flash_clear_ecc_fault(uint32_t address) {
  if (IS_FLASH_PROGRAM_ADDRESS_BANK1(address)) {
    FLASH->CCR1 |= (FLASH_FLAG_SNECCERR_BANK1 | FLASH_CCR_CLR_DBECCERR_Msk);
    __DSB();
    return true;
  }

  if (IS_FLASH_PROGRAM_ADDRESS_BANK2(address)) {
    FLASH->CCR2 |= (FLASH_FLAG_SNECCERR_BANK1 | FLASH_CCR_CLR_DBECCERR_Msk);
    __DSB();
    return true;
  }
  return false;
}

// sector erase method, large effect area
bool flash_fix_ecc_fault_FIRMWARE(uint32_t address) {
  const size_t header_backup_len = 4096;

  if (!((IS_FLASH_PROGRAM_ADDRESS_BANK1(address)) ||
        (IS_FLASH_PROGRAM_ADDRESS_BANK2(address)))) {
    return false;
  }

  // find which sector the address is on
  uint32_t offset = address - FLASH_BANK1_BASE;
  uint8_t sector = offset / FLASH_FIRMWARE_SECTOR_SIZE;

  // sanity check
  if (sector < FLASH_SECTOR_FIRMWARE_START ||
      sector > FLASH_SECTOR_FIRMWARE_END) {
    return false;
  }

  // wipe sector
  if (sector == FLASH_SECTOR_FIRMWARE_START) {
    // backup header
    uint8_t temp_header_backup[header_backup_len];
    memcpy(temp_header_backup, flash_get_address(sector, 0, header_backup_len),
           header_backup_len);
    // wipe sector
    flash_erase(sector);
    // restore header
    if (sectrue != flash_unlock_write()) {
      return false;
    }
    for (size_t sector_offset = 0; sector_offset < header_backup_len;
         sector_offset += 32) {
      if (sectrue !=
          flash_write_words(sector, sector_offset,
                            (uint32_t *)(temp_header_backup + sector_offset))) {
        return false;
      }
    }
    if (sectrue != flash_lock_write()) {
      return false;
    }
  } else {
    if (sectrue == flash_erase(sector)) {
    } else {
      return false;
    }
  }

  return true;
}

// write zero method, smaller effect area
bool flash_fix_ecc_fault_FIRMWARE_v2(uint32_t address) {
  // sanity check
  if (!((IS_FLASH_PROGRAM_ADDRESS_BANK1(address)) ||
        (IS_FLASH_PROGRAM_ADDRESS_BANK2(address)))) {
    return false;
  }

  // find which sector the address is on
  uint32_t offset = address - FLASH_BANK1_BASE;
  uint8_t sector = offset / FLASH_FIRMWARE_SECTOR_SIZE;

  // sanity check
  if (sector < FLASH_SECTOR_FIRMWARE_START ||
      sector > FLASH_SECTOR_FIRMWARE_END) {
    return false;
  }

  uint8_t zeros[32];  // has to be 32 bytes
  memset(zeros, 0x00, 32);
  uint32_t overwrite_offset =
      (address / sizeof(zeros)) * sizeof(zeros);  // force align

  // unlock for write
  if (sectrue != flash_unlock_write()) {
    return false;
  }

  // overwrite
  if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, overwrite_offset,
                                  (uint32_t)&zeros)) {
    return false;
  }

  // lock back
  if (sectrue != flash_lock_write()) {
    return false;
  }

  return true;
}
#else

const void *flash_get_address(uint8_t sector, uint32_t offset, uint32_t size) {
  if (sector >= FLASH_SECTOR_COUNT) {
    return NULL;
  }
  const uint32_t addr = FLASH_SECTOR_TABLE[sector] + offset;
  const uint32_t next = FLASH_SECTOR_TABLE[sector + 1];
  if (addr + size > next) {
    return NULL;
  }
  return (const void *)addr;
}

uint32_t flash_sector_size(uint8_t sector) {
  if (sector >= FLASH_SECTOR_COUNT) {
    return 0;
  }
  return FLASH_SECTOR_TABLE[sector + 1] - FLASH_SECTOR_TABLE[sector];
}

secbool flash_erase_sectors(const uint8_t *sectors, int len,
                            void (*progress)(int pos, int len)) {
  ensure(flash_unlock_write(), NULL);
  FLASH_EraseInitTypeDef EraseInitStruct;
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.NbSectors = 1;

#if defined(STM32H747xx)
  EraseInitStruct.Banks = FLASH_BANK_1;
#endif

  if (progress) {
    progress(0, len);
  }
  for (int i = 0; i < len; i++) {
    EraseInitStruct.Sector = sectors[i];
    uint32_t SectorError;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
      ensure(flash_lock_write(), NULL);
      return secfalse;
    }
    // check whether the sector was really deleted (contains only 0xFF)
    const uint32_t addr_start = FLASH_SECTOR_TABLE[sectors[i]],
                   addr_end = FLASH_SECTOR_TABLE[sectors[i] + 1];
    for (uint32_t addr = addr_start; addr < addr_end; addr += 4) {
      if (*((const uint32_t *)addr) != 0xFFFFFFFF) {
        ensure(flash_lock_write(), NULL);
        return secfalse;
      }
    }
    if (progress) {
      progress(i + 1, len);
    }
  }
  ensure(flash_lock_write(), NULL);
  return sectrue;
}
#if defined(STM32H747xx)
// align to 256bit
secbool flash_write_byte(uint8_t sector, uint32_t offset, uint8_t data) {
  uint32_t flash_word[8];
  uint32_t address = (uint32_t)flash_get_address(sector, offset, 1);
  if (address == 0) {
    return secfalse;
  }
  if (data != (data & *((const uint8_t *)address))) {
    return secfalse;
  }
  memcpy((uint8_t *)flash_word, (uint8_t *)(address & 0xffffffe0),
         sizeof(flash_word));
  *(uint8_t *)((uint32_t)&flash_word + address % 32) = data;
  if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address & 0xffffffe0,
                                  (uint32_t)&flash_word)) {
    return secfalse;
  }
  if (data != *((const uint8_t *)address)) {
    return secfalse;
  }

  return sectrue;
}

secbool flash_write_word(uint8_t sector, uint32_t offset, uint32_t data) {
  uint32_t flash_word[8];

  uint32_t address = (uint32_t)flash_get_address(sector, offset, 4);
  if (address == 0) {
    return secfalse;
  }
  if (offset % sizeof(uint32_t)) {  // we write only at 4-byte boundary
    return secfalse;
  }
  if (data != (data & *((const uint32_t *)address))) {
    return secfalse;
  }
  memcpy((uint8_t *)flash_word, (uint8_t *)(address & 0xffffffe0),
         sizeof(flash_word));
  flash_word[((address % 32) + 3) / 4] = data;

  if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address & 0xffffffe0,
                                  (uint32_t)&flash_word)) {
    return secfalse;
  }
  if (data != *((const uint32_t *)address)) {
    return secfalse;
  }
  return sectrue;
}

#else
secbool flash_write_byte(uint8_t sector, uint32_t offset, uint8_t data) {
  uint32_t address = (uint32_t)flash_get_address(sector, offset, 1);
  if (address == 0) {
    return secfalse;
  }
  if (data != (data & *((const uint8_t *)address))) {
    return secfalse;
  }
  if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address, data)) {
    return secfalse;
  }
  if (data != *((const uint8_t *)address)) {
    return secfalse;
  }
  return sectrue;
}

secbool flash_write_word(uint8_t sector, uint32_t offset, uint32_t data) {
  uint32_t address = (uint32_t)flash_get_address(sector, offset, 4);
  if (address == 0) {
    return secfalse;
  }
  if (offset % sizeof(uint32_t)) {  // we write only at 4-byte boundary
    return secfalse;
  }
  if (data != (data & *((const uint32_t *)address))) {
    return secfalse;
  }
  if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data)) {
    return secfalse;
  }
  if (data != *((const uint32_t *)address)) {
    return secfalse;
  }
  return sectrue;
}
#endif

#endif

#if defined(STM32H747xx)

void flash_otp_init(void) {
  if (memcmp(flash_otp_data->flag, "erased", strlen("erased")) == 0) {
    return;
  }
  uint8_t buf[32] = "erased";
  uint8_t secotrs[1];
  secotrs[0] = 15;

  ensure(flash_erase_sectors(secotrs, 1, NULL), "erase data sector 15");
  ensure(flash_unlock_write(), NULL);
  ensure(flash_write_words(15, 0, (uint32_t *)buf), "write init flag");
  ensure(flash_lock_write(), NULL);
  return;
}

secbool flash_otp_read(uint8_t block, uint8_t offset, uint8_t *data,
                       uint8_t datalen) {
  if (block >= FLASH_OTP_NUM_BLOCKS ||
      offset + datalen > FLASH_OTP_BLOCK_SIZE) {
    return secfalse;
  }
  memcpy(data, flash_otp_data->flash_otp[block] + offset, datalen);
  return sectrue;
}

secbool flash_otp_write(uint8_t block, uint8_t offset, const uint8_t *data,
                        uint8_t datalen) {
  if (block >= FLASH_OTP_NUM_BLOCKS || offset != 0 ||
      datalen != FLASH_OTP_BLOCK_SIZE) {
    return secfalse;
  }
  uint32_t address = (uint32_t)flash_otp_data->flash_otp[block];
  uint32_t flash_word[8];
  memcpy(flash_word, data, datalen);
  ensure(flash_unlock_write(), NULL);
  if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address,
                                  (uint32_t)&flash_word)) {
    ensure(flash_lock_write(), NULL);
    return secfalse;
  }
  ensure(flash_lock_write(), NULL);
  return sectrue;
}

secbool flash_otp_lock(uint8_t block) { return sectrue; }

secbool flash_otp_is_locked(uint8_t block) {
  if (0 ==
      memcmp(flash_otp_data->flash_otp[block],
             "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
             "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
             FLASH_OTP_BLOCK_SIZE)) {
    return secfalse;
  }
  return sectrue;
}
#else
#define FLASH_OTP_LOCK_BASE 0x1FFF7A00U

secbool flash_otp_read(uint8_t block, uint8_t offset, uint8_t *data,
                       uint8_t datalen) {
  if (block >= FLASH_OTP_NUM_BLOCKS ||
      offset + datalen > FLASH_OTP_BLOCK_SIZE) {
    return secfalse;
  }
  for (uint8_t i = 0; i < datalen; i++) {
    data[i] = *(__IO uint8_t *)(FLASH_OTP_BASE + block * FLASH_OTP_BLOCK_SIZE +
                                offset + i);
  }
  return sectrue;
}

secbool flash_otp_write(uint8_t block, uint8_t offset, const uint8_t *data,
                        uint8_t datalen) {
  if (block >= FLASH_OTP_NUM_BLOCKS ||
      offset + datalen > FLASH_OTP_BLOCK_SIZE) {
    return secfalse;
  }
  ensure(flash_unlock_write(), NULL);
  for (uint8_t i = 0; i < datalen; i++) {
    uint32_t address =
        FLASH_OTP_BASE + block * FLASH_OTP_BLOCK_SIZE + offset + i;
    ensure(sectrue * (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
                                                  address, data[i])),
           NULL);
  }
  ensure(flash_lock_write(), NULL);
  return sectrue;
}

secbool flash_otp_lock(uint8_t block) {
  if (block >= FLASH_OTP_NUM_BLOCKS) {
    return secfalse;
  }
  ensure(flash_unlock_write(), NULL);
  HAL_StatusTypeDef ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
                                            FLASH_OTP_LOCK_BASE + block, 0x00);
  ensure(flash_lock_write(), NULL);
  return sectrue * (ret == HAL_OK);
}

secbool flash_otp_is_locked(uint8_t block) {
  return sectrue * (0x00 == *(__IO uint8_t *)(FLASH_OTP_LOCK_BASE + block));
}
#endif

void flash_test(void) {
  // static uint8_t data[32];
  uint8_t sector = 15;
  ensure(flash_unlock_write(), NULL);
  ensure(flash_erase(sector), "erase failed");
  // ensure(flash_write_word(sector, 64, 0x12345678), NULL);
  // for (uint8_t i = 0; i < 32; i++) {
  //   data[i] = i;
  // }

  // ensure(flash_otp_write(0, 0, data, 32), NULL);
  // memset(data, 0x00, 32);
  // ensure(flash_otp_read(0, 0, data, 32), NULL);
  ensure(flash_lock_write(), NULL);
  while (1)
    ;
}
