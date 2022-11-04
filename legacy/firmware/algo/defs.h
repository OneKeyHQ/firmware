#ifndef __ALGO_DEFS_H__
#define __ALGO_DEFS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memzero.h"

#define CHECK_ZXERR(CALL)            \
  {                                  \
    zxerr_t err = CALL;              \
    if (err != zxerr_ok) return err; \
  }

#define __Z_INLINE inline __attribute__((always_inline)) static
#define CHECK_APP_CANARY() \
  {}

#define array_length(array) (sizeof(array) / sizeof((array)[0]))
__Z_INLINE void strncpy_s(char *dst, const char *src, size_t dstSize) {
  memset(dst, 0, dstSize);
  if (dstSize > 0) {
    strncpy(dst, src, dstSize - 1);
  }
}

__Z_INLINE uint8_t fpstr_to_str(char *out, uint16_t outLen, const char *number,
                                uint8_t decimals) {
  memset(out, 0, outLen);
  size_t digits = strlen(number);

  if (decimals == 0) {
    if (digits == 0) {
      snprintf(out, outLen, "0");
      return 0;
    }

    if (outLen < digits) {
      snprintf(out, outLen, "ERR");
      return 1;
    }

    // No need for formatting
    snprintf(out, outLen, "%s", number);
    return 0;
  }

  if ((outLen < decimals + 2)) {
    snprintf(out, outLen, "ERR");
    return 1;
  }

  if (outLen < digits + 2) {
    snprintf(out, outLen, "ERR");
    return 1;
  }

  if (digits <= decimals) {
    if (outLen <= decimals + 2) {
      snprintf(out, outLen, "ERR");
      return 1;
    }

    // First part
    snprintf(out, outLen, "0.");
    out += 2;
    outLen -= 2;

    memset(out, '0', decimals - digits);
    out += decimals - digits;
    outLen -= decimals - digits;

    snprintf(out, outLen, "%s", number);
    return 0;
  }

  const size_t shift = digits - decimals;
  snprintf(out, outLen, "%s", number);
  number += shift;

  out += shift;
  outLen -= shift;

  *out++ = '.';
  outLen--;
  snprintf(out, outLen, "%s", number);
  return 0;
}

__Z_INLINE void number_inplace_trimming(char *s, uint8_t non_trimmed) {
  const size_t len = strlen(s);
  if (len == 0 || len == 1 || len > 1024) {
    return;
  }

  int16_t dec_point = -1;
  for (int16_t i = 0; i < (int16_t)len && dec_point < 0; i++) {
    if (s[i] == '.') {
      dec_point = i;
    }
  }
  if (dec_point < 0) {
    return;
  }

  const size_t limit = (size_t)dec_point + non_trimmed;
  for (size_t i = (len - 1); i > limit && s[i] == '0'; i--) {
    s[i] = 0;
  }
}

typedef enum {
  zxerr_unknown = 0b00000000,
  zxerr_ok = 0b00000011,
  zxerr_no_data = 0b00000101,
  zxerr_buffer_too_small = 0b00000110,
  zxerr_out_of_bounds = 0b00001001,
  zxerr_encoding_failed = 0b00001010,
  zxerr_invalid_crypto_settings = 0b00001100,
  zxerr_ledger_api_error = 0b00001111,
} zxerr_t;

__Z_INLINE const char *int32_to_str(char *data, int dataLen, int32_t number) {
  if (dataLen < 2) return "Buffer too small";
  memzero(data, dataLen);
  char *p = data;
  if (number < 0) {
    *(p++) = '-';
    data++;
  } else if (number == 0) {
    *(p++) = '0';
  }
  int32_t tmp;
  while (number != 0) {
    if (p - data >= (dataLen - 1)) {
      return "Buffer too small";
    }
    tmp = number % 10;
    tmp = tmp < 0 ? -tmp : tmp;
    *(p++) = (char)('0' + tmp);
    number /= 10u;
  }
  while (p > data) {
    p--;
    char z = *data;
    *data = *p;
    *p = z;
    data++;
  }
  return NULL;
}

__Z_INLINE const char *int64_to_str(char *data, int dataLen, int64_t number) {
  if (dataLen < 2) return "Buffer too small";
  memzero(data, dataLen);
  char *p = data;
  if (number < 0) {
    *(p++) = '-';
    data++;
  } else if (number == 0) {
    *(p++) = '0';
  }
  int64_t tmp;
  while (number != 0) {
    if (p - data >= (dataLen - 1)) {
      return "Buffer too small";
    }
    tmp = number % 10;
    tmp = tmp < 0 ? -tmp : tmp;
    *(p++) = (char)('0' + tmp);
    number /= 10u;
  }
  while (p > data) {
    p--;
    char z = *data;
    *data = *p;
    *p = z;
    data++;
  }
  return NULL;
}

