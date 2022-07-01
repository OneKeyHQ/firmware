#include STM32_HAL_H

#include <stdio.h>

#include <string.h>
#include "common.h"
#include "emmc.h"

static MMC_HandleTypeDef hmmc1;

void emmc_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  __HAL_RCC_SDMMC1_CLK_ENABLE();

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  /**SDMMC1 GPIO Configuration
  PC10     ------> SDMMC1_D2
  PC11     ------> SDMMC1_D3
  PC12     ------> SDMMC1_CK
  PB9     ------> SDMMC1_D5
  PB8     ------> SDMMC1_D4
  PD2     ------> SDMMC1_CMD
  PC8     ------> SDMMC1_D0
  PC9     ------> SDMMC1_D1
  PC7     ------> SDMMC1_D7
  PC6     ------> SDMMC1_D6
  */
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_8 |
                        GPIO_PIN_9 | GPIO_PIN_7 | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  hmmc1.Instance = SDMMC1;
  hmmc1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hmmc1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hmmc1.Init.BusWide = SDMMC_BUS_WIDE_8B;
  hmmc1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
  hmmc1.Init.ClockDiv = 2;
  if (HAL_MMC_Init(&hmmc1) != HAL_OK) {
    ensure(0, "mmc init fail");
  }

  if (HAL_MMC_ConfigWideBusOperation(&hmmc1, SDMMC_BUS_WIDE_8B) != HAL_OK) {
    ensure(0, "mmc wide set fail");
  }

  if (HAL_MMC_ConfigSpeedBusOperation(&hmmc1, SDMMC_SPEED_MODE_DDR) != HAL_OK) {
    ensure(0, "mmc speed set fail");
  }
}

uint8_t emmc_get_card_state(void) {
  return ((HAL_MMC_GetCardState(&hmmc1) == HAL_MMC_CARD_TRANSFER)
              ? MMC_TRANSFER_OK
              : MMC_TRANSFER_BUSY);
}

void emmc_get_card_info(EMMC_CardInfoTypeDef* card_info) {
  if (HAL_MMC_GetCardInfo(&hmmc1, card_info) == HAL_OK) {
  }
}

uint8_t emmc_read_blocks(uint8_t* data, uint32_t address, uint32_t nums,
                         uint32_t timeout) {
  if (HAL_MMC_ReadBlocks(&hmmc1, data, address, nums, timeout) == HAL_OK) {
    return MMC_OK;
  } else {
    return MMC_ERROR;
  }
}

uint8_t emmc_write_blocks(uint8_t* data, uint32_t address, uint32_t nums,
                          uint32_t timeout) {
  if (HAL_MMC_WriteBlocks(&hmmc1, data, address, nums, timeout) == HAL_OK) {
    return MMC_OK;
  } else {
    return MMC_ERROR;
  }
}

uint8_t emmc_read_blocks_dma(uint8_t* data, uint32_t address, uint32_t nums,
                             uint32_t timeout) {
  uint32_t tickstart = HAL_GetTick();
  if (HAL_MMC_ReadBlocks_DMA(&hmmc1, data, address, nums) == HAL_OK) {
    while (emmc_get_card_state() != MMC_TRANSFER_OK) {
      /* Check for the Timeout */
      if (timeout != HAL_MAX_DELAY) {
        if (((HAL_GetTick() - tickstart) > timeout) || (timeout == 0U)) {
          return MMC_ERROR;
        }
      }
    }
    return MMC_OK;
  } else {
    return MMC_ERROR;
  }
}

uint8_t emmc_write_blocks_dma(uint8_t* data, uint32_t address, uint32_t nums,
                              uint32_t timeout) {
  if (HAL_MMC_WriteBlocks_DMA(&hmmc1, data, address, nums) == HAL_OK) {
    return MMC_OK;
  } else {
    return MMC_ERROR;
  }
}

uint8_t emmc_erase(uint32_t start_address, uint32_t end_address) {
  return HAL_MMC_Erase(&hmmc1, start_address, end_address);
}

uint64_t emmc_get_capacity_in_bytes(void) {
  EMMC_CardInfoTypeDef card_info = {0};
  emmc_get_card_info(&card_info);
  return (uint64_t)card_info.LogBlockNbr * card_info.LogBlockSize;
}

void emmc_test(void) {
  uint8_t buf[512];
  for (int i = 0; i < 512; i++) {
    buf[i] = i;
  }
  emmc_write_blocks(buf, 0, 1, 500);
  memset(buf, 0x00, 512);
  emmc_read_blocks(buf, 0, 1, 500);
  for (int i = 0; i < 512; i++) {
    // display_printf(" %X\n", buf[i]);
  }
}
