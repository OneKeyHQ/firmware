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

#ifndef __TRON_UI_H__
#define __TRON_UI_H__

#include <stdint.h>
#include "tron_tokens.h"

void layoutTronConfirmTx(const char *to_str, const uint64_t amount,
                         const uint8_t *value_bytes, ConstTronTokenPtr token);
void layoutTronFee(const uint64_t amount, const uint8_t *value_bytes,
                   ConstTronTokenPtr token, const uint64_t fee);

#endif  // __TRON_UI_H__
