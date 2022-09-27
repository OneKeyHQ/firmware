#ifndef TREZORHAL_EMMC_H
#define TREZORHAL_EMMC_H

#include STM32_HAL_H

#define EMMC_CardInfoTypeDef HAL_MMC_CardInfoTypeDef

#define MMC_OK ((uint8_t)0x00)
#define MMC_ERROR ((uint8_t)0x01)

#define MMC_TRANSFER_OK ((uint8_t)0x00)
#define MMC_TRANSFER_BUSY ((uint8_t)0x01)

#define EMMC_TIMEOUT 500
#define EMMC_BLOCK_SIZE 512

#define BOOT_EMMC_BLOCKS (2 * 1024 * 1024)  // 1GB

void emmc_init();
uint8_t emmc_get_card_state(void);
void emmc_get_card_info(EMMC_CardInfoTypeDef *card_info);
uint8_t emmc_read_blocks(uint8_t *data, uint32_t address, uint32_t nums,
                         uint32_t timeout);
uint8_t emmc_write_blocks(uint8_t *data, uint32_t address, uint32_t nums,
                          uint32_t timeout);
uint8_t emmc_erase(uint32_t start_address, uint32_t end_address);
uint64_t emmc_get_capacity_in_bytes(void);

#endif
