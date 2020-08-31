#include "storage_ex.h"
#include "common.h"
#include "flash.h"
#include "sha2.h"

// flash info
#define FLASH_USER_DATA_SECTOR 10
#define FLASH_UTXO_CACHE_START 0x080C0000
#define FLASH_UTXO_CACHE_SIZE (112 * 1024)
#define FLASH_WHITE_LIST_OFFSET FLASH_UTXO_CACHE_SIZE
#define FLASH_WHITE_LIST_START (FLASH_UTXO_CACHE_START + FLASH_UTXO_CACHE_SIZE)
#define FLASH_WHITE_LIST_SIZE (16 * 1024)

// utxo cache
#define UTXO_CACHE_HEADER 4
#define UTXO_INFO_LEN 28  // hash(prvid+index)+amount
#define UTXO_INFO_TOTLE_COUNT \
  ((FLASH_UTXO_CACHE_SIZE - UTXO_CACHE_HEADER) / UTXO_INFO_LEN)
#define UTXO_INFO_USEFUL_COUNT 400

// white list
#define WL_HEADER 4
#define WL_MAX_COUNTS ((FLASH_WHITE_LIST_SIZE - WL_HEADER) / sizeof(addr_info))
#define WL_USEFUL_COUNT 10

static uint32_t INIT_FLAG = 0x5A5A5A5A;

static uint32_t uxto_cache_count = 0;
static uint32_t uxto_cache_start = 0;

void utxo_cache_info(void) {
  const uint8_t* data;
  uint32_t i, m;
  uxto_cache_count = 0;
  uxto_cache_start = 0;
  data =
      (uint8_t*)flash_get_address(FLASH_USER_DATA_SECTOR, UTXO_CACHE_HEADER, 0);

  for (i = 0; i < UTXO_INFO_TOTLE_COUNT; i++) {
    for (m = 0; m < UTXO_INFO_LEN; m++) {
      if (data[m] != 0xff) break;
    }
    if (m == UTXO_INFO_LEN) break;
    uxto_cache_count++;
    data += UTXO_INFO_LEN;
  }
  if (uxto_cache_count > UTXO_INFO_USEFUL_COUNT) {
    uxto_cache_start = uxto_cache_count - UTXO_INFO_USEFUL_COUNT;
    uxto_cache_count = UTXO_INFO_USEFUL_COUNT;
  }
}
void sector_packet(void) {
  uint32_t i;
  const uint8_t* utxo_data;
  uint8_t utxo_buf[UTXO_INFO_USEFUL_COUNT * UTXO_INFO_LEN];

  const addr_info* addr_data;
  addr_info addr_list[WL_USEFUL_COUNT];
  uint32_t addr_count = 0;

  uint8_t* p = (uint8_t*)addr_list;

  utxo_data = (uint8_t*)flash_get_address(
      FLASH_USER_DATA_SECTOR,
      uxto_cache_start * UTXO_INFO_LEN + UTXO_CACHE_HEADER, 0);
  // utxo
  memcpy(utxo_buf, utxo_data, uxto_cache_count * UTXO_INFO_LEN);

  // white list
  addr_data = (addr_info*)flash_get_address(
      FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET + WL_HEADER, 0);
  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xaa) {
      memcpy(&addr_list[addr_count++], addr_data, sizeof(addr_info));
    }
  }
  ensure(flash_erase(FLASH_USER_DATA_SECTOR), "erase failed");
  ensure(flash_unlock_write(), NULL);
  for (i = 0; i < sizeof(utxo_buf); i++) {
    ensure(flash_write_byte(FLASH_USER_DATA_SECTOR, UTXO_CACHE_HEADER + i,
                            utxo_buf[i]),
           NULL);
  }
  for (i = 0; i < addr_count * sizeof(addr_info); i++) {
    ensure(flash_write_byte(FLASH_USER_DATA_SECTOR,
                            WL_HEADER + FLASH_WHITE_LIST_OFFSET + i, p[i]),
           NULL);
  }
  ensure(flash_write_word(FLASH_USER_DATA_SECTOR, 0, INIT_FLAG), NULL);
  ensure(flash_write_word(FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET,
                          INIT_FLAG),
         NULL);
  ensure(flash_lock_write(), NULL);
  uxto_cache_start = 0;
}

bool utxo_cache_check(uint8_t* prv_id, uint32_t index, uint64_t amount) {
  const uint8_t* data;
  uint32_t i;
  SHA1_CTX sha1_ctx;
  uint8_t sha1_hash[20];
  uint8_t utxo[UTXO_INFO_LEN];

  sha1_Init(&sha1_ctx);
  sha1_Update(&sha1_ctx, prv_id, 32);
  sha1_Update(&sha1_ctx, (uint8_t*)&index, 4);
  sha1_Final(&sha1_ctx, sha1_hash);

  memcpy(utxo, sha1_hash, sizeof(sha1_hash));
  memcpy(utxo + 20, (uint8_t*)&amount, sizeof(amount));

  data =
      (uint8_t*)flash_get_address(FLASH_USER_DATA_SECTOR, UTXO_CACHE_HEADER, 0);
  data += uxto_cache_start * UTXO_INFO_LEN;

  for (i = 0; i < uxto_cache_count; i++) {
    if (memcmp(sha1_hash, data, 20) == 0) {
      if (memcmp(data + 20, (uint8_t*)&amount, sizeof(amount))) {
        return false;
      } else
        return true;
    }
    data += UTXO_INFO_LEN;
  }
  // add new item
  if (data + UTXO_INFO_LEN > (uint8_t*)flash_get_address(FLASH_USER_DATA_SECTOR,
                                                         FLASH_UTXO_CACHE_SIZE,
                                                         0)) {
    sector_packet();
  }
  ensure(flash_unlock_write(), NULL);
  for (i = 0; i < UTXO_INFO_LEN; i++) {
    ensure(
        flash_write_byte(FLASH_USER_DATA_SECTOR,
                         (uxto_cache_start + uxto_cache_count) * UTXO_INFO_LEN +
                             UTXO_CACHE_HEADER + i,
                         utxo[i]),
        NULL);
  }
  ensure(flash_lock_write(), NULL);
  uxto_cache_count++;
  if (uxto_cache_count > UTXO_INFO_USEFUL_COUNT) {
    uxto_cache_start++;
    uxto_cache_count = UTXO_INFO_USEFUL_COUNT;
  }
  return true;
}