__Z_INLINE const char *uint64_to_str(char *data, int dataLen, uint64_t number) {
  if (dataLen < 2) return "Buffer too small";
  memzero(data, dataLen);
  char *p = data;
  if (number == 0) {
    *(p++) = '0';
  }
  uint64_t tmp;
  while (number != 0) {
    if (p - data >= (dataLen - 1)) {
      return "Buffer too small";
    }
    tmp = number % 10;
    *(p++) = (char)('0' + tmp);
    number /= 10u;
  }
  while (p > data) {
    p--;
    char z = *data;
    *data = *p;
    *p = z;
    data++;
  }
  return NULL;
}

__Z_INLINE uint8_t intstr_to_fpstr_inplace(char *number, size_t number_max_size,
                                           uint8_t decimalPlaces) {
  uint16_t numChars = strnlen(number, number_max_size);
  memzero(number + numChars, number_max_size - numChars);

  if (number_max_size < 1) {
    // No space to do anything
    return 0;
  }

  if (number_max_size <= numChars) {
    // No space to do anything
    return 0;
  }

  if (numChars == 0) {
    // Empty number, make a zero
    snprintf(number, number_max_size, "0");
    numChars = 1;
  }

  // Check all are numbers
  uint16_t firstDigit = numChars;
  for (int i = 0; i < numChars; i++) {
    if (number[i] < '0' || number[i] > '9') {
      snprintf(number, number_max_size, "ERR");
      return 0;
    }
    if (number[i] != '0' && firstDigit > i) {
      firstDigit = i;
    }
  }

  // Trim any incorrect leading zeros
  if (firstDigit == numChars) {
    snprintf(number, number_max_size, "0");
    numChars = 1;
  } else {
    // Trim leading zeros
    memmove(number, number + firstDigit, numChars - firstDigit);
    memzero(number + numChars - firstDigit, firstDigit);
  }

  // If there are no decimal places return
  if (decimalPlaces == 0) {
    return numChars;
  }

  // Now insert decimal point

  //        0123456789012     <-decimal places
  //        abcd              < numChars = 4
  //                 abcd     < shift
  //        000000000abcd     < fill
  //        0.00000000abcd    < add decimal point

  if (numChars < decimalPlaces + 1) {
    // Move to end
    const uint16_t padSize = decimalPlaces - numChars + 1;
    memmove(number + padSize, number, numChars);
    memset(number, '0', padSize);
    numChars = strnlen(number, number_max_size);
  }

  // add decimal point
  const uint16_t pointPosition = numChars - decimalPlaces;
  memmove(number + pointPosition + 1, number + pointPosition,
          decimalPlaces);  // shift content
  number[pointPosition] = '.';

  numChars = strnlen(number, number_max_size);
  return numChars;
}

__Z_INLINE size_t z_strlen(const char *buffer, size_t maxSize) {
  if (buffer == NULL) return 0;
  return strnlen(buffer, maxSize);
}

__Z_INLINE zxerr_t z_str3join(char *buffer, size_t bufferSize,
                              const char *prefix, const char *suffix) {
  size_t messageSize = z_strlen(buffer, bufferSize);
  const size_t prefixSize = z_strlen(prefix, bufferSize);
  const size_t suffixSize = z_strlen(suffix, bufferSize);

  size_t requiredSize =
      1 /* termination */ + messageSize + prefixSize + suffixSize;

  if (bufferSize < requiredSize) {
    snprintf(buffer, bufferSize, "ERR???");
    return zxerr_buffer_too_small;
  }

  if (suffixSize > 0) {
    memmove(buffer + messageSize, suffix, suffixSize);
    buffer[messageSize + suffixSize] = 0;
  }

  // shift and add prefix
  if (prefixSize > 0) {
    memmove(buffer + prefixSize, buffer, messageSize + suffixSize + 1);
    memmove(buffer, prefix, prefixSize);
  }

  return zxerr_ok;
}

__Z_INLINE void pageStringExt(char *outValue, uint16_t outValueLen,
                              const char *inValue, uint16_t inValueLen,
                              uint8_t pageIdx, uint8_t *pageCount) {
  memset(outValue, 0, outValueLen);
  *pageCount = 0;

  outValueLen--;  // leave space for NULL termination
  if (outValueLen == 0) {
    return;
  }

  if (inValueLen == 0) {
    return;
  }

  *pageCount = (uint8_t)(inValueLen / outValueLen);
  const uint16_t lastChunkLen = (inValueLen % outValueLen);

  if (lastChunkLen > 0) {
    (*pageCount)++;
  }

  if (pageIdx < *pageCount) {
    if (lastChunkLen > 0 && pageIdx == *pageCount - 1) {
      memcpy(outValue, inValue + (pageIdx * outValueLen), lastChunkLen);
    } else {
      memcpy(outValue, inValue + (pageIdx * outValueLen), outValueLen);
    }
  }
}

__Z_INLINE void pageString(char *outValue, uint16_t outValueLen,
                           const char *inValue, uint8_t pageIdx,
                           uint8_t *pageCount) {
  pageStringExt(outValue, outValueLen, inValue, (uint16_t)strlen(inValue),
                pageIdx, pageCount);
}

#endif  //__ALGO_DEFS_H__
