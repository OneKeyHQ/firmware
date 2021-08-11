#include "w25qxx.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define W25QXX_DUMMY_BYTE 0xA5
#define W25QXX_TIMEOUT 10000

w25qxx_t w25qxx = {.desc = "uninitialized"};

void spi_delay(unsigned int delay_ms) {
  uint32_t timeout = delay_ms * 5000;

  while (timeout--) {
    __asm__("nop");
  }
}

#define w25qxx_delay(delay) spi_delay(delay)

void gpio_write_pin(uint32_t port, uint32_t gpio, uint32_t value) {
  if (value == GPIO_PIN_RESET) {
    gpio_clear(port, gpio);
  } else {
    gpio_set(port, gpio);
  }
}

#define W25QXX_CS_LOW \
  gpio_write_pin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET)
#define W25QXX_CS_HIGH \
  gpio_write_pin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET)

void hal_spi_transmit(uint8_t *data, uint16_t size, uint32_t timeout) {
  if ((data == NULL) || (size > (w25qxx.capacity_in_kilobyte * 1024))) {
    return;
  }

  timeout = timeout;

  for (int i = 0; i < size; i++) {
    spi_send(_W25QXX_SPI, data[i]);
  }

  while (!(SPI_SR(_W25QXX_SPI) & SPI_SR_TXE))
    ;

  while ((SPI_SR(_W25QXX_SPI) & SPI_SR_BSY))
    ;
}

void hal_spi_receive(uint8_t *data, uint32_t size, uint32_t timeout) {
  if (data == NULL) {
    return;
  }

  timeout = timeout;

  for (uint32_t i = 0; i < size; i++) {
    data[i] = (uint8_t)spi_xfer(_W25QXX_SPI, W25QXX_DUMMY_BYTE);
  }
}

uint8_t w25qxx_spi(uint8_t Data) {
  uint8_t ret = 0;

  ret = (uint8_t)spi_xfer(_W25QXX_SPI, Data);
  return ret;
}

uint32_t w25qxx_read_id(void) {
  uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_JEDEC_ID);

  temp0 = w25qxx_spi(W25QXX_DUMMY_BYTE);
  temp1 = w25qxx_spi(W25QXX_DUMMY_BYTE);
  temp2 = w25qxx_spi(W25QXX_DUMMY_BYTE);

  W25QXX_CS_HIGH;

  temp = (temp0 << 16) | (temp1 << 8) | temp2;
  return temp;
}

void w25qxx_read_uniq_id(void) {
  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_READ_UNIQUE_ID);

  for (uint8_t i = 0; i < 4; i++) {
    w25qxx_spi(W25QXX_DUMMY_BYTE);
  }

  for (uint8_t i = 0; i < 8; i++) {
    w25qxx.uniq_id[i] = w25qxx_spi(W25QXX_DUMMY_BYTE);
  }

  W25QXX_CS_HIGH;
}

void w25qxx_write_enable(void) {
  W25QXX_CS_LOW;
  w25qxx_spi(W25QXX_CMD_ENABLE_WRITE);
  W25QXX_CS_HIGH;
  w25qxx_delay(1);
}

void w25qxx_write_disable(void) {
  W25QXX_CS_LOW;
  w25qxx_spi(W25QXX_CMD_DISABLE_WRITE);
  W25QXX_CS_HIGH;
  w25qxx_delay(1);
}

uint8_t w25qxx_read_status_register(uint8_t select) {
  uint8_t status = 0;

  W25QXX_CS_LOW;

  if (select == 1) {
    w25qxx_spi(W25QXX_CMD_READ_REG_SR1);
    status = w25qxx_spi(W25QXX_DUMMY_BYTE);
    w25qxx.status_register1 = status;
  } else if (select == 2) {
    w25qxx_spi(W25QXX_CMD_READ_REG_SR2);
    status = w25qxx_spi(W25QXX_DUMMY_BYTE);
    w25qxx.status_register2 = status;
  } else {
    w25qxx_spi(0x15);
    status = w25qxx_spi(W25QXX_DUMMY_BYTE);
    w25qxx.status_register3 = status;
  }

  W25QXX_CS_HIGH;
  return status;
}

