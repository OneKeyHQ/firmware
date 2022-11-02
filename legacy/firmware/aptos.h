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

#ifndef __APTOS_H__
#define __APTOS_H__
#include <stdint.h>
#include "bip32.h"
#include "messages-aptos.pb.h"

/**
 * Size of Public key in bytes
 */
#define SIZE_PUBKEY 32

void aptos_get_address_from_public_key(const uint8_t *public_key,
                                       char *address);
void aptos_sign_tx(const AptosSignTx *msg, const HDNode *node,
                   AptosSignedTx *resp);
void aptos_sign_message(const AptosSignMessage *msg, const HDNode *node,
                        AptosMessageSignature *resp);
#endif  // __APTOS_H__