void user_data_init(void) {
  const uint8_t* utxo_data;
  const uint8_t* addr_data;

  bool utxo_initialized = false;
  bool list_initialized = false;
  utxo_data =
      (uint8_t*)flash_get_address(FLASH_USER_DATA_SECTOR, 0, UTXO_CACHE_HEADER);
  addr_data = (uint8_t*)flash_get_address(FLASH_USER_DATA_SECTOR,
                                          FLASH_UTXO_CACHE_SIZE, WL_HEADER);
  // not init
  if (!memcmp(utxo_data, &INIT_FLAG, sizeof(INIT_FLAG))) {
    utxo_initialized = true;
  }
  if (!memcmp(addr_data, &INIT_FLAG, sizeof(INIT_FLAG))) {
    list_initialized = true;
  }
  if (!utxo_initialized && !list_initialized) {
    ensure(flash_erase(FLASH_USER_DATA_SECTOR), "erase failed");
    ensure(flash_unlock_write(), NULL);
    ensure(flash_write_word(FLASH_USER_DATA_SECTOR, 0, INIT_FLAG), NULL);
    ensure(flash_write_word(FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET,
                            INIT_FLAG),
           NULL);
    ensure(flash_lock_write(), NULL);
  } else if (utxo_initialized && !list_initialized) {
    utxo_cache_info();
    sector_packet();
  }
  utxo_cache_info();
}

uint32_t white_list_get_count(void) {
  const addr_info* addr_data;
  uint32_t i, count;
  count = 0;
  addr_data = (addr_info*)flash_get_address(
      FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET + WL_HEADER, 0);

  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xaa) {
      count++;
    } else if (addr_data[i].state == 0xff) {
      break;
    }
  }
  return count;
}

bool white_list_check(const char* addr) {
  const addr_info* addr_data;
  uint32_t i;
  addr_data = (addr_info*)flash_get_address(
      FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET + WL_HEADER, 0);

  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xaa) {
      if (!memcmp(addr, addr_data[i].address, strlen(addr))) {
        return true;
      }
    } else if (addr_data[i].state == 0xff) {
      break;
    }
  }
  return false;
}

int white_list_add(const char* addr) {
  const addr_info* addr_data;
  addr_info addr_buf = {0};
  uint32_t i, count = 0;
  addr_data = (addr_info*)flash_get_address(
      FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET + WL_HEADER, 0);

  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xaa) {
      count++;
      if (!memcmp(addr, addr_data[i].address, strlen(addr))) {
        return WHILT_LIST_ADDR_EXIST;
      }
    } else if (addr_data[i].state == 0xff) {
      break;
    }
  }
  if (count == WL_USEFUL_COUNT) {
    return WHILT_LIST_FULL;
  }
  if (i == WL_MAX_COUNTS) {
    sector_packet();
  }
  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xff) {
      uint32_t m;
      uint8_t* p = (uint8_t*)&addr_buf;
      ensure(flash_unlock_write(), NULL);
      addr_buf.state = 0xaa;
      memset(addr_buf.rfu, 0xff, sizeof(addr_buf.rfu));
      memcpy(addr_buf.address, addr, strlen(addr));

      for (m = 0; m < sizeof(addr_buf); m++) {
        ensure(flash_write_byte(FLASH_USER_DATA_SECTOR,
                                i * sizeof(addr_info) +
                                    FLASH_WHITE_LIST_OFFSET + WL_HEADER + m,
                                p[m]),
               NULL);
      }
      ensure(flash_lock_write(), NULL);
      break;
    }
  }
  return WHITE_LIST_OK;
}

void white_list_delete(const char* addr) {
  const addr_info* addr_data;
  uint32_t i;
  addr_data = (addr_info*)flash_get_address(
      FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET + WL_HEADER, 0);

  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xaa) {
      if (!memcmp(addr, addr_data[i].address, strlen(addr))) {
        ensure(flash_unlock_write(), NULL);
        ensure(flash_write_byte(
                   FLASH_USER_DATA_SECTOR,
                   i * sizeof(addr_info) + FLASH_WHITE_LIST_OFFSET + WL_HEADER,
                   0x00),
               NULL);
        ensure(flash_lock_write(), NULL);
        break;
      }
    }
  }
}

void white_list_inquiry(char data[][130], uint16_t* count) {
  const addr_info* addr_data;
  uint32_t i, num = 0;
  addr_data = (addr_info*)flash_get_address(
      FLASH_USER_DATA_SECTOR, FLASH_WHITE_LIST_OFFSET + WL_HEADER, 0);

  for (i = 0; i < WL_MAX_COUNTS; i++) {
    if (addr_data[i].state == 0xaa) {
      memcpy(data[num++], addr_data[i].address, strlen(addr_data[i].address));
    } else if (addr_data[i].state == 0xff) {
      break;
    }
  }
  *count = num;
}