void w25qxx_write_status_register(uint8_t select, uint8_t data) {
  W25QXX_CS_LOW;

  if (select == 1) {
    w25qxx_spi(W25QXX_CMD_WRITE_REG_SR);
    w25qxx.status_register1 = data;
  } else if (select == 2) {
    w25qxx_spi(0x31);
    w25qxx.status_register2 = data;
  } else {
    w25qxx_spi(0x11);
    w25qxx.status_register3 = data;
  }

  w25qxx_spi(data);

  W25QXX_CS_HIGH;
}

bool w25qxx_wait_for_write_end(void) {
  int32_t cnt = W25QXX_TIMEOUT;

  w25qxx_delay(1);

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_READ_REG_SR1);

  do {
    w25qxx.status_register1 = w25qxx_spi(W25QXX_DUMMY_BYTE);
    w25qxx_delay(1);
  } while (((w25qxx.status_register1 & 0x01) == 0x01) && (cnt-- >= 0));

  W25QXX_CS_HIGH;

  if (cnt < 0) {
    return false;
  } else {
    return true;
  }
}

bool w25qxx_init(void) {
  uint32_t id;

  w25qxx.lock = 1;

  W25QXX_CS_HIGH;

  w25qxx_delay(100);

  id = w25qxx_read_id();

  switch (id & 0x000000FF) {
    case 0x20:  // w25q512
      w25qxx.id = W25Q512;
      w25qxx.block_count = 1024;
      w25qxx.desc = "64M bytes";
      break;
    case 0x19:  // w25q256
      w25qxx.id = W25Q256;
      w25qxx.block_count = 512;
      w25qxx.desc = "32M bytes";
      break;
    case 0x18:  // w25q128
      w25qxx.id = W25Q128;
      w25qxx.block_count = 256;
      w25qxx.desc = "16M bytes";
      break;
    case 0x17:  // w25q64
      w25qxx.id = W25Q64;
      w25qxx.block_count = 128;
      w25qxx.desc = "8M bytes";
      break;
    case 0x16:  // w25q32
      w25qxx.id = W25Q32;
      w25qxx.block_count = 64;
      w25qxx.desc = "4M bytes";
      break;
    case 0x15:  // w25q16
      w25qxx.id = W25Q16;
      w25qxx.block_count = 32;
      w25qxx.desc = "2M bytes";
      break;
    case 0x14:  // w25q80
      w25qxx.id = W25Q80;
      w25qxx.block_count = 16;
      w25qxx.desc = "1M bytes";
      break;
    case 0x13:  // w25q40
      w25qxx.id = W25Q40;
      w25qxx.block_count = 8;
      w25qxx.desc = "512K bytes";
      break;
    case 0x12:  // w25q20
      w25qxx.id = W25Q20;
      w25qxx.block_count = 4;
      w25qxx.desc = "256K bytes";
      break;
    case 0x11:  // w25q10
      w25qxx.id = W25Q10;
      w25qxx.block_count = 2;
      w25qxx.desc = "128K bytes";
      break;
    default:
      w25qxx.lock = 0;
      w25qxx.desc = "unknown flash";
      return false;
  }

  w25qxx.page_size = 256;
  w25qxx.sector_size = 0x1000;
  w25qxx.sector_count = w25qxx.block_count * 16;
  w25qxx.page_count =
      (w25qxx.sector_count * w25qxx.sector_size) / w25qxx.page_size;
  w25qxx.block_size = w25qxx.sector_size * 16;
  w25qxx.capacity_in_kilobyte =
      (w25qxx.sector_count * w25qxx.sector_size) / 1024;
  w25qxx_read_uniq_id();
  w25qxx_read_status_register(1);
  w25qxx_read_status_register(2);
  w25qxx_read_status_register(3);

  w25qxx.lock = 0;
  return true;
}

