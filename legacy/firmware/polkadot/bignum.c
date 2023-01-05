#include "bignum.h"
#include <string.h>

bool bignumLittleEndian_bcdprint(char *outBuffer, uint16_t outBufferLen,
                                 const uint8_t *inBCD, uint16_t inBCDLen) {
  static const char hexchars[] = "0123456789ABCDEF";
  uint8_t started = 0;
  memset(outBuffer, 0, outBufferLen);

  if (outBufferLen < 4) {
    return false;
  }

  if (inBCDLen * 2 > outBufferLen) {
    snprintf(outBuffer, outBufferLen, "ERR");
    return false;
  }

  for (uint8_t i = 0; i < inBCDLen; i++, inBCD++) {
    if (started || *inBCD != 0) {
      if (started || (*inBCD >> 4u) != 0) {
        *outBuffer = hexchars[*inBCD >> 4u];
        outBuffer++;
      }
      *outBuffer = hexchars[*inBCD & 0x0Fu];
      outBuffer++;
      started = 1;
    }
  }

  if (!started) {
    snprintf(outBuffer, outBufferLen, "0");
  }

  return true;
}

void bignumLittleEndian_to_bcd(uint8_t *bcdOut, uint16_t bcdOutLen,
                               const uint8_t *binValue, uint16_t binValueLen) {
  memset(bcdOut, 0, bcdOutLen);

  uint8_t carry = 0;
  for (uint16_t bitIdx = 0; bitIdx < binValueLen * 8; bitIdx++) {
    // Fix bcd
    for (uint16_t j = 0; j < bcdOutLen; j++) {
      if ((bcdOut[j] & 0x0Fu) > 0x04u) {
        bcdOut[j] += 0x03u;
      }
      if ((bcdOut[j] & 0xF0u) > 0x40u) {
        bcdOut[j] += 0x30u;
      }
    }

    // get bit
    const uint16_t byteIdx = bitIdx >> 3u;
    const uint8_t mask = 0x80u >> (bitIdx & 0x7u);
    carry = (uint8_t)((binValue[binValueLen - byteIdx - 1] & mask) > 0);

    // Shift bcd
    for (uint16_t j = 0; j < bcdOutLen; j++) {
      uint8_t carry2 = (uint8_t)(bcdOut[bcdOutLen - j - 1] > 127u);
      bcdOut[bcdOutLen - j - 1] <<= 1u;
      bcdOut[bcdOutLen - j - 1] += carry;
      carry = carry2;
    }
  }
}

bool bignumBigEndian_bcdprint(char *outBuffer, uint16_t outBufferLen,
                              const uint8_t *bcdIn, uint16_t bcdInLen) {
  static const char hexchars[] = "0123456789ABCDEF";
  uint8_t started = 0;
  memset(outBuffer, 0, outBufferLen);

  if (outBufferLen < 4) {
    return false;
  }

  if (bcdInLen * 2 > outBufferLen) {
    snprintf(outBuffer, outBufferLen, "ERR");
    return false;
  }

  for (uint16_t i = 0; i < bcdInLen; i++) {
    uint8_t v = bcdIn[bcdInLen - i - 1];
    if (started || v != 0) {
      if (started || (v >> 4u) != 0) {
        *outBuffer = hexchars[v >> 4u];
        outBuffer++;
      }
      *outBuffer = hexchars[v & 0x0Fu];
      outBuffer++;
      started = 1;
    }
  }

  if (!started) {
    snprintf(outBuffer, outBufferLen, "0");
  }

  return true;
}

void bignumBigEndian_to_bcd(uint8_t *bcdOut, uint16_t bcdOutLen,
                            const uint8_t *binValue, uint16_t binValueLen) {
  memset(bcdOut, 0, bcdOutLen);

  uint8_t carry = 0;
  for (uint16_t bitIdx = 0; bitIdx < binValueLen * 8; bitIdx++) {
    // Fix bcd
    for (uint16_t j = 0; j < bcdOutLen; j++) {
      if ((bcdOut[j] & 0x0Fu) > 0x04u) {
        bcdOut[j] += 0x03u;
      }
      if ((bcdOut[j] & 0xF0u) > 0x40u) {
        bcdOut[j] += 0x30u;
      }
    }

    // get bit
    const uint16_t byteIdx = bitIdx >> 3u;
    const uint8_t mask = 0x80u >> (bitIdx & 0x7u);
    carry = (uint8_t)((binValue[byteIdx] & mask) > 0);

    // Shift bcd
    for (uint16_t j = 0; j < bcdOutLen; j++) {
      uint8_t carry2 = (uint8_t)(bcdOut[j] > 127u);
      bcdOut[j] <<= 1u;
      bcdOut[j] += carry;
      carry = carry2;
    }
  }
}
