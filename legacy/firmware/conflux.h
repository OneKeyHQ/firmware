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

#ifndef __CONFLUX_H__
#define __CONFLUX_H__

#include <stdbool.h>
#include <stdint.h>
#include "bip32.h"
#include "messages-conflux.pb.h"

void conflux_signing_init(ConfluxSignTx *msg, const HDNode *node);
void conflux_signing_abort(void);
void conflux_signing_txack(const ConfluxTxAck *msg);

void conflux_message_sign(const ConfluxSignMessage *msg, const HDNode *node,
                          ConfluxMessageSignature *resp);
bool conflux_parse(const char *address, uint8_t pubkeyhash[20]);
void get_ethereum_format_address(uint8_t pubkeyhash[20], char *address);
int get_base32_encode_address(uint8_t *in, char *out, size_t out_len,
                              uint32_t chain_id, bool is_sign);
void conflux_message_sign_cip23(const ConfluxSignMessageCIP23 *msg,
                                const HDNode *node,
                                ConfluxMessageSignature *resp);
#endif
