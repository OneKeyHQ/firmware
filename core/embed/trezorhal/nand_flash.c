#include STM32_HAL_H
#include "nand_flash.h"

#include <string.h>

NAND_HandleTypeDef hnand1;
nand_device nand_dev;

int nand_flash_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PG9   ------> FMC_NCE
  PD5   ------> FMC_NWE
  PD4   ------> FMC_NOE
  PD6   ------> FMC_NWAIT
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PE10   ------> FMC_D7
  PE9   ------> FMC_D6
  PD15   ------> FMC_D1
  PD14   ------> FMC_D0
  PD11   ------> FMC_CLE
  PD12   ------> FMC_ALE
  PE8   ------> FMC_D5
  PE7   ------> FMC_D4
  */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_0 | GPIO_PIN_1 |
                        GPIO_PIN_15 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  FMC_NAND_PCC_TimingTypeDef ComSpaceTiming = {0};
  FMC_NAND_PCC_TimingTypeDef AttSpaceTiming = {0};

  hnand1.Instance = FMC_NAND_DEVICE;
  /* hnand1.Init */
  hnand1.Init.NandBank = FMC_NAND_BANK3;
  hnand1.Init.Waitfeature = FMC_NAND_WAIT_FEATURE_ENABLE;
  hnand1.Init.MemoryDataWidth = FMC_NAND_MEM_BUS_WIDTH_8;
  hnand1.Init.EccComputation = FMC_NAND_ECC_DISABLE;
  hnand1.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_256BYTE;
  hnand1.Init.TCLRSetupTime = 2;
  hnand1.Init.TARSetupTime = 2;

  /* ComSpaceTiming */
  ComSpaceTiming.SetupTime = 2;
  ComSpaceTiming.WaitSetupTime = 5;
  ComSpaceTiming.HoldSetupTime = 3;
  ComSpaceTiming.HiZSetupTime = 5;
  /* AttSpaceTiming */
  AttSpaceTiming.SetupTime = 2;
  AttSpaceTiming.WaitSetupTime = 5;
  AttSpaceTiming.HoldSetupTime = 3;
  AttSpaceTiming.HiZSetupTime = 5;

  if (HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK) {
    return HAL_ERROR;
  }

  HAL_NAND_Reset(&hnand1);
  HAL_Delay(100);
  NAND_IDTypeDef nand_id;
  if (HAL_NAND_Read_ID(&hnand1, &nand_id) == HAL_OK) {
    if ((nand_id.Maker_Id == 0x2C) && (nand_id.Device_Id == 0xDC)) {
      /* hnand1.Config */
      hnand1.Config.PageSize = 2048;
      hnand1.Config.SpareAreaSize = 64;
      hnand1.Config.BlockSize = 64;
      hnand1.Config.BlockNbr = 4096;
      hnand1.Config.PlaneNbr = 2;
      hnand1.Config.PlaneSize = 2048;
      hnand1.Config.ExtraCommandEnable = DISABLE;

      nand_dev.page_size = 2048;
      nand_dev.spare_size = 64;
      nand_dev.page_total_size = 2048 + 64;
      nand_dev.block_size = 64;
      nand_dev.block_num = 2048;
      nand_dev.block_total_num = 4096;
    }
  } else {
    return HAL_ERROR;
  }

  return HAL_OK;
}

// clang-format off