char *w25qxx_get_desc(void) { return w25qxx.desc; }

bool w25qxx_erase_chip(void) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  w25qxx_write_enable();

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_ERASE_CHIP);

  W25QXX_CS_HIGH;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx_delay(10);
  w25qxx.lock = 0;
  return true;
}

bool w25qxx_erase_sector(uint32_t sector_addr) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  sector_addr = sector_addr * w25qxx.sector_size;

  w25qxx_write_enable();

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_ERASE_SECTOR);

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((sector_addr & 0xFF000000) >> 24);
  }
  w25qxx_spi((sector_addr & 0xFF0000) >> 16);
  w25qxx_spi((sector_addr & 0xFF00) >> 8);
  w25qxx_spi(sector_addr & 0xFF);

  W25QXX_CS_HIGH;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx_delay(1);
  w25qxx.lock = 0;

  return true;
}

bool w25qxx_erase_block(uint32_t block_addr) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  block_addr = block_addr * w25qxx.sector_size * 16;

  w25qxx_write_enable();

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_ERASE_BLOCK_64KB);

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((block_addr & 0xFF000000) >> 24);
  }
  w25qxx_spi((block_addr & 0xFF0000) >> 16);
  w25qxx_spi((block_addr & 0xFF00) >> 8);
  w25qxx_spi(block_addr & 0xFF);

  W25QXX_CS_HIGH;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx_delay(1);
  w25qxx.lock = 0;

  return true;
}

uint32_t w25qxx_page_to_sector(uint32_t page_addr) {
  return ((page_addr * w25qxx.page_size) / w25qxx.sector_size);
}

uint32_t w25qxx_page_to_block(uint32_t page_addr) {
  return ((page_addr * w25qxx.page_size) / w25qxx.block_size);
}

uint32_t w25qxx_sector_to_block(uint32_t sector_addr) {
  return ((sector_addr * w25qxx.sector_size) / w25qxx.block_size);
}

uint32_t w25qxx_sector_to_page(uint32_t sector_addr) {
  return (sector_addr * w25qxx.sector_size) / w25qxx.page_size;
}

uint32_t w25qxx_block_to_page(uint32_t block_addre) {
  return (block_addre * w25qxx.block_size) / w25qxx.page_size;
}

bool w25qxx_is_empty_page(uint32_t page_addr, uint32_t offset,
                          uint32_t number) {
  uint8_t buffer[32];
  uint32_t work_addr;
  uint32_t i;
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if (((number + offset) > w25qxx.page_size) || (number == 0)) {
    number = w25qxx.page_size - offset;
  }

  for (i = offset; i < w25qxx.page_size; i += sizeof(buffer)) {
    W25QXX_CS_LOW;

    work_addr = (i + page_addr * w25qxx.page_size);

    w25qxx_spi(W25QXX_CMD_READ_FAST);

    if (w25qxx.id >= W25Q256) w25qxx_spi((work_addr & 0xFF000000) >> 24);
    w25qxx_spi((work_addr & 0xFF0000) >> 16);
    w25qxx_spi((work_addr & 0xFF00) >> 8);
    w25qxx_spi(work_addr & 0xFF);
    w25qxx_spi(0);

    memset(buffer, 0, sizeof(buffer));
    hal_spi_receive(buffer, sizeof(buffer), 100);

    W25QXX_CS_HIGH;

    for (uint8_t x = 0; x < sizeof(buffer); x++) {
      if (buffer[x] != 0xFF) goto NOT_EMPTY;
    }
  }

  if ((w25qxx.page_size + offset) % sizeof(buffer) != 0) {
    i -= sizeof(buffer);

    for (; i < w25qxx.page_size; i++) {
      W25QXX_CS_LOW;

      work_addr = (i + page_addr * w25qxx.page_size);

      w25qxx_spi(W25QXX_CMD_READ_FAST);

      if (w25qxx.id >= W25Q256) {
        w25qxx_spi((work_addr & 0xFF000000) >> 24);
      }
      w25qxx_spi((work_addr & 0xFF0000) >> 16);
      w25qxx_spi((work_addr & 0xFF00) >> 8);
      w25qxx_spi(work_addr & 0xFF);
      w25qxx_spi(0);

      memset(buffer, 0, sizeof(buffer));
      hal_spi_receive(buffer, 1, 100);

      W25QXX_CS_HIGH;

      if (buffer[0] != 0xFF) {
        goto NOT_EMPTY;
      }
    }
  }

  w25qxx.lock = 0;
  return true;

NOT_EMPTY:
  w25qxx.lock = 0;
  return false;
}

