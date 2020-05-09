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

#include <string.h>

#include "common.h"
#include "layout2.h"
#include "oled.h"
#include "pinmatrix.h"
#include "rng.h"

static char pinmatrix_perm[10] = "XXXXXXXXX";

void pinmatrix_draw(const char *text) {
  layoutSwipe();
  const int w = 12, h = 12, pad = 1;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      // use (2 - j) instead of j to achieve 789456123 layout
      int k = pinmatrix_perm[i + (2 - j) * 3] - '0';
      if (text) {
        oledDrawStringCenter_zh(OLED_WIDTH / 2, 8, text);
      }
      oledDrawNumber_zh((OLED_WIDTH - 3 * w - 2 * pad) / 2 + i * (w + pad),
                        OLED_HEIGHT - 3 * h - 2 * pad + j * (h + pad), k + '0');
      oledRefresh();
    }
  }
  oledRefresh();
}

void pinmatrix_start(const char *text) {
  for (int i = 0; i < 9; i++) {
    pinmatrix_perm[i] = '1' + i;
  }
  pinmatrix_perm[9] = 0;
  random_permute(pinmatrix_perm, 9);
  pinmatrix_draw(text);
}

secbool pinmatrix_done(bool clear, char *pin) {
  int i = 0, k = 0;
  secbool ret = sectrue;
  while (pin && pin[i]) {
    k = pin[i] - '1';
    if (k >= 0 && k <= 8) {
      pin[i] = pinmatrix_perm[k];
    } else {
      pin[i] = 'X';
      ret = secfalse;
    }
    i++;
  }
  if (clear) memset(pinmatrix_perm, 'X', sizeof(pinmatrix_perm) - 1);
  return ret;
}

#if DEBUG_LINK

const char *pinmatrix_get(void) { return pinmatrix_perm; }

#endif
