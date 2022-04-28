#ifndef TREZORHAL_QSPI_FLASH_
#define TREZORHAL_QSPI_FLASH_

#define QSPI_FLASH_SIZE 23
#define QSPI_SECTOR_SIZE (4 * 1024)
#define QSPI_PAGE_SIZE 256
#define QSPI_END_ADDR (1 << QSPI_FLASH_SIZE)
#define QSPI_FLASH_SIZES 8 * 1024 * 1024

#define WRITE_ENABLE_CMD 0x06
#define READ_ID_CMD2 0x9F
#define READ_STATUS_REG_CMD 0x05
#define READ_STATUS2_REG_CMD 0x35
#define WRITE_STATUS_REG_CMD 0x01
#define WRITE_STATUS2_REG_CMD 0x31
#define BULK_ERASE_CMD 0xC7
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD 0x20
#define SECTOR_64K_ERASE_CMD 0xD8
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD 0x32
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEB

#define QSPI_FLASH_BASE_ADDRESS (0x90000000)

typedef struct {
  uint8_t uniq_id[8];
  uint16_t page_size;
  uint32_t page_count;
  uint32_t sector_size;
  uint32_t sector_count;
  uint32_t block_size;
  uint32_t block_count;
  uint32_t capacity_in_kilobyte;
  uint8_t status_register1;
  uint8_t status_register2;
  uint8_t status_register3;
  uint8_t lock;
  char *desc;
} spi_flash_info;

int qspi_flash_init(void);
int qspi_flash_config(void);
uint32_t qspi_flash_read_id(void);
int qspi_flash_memory_mapped(void);
int qspi_flash_quit_memory_mapped(void);
int qspi_flash_erase_block_64k(uint32_t address);
int qspi_flash_write_page(uint8_t *data, uint32_t address, uint16_t len);
int qspi_flash_write_buffer_unsafe(uint8_t *data, uint32_t address,
                                   uint32_t len);
int qspi_flash_read_buffer(uint8_t *data, uint32_t address, uint32_t len);
void qspi_flash_test(void);

#endif
