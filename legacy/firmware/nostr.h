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

#ifndef __NOSTR_H__
#define __NOSTR_H__
#include <stdint.h>
#include "bip32.h"
#include "messages-nostr.pb.h"

int nostr_get_pubkey(char *address, const uint8_t *public_key);
bool nostr_sign_event(const NostrSignEvent *msg, const HDNode *node,
                      NostrSignedEvent *resp);
bool nostr_encrypt_message(NostrEncryptMessage *msg, const HDNode *node,
                           NostrEncryptedMessage *resp);
bool nostr_decrypt_message(NostrDecryptMessage *msg, const HDNode *node,
                           NostrDecryptedMessage *resp);
bool nostr_sign_schnorr(const NostrSignSchnorr *msg, const HDNode *node,
                        NostrSignedSchnorr *resp);

#endif  // __NOSTR_H__
