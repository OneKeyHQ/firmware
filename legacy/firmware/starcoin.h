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

#ifndef __STARCOIN_H__
#define __STARCOIN_H__

#include <stdbool.h>
#include "bip32.h"
#include "messages-starcoin.pb.h"

#define MAX_STARCOIN_ADDRESS_SIZE 34

const HDNode *starcoin_deriveNode(const uint32_t *address_n,
                                  size_t address_n_count);
void starcoin_sign_tx(const StarcoinSignTx *msg, const HDNode *node,
                      StarcoinSignedTx *resp);
void starcoin_sign_message(const HDNode *node, const StarcoinSignMessage *msg,
                           StarcoinMessageSignature *resp);
bool starcoin_verify_message(const StarcoinVerifyMessage *msg);

// Helpers
void starcoin_get_address_from_public_key(const uint8_t *public_key,
                                          char *address);

// Layout
void layoutRequireConfirmSignTx(char *address);
#endif  // __STARCOIN_H__
