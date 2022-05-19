// clang-format off
#include "ff.h"
#include "diskio.h"
#include "emmc.h"
// clang-format on

DSTATUS disk_initialize(BYTE pdrv) { return RES_OK; }
DSTATUS disk_status(BYTE pdrv) { return RES_OK; }
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
  DRESULT res = RES_ERROR;
  if (emmc_read_blocks((uint8_t *)buff, (uint32_t)(sector), count,
                       EMMC_TIMEOUT) == MMC_OK) {
    res = RES_OK;
  }

  return res;
}
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
  DRESULT res = RES_ERROR;
  if (emmc_write_blocks((uint8_t *)buff, (uint32_t)(sector), count,
                        EMMC_TIMEOUT) == MMC_OK) {
    res = RES_OK;
  }
  return res;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  (void)pdrv;
  EMMC_CardInfoTypeDef card_info = {0};
  switch (cmd) {
    case CTRL_SYNC:
      return RES_OK;
    case GET_SECTOR_COUNT:
      emmc_get_card_info(&card_info);
      *((DWORD *)buff) = card_info.LogBlockNbr;
      return RES_OK;
    case GET_SECTOR_SIZE:
      emmc_get_card_info(&card_info);
      *((DWORD *)buff) = card_info.LogBlockSize;
      return RES_OK;
    case GET_BLOCK_SIZE:
      emmc_get_card_info(&card_info);
      *((DWORD *)buff) = card_info.LogBlockSize / EMMC_BLOCK_SIZE;
      return RES_OK;
    default:
      return RES_PARERR;
  }
}
