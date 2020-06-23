#ifndef UTXO_CACHE_H_
#define UTXO_CACHE_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define UTXO_CACHE_HEADER 4
#define UTXO_INFO_LEN 28  // hash(prvid+index)+amount
#define UTXO_INFO_TOTLE_COUNT (128 * 1024 / UTXO_INFO_LEN)
#define UTXO_INFO_USEFUL_COUNT 400

typedef struct {
  uint8_t hash[20];
  uint64_t amount;
} utxo_info;

void utxo_cache_init(void);
bool utxo_cache_check(uint8_t* prv_id, uint32_t index, uint64_t amount);

#endif
