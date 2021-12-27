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

inline void delay(uint32_t wait) {
  while (--wait > 0) __asm__("nop");
}

static const char *hexdigits = "0123456789ABCDEF";

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

bool check_all_ones(const void *data, int len) {
  if (!data) return false;
  uint8_t result = 0xff;
  const uint8_t *ptr = (const uint8_t *)data;

  for (; len; len--, ptr++) {
    result &= *ptr;
    if (result != 0xff) break;
  }

  return (result == 0xff);
}

bool check_all_zeros(const void *data, int len) {
  if (!data) return false;
  uint8_t result = 0x0;
  const uint8_t *ptr = (const uint8_t *)data;

  for (; len; len--, ptr++) {
    result |= *ptr;
    if (result) break;
  }

  return (result == 0x00);
}
