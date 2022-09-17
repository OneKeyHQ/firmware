// clang-format off

/**
  ******************************************************************************
  * @file    usbd_msc_storage_template.c
  * @author  MCD Application Team
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
- "stm32xxxxx_{eval}{discovery}{adafruit}_sd.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_storage.h"

#include "emmc.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define STORAGE_LUN_NBR                  1U
#define STORAGE_BLK_NBR                  0x10000U
#define STORAGE_BLK_SIZ                  0x200U

int8_t STORAGE_Init(uint8_t lun);

int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num,
                           uint16_t *block_size);

int8_t  STORAGE_IsReady(uint8_t lun);

int8_t  STORAGE_IsWriteProtected(uint8_t lun);

int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr,
                    uint16_t blk_len);

int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr,
                     uint16_t blk_len);

int8_t STORAGE_GetMaxLun(void);

/* USB Mass storage Standard Inquiry Data */
int8_t  STORAGE_Inquirydata[] =  /* 36 */
{

  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'O', 'N', 'E', 'K', 'E', 'Y', ' ', ' ', /* Manufacturer : 8 bytes */
  'T', 'O', 'U', 'C', 'H', '-', 'B', 'O', /* Product      : 16 Bytes */
  'A', 'R', 'D', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0', '1',                     /* Version      : 4 Bytes */
};

USBD_StorageTypeDef USBD_DISK_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata,

};

static EMMC_CardInfoTypeDef emmc_card_info={0};
/*******************************************************************************
  * Function Name  : Read_Memory
  * Description    : Handle the Read operation from the microSD card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t STORAGE_Init(uint8_t lun)
{
  emmc_get_card_info(&emmc_card_info);
  return (0);
}

/*******************************************************************************
  * Function Name  : Read_Memory
  * Description    : Handle the Read operation from the STORAGE card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  #ifdef APP_VER
    *block_num = emmc_card_info.LogBlockNbr - BOOT_EMMC_BLOCKS - 1;
    *block_size = emmc_card_info.LogBlockSize;
  #else
    *block_num = emmc_card_info.LogBlockNbr - 1;
    *block_size = emmc_card_info.LogBlockSize;
  #endif
  return (0);
}

/*******************************************************************************
  * Function Name  : Read_Memory
  * Description    : Handle the Read operation from the STORAGE card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t  STORAGE_IsReady(uint8_t lun)
{
  if(emmc_get_card_state()==MMC_TRANSFER_OK){
    return (0);
  }
  return (-1);
}

/*******************************************************************************
  * Function Name  : Read_Memory
  * Description    : Handle the Read operation from the STORAGE card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t  STORAGE_IsWriteProtected(uint8_t lun)
{
  return  0;
}

/*******************************************************************************
  * Function Name  : Read_Memory
  * Description    : Handle the Read operation from the STORAGE card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf,
                    uint32_t blk_addr, uint16_t blk_len)
{
  #ifdef APP_VER
    if(emmc_read_blocks(buf, blk_addr + BOOT_EMMC_BLOCKS, blk_len, EMMC_TIMEOUT) == MMC_OK){
      return (0);
    }
  #else
  if(emmc_read_blocks(buf, blk_addr, blk_len, EMMC_TIMEOUT) == MMC_OK){
      while (emmc_get_card_state()!=MMC_TRANSFER_OK);  
      return (0);
    }
  #endif
  return (-1);
  return 0;
}
/*******************************************************************************
  * Function Name  : Write_Memory
  * Description    : Handle the Write operation to the STORAGE card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf,
                     uint32_t blk_addr, uint16_t blk_len)
{
  #ifdef APP_VER
    if(emmc_write_blocks(buf, blk_addr + BOOT_EMMC_BLOCKS, blk_len, EMMC_TIMEOUT) == MMC_OK){
      return (0);
    }
  #else
    if(emmc_write_blocks(buf, blk_addr, blk_len, EMMC_TIMEOUT) == MMC_OK){
      while (emmc_get_card_state()!=MMC_TRANSFER_OK);      
      return (0);
    }
  #endif
  return (-1);
}
/*******************************************************************************
  * Function Name  : Write_Memory
  * Description    : Handle the Write operation to the STORAGE card.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  *******************************************************************************/
int8_t STORAGE_GetMaxLun(void)
{
  return (STORAGE_LUN_NBR - 1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

