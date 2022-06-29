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

#include "starcoin.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "sha3.h"

// tx prefix: sha3_256("STARCOIN::RawUserTransaction") ->
// result = "f7abb31497be2d952de2e1c64e2ce3edae7c4d9f5a522386a38af0c76457301e";
const uint8_t STC_RAW_USER_TX_PREFIX_HASH[32] = {
    247, 171, 179, 20,  151, 190, 45,  149, 45,  226, 225,
    198, 78,  44,  227, 237, 174, 124, 77,  159, 90,  82,
    35,  134, 163, 138, 240, 199, 100, 87,  48,  30};

// msg_sign_prefix: sha3_256("STARCOIN::SigningMessage") ->
// result = "1e350a8f0e461f0f6d89beaabf501711583b40deaeb045b0ccb44dd1e071733e";
const uint8_t STC_MSG_SIGN_PREFIX_HASH[32] = {
    30, 53, 10, 143, 14,  70,  31, 15,  109, 137, 190, 170, 191, 80,  23,  17,
    88, 59, 64, 222, 174, 176, 69, 176, 204, 180, 77,  209, 224, 113, 115, 62};

void starcoin_get_address_from_public_key(const uint8_t *public_key,
                                          char *address) {
  uint8_t buf[32] = {0};
  struct SHA3_CTX ctx = {0};

  sha3_256_Init(&ctx);
  sha3_Update(&ctx, public_key, 32);
  sha3_Update(&ctx, (const uint8_t *)"\x00", 1);
  sha3_Final(&ctx, buf);

  const char *hex = "0123456789abcdef";
  for (int i = 0; i < 16; i++) {
    address[i * 2] = hex[(buf[i + 16] >> 4) & 0xF];
    address[i * 2 + 1] = hex[buf[i + 16] & 0xF];
  }
}

bool starcoin_sign_tx(const StarcoinSignTx *msg, const HDNode *node,
                      StarcoinSignedTx *resp) {
  char address[MAX_STARCOIN_ADDRESS_SIZE] = {'0', 'x'};
  starcoin_get_address_from_public_key(node->public_key + 1, address + 2);
  layoutRequireConfirmSignTx(address);
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }

  layoutRequireConfirmPayload();
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }

  uint8_t buf[sizeof(StarcoinSignTx_raw_tx_t) + 32] = {0};

  memcpy(buf, STC_RAW_USER_TX_PREFIX_HASH, 32);
  memcpy(buf + 32, msg->raw_tx.bytes, msg->raw_tx.size);

  ed25519_sign(buf, msg->raw_tx.size + 32, node->private_key,
               &node->public_key[1], resp->signature.bytes);

  memcpy(resp->public_key.bytes, &node->public_key[1], 32);
  resp->signature.size = 64;
  resp->public_key.size = 32;
  return true;
}

static void unsigned_int_to_leb128(uint32_t val, uint8_t *s) {
  uint8_t c;
  int more;

  do {
    c = val & 0x7f;
    val >>= 7;
    more = val != 0;
    *s++ = c | (more ? 0x80 : 0);
  } while (more);
}

bool starcoin_sign_message(const HDNode *node, const StarcoinSignMessage *msg,
                           StarcoinMessageSignature *resp) {
  if (!fsm_layoutSignMessage(msg->message.bytes, msg->message.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return false;
  }

  layoutProgressSwipe(_("Signing"), 0);

  uint8_t buf[sizeof(StarcoinSignMessage_message_t) + 32 + 2] = {0};
  uint8_t msg_length_data[8] = {0};
  unsigned_int_to_leb128(msg->message.size, msg_length_data);
  uint8_t msg_header_size = strlen((const char *)msg_length_data);

  memcpy(buf, STC_MSG_SIGN_PREFIX_HASH, 32);
  memcpy(buf + 32, (uint8_t *)&msg_length_data, msg_header_size);
  memcpy(buf + 32 + msg_header_size, msg->message.bytes, msg->message.size);
  ed25519_sign(buf, 32 + msg_header_size + msg->message.size, node->private_key,
               &node->public_key[1], resp->signature.bytes);

  memcpy(resp->public_key.bytes, &node->public_key[1], 32);

  resp->signature.size = 64;
  resp->public_key.size = 32;
  return true;
}

bool starcoin_verify_message(const StarcoinVerifyMessage *msg) {
  uint8_t buf[sizeof(StarcoinVerifyMessage_message_t) + 32 + 2] = {0};
  uint8_t msg_length_data[8] = {0};
  unsigned_int_to_leb128(msg->message.size, msg_length_data);
  uint8_t msg_header_size = strlen((const char *)msg_length_data);

  memcpy(buf, STC_MSG_SIGN_PREFIX_HASH, 32);
  memcpy(buf + 32, (uint8_t *)&msg_length_data, msg_header_size);
  memcpy(buf + 32 + msg_header_size, msg->message.bytes, msg->message.size);
  return 0 == ed25519_sign_open(buf, 32 + msg_header_size + msg->message.size,
                                msg->public_key.bytes, msg->signature.bytes);
}

// Layouts
void layoutRequireConfirmSignTx(char *address) {
  const char **str =
      split_message((const uint8_t *)address, strlen(address), 16);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Confirm signning"), NULL, _("from:"), str[0], str[1],
                    NULL);
}

void layoutRequireConfirmPayload() {
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("APPROVE"), NULL,
                    _("Confirm signning"), NULL, _("transaction:"),
                    _("transaction cannot be decoded"), NULL, NULL);
}
