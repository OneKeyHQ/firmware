#ifndef TREZORHAL_FMC_NAND_
#define TREZORHAL_FMC_NAND_

#define NAND_MAX_PAGE_SIZE 4096
#define NAND_ECC_SIZE 512
#define LOGIC_SECTOR_SIZE 512

typedef struct {
  uint16_t page_total_size;
  uint16_t page_size;
  uint16_t spare_size;
  uint8_t block_size;
  uint16_t block_num;
  uint16_t block_total_num;
  uint16_t good_block_num;
  uint16_t valid_block_num;
  uint32_t id;
  uint16_t *lut;
} nand_device;

int nand_flash_init(void);
int nand_read_page(uint32_t page, uint16_t offset, uint8_t *buffer,
                   uint16_t length);
int nand_write_page(uint32_t page, uint16_t offset, uint8_t *buffer,
                    uint16_t length);
void nand_flash_test(void);
int nand_erase_block(uint32_t block_num);

#endif
