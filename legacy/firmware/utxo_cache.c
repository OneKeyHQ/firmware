#include "utxo_cache.h"
#include "common.h"
#include "flash.h"
#include "sha2.h"

#define FLASH_UTXO_CACHE_SECTOR 10
#define FLASH_UTXO_CACHE_START 0x080C0000
#define FLASH_UTXO_CACHE_LEN (128 * 1024)

static uint32_t UTXO_CACHE_USED = 0x5A5A5A5A;

static uint32_t uxto_cache_count = 0;
static uint32_t uxto_cache_start = 0;

void utxo_cache_init(void) {
  const uint8_t* data;
  uint32_t i, m;
  data = (uint8_t*)flash_get_address(FLASH_UTXO_CACHE_SECTOR, 0,
                                     UTXO_CACHE_HEADER);
  if (memcmp(data, &UTXO_CACHE_USED, sizeof(UTXO_CACHE_USED))) {
    ensure(flash_erase(FLASH_UTXO_CACHE_SECTOR), "erase failed");
    ensure(flash_unlock_write(), NULL);
    ensure(flash_write_word(FLASH_UTXO_CACHE_SECTOR, 0, UTXO_CACHE_USED), NULL);
    ensure(flash_lock_write(), NULL);
  } else {
    data += 4;
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
}

bool utxo_cache_check(uint8_t* prv_id, uint32_t index, uint64_t amount) {
  const uint8_t *data, *start;
  uint32_t i;
  SHA1_CTX sha1_ctx;
  uint8_t sha1_hash[20];
  uint8_t utxo[UTXO_INFO_LEN];
  uint8_t utxo_buf[UTXO_INFO_USEFUL_COUNT * UTXO_INFO_LEN];
  sha1_Init(&sha1_ctx);
  sha1_Update(&sha1_ctx, prv_id, 32);
  sha1_Update(&sha1_ctx, (uint8_t*)&index, 4);
  sha1_Final(&sha1_ctx, sha1_hash);

  memcpy(utxo, sha1_hash, sizeof(sha1_hash));
  memcpy(utxo + 20, (uint8_t*)&amount, sizeof(amount));

  data = (uint8_t*)flash_get_address(FLASH_UTXO_CACHE_SECTOR, UTXO_CACHE_HEADER,
                                     0);
  data += uxto_cache_start * UTXO_INFO_LEN;
  start = data;

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
  if ((uint32_t)data + UTXO_INFO_LEN >=
      (FLASH_UTXO_CACHE_START + FLASH_UTXO_CACHE_LEN)) {
    memcpy(utxo_buf, start + UTXO_INFO_LEN, sizeof(utxo_buf) - UTXO_INFO_LEN);
    memcpy(utxo_buf + (UTXO_INFO_USEFUL_COUNT - 1) * UTXO_INFO_LEN, utxo,
           UTXO_INFO_LEN);
    ensure(flash_erase(FLASH_UTXO_CACHE_SECTOR), "erase failed");
    ensure(flash_unlock_write(), NULL);
    for (i = 0; i < sizeof(utxo_buf); i++) {
      ensure(flash_write_byte(FLASH_UTXO_CACHE_SECTOR, UTXO_CACHE_HEADER + i,
                              utxo_buf[i]),
             NULL);
    }
    ensure(flash_write_word(FLASH_UTXO_CACHE_SECTOR, 0, UTXO_CACHE_USED), NULL);
    ensure(flash_lock_write(), NULL);
    uxto_cache_start = 0;
  } else {
    ensure(flash_unlock_write(), NULL);
    for (i = 0; i < UTXO_INFO_LEN; i++) {
      ensure(flash_write_byte(FLASH_UTXO_CACHE_SECTOR,
                              (uint32_t)data - FLASH_UTXO_CACHE_START + i,
                              utxo[i]),
             NULL);
    }
    ensure(flash_lock_write(), NULL);
    uxto_cache_count++;
    if (uxto_cache_count > UTXO_INFO_USEFUL_COUNT) {
      uxto_cache_start++;
      uxto_cache_count = UTXO_INFO_USEFUL_COUNT;
    }
  }
  return true;
}