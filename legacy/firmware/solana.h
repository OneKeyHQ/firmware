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

#ifndef __SOLANA_H__
#define __SOLANA_H__
#include <stdbool.h>
#include <stdint.h>
#include "bip32.h"
#include "messages-solana.pb.h"
#include "sol/parser.h"

/**
 * Size of Public key in bytes
 */
#define SIZE_PUBKEY 32

void solana_get_address_from_public_key(const uint8_t *public_key,
                                        char *address);
void solana_sign_tx(const SolanaSignTx *msg, const HDNode *node,
                    SolanaSignedTx *resp);

bool solana_sanitize_offchain_message(const SolanaSignOffChainMessage *msg);
void solana_sign_offchain_message(const SolanaSignOffChainMessage *msg,
                                  const HDNode *node,
                                  SolanaMessageSignature *resp);
#endif  // __SOLANA_H__
