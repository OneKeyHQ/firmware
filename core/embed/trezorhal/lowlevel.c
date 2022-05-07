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

#include "lowlevel.h"
#include "common.h"
#include "flash.h"

#pragma GCC optimize( \
    "no-stack-protector")  // applies to all functions in this file

#if defined(STM32F427xx) || defined(STM32F405xx)

#if PRODUCTION
#define WANT_RDP_LEVEL (OB_RDP_LEVEL_2)
#define WANT_WRP_SECTORS (OB_WRP_SECTOR_0 | OB_WRP_SECTOR_1 | OB_WRP_SECTOR_2)
#else
#define WANT_RDP_LEVEL (OB_RDP_LEVEL_0)
#define WANT_WRP_SECTORS (0)
#endif

// BOR LEVEL 3: Reset level threshold is around 2.5 V
#define WANT_BOR_LEVEL (OB_BOR_LEVEL3)

// reference RM0090 section 3.9.10; SPRMOD is 0 meaning PCROP disabled.; DB1M is
// 0 because we use 2MB dual-bank; BFB2 is 0 allowing boot from flash;
#define FLASH_OPTCR_VALUE                                                   \
  ((((~WANT_WRP_SECTORS) << FLASH_OPTCR_nWRP_Pos) & FLASH_OPTCR_nWRP_Msk) | \
   (WANT_RDP_LEVEL << FLASH_OPTCR_RDP_Pos) | FLASH_OPTCR_nRST_STDBY |       \
   FLASH_OPTCR_nRST_STOP | FLASH_OPTCR_WDG_SW | WANT_BOR_LEVEL)

// reference RM0090 section 3.7.1 table 16
#define OPTION_BYTES_RDP_USER_VALUE                            \
  ((uint16_t)((WANT_RDP_LEVEL << FLASH_OPTCR_RDP_Pos) |        \
              FLASH_OPTCR_nRST_STDBY | FLASH_OPTCR_nRST_STOP | \
              FLASH_OPTCR_WDG_SW | WANT_BOR_LEVEL))
#define OPTION_BYTES_BANK1_WRP_VALUE ((uint16_t)((~WANT_WRP_SECTORS) & 0xFFFU))
#define OPTION_BYTES_BANK2_WRP_VALUE ((uint16_t)0xFFFU)

// reference RM0090 section 3.7.1 table 16. use 16 bit pointers because the top
// 48 bits are all reserved.
#define OPTION_BYTES_RDP_USER (*(volatile uint16_t* const)0x1FFFC000U)
#define OPTION_BYTES_BANK1_WRP (*(volatile uint16_t* const)0x1FFFC008U)
#define OPTION_BYTES_BANK2_WRP (*(volatile uint16_t* const)0x1FFEC008U)

uint32_t flash_wait_and_clear_status_flags(void) {
  while (FLASH->SR & FLASH_SR_BSY)
    ;  // wait for all previous flash operations to complete
  const uint32_t result =
      FLASH->SR & FLASH_STATUS_ALL_FLAGS;  // get the current status flags
  FLASH->SR |= FLASH_STATUS_ALL_FLAGS;     // clear all status flags
  return result;
}

secbool flash_check_option_bytes(void) {
  flash_wait_and_clear_status_flags();
  // check values stored in flash interface registers
  if ((FLASH->OPTCR & ~3) !=
      FLASH_OPTCR_VALUE) {  // ignore bits 0 and 1 because they are control bits
    return secfalse;
  }
  if (FLASH->OPTCR1 != FLASH_OPTCR1_nWRP) {
    return secfalse;
  }
  // check values stored in flash memory
  if ((OPTION_BYTES_RDP_USER & ~3) !=
      OPTION_BYTES_RDP_USER_VALUE) {  // bits 0 and 1 are unused
    return secfalse;
  }
  if ((OPTION_BYTES_BANK1_WRP & 0xCFFFU) !=
      OPTION_BYTES_BANK1_WRP_VALUE) {  // bits 12 and 13 are unused
    return secfalse;
  }
  if ((OPTION_BYTES_BANK2_WRP & 0xFFFU) !=
      OPTION_BYTES_BANK2_WRP_VALUE) {  // bits 12, 13, 14, and 15 are unused
    return secfalse;
  }
  return sectrue;
}

void flash_lock_option_bytes(void) {
  FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;  // lock the option bytes
}

void flash_unlock_option_bytes(void) {
  if ((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) == 0) {
    return;  // already unlocked
  }
  // reference RM0090 section 3.7.2
  // write the special sequence to unlock
  FLASH->OPTKEYR = FLASH_OPT_KEY1;
  FLASH->OPTKEYR = FLASH_OPT_KEY2;
  while (FLASH->OPTCR & FLASH_OPTCR_OPTLOCK)
    ;  // wait until the flash option control register is unlocked
}

