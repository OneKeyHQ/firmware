#ifndef __ALGO_ASA_H__
#define __ALGO_ASA_H__

#include <stdint.h>

typedef struct {
  uint64_t assetId;
  uint8_t decimals;
  const char unit[15];
  const char name[32];
} __attribute__((packed)) algo_asset_info_t;

const algo_asset_info_t *algo_asa_get(uint64_t id);

#endif