HAL_StatusTypeDef HAL_NAND_Write_Page_ex(NAND_HandleTypeDef *hnand,  uint32_t page_address, uint16_t page_col, uint8_t *pBuffer, uint16_t NumBytesToWrite)
{
  uint32_t index;
  uint32_t tickstart;
  uint32_t deviceAddress, nandAddress;
  uint8_t * buff = pBuffer;

  /* Check the NAND controller state */
  if (hnand->State == HAL_NAND_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (hnand->State == HAL_NAND_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hnand);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Identify the device address */
    deviceAddress = NAND_DEVICE;

    /* NAND raw address calculation */
    nandAddress = page_address;

    if((nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {
      /* Send write page command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
      __DSB();

      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(page_col);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(page_col);;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }

      /* Write data to memory */
      for (index = 0U; index < NumBytesToWrite; index++)
      {
        *(__IO uint8_t *)deviceAddress = *buff;
        buff++;
        __DSB();
      }

      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
      __DSB();

      /* Get tick */
      tickstart = HAL_GetTick();

      /* Read status until NAND is ready */
      while (HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
        {
          /* Update the NAND controller state */
          hnand->State = HAL_NAND_STATE_ERROR;

          /* Process unlocked */
          __HAL_UNLOCK(hnand);

          return HAL_TIMEOUT;
        }
      }

    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_NAND_Write_SpareArea_ex(NAND_HandleTypeDef *hnand, uint32_t page_address, uint8_t spare_offset, uint8_t *pBuffer, uint32_t NumSpareBytesTowrite)
{
  uint32_t index;
  uint32_t tickstart;
  uint32_t deviceAddress, nandAddress, columnAddress;
  uint8_t * buff = pBuffer;

  /* Check the NAND controller state */
  if (hnand->State == HAL_NAND_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (hnand->State == HAL_NAND_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hnand);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Identify the device address */
    deviceAddress = NAND_DEVICE;

    /* Page address calculation */
    nandAddress = page_address;

    /* Column in page address */
    columnAddress = COLUMN_ADDRESS(hnand) + spare_offset;

    if((nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {
      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        /* Send write Spare area command sequence */
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_C;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
        __DSB();

        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        /* Send write Spare area command sequence */
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
        __DSB();

        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }

      /* Write data to memory */
      for (index = 0U; index < NumSpareBytesTowrite; index++)
      {
        *(__IO uint8_t *)deviceAddress = *buff;
        buff++;
        __DSB();
      }

      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
      __DSB();

      /* Get tick */
      tickstart = HAL_GetTick();

      /* Read status until NAND is ready */
      while (HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
        {
          /* Update the NAND controller state */
          hnand->State = HAL_NAND_STATE_ERROR;

          /* Process unlocked */
          __HAL_UNLOCK(hnand);

          return HAL_TIMEOUT;
        }
      }

    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_NAND_Read_Page_ex(NAND_HandleTypeDef *hnand, uint32_t page_address, uint16_t page_col, uint8_t *pBuffer, uint16_t NumBytesToRead)
{
  uint32_t index;
  uint32_t tickstart;
  uint32_t deviceAddress, nandAddress;
  uint8_t * buff = pBuffer;

  /* Check the NAND controller state */
  if (hnand->State == HAL_NAND_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (hnand->State == HAL_NAND_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hnand);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Identify the device address */
    deviceAddress = NAND_DEVICE;

    nandAddress = page_address;

    if((page_address < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {
      /* Send read page command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();

      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(page_col);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(page_col);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }

      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;
      __DSB();


      if (hnand->Config.ExtraCommandEnable == ENABLE)
      {
        /* Get tick */
        tickstart = HAL_GetTick();

        /* Read status until NAND is ready */
        while (HAL_NAND_Read_Status(hnand) != NAND_READY)
        {
          if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
          {
            /* Update the NAND controller state */
            hnand->State = HAL_NAND_STATE_ERROR;

            /* Process unlocked */
            __HAL_UNLOCK(hnand);

            return HAL_TIMEOUT;
          }
        }

        /* Go back to read mode */
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = ((uint8_t)0x00U);
        __DSB();
      }

      /* Get Data into Buffer */
      for (index = 0U; index < NumBytesToRead; index++)
      {
        *buff = *(uint8_t *)deviceAddress;
        buff++;
      }

    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_NAND_Read_SpareArea_ex(NAND_HandleTypeDef *hnand, uint32_t page_address, uint8_t spare_offset, uint8_t *pBuffer, uint32_t NumSpareBytesToRead)
{
  uint32_t index;
  uint32_t tickstart;
  uint32_t deviceAddress, nandAddress, columnAddress;
  uint8_t * buff = pBuffer;

  /* Check the NAND controller state */
  if (hnand->State == HAL_NAND_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (hnand->State == HAL_NAND_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hnand);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Identify the device address */
    deviceAddress = NAND_DEVICE;

    /* NAND raw address calculation */
    nandAddress = page_address;

    /* Column in page address */
    columnAddress = COLUMN_ADDRESS(hnand) + spare_offset;

    if((nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {
      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        /* Send read spare area command sequence */
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_C;
        __DSB();

        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        /* Send read spare area command sequence */
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
        __DSB();

        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }

      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_TRUE1;
      __DSB();

      if (hnand->Config.ExtraCommandEnable == ENABLE)
      {
        /* Get tick */
        tickstart = HAL_GetTick();

        /* Read status until NAND is ready */
        while (HAL_NAND_Read_Status(hnand) != NAND_READY)
        {
          if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
          {
            /* Update the NAND controller state */
            hnand->State = HAL_NAND_STATE_ERROR;

            /* Process unlocked */
            __HAL_UNLOCK(hnand);

            return HAL_TIMEOUT;
          }
        }

        /* Go back to read mode */
        *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = ((uint8_t)0x00U);
        __DSB();
      }

      /* Get Data into Buffer */
      for (index = 0U; index < NumSpareBytesToRead; index++)
      {
        *buff = *(uint8_t *)deviceAddress;
        buff++;
      }

    }

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_NAND_Internal_Move(NAND_HandleTypeDef *hnand, uint32_t source_page, uint32_t target_page)
{
  uint32_t tickstart;
  uint32_t deviceAddress, nandAddress;

  uint32_t source_block = source_page/nand_dev.block_size;
  uint32_t target_block = target_page/nand_dev.block_size;
  if(source_block % 2 != target_block % 2){
    return HAL_ERROR;
  }

  /* Check the NAND controller state */
  if (hnand->State == HAL_NAND_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (hnand->State == HAL_NAND_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hnand);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Identify the device address */
    deviceAddress = NAND_DEVICE;

    nandAddress = source_page;

    if((source_page < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {
      /* Send read page command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();

      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      //internal data move
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA))  = 0x35;
      __DSB();


      if (hnand->Config.ExtraCommandEnable == ENABLE)
      {
        /* Get tick */
        tickstart = HAL_GetTick();

        /* Read status until NAND is ready */
        while (HAL_NAND_Read_Status(hnand) != NAND_READY)
        {
          if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
          {
            /* Update the NAND controller state */
            hnand->State = HAL_NAND_STATE_ERROR;

            /* Process unlocked */
            __HAL_UNLOCK(hnand);

            return HAL_TIMEOUT;
          }
        }        
      }

      nandAddress = target_page;

      /* Send read page command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = 0x85;
      __DSB();

      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      //internal data move
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA))  = 0x10;
      __DSB();

      /* Get tick */
      tickstart = HAL_GetTick();

      /* Read status until NAND is ready */
      while (HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
        {
          /* Update the NAND controller state */
          hnand->State = HAL_NAND_STATE_ERROR;

          /* Process unlocked */
          __HAL_UNLOCK(hnand);

          return HAL_TIMEOUT;
        }
      }

    }
    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_NAND_Internal_Write(NAND_HandleTypeDef *hnand, uint32_t source_page, uint32_t dest_page,uint16_t page_col,  uint8_t *buffer, uint32_t length)
{
  uint32_t tickstart;
  uint32_t deviceAddress, nandAddress;

  // uint8_t ecc_count = 0;
  // uint8_t ecc_start = 0;

  uint32_t source_block = source_page/nand_dev.block_size;
  uint32_t dest_block = dest_page/nand_dev.block_size;
  if(source_block % 2 != dest_block % 2){
    return HAL_ERROR;
  }

  // ecc_count = length / NAND_ECC_SIZE;
  // ecc_start = page_col / NAND_ECC_SIZE;

  /* Check the NAND controller state */
  if (hnand->State == HAL_NAND_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (hnand->State == HAL_NAND_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hnand);

    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Identify the device address */
    deviceAddress = NAND_DEVICE;

    nandAddress = source_page;

    if((source_page < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {
      /* Send read page command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();

      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      //internal data move
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA))  = 0x35;
      __DSB();


      if (hnand->Config.ExtraCommandEnable == ENABLE)
      {
        /* Get tick */
        tickstart = HAL_GetTick();

        /* Read status until NAND is ready */
        while (HAL_NAND_Read_Status(hnand) != NAND_READY)
        {
          if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
          {
            /* Update the NAND controller state */
            hnand->State = HAL_NAND_STATE_ERROR;

            /* Process unlocked */
            __HAL_UNLOCK(hnand);

            return HAL_TIMEOUT;
          }
        }        
      }

      nandAddress = dest_page;

      /* Send read page command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = 0x85;
      __DSB();

      /* Cards with page size <= 512 bytes */
      if ((hnand->Config.PageSize) <= 512U)
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }
      else /* (hnand->Config.PageSize) > 512 */
      {
        if (((hnand->Config.BlockSize) * (hnand->Config.BlockNbr)) <= 65535U)
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00U;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
        }
        else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
        {
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(page_col);;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(page_col);;
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
          __DSB();
          *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
          __DSB();
        }
      }

       /* Write data to memory */
      for (uint32_t index = 0U; index < length; index++)
      {
        *(__IO uint8_t *)deviceAddress = *buffer;
        buffer++;
        __DSB();
      }

      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = 0x10;
      __DSB();

      /* Get tick */
      tickstart = HAL_GetTick();

      /* Read status until NAND is ready */
      while (HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if ((HAL_GetTick() - tickstart) > NAND_WRITE_TIMEOUT)
        {
          /* Update the NAND controller state */
          hnand->State = HAL_NAND_STATE_ERROR;

          /* Process unlocked */
          __HAL_UNLOCK(hnand);

          return HAL_TIMEOUT;
        }
      }
    }
    /* Update the NAND controller state */
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

// clang-format on

uint16_t ecc_get_oe(uint8_t oe, uint32_t ecc_val) {
  uint16_t ecc_temp = 0;
  for (uint8_t i = 0; i < 24; i++) {
    if ((i % 2) == oe) {
      if ((ecc_val >> i) & 0X01) {
        ecc_temp += 1 << (i >> 1);
      }
    }
  }
  return ecc_temp;
}

int ecc_correction(uint8_t *data, uint32_t oob_ecc, uint32_t ecc_val) {
  uint16_t eccrdo, eccrde, eccclo, ecccle;
  uint16_t eccchk = 0;
  uint16_t errorpos = 0;
  uint32_t bytepos = 0;
  eccrdo = ecc_get_oe(1, oob_ecc);
  eccrde = ecc_get_oe(0, oob_ecc);
  eccclo = ecc_get_oe(1, ecc_val);
  ecccle = ecc_get_oe(0, ecc_val);
  eccchk = eccrdo ^ eccrde ^ eccclo ^ ecccle;
  if (eccchk == 0XFFF)  // 1bit error
  {
    errorpos = eccrdo ^ eccclo;
    bytepos = errorpos / 8;
    data[bytepos] ^= 1 << (errorpos % 8);
  } else {
    return 1;
  }
  return 0;
}

int nand_erase_block(uint32_t block_num) {
  NAND_AddressTypeDef address;

  address.Plane = block_num / nand_dev.block_num;
  address.Block = block_num % nand_dev.block_num;
  address.Page = 0;

  return HAL_NAND_Erase_Block(&hnand1, &address);
}

int nand_read_spare(uint32_t page, uint8_t offset, uint8_t *buffer,
                    uint8_t length) {
  return HAL_NAND_Read_SpareArea_ex(&hnand1, page, offset, buffer, length);
}

int nand_write_spare(uint32_t page, uint8_t offset, uint8_t *buffer,
                     uint8_t length) {
  return HAL_NAND_Write_SpareArea_ex(&hnand1, page, offset, buffer, length);
}

int nand_read_page(uint32_t page, uint16_t offset, uint8_t *buffer,
                   uint16_t length) {
  if (offset % LOGIC_SECTOR_SIZE || length % LOGIC_SECTOR_SIZE) {
    return 1;
  }

  uint8_t ecc_count = 0;
  uint8_t ecc_start = 0;
  uint32_t ecc_buf[4], ecc_temp;
  uint8_t oob_data[64];

  ecc_count = length / NAND_ECC_SIZE;
  ecc_start = offset / NAND_ECC_SIZE;

  nand_read_spare(page, 0, oob_data, 64);

  for (int i = 0; i < ecc_count; i++) {
    HAL_NAND_ECC_Enable(&hnand1);
    HAL_NAND_Read_Page_ex(&hnand1, page, offset, buffer, NAND_ECC_SIZE);
    HAL_NAND_GetECC(&hnand1, &ecc_buf[ecc_start + i], 10);
    HAL_NAND_ECC_Disable(&hnand1);

    ecc_temp = *(uint32_t *)(oob_data + 0x10 + (ecc_start + i) * 4);

    if (ecc_buf[ecc_start + i] != ecc_temp) {
      if (ecc_correction(buffer, ecc_temp, ecc_buf[ecc_start + i])) {
        return HAL_ERROR;
      }
    }
    offset += NAND_ECC_SIZE;
    buffer += NAND_ECC_SIZE;
  }
  return HAL_OK;
}

int nand_write_page(uint32_t page, uint16_t offset, uint8_t *buffer,
                    uint16_t length) {
  if (offset % LOGIC_SECTOR_SIZE || length % LOGIC_SECTOR_SIZE) {
    return 1;
  }

  uint8_t ecc_count = 0;
  uint8_t ecc_start = 0;
  uint32_t ecc_buf[4];

  ecc_count = length / NAND_ECC_SIZE;
  ecc_start = offset / NAND_ECC_SIZE;

  for (int i = 0; i < ecc_count; i++) {
    HAL_NAND_ECC_Enable(&hnand1);
    HAL_NAND_Write_Page_ex(&hnand1, page, offset, buffer, NAND_ECC_SIZE);
    HAL_NAND_GetECC(&hnand1, &ecc_buf[ecc_start + i], 10);
    HAL_NAND_ECC_Disable(&hnand1);

    offset += NAND_ECC_SIZE;
    buffer += NAND_ECC_SIZE;
  }
  HAL_NAND_Write_SpareArea_ex(&hnand1, page, 0x10 + ecc_start,
                              (uint8_t *)&ecc_buf[ecc_start], ecc_count * 4);
  return HAL_OK;
}

int nand_internal_move(uint32_t source_page, uint32_t target_page) {
  return HAL_NAND_Internal_Move(&hnand1, source_page, target_page);
}
int nand_internal_write(uint32_t source_page, uint32_t target_page,
                        uint32_t offset, uint8_t *buf, uint32_t length) {
  if (offset % LOGIC_SECTOR_SIZE || length % LOGIC_SECTOR_SIZE) {
    return 1;
  }
  return HAL_NAND_Internal_Write(&hnand1, source_page, target_page, offset, buf,
                                 length);
}

static uint8_t buffer[2048], oob[64];

void nand_flash_test(void) {
  nand_read_page(64, 0, buffer, sizeof(buffer));
  nand_read_spare(64, 0, oob, 64);
  for (int i = 0; i < 2048; i++) {
    buffer[i] = i;
  }
  nand_write_page(64, 0, buffer, sizeof(buffer));
  memset(buffer, 0x00, 2048);
  nand_read_page(64, 0, buffer, sizeof(buffer));
  nand_read_spare(64, 0, oob, 64);

  HAL_NAND_Internal_Move(&hnand1, 64, 65);

  memset(buffer, 0x00, 2048);
  nand_read_page(65, 0, buffer, sizeof(buffer));
  nand_read_spare(65, 0, oob, 64);

  nand_erase_block(1);
  nand_read_page(64, 0, buffer, sizeof(buffer));
  nand_read_spare(64, 0, oob, 64);
  while (1) {
  }
}
