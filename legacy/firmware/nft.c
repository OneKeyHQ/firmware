#include "nft.h"
#include "w25qxx.h"

#define NFT_HEADER_SIZE 128
#define NFT_DATA_SIZE (32 * 1024)
#define NFT_HEADER_COUNT (SPI_FLASH_NFT_HEADER_LEN / NFT_HEADER_SIZE)

#define UNSET 0xffffffff
#define INITIALIZED 0x5A5AA5A5
#define DELETED 0x00000000

static uint32_t nft_data_len = 0;
static uint32_t nft_data_offset = 0;
static uint32_t nft_index = 0;
static bool nft_update = false;

bool nft_add_info(NFTInformation *nft_info) {
  NFTHeader nft_header = {0};

  if (!nft_info) {
    return false;
  }

  if (nft_info->index >= NFT_HEADER_COUNT) {
    return false;
  }

  nft_data_len = nft_info->height * nft_info->width * 2;
  if (nft_data_len > NFT_DATA_SIZE) {
    return false;
  }
  nft_header.state = UNSET;
  memcpy(&nft_header.nft_info, nft_info, sizeof(NFTInformation));
  nft_data_offset = 0;
  nft_index = nft_info->index;
  nft_update = true;

  return w25qxx_write_buffer(
      (uint8_t *)&nft_header,
      SPI_FLASH_NFT_HEADER_ADDR_START + nft_index * NFT_HEADER_SIZE,
      sizeof(NFTHeader));
}

bool nft_add_data(uint32_t index, uint8_t *data, uint32_t offset,
                  uint32_t len) {
  uint32_t state;

  if (!nft_update) {
    goto end;
  }
  if (index != nft_index) {
    goto end;
  }
  if (offset + len > nft_data_len) {
    goto end;
  }
  if (offset != nft_data_offset) {
    goto end;
  }
  if (!w25qxx_write_buffer(
          data,
          SPI_FLASH_NFT_DATA_ADDR_START + nft_index * NFT_DATA_SIZE + offset,
          len)) {
    goto end;
  }
  nft_data_offset += len;
  if (nft_data_offset == nft_data_len) {
    state = INITIALIZED;
    nft_update = false;
    return w25qxx_write_buffer(
        (uint8_t *)&state,
        SPI_FLASH_NFT_HEADER_ADDR_START + nft_index * NFT_HEADER_SIZE,
        sizeof(state));
  }

end:
  nft_update = false;
  nft_data_offset = 0;
  nft_data_len = 0;
  return false;
}

bool nft_is_initialized(uint32_t index) {
  NFTHeader nft_header = {0};

  if (index >= NFT_HEADER_COUNT) {
    return false;
  }
  w25qxx_read_bytes(
      (uint8_t *)&nft_header,
      SPI_FLASH_NFT_HEADER_ADDR_START + nft_index * NFT_HEADER_SIZE,
      sizeof(NFTHeader));

  return nft_header.state == INITIALIZED;
}

bool nft_get_info(uint32_t index, NFTInformation *nft_info) {
  NFTHeader nft_header = {0};

  if (!nft_info) {
    return false;
  }

  if (index >= NFT_HEADER_COUNT) {
    return false;
  }
  w25qxx_read_bytes(
      (uint8_t *)&nft_header,
      SPI_FLASH_NFT_HEADER_ADDR_START + nft_index * NFT_HEADER_SIZE,
      sizeof(NFTHeader));

  if (nft_header.state != INITIALIZED) {
    return false;
  }

  memcpy(nft_info, &nft_header.nft_info, sizeof(NFTInformation));

  return true;
}

bool nft_get_data(uint32_t index, uint8_t *data, uint32_t offset,
                  uint32_t len) {
  NFTHeader nft_header = {0};
  uint32_t nft_len = 0;
  if (index >= NFT_HEADER_COUNT) {
    return false;
  }
  w25qxx_read_bytes(
      (uint8_t *)&nft_header,
      SPI_FLASH_NFT_HEADER_ADDR_START + nft_index * NFT_HEADER_SIZE,
      sizeof(NFTHeader));

  if (nft_header.state != INITIALIZED) {
    return false;
  }
  nft_len = nft_header.nft_info.width * nft_header.nft_info.height * 2;
  if (offset + len > nft_len) {
    return false;
  }
  return w25qxx_read_bytes(
      data, SPI_FLASH_NFT_DATA_ADDR_START + nft_index * NFT_DATA_SIZE + offset,
      len);
}