bool w25qxx_is_empty_sector(uint32_t sector_addr, uint32_t offset,
                            uint32_t number) {
  uint8_t buffer[32];
  uint32_t work_addr;
  uint32_t i;
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if ((number > w25qxx.sector_size) || (number == 0))
    number = w25qxx.sector_size;

  for (i = offset; i < w25qxx.sector_size; i += sizeof(buffer)) {
    W25QXX_CS_LOW;

    work_addr = (i + sector_addr * w25qxx.sector_size);

    w25qxx_spi(W25QXX_CMD_READ_FAST);

    if (w25qxx.id >= W25Q256) {
      w25qxx_spi((work_addr & 0xFF000000) >> 24);
    }
    w25qxx_spi((work_addr & 0xFF0000) >> 16);
    w25qxx_spi((work_addr & 0xFF00) >> 8);
    w25qxx_spi(work_addr & 0xFF);
    w25qxx_spi(0);

    hal_spi_receive(buffer, sizeof(buffer), 100);

    W25QXX_CS_HIGH;

    for (uint8_t x = 0; x < sizeof(buffer); x++) {
      if (buffer[x] != 0xFF) goto NOT_EMPTY;
    }
  }

  if ((w25qxx.sector_size + offset) % sizeof(buffer) != 0) {
    i -= sizeof(buffer);
    for (; i < w25qxx.sector_size; i++) {
      W25QXX_CS_LOW;

      work_addr = (i + sector_addr * w25qxx.sector_size);

      w25qxx_spi(W25QXX_CMD_READ_FAST);

      if (w25qxx.id >= W25Q256) {
        w25qxx_spi((work_addr & 0xFF000000) >> 24);
      }
      w25qxx_spi((work_addr & 0xFF0000) >> 16);
      w25qxx_spi((work_addr & 0xFF00) >> 8);
      w25qxx_spi(work_addr & 0xFF);
      w25qxx_spi(0);

      memset(buffer, 0, sizeof(buffer));
      hal_spi_receive(buffer, 1, 100);

      W25QXX_CS_HIGH;

      if (buffer[0] != 0xFF) {
        goto NOT_EMPTY;
      }
    }
  }

  w25qxx.lock = 0;
  return true;

NOT_EMPTY:
  w25qxx.lock = 0;
  return false;
}

