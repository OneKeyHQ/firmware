#ifndef __POLKADOT_CRYPTO_HELPER_H__
#define __POLKADOT_CRYPTO_HELPER_H__

#include <stdint.h>

#define SS58_BLAKE_PREFIX (const unsigned char *)"SS58PRE"
#define SS58_BLAKE_PREFIX_LEN 7
#define SS58_ADDRESS_MAX_LEN 60u

uint8_t crypto_SS58CalculatePrefix(uint16_t addressType, uint8_t *prefixBytes);

uint8_t crypto_SS58EncodePubkey(uint8_t *buffer, uint16_t buffer_len,
                                uint16_t addressType, const uint8_t *pubkey);

#endif