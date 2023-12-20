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

#include "polkadot.h"
#include "base32.h"
#include "buttons.h"
#include "config.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "polkadot/crypto_helper.h"
#include "polkadot/tx.h"
#include "protect.h"
#include "sha3.h"
#include "util.h"

char polkadot_network[32] = {0};

void polkadot_get_address_from_public_key(const uint8_t *public_key,
                                          char *address, uint16_t addressType) {
  crypto_SS58EncodePubkey((uint8_t *)address, 64, addressType, public_key);
}

static bool layoutPolkadotSign(void) {
  const struct font_desc *font = find_cur_font();
  bool result = false;
  int index = 1;
  int y = 0;
  uint8_t key = KEY_NULL;
  uint8_t numItems = 0;
  uint8_t max_index = 0;
  char token_key[64];
  char token_val[64];
  uint8_t pageCount = 0;

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

  polkadot_tx_getNumItems(&numItems);
  max_index = numItems - 1;
refresh_menu:
  layoutSwipe();
  oledClear();
  y = 0;
  polkadot_tx_getItem(index, token_key, sizeof(token_key), token_val,
                      sizeof(token_val), 0, &pageCount);
  if (index == 1) {
    y += bmp_btn_up.height + 1;
    oledDrawStringAdapter(0, y, _(token_key), FONT_STANDARD);
    y += font->pixel + 5;
    oledDrawStringAdapter(0, y, token_val, FONT_STANDARD);

    // scrollbar
    for (int i = 0; i < OLED_HEIGHT; i += 3) {
      oledDrawPixel(OLED_WIDTH - 1, i);
    }
    for (int i = 0; i < OLED_HEIGHT / (numItems - 1); i++) {
      oledDrawPixel(OLED_WIDTH - 1, i);
      oledDrawPixel(OLED_WIDTH - 2, i);
    }
    oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, OLED_HEIGHT - 8,
                   &bmp_btn_down);
  } else if (max_index == index) {
    oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, 0, &bmp_btn_up);
    y += bmp_btn_up.height + 1;

    oledDrawStringAdapter(0, y, _(token_key), FONT_STANDARD);
    y += font->pixel + 5;
    oledDrawStringAdapter(0, y, token_val, FONT_STANDARD);

    // scrollbar
    for (int i = 0; i < OLED_HEIGHT; i += 3) {
      oledDrawPixel(OLED_WIDTH - 1, i);
    }
    for (int i = (index - 1) * OLED_HEIGHT / (numItems - 1); i < OLED_HEIGHT;
         i++) {
      oledDrawPixel(OLED_WIDTH - 1, i);
      oledDrawPixel(OLED_WIDTH - 2, i);
    }

    layoutButtonNoAdapter(_("CANCEL"), &bmp_btn_cancel);
    layoutScroollbarButtonYesAdapter(_("APPROVE"), &bmp_btn_confirm);
  } else {
    oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, 0, &bmp_btn_up);
    y += bmp_btn_up.height + 1;

    oledDrawStringAdapter(0, y, _(token_key), FONT_STANDARD);
    y += font->pixel + 5;
    oledDrawStringAdapter(0, y, token_val, FONT_STANDARD);

    // scrollbar
    for (int i = 0; i < OLED_HEIGHT; i += 3) {
      oledDrawPixel(OLED_WIDTH - 1, i);
    }
    for (int i = (index - 1) * OLED_HEIGHT / (numItems - 1);
         i < (index)*OLED_HEIGHT / (numItems - 1); i++) {
      oledDrawPixel(OLED_WIDTH - 1, i);
      oledDrawPixel(OLED_WIDTH - 2, i);
    }

    oledDrawBitmap((OLED_WIDTH - bmp_btn_down.width) / 2, OLED_HEIGHT - 8,
                   &bmp_btn_down);
  }
  oledRefresh();

scan_key:
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 1) {
        index--;
        goto refresh_menu;
      } else {
        goto scan_key;
      }
    case KEY_DOWN:
      if (index < max_index) {
        index++;
        goto refresh_menu;
      } else {
        goto scan_key;
      }
    case KEY_CONFIRM:
      if (max_index == index) {
        result = true;
        break;
      }
      goto scan_key;
    case KEY_CANCEL:
      if (max_index == index) {
        result = false;
        break;
      }
      goto scan_key;
    default:
      break;
  }

  return result;
}

static bool get_signer_address(const PolkadotSignTx *msg, const HDNode *node,
                               char *address) {
  uint16_t addressType = 0;
  if (!strncmp(msg->network, "polkadot", 8)) {
    addressType = 0;
  } else if (!strncmp(msg->network, "kusama", 6)) {
    addressType = 2;
  } else if (!strncmp(msg->network, "astar", 5)) {
    addressType = 5;
  } else if (!strncmp(msg->network, "westend", 5)) {
    addressType = 42;
  } else if (!strncmp(msg->network, "joystream", 9)) {
    addressType = 126;
  } else {
    return false;
  }
  polkadot_get_address_from_public_key(node->public_key + 1, address,
                                       addressType);
  return true;
}

bool polkadot_sign_tx(const PolkadotSignTx *msg, const HDNode *node,
                      PolkadotSignedTx *resp) {
  char signer[64] = {0};
  if (!get_signer_address(msg, node, signer)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }

  memcpy(polkadot_network, msg->network, strlen(msg->network) + 1);
  parser_error_t ret = polkadot_tx_parse(msg->raw_tx.bytes, msg->raw_tx.size);
  if (ret == parser_unexpected_callIndex) {
    if (!layoutBlindSign(polkadot_network, signer)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
      layoutHome();
      return false;
    }
  } else if (ret != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "Tx invalid");
    layoutHome();
    return false;
  } else {
    if (!layoutPolkadotSign()) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
      layoutHome();
      return false;
    }
  }

  ed25519_sign(msg->raw_tx.bytes, msg->raw_tx.size, node->private_key,
               resp->signature.bytes);
  resp->signature.size = 64;

  return true;
}