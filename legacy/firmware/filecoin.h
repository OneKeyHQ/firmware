/*
 * This file is part of the OneKey project, https://onekey.so/
 *
 * Copyright (C) 2021 OneKey Team <core@onekey.so>
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

#ifndef __FILECOIN_H__
#define __FILECOIN_H__
#include <stdint.h>
#include "bip32.h"
#include "messages-filecoin.pb.h"

bool get_filecoin_addr(uint8_t *pubkey, FilecoinAddress *address);
bool filecoin_sign_tx(const FilecoinSignTx *msg, const HDNode *node,
                      FilecoinSignedTx *resp);

#endif  // __FILECOIN_H__
