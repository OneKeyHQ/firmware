#ifndef __POLKADOT_BIGNUM_H__
#define __POLKADOT_BIGNUM_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool bignumLittleEndian_bcdprint(char *outBuffer, uint16_t outBufferLen,
                                 const uint8_t *inBCD, uint16_t inBCDLen);
void bignumLittleEndian_to_bcd(uint8_t *bcdOut, uint16_t bcdOutLen,
                               const uint8_t *binValue, uint16_t binValueLen);

bool bignumBigEndian_bcdprint(char *outBuffer, uint16_t outBufferLen,
                              const uint8_t *bcdIn, uint16_t bcdInLen);
void bignumBigEndian_to_bcd(uint8_t *bcdOut, uint16_t bcdOutLen,
                            const uint8_t *binValue, uint16_t binValueLen);

#endif