bool w25qxx_is_empty_block(uint32_t block_addr, uint32_t offset,
                           uint32_t number) {
  uint8_t buffer[32];
  uint32_t work_addr;
  uint32_t i;
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if ((number > w25qxx.block_size) || (number == 0)) {
    number = w25qxx.block_size;
  }

  for (i = offset; i < w25qxx.block_size; i += sizeof(buffer)) {
    W25QXX_CS_LOW;

    work_addr = (i + block_addr * w25qxx.block_size);

    w25qxx_spi(W25QXX_CMD_READ_FAST);

    if (w25qxx.id >= W25Q256) {
      w25qxx_spi((work_addr & 0xFF000000) >> 24);
    }
    w25qxx_spi((work_addr & 0xFF0000) >> 16);
    w25qxx_spi((work_addr & 0xFF00) >> 8);
    w25qxx_spi(work_addr & 0xFF);
    w25qxx_spi(0);

    memset(buffer, 0, sizeof(buffer));
    hal_spi_receive(buffer, sizeof(buffer), 100);

    W25QXX_CS_HIGH;

    for (uint8_t x = 0; x < sizeof(buffer); x++) {
      if (buffer[x] != 0xFF) goto NOT_EMPTY;
    }
  }

  if ((w25qxx.block_size + offset) % sizeof(buffer) != 0) {
    i -= sizeof(buffer);
    for (; i < w25qxx.block_size; i++) {
      W25QXX_CS_LOW;

      work_addr = (i + block_addr * w25qxx.block_size);

      w25qxx_spi(W25QXX_CMD_READ_FAST);

      if (w25qxx.id >= W25Q256) {
        w25qxx_spi((work_addr & 0xFF000000) >> 24);
      }
      w25qxx_spi((work_addr & 0xFF0000) >> 16);
      w25qxx_spi((work_addr & 0xFF00) >> 8);
      w25qxx_spi(work_addr & 0xFF);
      w25qxx_spi(0);

      memset(buffer, 0, sizeof(buffer));
      hal_spi_receive(buffer, 1, 100);

      W25QXX_CS_HIGH;

      if (buffer[0] != 0xFF) {
        goto NOT_EMPTY;
      }
    }
  }

  w25qxx.lock = 0;
  return true;

NOT_EMPTY:
  w25qxx.lock = 0;
  return false;
}

bool w25qxx_write_byte(uint8_t buffer, uint32_t write_addr) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx_write_enable();

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_PAGE_PROGRAM);

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((write_addr & 0xFF000000) >> 24);
  }
  w25qxx_spi((write_addr & 0xFF0000) >> 16);
  w25qxx_spi((write_addr & 0xFF00) >> 8);
  w25qxx_spi(write_addr & 0xFF);
  w25qxx_spi(buffer);

  W25QXX_CS_HIGH;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx.lock = 0;
  return true;
}

bool w25qxx_write_page(uint8_t *buffer, uint32_t page_addr, uint32_t offset,
                       uint32_t number) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if (((number + offset) > w25qxx.page_size) || (number == 0)) {
    number = w25qxx.page_size - offset;
  }

  if ((offset + number) > w25qxx.page_size) {
    number = w25qxx.page_size - offset;
  }

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx_write_enable();

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_PAGE_PROGRAM);

  page_addr = (page_addr * w25qxx.page_size) + offset;

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((page_addr & 0xFF000000) >> 24);
  }
  w25qxx_spi((page_addr & 0xFF0000) >> 16);
  w25qxx_spi((page_addr & 0xFF00) >> 8);
  w25qxx_spi(page_addr & 0xFF);

  hal_spi_transmit(buffer, number, 100);

  W25QXX_CS_HIGH;

  if (!w25qxx_wait_for_write_end()) {
    w25qxx.lock = 0;
    return false;
  }

  w25qxx_delay(1);
  w25qxx.lock = 0;
  return true;
}

bool w25qxx_write_sector(uint8_t *buffer, uint32_t sector_addr, uint32_t offset,
                         uint32_t number) {
  uint32_t start_page;
  int32_t bytes_to_write;
  uint32_t local_offset;

  if ((number > w25qxx.sector_size) || (number == 0)) {
    number = w25qxx.sector_size;
  }

  if (offset >= w25qxx.sector_size) {
    return false;
  }

  if ((offset + number) > w25qxx.sector_size) {
    bytes_to_write = w25qxx.sector_size - offset;
  } else {
    bytes_to_write = number;
  }

  start_page = w25qxx_sector_to_page(sector_addr) + (offset / w25qxx.page_size);
  local_offset = offset % w25qxx.page_size;

  do {
    w25qxx_write_page(buffer, start_page, local_offset, bytes_to_write);
    start_page++;
    bytes_to_write -= w25qxx.page_size - local_offset;
    buffer += w25qxx.page_size - local_offset;
    local_offset = 0;
  } while (bytes_to_write > 0);

  return true;
}

