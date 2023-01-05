#include "crypto_helper.h"
#include <string.h>
#include "base58.h"
#include "hasher.h"
#include "memzero.h"
#include "util.h"

static int ss58hash(const unsigned char *in, unsigned int inLen,
                    unsigned char *out) {
  Hasher hasher = {0};
  hasher.type = HASHER_BLAKE2B;
  blake2b_Init(&hasher.ctx.blake2b, 64);
  hasher_Update(&hasher, SS58_BLAKE_PREFIX, SS58_BLAKE_PREFIX_LEN);
  hasher_Update(&hasher, in, inLen);
  blake2b_Final(&hasher.ctx.blake2b, out, 64);
  return 0;
}

uint8_t crypto_SS58CalculatePrefix(uint16_t addressType, uint8_t *prefixBytes) {
  if (addressType > 16383) {
    return 0;
  }

  if (addressType > 63) {
    prefixBytes[0] = 0x40 | ((addressType >> 2) & 0x3F);
    prefixBytes[1] = ((addressType & 0x3) << 6) + ((addressType >> 8) & 0x3F);
    return 2;
  }

  prefixBytes[0] = addressType & 0x3F;  // address type
  return 1;
}

uint8_t crypto_SS58EncodePubkey(uint8_t *buffer, uint16_t buffer_len,
                                uint16_t addressType, const uint8_t *pubkey) {
  // based on https://docs.substrate.io/v3/advanced/ss58/
  if (buffer == NULL || buffer_len < SS58_ADDRESS_MAX_LEN) {
    return 0;
  }
  if (pubkey == NULL) {
    return 0;
  }
  memzero(buffer, buffer_len);

  uint8_t hash[64];
  uint8_t unencoded[36];

  const uint8_t prefixSize = crypto_SS58CalculatePrefix(addressType, unencoded);
  if (prefixSize == 0) {
    return 0;
  }

  memcpy(unencoded + prefixSize, pubkey, 32);  // account id
  ss58hash((uint8_t *)unencoded, 32 + prefixSize, hash);
  unencoded[32 + prefixSize] = hash[0];
  unencoded[33 + prefixSize] = hash[1];

  size_t outLen = buffer_len;
  int r = base58_encode(unencoded, 34 + prefixSize, (char *)buffer, outLen);
  return r;
}
