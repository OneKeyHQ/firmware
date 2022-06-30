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
#if defined(STM32H747xx)
#include "stm32h7xx_ll_cortex.h"
#else
#include "stm32f4xx_ll_cortex.h"
#endif
// http://infocenter.arm.com/help/topic/com.arm.doc.dui0552a/BABDJJGF.html
#define MPU_RASR_ATTR_FLASH (MPU_RASR_C_Msk)
#define MPU_RASR_ATTR_SRAM (MPU_RASR_C_Msk | MPU_RASR_S_Msk)
#define MPU_RASR_ATTR_PERIPH (MPU_RASR_B_Msk | MPU_RASR_S_Msk)

#define MPU_SUBREGION_DISABLE(X) ((X) << MPU_RASR_SRD_Pos)

void mpu_config_off(void) {
  // Disable MPU
  HAL_MPU_Disable();
}

void mpu_config_bootloader(void) {
  // Disable MPU
  HAL_MPU_Disable();
  // Note: later entries overwrite previous ones
#if defined(STM32H747xx)

  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  // inner ram ,AXI SRAM,SRAM1-SRAM4
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // SRAM3 for DMA
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // // Peripherals device
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x40000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // SPI FLASH
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;  // MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER5;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#else

  // Everything (0x00000000 - 0xFFFFFFFF, 4 GiB, read-write)
  MPU->RNR = MPU_REGION_NUMBER0;
  MPU->RBAR = 0;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_4GB | LL_MPU_REGION_FULL_ACCESS;

  // Flash (0x0800C000 - 0x0800FFFF, 16 KiB, no access)
  MPU->RNR = MPU_REGION_NUMBER1;
  MPU->RBAR = FLASH_BASE + 0xC000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_16KB | LL_MPU_REGION_NO_ACCESS;

  // Flash (0x0810C000 - 0x0810FFFF, 16 KiB, no access)
  MPU->RNR = MPU_REGION_NUMBER2;
  MPU->RBAR = FLASH_BASE + 0x10C000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_16KB | LL_MPU_REGION_NO_ACCESS;

  // SRAM (0x20000000 - 0x2002FFFF, 192 KiB = 256 KiB except 2/8 at end,
  // read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER3;
  MPU->RBAR = SRAM_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_SRAM |
              LL_MPU_REGION_SIZE_256KB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk | MPU_SUBREGION_DISABLE(0xC0);

  // Peripherals (0x40000000 - 0x5FFFFFFF, read-write, execute never)
  // External RAM (0x60000000 - 0x7FFFFFFF, read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER4;
  MPU->RBAR = PERIPH_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_PERIPH |
              LL_MPU_REGION_SIZE_1GB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk;

#ifdef STM32F427xx
  // CCMRAM (0x10000000 - 0x1000FFFF, read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER5;
  MPU->RBAR = CCMDATARAM_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_SRAM |
              LL_MPU_REGION_SIZE_64KB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk;
#elif STM32F405xx
  // no CCMRAM
#else
#error Unsupported MCU
#endif
  // Enable MPU
  HAL_MPU_Enable(LL_MPU_CTRL_HARDFAULT_NMI);
#endif
}

void mpu_config_firmware(void) {
  // Disable MPU
  HAL_MPU_Disable();
  // Note: later entries overwrite previous ones
#if defined(STM32H747xx)

  MPU_Region_InitTypeDef MPU_InitStruct;
  // flash ,read only
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x08000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_2MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x081E0000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  //
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // AXI SRAM
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // SRAM1+SRAM2
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // SRAM3 for DMA
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER5;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // Peripherals device
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x40000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER6;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER7;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xD1000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_2MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER8;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // SPI FLASH
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;  // MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER9;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x80;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90700000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_1MB;  // MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER10;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#else

  /*
      // Boardloader (0x08000000 - 0x0800FFFF, 64 KiB, read-only, execute never)
      MPU->RBAR = FLASH_BASE | MPU_REGION_NUMBER0;
      MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
     LL_MPU_REGION_SIZE_64KB | LL_MPU_REGION_PRIV_RO_URO | MPU_RASR_XN_Msk;
  */

  // Bootloader (0x08020000 - 0x0803FFFF, 64 KiB, read-only)
  MPU->RNR = MPU_REGION_NUMBER0;
  MPU->RBAR = FLASH_BASE + 0x20000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_64KB | LL_MPU_REGION_PRIV_RO_URO;

  // Storage#1 (0x08010000 - 0x0801FFFF, 64 KiB, read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER1;
  MPU->RBAR = FLASH_BASE + 0x10000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_64KB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk;
  // Storage#2 (0x08110000 - 0x0811FFFF, 64 KiB, read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER2;
  MPU->RBAR = FLASH_BASE + 0x110000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_64KB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk;

  // Firmware (0x08040000 - 0x080FFFFF, 6 * 128 KiB = 1024 KiB except 2/8 at
  // start = 768 KiB, read-only)
  MPU->RNR = MPU_REGION_NUMBER3;
  MPU->RBAR = FLASH_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_1MB | LL_MPU_REGION_PRIV_RO_URO |
              MPU_SUBREGION_DISABLE(0x03);

  // Firmware extra (0x08120000 - 0x081FFFFF, 7 * 128 KiB = 1024 KiB except 1/8
  // at start = 896 KiB, read-only)
  MPU->RNR = MPU_REGION_NUMBER4;
  MPU->RBAR = FLASH_BASE + 0x100000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_FLASH |
              LL_MPU_REGION_SIZE_1MB | LL_MPU_REGION_PRIV_RO_URO |
              MPU_SUBREGION_DISABLE(0x01);

  // SRAM (0x20000000 - 0x2002FFFF, 192 KiB = 256 KiB except 2/8 at end,
  // read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER5;
  MPU->RBAR = SRAM_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_SRAM |
              LL_MPU_REGION_SIZE_256KB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk | MPU_SUBREGION_DISABLE(0xC0);

  // Peripherals (0x40000000 - 0x5FFFFFFF, read-write, execute never)
  // External RAM (0x60000000 - 0x7FFFFFFF, read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER6;
  MPU->RBAR = PERIPH_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_PERIPH |
              LL_MPU_REGION_SIZE_1GB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk;

#ifdef STM32F427xx
  // CCMRAM (0x10000000 - 0x1000FFFF, read-write, execute never)
  MPU->RNR = MPU_REGION_NUMBER7;
  MPU->RBAR = CCMDATARAM_BASE;
  MPU->RASR = MPU_RASR_ENABLE_Msk | MPU_RASR_ATTR_SRAM |
              LL_MPU_REGION_SIZE_64KB | LL_MPU_REGION_FULL_ACCESS |
              MPU_RASR_XN_Msk;
#elif STM32F405xx
  // no CCMRAM
#else
#error Unsupported MCU
#endif
  // Enable MPU
  HAL_MPU_Enable(LL_MPU_CTRL_HARDFAULT_NMI);
#endif

  __asm__ volatile("dsb");
  __asm__ volatile("isb");
}