bool w25qxx_write_block(uint8_t *buffer, uint32_t block_addr, uint32_t offset,
                        uint32_t number) {
  uint32_t start_page;
  int32_t bytes_to_write;
  uint32_t local_offset;

  if ((number > w25qxx.block_size) || (number == 0)) {
    number = w25qxx.block_size;
  }

  if (offset >= w25qxx.block_size) {
    return false;
  }

  if ((offset + number) > w25qxx.block_size) {
    bytes_to_write = w25qxx.block_size - offset;
  } else {
    bytes_to_write = number;
  }

  start_page = w25qxx_block_to_page(block_addr) + (offset / w25qxx.page_size);
  local_offset = offset % w25qxx.page_size;

  do {
    w25qxx_write_page(buffer, start_page, local_offset, bytes_to_write);
    start_page++;
    bytes_to_write -= w25qxx.page_size - local_offset;
    buffer += w25qxx.page_size - local_offset;
    local_offset = 0;
  } while (bytes_to_write > 0);

  return true;
}

bool w25qxx_read_byte(uint8_t *buffer, uint32_t bytes_address) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_READ_FAST);

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((bytes_address & 0xFF000000) >> 24);
  }
  w25qxx_spi((bytes_address & 0xFF0000) >> 16);
  w25qxx_spi((bytes_address & 0xFF00) >> 8);
  w25qxx_spi(bytes_address & 0xFF);
  w25qxx_spi(0);

  *buffer = w25qxx_spi(W25QXX_DUMMY_BYTE);

  W25QXX_CS_HIGH;

  w25qxx.lock = 0;
  return true;
}

bool w25qxx_read_bytes(uint8_t *buffer, uint32_t read_addr, uint32_t number) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  W25QXX_CS_LOW;

  w25qxx_spi(0x0B);

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((read_addr & 0xFF000000) >> 24);
  }
  w25qxx_spi((read_addr & 0xFF0000) >> 16);
  w25qxx_spi((read_addr & 0xFF00) >> 8);
  w25qxx_spi(read_addr & 0xFF);
  w25qxx_spi(0);

  hal_spi_receive(buffer, number, 2000);

  W25QXX_CS_HIGH;

  w25qxx_delay(1);
  w25qxx.lock = 0;
  return true;
}

bool w25qxx_read_page(uint8_t *buffer, uint32_t page_addr, uint32_t offset,
                      uint32_t number) {
  int32_t cnt = W25QXX_TIMEOUT;

  while ((w25qxx.lock == 1) && (cnt-- >= 0)) {
    w25qxx_delay(1);
  }

  if (cnt < 0) {
    return false;
  }

  w25qxx.lock = 1;

  if ((number > w25qxx.page_size) || (number == 0)) {
    number = w25qxx.page_size;
  }

  if ((offset + number) > w25qxx.page_size) {
    number = w25qxx.page_size - offset;
  }

  page_addr = page_addr * w25qxx.page_size + offset;

  W25QXX_CS_LOW;

  w25qxx_spi(W25QXX_CMD_READ_FAST);

  if (w25qxx.id >= W25Q256) {
    w25qxx_spi((page_addr & 0xFF000000) >> 24);
  }
  w25qxx_spi((page_addr & 0xFF0000) >> 16);
  w25qxx_spi((page_addr & 0xFF00) >> 8);
  w25qxx_spi(page_addr & 0xFF);
  w25qxx_spi(0);

  hal_spi_receive(buffer, number, 100);

  W25QXX_CS_HIGH;

  w25qxx_delay(1);
  w25qxx.lock = 0;
  return true;
}

