/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "util.h"
#include <stdio.h>
#include <string.h>

inline void delay(uint32_t wait) {
  while (--wait > 0) __asm__("nop");
}

static const char *hexdigits = "0123456789ABCDEF";
static const char *hexaddrdigits = "0123456789abcdef";

void uint32hex(uint32_t num, char *str) {
  for (uint32_t i = 0; i < 8; i++) {
    str[i] = hexdigits[(num >> (28 - i * 4)) & 0xF];
  }
}

// converts data to hexa
void data2hex(const uint8_t *data, uint32_t len, char *str) {
  for (uint32_t i = 0; i < len; i++) {
    str[i * 2] = hexdigits[(data[i] >> 4) & 0xF];
    str[i * 2 + 1] = hexdigits[data[i] & 0xF];
  }
  str[len * 2] = 0;
}

// converts data to hexa
void data2hexaddr(const uint8_t *data, uint32_t len, char *str) {
  for (uint32_t i = 0; i < len; i++) {
    str[i * 2] = hexaddrdigits[(data[i] >> 4) & 0xF];
    str[i * 2 + 1] = hexaddrdigits[data[i] & 0xF];
  }
  str[len * 2] = 0;
}

// converts data to hexa
void uint2str(uint32_t num, char *str) {
  uint8_t i = 0, j;
  char temp;

  do {
    str[i++] = hexdigits[num % 10];
    num /= 10;
  } while (num);
  str[i] = 0;

  for (j = 0; j <= (i - 1) / 2; j++) {
    temp = str[j];
    str[j] = str[i - 1 - j];
    str[i - 1 - j] = temp;
  }
}

int hex2data(const char *hexStr, unsigned char *output,
             unsigned int *outputLen) {
  size_t len = strlen(hexStr);
  if (len % 2 != 0) {
    return -1;
  }
  size_t finalLen = len / 2;
  *outputLen = finalLen;
  for (size_t inIdx = 0, outIdx = 0; outIdx < finalLen; inIdx += 2, outIdx++) {
    if ((hexStr[inIdx] - 48) <= 9 && (hexStr[inIdx + 1] - 48) <= 9) {
      goto convert;
    } else {
      if (((hexStr[inIdx] - 65) <= 5 && (hexStr[inIdx + 1] - 65) <= 5) ||
          ((hexStr[inIdx] - 97) <= 5 && (hexStr[inIdx + 1] - 97) <= 5)) {
        goto convert;
      } else {
        *outputLen = 0;
        return -1;
      }
    }
  convert:
    output[outIdx] =
        (hexStr[inIdx] % 32 + 9) % 25 * 16 + (hexStr[inIdx + 1] % 32 + 9) % 25;
  }
  output[finalLen] = '\0';
  return 0;
}

bool is_valid_ascii(const uint8_t *data, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    if (data[i] < ' ' || data[i] > '~') {
      return false;
    }
  }
  return true;
}

/**
 * Checks if data is in UTF-8 format.
 * Adapted from: https://www.cl.cam.ac.uk/~mgk25/ucs/utf8_check.c
 */
bool is_valid_utf8(const uint8_t *data, size_t length) {
  if (!data) {
    return false;
  }
  size_t i = 0;
  while (i < length) {
    if (data[i] < 0x80) {
      /* 0xxxxxxx */
      ++i;
    } else if ((data[i] & 0xe0) == 0xc0) {
      /* 110XXXXx 10xxxxxx */
      if (i + 1 >= length || (data[i + 1] & 0xc0) != 0x80 ||
          (data[i] & 0xfe) == 0xc0) /* overlong? */ {
        return false;
      } else {
        i += 2;
      }
    } else if ((data[i] & 0xf0) == 0xe0) {
      /* 1110XXXX 10Xxxxxx 10xxxxxx */
      if (i + 2 >= length || (data[i + 1] & 0xc0) != 0x80 ||
          (data[i + 2] & 0xc0) != 0x80 ||
          (data[i] == 0xe0 && (data[i + 1] & 0xe0) == 0x80) || /* overlong? */
          (data[i] == 0xed && (data[i + 1] & 0xe0) == 0xa0) || /* surrogate? */
          (data[i] == 0xef && data[i + 1] == 0xbf &&
           (data[i + 2] & 0xfe) == 0xbe)) /* U+FFFE or U+FFFF? */ {
        return false;
      } else {
        i += 3;
      }
    } else if ((data[i] & 0xf8) == 0xf0) {
      /* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
      if (i + 3 >= length || (data[i + 1] & 0xc0) != 0x80 ||
          (data[i + 2] & 0xc0) != 0x80 || (data[i + 3] & 0xc0) != 0x80 ||
          (data[i] == 0xf0 && (data[i + 1] & 0xf0) == 0x80) || /* overlong? */
          (data[i] == 0xf4 && data[i + 1] > 0x8f) ||
          data[i] > 0xf4) /* > U+10FFFF? */ {
        return false;
      } else {
        i += 4;
      }
    } else {
      return false;
    }
  }
  return true;
}

bool is_printable(const uint8_t *data, uint32_t length) {
  return is_valid_ascii(data, length) || is_valid_utf8(data, length);
}

void init_buffer_writer(BufferWriter *writer, uint8_t *buffer, size_t length) {
  writer->buffer = buffer;
  writer->length = length;
  writer->position = 0;
}

int write_bytes(const uint8_t *src, size_t count, BufferWriter *writer) {
  if (writer->buffer == NULL && writer->length == 0) {
    writer->position += count;
    return 1;
  }
  if (writer->position + count > writer->length) {
    return 0;
  }
  memcpy(writer->buffer + writer->position, src, count);
  writer->position += count;
  return 1;
}
