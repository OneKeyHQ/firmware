#ifndef _NFT_H_
#define _NFT_H_

#include <stdbool.h>
#include <stdint.h>

#include "memory.h"

typedef struct __attribute__((packed)) {
  uint32_t index;
  uint16_t width;
  uint16_t height;
  char name_zh[32];
  char name_en[32];
} NFTInformation;

typedef struct __attribute__((packed)) {
  uint32_t state;
  NFTInformation nft_info;
} NFTHeader;

bool nft_add_info(NFTInformation *nft_info);
bool nft_add_data(uint32_t index, uint8_t *data, uint32_t offset, uint32_t len);
bool nft_get_info(uint32_t index, NFTInformation *nft_info);
bool nft_get_data(uint32_t index, uint8_t *data, uint32_t offset, uint32_t len);

#endif