bool w25qxx_read_sector(uint8_t *buffer, uint32_t sector_addr, uint32_t offset,
                        uint32_t number) {
  uint32_t start_page;
  int32_t bytes_to_read;
  uint32_t local_offset;

  if ((number > w25qxx.sector_size) || (number == 0)) {
    number = w25qxx.sector_size;
  }

  if (offset >= w25qxx.sector_size) {
    return false;
  }

  if ((offset + number) > w25qxx.sector_size) {
    bytes_to_read = w25qxx.sector_size - offset;
  } else {
    bytes_to_read = number;
  }

  start_page = w25qxx_sector_to_page(sector_addr) + (offset / w25qxx.page_size);
  local_offset = offset % w25qxx.page_size;

  do {
    w25qxx_read_page(buffer, start_page, local_offset, bytes_to_read);
    start_page++;
    bytes_to_read -= w25qxx.page_size - local_offset;
    buffer += w25qxx.page_size - local_offset;
    local_offset = 0;
  } while (bytes_to_read > 0);

  return true;
}

bool w25qxx_read_block(uint8_t *buffer, uint32_t block_addr, uint32_t offset,
                       uint32_t number) {
  uint32_t start_page;
  int32_t bytes_to_read;
  uint32_t local_offset;

  if ((number > w25qxx.block_size) || (number == 0)) {
    number = w25qxx.block_size;
  }

  if (offset >= w25qxx.block_size) {
    return false;
  }

  if ((offset + number) > w25qxx.block_size) {
    bytes_to_read = w25qxx.block_size - offset;
  } else {
    bytes_to_read = number;
  }

  start_page = w25qxx_block_to_page(block_addr) + (offset / w25qxx.page_size);
  local_offset = offset % w25qxx.page_size;

  do {
    w25qxx_read_page(buffer, start_page, local_offset, bytes_to_read);
    start_page++;
    bytes_to_read -= w25qxx.page_size - local_offset;
    buffer += w25qxx.page_size - local_offset;
    local_offset = 0;
  } while (bytes_to_read > 0);

  return true;
}

bool w25qxx_write_buffer_unsafe(uint8_t *buffer, uint32_t address,
                                uint32_t len) {
  uint32_t page_remain = 0;

  if (address + len > (w25qxx.capacity_in_kilobyte * 1024)) {
    return false;
  }

  page_remain = w25qxx.page_size - address % w25qxx.page_size;

  while (len) {
    page_remain = len > page_remain ? page_remain : len;
    w25qxx_write_page(buffer, address / w25qxx.page_size,
                      address % w25qxx.page_size, page_remain);
    len -= page_remain;
    buffer += page_remain;
    address += page_remain;
    page_remain = w25qxx.page_size;
  }

  return true;
}

bool w25qxx_write_buffer(uint8_t *buffer, uint32_t address, uint32_t len) {
  uint32_t remain = 0, offset = 0, i = 0;
  uint8_t sector_buffer[4096];

  if (address + len > (w25qxx.capacity_in_kilobyte * 1024)) {
    return false;
  }

  offset = address % w25qxx.sector_size;
  remain = w25qxx.sector_size - offset;

  while (len) {
    remain = len > remain ? remain : len;
    w25qxx_read_bytes(sector_buffer, address, remain);
    for (i = 0; i < remain; i++) {
      if (sector_buffer[i] != 0xff) {
        break;
      }
    }
    // need erase
    if (i < remain) {
      w25qxx_read_bytes(sector_buffer,
                        (address / w25qxx.sector_size) * w25qxx.sector_size,
                        w25qxx.sector_size);
      w25qxx_erase_sector(address / w25qxx.sector_size);
      memcpy(sector_buffer + offset, buffer, remain);
      w25qxx_write_buffer_unsafe(
          sector_buffer, (address / w25qxx.sector_size) * w25qxx.sector_size,
          w25qxx.sector_size);
    } else {
      w25qxx_write_buffer_unsafe(buffer, address, remain);
    }
    address += remain;
    buffer += remain;
    len -= remain;
    remain = w25qxx.sector_size;
    offset = 0;
  }

  return true;
}