uint32_t flash_set_option_bytes(void) {
  // reference RM0090 section 3.7.2
  flash_wait_and_clear_status_flags();
  flash_unlock_option_bytes();
  flash_wait_and_clear_status_flags();
  FLASH->OPTCR1 =
      FLASH_OPTCR1_nWRP;  // no write protection on any sectors in bank 2
  FLASH->OPTCR =
      FLASH_OPTCR_VALUE;  // WARNING: dev board safe unless you compile for
                          // PRODUCTION or change this value!!!
  FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;  // begin committing changes to flash
  const uint32_t result =
      flash_wait_and_clear_status_flags();  // wait until changes are committed
  flash_lock_option_bytes();
  return result;
}

secbool flash_configure_option_bytes(void) {
  if (sectrue == flash_check_option_bytes()) {
    return sectrue;  // we DID NOT have to change the option bytes
  }

  do {
    flash_set_option_bytes();
  } while (sectrue != flash_check_option_bytes());

  return secfalse;  // notify that we DID have to change the option bytes
}

void periph_init(void) {
  // STM32F4xx HAL library initialization:
  //  - configure the Flash prefetch, instruction and data caches
  //  - configure the Systick to generate an interrupt each 1 msec
  //  - set NVIC Group Priority to 4
  //  - global MSP (MCU Support Package) initialization
  HAL_Init();

  // Enable GPIO clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  // enable the PVD (programmable voltage detector).
  // select the "2.7V" threshold (level 5).
  // this detector will be active regardless of the
  // flash option byte BOR setting.
  __HAL_RCC_PWR_CLK_ENABLE();
  PWR_PVDTypeDef pvd_config;
  pvd_config.PVDLevel = PWR_PVDLEVEL_5;
  pvd_config.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
  HAL_PWR_ConfigPVD(&pvd_config);
  HAL_PWR_EnablePVD();
  NVIC_EnableIRQ(PVD_IRQn);
}

secbool reset_flags_check(void) {
#if PRODUCTION
  // this is effective enough that it makes development painful, so only use it
  // for production. check the reset flags to assure that we arrive here due to
  // a regular full power-on event, and not as a result of a lesser reset.
  if ((RCC->CSR & (RCC_CSR_LPWRRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_IWDGRSTF |
                   RCC_CSR_SFTRSTF | RCC_CSR_PORRSTF | RCC_CSR_PINRSTF |
                   RCC_CSR_BORRSTF)) !=
      (RCC_CSR_PORRSTF | RCC_CSR_PINRSTF | RCC_CSR_BORRSTF)) {
    return secfalse;
  }
#endif
  return sectrue;
}

void reset_flags_reset(void) {
  RCC->CSR |= RCC_CSR_RMVF;  // clear the reset flags
}
#elif defined(STM32H747xx)

#if PRODUCTION
#define WANT_RDP_LEVEL (OB_RDP_LEVEL_2)
#define WANT_WRP_SECTORS (OB_WRP_SECTOR_0)
#else
#define WANT_RDP_LEVEL (OB_RDP_LEVEL_0)
#define WANT_WRP_SECTORS (0)
#endif

// BOR LEVEL 3: Reset level threshold is around 2.5 V
#define WANT_BOR_LEVEL (OB_BOR_LEVEL3)

/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
void cpu_cache_enable(void) {
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
 * @brief  Configure the MPU attributes as Write Through for External SDRAM.
 * @note   The Base Address is 0xD0000000 .
 *         The Configured Region Size is 32MB because same as SDRAM size.
 * @param  None
 * @retval None
 */
void mpu_config(void) {
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = SDRAM_DEVICE_ADDR;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // NAND FLASH
  // MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  // MPU_InitStruct.BaseAddress      = 0x80000000;
  // MPU_InitStruct.Size             = MPU_REGION_SIZE_512MB;	//
  // MPU_REGION_SIZE_512MB; MPU_InitStruct.AccessPermission =
  // MPU_REGION_FULL_ACCESS; MPU_InitStruct.IsBufferable     =
  // MPU_ACCESS_BUFFERABLE; MPU_InitStruct.IsCacheable      =
  // MPU_ACCESS_NOT_CACHEABLE; MPU_InitStruct.IsShareable      =
  // MPU_ACCESS_NOT_SHAREABLE; MPU_InitStruct.Number           =
  // MPU_REGION_NUMBER1; MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  // MPU_InitStruct.SubRegionDisable = 0x00;
  // MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  // HAL_MPU_ConfigRegion(&MPU_InitStruct);

  // SPI FLASH
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;  // MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 400000000 (CM7 CPU Clock)
 *            HCLK(Hz)                       = 200000000 (CM4 CPU, AXI and AHBs
 * Clock) AHB Prescaler                  = 2 D1 APB3 Prescaler              = 2
 * (APB3 Clock  100MHz) D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
 *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
 *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 5
 *            PLL_N                          = 160
 *            PLL_P                          = 2
 *            PLL_Q                          = 4
 *            PLL_R                          = 2
 *            VDD(V)                         = 3.3
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
void system_clock_config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /*!< Supply configuration update enable */
  // HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device
     is clocked below the maximum system frequency, to update the voltage
     scaling value regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
  }

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (ret != HAL_OK) {
    ensure(secfalse, "HAL_RCC_OscConfig failed");
  }

  /* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType =
      (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 |
       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if (ret != HAL_OK) {
    ensure(secfalse, "HAL_RCC_ClockConfig failed");
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    ensure(secfalse, "HAL_RCCEx_PeriphCLKConfig failed");
  }

  /*
   Note : The activation of the I/O Compensation Cell is recommended with
   communication  interfaces (GPIO, SPI, FMC, QSPI ...)  when  operating at high
   frequencies(please refer to product datasheet) The I/O Compensation Cell
   activation  procedure requires :
         - The activation of the CSI clock
         - The activation of the SYSCFG clock
         - Enabling the I/O Compensation Cell : setting bit[0] of register
   SYSCFG_CCCSR
  */

  /*activate CSI clock mondatory for I/O Compensation Cell*/
  __HAL_RCC_CSI_ENABLE();

  /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* Enables the I/O Compensation Cell */
  HAL_EnableCompensationCell();

  SCB->CPACR |=
      ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */

  SystemCoreClockUpdate();
}

