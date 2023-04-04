/*
 * This file is part of the OneKey project, https://onekey.so/
 *
 * Copyright (C) 2022 OneKey Team <core@onekey.so>
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

#include "algorand.h"
#include "algo/tx.h"
#include "base32.h"
#include "buttons.h"
#include "config.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "sha3.h"
#include "sha512.h"
#include "util.h"

void algorand_get_address_from_public_key(const uint8_t *public_key,
                                          char *address) {
  uint8_t messageDigest[64];
  SHA512_256(public_key, 32, messageDigest);
  uint8_t checksummed[36];
  memmove(&checksummed[0], public_key, 32);
  memmove(&checksummed[32], &messageDigest[28], 4);

  base32_encode(checksummed, 36, address, 64, BASE32_ALPHABET_RFC4648);
}

static bool layoutAlgoSign(void) {
  bool result = false;
  int index = 0;
  int y = 0;
  uint8_t key = KEY_NULL;
  uint8_t numItems = 0;
  uint8_t max_index = 0;
  char token_key[64];
  char token_val[64];
  uint8_t pageCount = 0;
  char desc[64];
  const char **tx_msg = format_tx_message("Algorand");

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

  tx_getNumItems(&numItems);
  max_index = numItems;

refresh_menu:
  layoutSwipe();
  oledClear();
  y = 13;
  tx_getItem(index, token_key, sizeof(token_key), token_val, sizeof(token_val),
             0, &pageCount);
  memset(desc, 0, 64);
  strcat(desc, _(token_key));
  strcat(desc, ":");
  if (index == 0) {
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, token_val, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (max_index == index) {
    layoutHeader(_("Sign Transaction"));
    oledDrawStringAdapter(0, y, tx_msg[1], FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  } else {
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, token_val, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

scan_key:
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      goto scan_key;
    case KEY_DOWN:
      goto scan_key;
    case KEY_CONFIRM:
      if (max_index == index) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      goto refresh_menu;
    case KEY_CANCEL:
      if (0 == index || max_index == index) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    default:
      break;
  }

  return result;
}

bool algorand_sign_tx(const AlgorandSignTx *msg, const HDNode *node,
                      AlgorandSignedTx *resp) {
  const char *error_msg =
      tx_parse(msg->raw_tx.bytes + 2,  // 'TX' is prepended to input buffer
               msg->raw_tx.size);
  if (error_msg) {
    fsm_sendFailure(FailureType_Failure_DataError, "Tx invalid");
    layoutHome();
    return false;
  }

  if (!layoutAlgoSign()) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }
  ed25519_sign(msg->raw_tx.bytes, msg->raw_tx.size, node->private_key,
               resp->signature.bytes);
  resp->signature.size = 64;

  return true;
}