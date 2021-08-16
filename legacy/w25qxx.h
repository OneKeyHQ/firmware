#ifndef _W25QXX_H
#define _W25QXX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "util.h"

#define MF_ID_WB 0xEF  // WINBOUND
#define MF_ID_GD 0xC8  // GigaDevice

// config info
#define _W25QXX_SPI SPI2
#define _W25QXX_CS_GPIO GPIOB
#define _W25QXX_CS_PIN GPIO12
#define _W25QXX_DEBUG 0

#define GPIO_PIN_RESET 0
#define GPIO_PIN_SET 1

#define W25QXX_CMD_ENABLE_WRITE 0x06
#define W25QXX_CMD_ENABLE_VOLATILE_WRITE 0x50
#define W25QXX_CMD_DISABLE_WRITE 0x04
#define W25QXX_CMD_READ_REG_SR1 0x05
#define W25QXX_CMD_READ_REG_SR2 0x35
#define W25QXX_CMD_WRITE_REG_SR 0x01
#define W25QXX_CMD_READ_DATA 0x03
#define W25QXX_CMD_READ_FAST 0x0B
#define W25QXX_CMD_READ_DUAL_FAST 0x3B
#define W25QXX_CMD_PAGE_PROGRAM 0x02
#define W25QXX_CMD_ERASE_SECTOR 0x20
#define W25QXX_CMD_ERASE_BLOCK_64KB 0xD8
#define W25QXX_CMD_ERASE_BLOCK_32KB 0x52
#define W25QXX_CMD_ERASE_CHIP 0xC7
#define W25QXX_CMD_ERASE_SUSPEND 0x75
#define W25QXX_CMD_ERASE_RESUME 0x7A
#define W25QXX_CMD_POWER_DOWN 0xB9
#define W25QXX_CMD_RELEASE_POWER_DOWN 0xAB
#define W25QXX_CMD_DEVICE_ID 0xAB
#define W25QXX_CMD_MANUFACT_DEVICE_ID 0x90
#define W25QXX_CMD_READ_UNIQUE_ID 0x4B
#define W25QXX_CMD_JEDEC_ID 0x9F
#define W25QXX_CMD_ERASE_SECURITY_REG 0x44
#define W25QXX_CMD_PROGRAM_SECURITY_REG 0x42
#define W25QXX_CMD_READ_SECURITY_REG 0x48
#define W25QXX_CMD_ENABLE_RESET 0x66
#define W25QXX_CMD_RESET 0x99

typedef enum {
  W25Q10 = 1,
  W25Q20,
  W25Q40,
  W25Q80,
  W25Q16,
  W25Q32,
  W25Q64,
  W25Q128,
  W25Q256,
  W25Q512,
} W25QXX_ID_t;

typedef struct {
  W25QXX_ID_t id;
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
} w25qxx_t;

extern w25qxx_t w25qxx;

// in Page, Sector and block read/write functions, can put 0 to read maximum
// bytes
bool w25qxx_init(void);
char *w25qxx_get_desc(void);
uint32_t w25qxx_read_id(void);

bool w25qxx_erase_chip(void);
bool w25qxx_erase_sector(uint32_t sector_addr);
bool w25qxx_erase_block(uint32_t block_addr);

uint32_t w25qxx_page_to_sector(uint32_t page_addr);
uint32_t w25qxx_page_to_block(uint32_t page_addr);
uint32_t w25qxx_sector_to_block(uint32_t sector_addr);
uint32_t w25qxx_sector_to_page(uint32_t sector_addr);
uint32_t w25qxx_block_to_page(uint32_t block_addr);

bool w25qxx_is_empty_page(uint32_t page_addr, uint32_t offset, uint32_t number);
bool w25qxx_is_empty_sector(uint32_t sector_addr, uint32_t offset,
                            uint32_t number);
bool w25qxx_is_empty_block(uint32_t block_addr, uint32_t offset,
                           uint32_t number);

bool w25qxx_write_byte(uint8_t buffer, uint32_t bytes_addr);
bool w25qxx_write_page(uint8_t *buffer, uint32_t page_addr, uint32_t offset,
                       uint32_t number);
bool w25qxx_write_sector(uint8_t *buffer, uint32_t sector_addr, uint32_t offset,
                         uint32_t number);
bool w25qxx_write_block(uint8_t *buffer, uint32_t block_addr, uint32_t offset,
                        uint32_t number);

bool w25qxx_read_byte(uint8_t *buffer, uint32_t bytes_addr);
bool w25qxx_read_bytes(uint8_t *buffer, uint32_t read_addr, uint32_t number);
bool w25qxx_read_page(uint8_t *buffer, uint32_t page_addr, uint32_t offset,
                      uint32_t number);
bool w25qxx_read_sector(uint8_t *buffer, uint32_t sector_addr, uint32_t offset,
                        uint32_t number);
bool w25qxx_read_block(uint8_t *buffer, uint32_t block_addr, uint32_t offset,
                       uint32_t number);
bool w25qxx_write_buffer_unsafe(uint8_t *buffer, uint32_t address,
                                uint32_t len);
bool w25qxx_write_buffer(uint8_t *buffer, uint32_t address, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
