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

#include "near.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "sha3.h"
#include "util.h"

extern bool layoutRequireConfirmBlidSign(char *address);

void near_get_address_from_public_key(const uint8_t *public_key,
                                      char *address) {
  const char *hex = "0123456789abcdef";
  for (uint32_t i = 0; i < 32; i++) {
    address[i * 2] = hex[(public_key[i] >> 4) & 0xF];
    address[i * 2 + 1] = hex[public_key[i] & 0xF];
  }
  address[32 * 2] = 0;
}

bool near_sign_tx(const NearSignTx *msg, const HDNode *node,
                  NearSignedTx *resp) {
  char address[67] = {'0', 'x'};
  near_get_address_from_public_key(node->public_key + 1, address + 2);
  if (!layoutBlidSign(address)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }

  // hash the tx
  uint8_t hash[32];
  SHA256_CTX ctx;
  sha256_Init(&ctx);
  sha256_Update(&ctx, msg->raw_tx.bytes, msg->raw_tx.size);
  sha256_Final(&ctx, hash);

  ed25519_sign(hash, 32, node->private_key, &node->public_key[1],
               resp->signature.bytes);

  resp->signature.size = 64;
  return true;
}
