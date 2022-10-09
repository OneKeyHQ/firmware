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

#include "cosmos.h"
#include "buttons.h"
#include "config.h"
#include "cosmos/json_parser.h"
#include "cosmos/parser.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "jsmn.h"
#include "layout2.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "ripemd160.h"
#include "secp256k1.h"
#include "segwit_addr.h"
#include "sha3.h"
#include "util.h"

extern int ethereum_is_canonic(uint8_t v, uint8_t signature[64]);
extern int convert_bits(uint8_t *out, size_t *outlen, int outbits,
                        const uint8_t *in, size_t inlen, int inbits, int pad);

int cosmos_get_address(char *address, const uint8_t *public_key,
                       const char *hrp) {
  uint8_t hash[32];
  uint8_t ripemd[RIPEMD160_DIGEST_LENGTH] = {0};
  uint8_t data[65] = {0};
  size_t datalen = 0;

  // hash the public_key
  SHA256_CTX ctx;
  sha256_Init(&ctx);
  sha256_Update(&ctx, public_key, 33);
  sha256_Final(&ctx, hash);

  // ripemd160
  ripemd160(hash, 32, ripemd);

  // bech32_encode
  bech32_encoding enc = BECH32_ENCODING_BECH32;
  convert_bits(data, &datalen, 5, ripemd, RIPEMD160_DIGEST_LENGTH, 8, 1);

  return bech32_encode(address, hrp, data, datalen, enc);
}

bool layoutCosmosSign(parser_context_t *ctx) {
  const struct font_desc *font = find_cur_font();
  bool result = false;
  int index = 0;
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

  parser_getNumItems(ctx, &numItems);
  max_index = numItems - 1;

refresh_menu:
  layoutSwipe();
  oledClear();
  y = 0;
  parser_getItem(ctx, index, token_key, sizeof(token_key), token_val,
                 sizeof(token_val), 0, &pageCount);
  if (index == 0) {
    y += bmp_btn_up.height + 1;
    oledDrawStringAdapter(0, y, _(token_key), FONT_STANDARD);
    y += font->pixel + 5;
    oledDrawStringAdapter(0, y, token_val, FONT_STANDARD);

    // scrollbar
    for (int i = 0; i < OLED_HEIGHT; i += 3) {
      oledDrawPixel(OLED_WIDTH - 1, i);
    }
    for (int i = 0; i < OLED_HEIGHT / numItems; i++) {
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
    for (int i = index * OLED_HEIGHT / numItems; i < OLED_HEIGHT; i++) {
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
    for (int i = index * OLED_HEIGHT / numItems;
         i < (index + 1) * OLED_HEIGHT / numItems; i++) {
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
      if (index > 0) {
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

bool cosmos_sign_tx(const CosmosSignTx *msg, const HDNode *node,
                    CosmosSignedTx *resp) {
  parser_context_t ctx_parsed_tx;

  uint8_t err =
      parser_parse(&ctx_parsed_tx, (const unsigned char *)msg->raw_tx.bytes,
                   msg->raw_tx.size);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "Json parse failed");
    layoutHome();
    return false;
  }

  err = parser_validate(&ctx_parsed_tx);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "Tx invalid");
    layoutHome();
    return false;
  }

  if (!layoutCosmosSign(&ctx_parsed_tx)) {
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

  // sign tx hash
  uint8_t v;
  if (ecdsa_sign_digest(&secp256k1, node->private_key, hash,
                        resp->signature.bytes, &v, ethereum_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    layoutHome();
    return false;
  }

  resp->signature.size = 64;

  return true;
}