void periph_init(void) {
  // STM32F4xx HAL library initialization:
  //  - configure the Flash prefetch, instruction and data caches
  //  - configure the Systick to generate an interrupt each 1 msec
  //  - set NVIC Group Priority to 4
  //  - global MSP (MCU Support Package) initialization
  HAL_Init();

  // enable the PVD (programmable voltage detector).
  // select the "2.7V" threshold (level 5).
  // this detector will be active regardless of the
  // flash option byte BOR setting.

  PWR_PVDTypeDef pvd_config;
  pvd_config.PVDLevel = PWR_PVDLEVEL_5;
  pvd_config.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
  HAL_PWR_ConfigPVD(&pvd_config);
  HAL_PWR_EnablePVD();
  NVIC_EnableIRQ(PVD_AVD_IRQn);
}

void reset_flags_reset(void) {
  RCC->RSR |= RCC_RSR_RMVF;  // clear the reset flags
}

void flash_option_bytes_init(void) {
  FLASH_OBProgramInitTypeDef ob_config;

  HAL_FLASHEx_OBGetConfig(&ob_config);

  if (ob_config.RDPLevel != OB_RDP_LEVEL_2) {
    if ((ob_config.USERConfig & OB_BCM4_ENABLE) != OB_BCM4_DISABLE) {
      ob_config.OptionType |= OPTIONBYTE_USER;
      ob_config.USERType |= OB_USER_BCM4;
      ob_config.USERConfig &= ~OB_BCM4_ENABLE;

      HAL_FLASH_Unlock();
      HAL_FLASH_OB_Unlock();

      if (HAL_FLASHEx_OBProgram(&ob_config) != HAL_OK) {
        ensure(secfalse, "HAL_FLASHEx_OBProgram failed");
      }

      if (HAL_FLASH_OB_Launch() != HAL_OK) {
        ensure(secfalse, "HAL_FLASH_OB_Launch failed");
      }

      HAL_FLASH_OB_Lock();
      HAL_FLASH_Lock();
    }
  }

  if (ob_config.RDPLevel != WANT_RDP_LEVEL) {
    ob_config.OptionType |=
        OPTIONBYTE_WRP | OPTIONBYTE_RDP | OPTIONBYTE_USER | OPTIONBYTE_BOR;
    ob_config.RDPLevel = WANT_RDP_LEVEL;  //;
    ob_config.BORLevel = WANT_BOR_LEVEL;
    ob_config.WRPSector = WANT_WRP_SECTORS;
    ob_config.USERType =
        OB_USER_IWDG1_SW | OB_USER_IWDG2_SW | OB_USER_NRST_STOP_D1 |
        OB_USER_NRST_STOP_D2 | OB_USER_NRST_STDBY_D1 | OB_USER_NRST_STDBY_D2 |
        OB_USER_IWDG_STOP | OB_USER_IWDG_STDBY | OB_USER_IOHSLV |
        OB_USER_SWAP_BANK | OB_USER_SECURITY | OB_USER_BCM4;
    ob_config.USERConfig =
        OB_IWDG1_SW | OB_IWDG2_SW | OB_STOP_NO_RST_D1 | OB_STOP_NO_RST_D2 |
        OB_STDBY_NO_RST_D1 | OB_STDBY_NO_RST_D2 | OB_IWDG_STOP_FREEZE |
        OB_IWDG_STDBY_FREEZE | OB_IOHSLV_ENABLE | OB_SWAP_BANK_DISABLE |
        OB_SECURITY_DISABLE | OB_BCM4_DISABLE;

    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();

    if (HAL_FLASHEx_OBProgram(&ob_config) != HAL_OK) {
      ensure(secfalse, "HAL_FLASHEx_OBProgram failed");
    }

    if (HAL_FLASH_OB_Launch() != HAL_OK) {
      ensure(secfalse, "HAL_FLASH_OB_Launch failed");
    }

    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();

    HAL_FLASHEx_OBGetConfig(&ob_config);
  }
}

void gpio_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);
}

void led_on(void) { HAL_GPIO_WritePin(GPIOI, GPIO_PIN_14, GPIO_PIN_RESET); }

void led_off(void) { HAL_GPIO_WritePin(GPIOI, GPIO_PIN_14, GPIO_PIN_SET); }

#endif
