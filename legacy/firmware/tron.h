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

#ifndef __TRON_H__
#define __TRON_H__

#include <stdbool.h>
#include <stdint.h>
#include "bip32.h"
#include "messages-tron.pb.h"
#include "tron_tokens.h"

void tron_message_sign(TronSignMessage *msg, const HDNode *node,
                       TronMessageSignature *resp);
int tron_eth_2_trx_address(const uint8_t *eth_address, char *str, int strsize);
bool tron_sign_tx(TronSignTx *msg, const char *owner_address,
                  const HDNode *node, TronSignedTx *resp);

void tron_format_amount(const uint64_t amount, char *buf, int buflen);
void tron_format_token_amount(const bignum256 *amnt, ConstTronTokenPtr token,
                              char *buf, int buflen);

#endif  // __TRON_H__
