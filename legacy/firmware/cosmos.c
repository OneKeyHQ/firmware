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
  bool result = false;
  int index = 0, sub_index = 0;
  int y = 0;
  uint8_t key = KEY_NULL;
  uint8_t numItems = 0;
  uint8_t max_index = 0;
  char token_key[64];
  char desc[64];
  char token_val[64];
  uint8_t pageCount = 0, max_page_count;
  const char **tx_msg = format_tx_message("Cosmos");

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

  cosmos_parser_getNumItems(ctx, &numItems);
  max_index = numItems;

refresh_menu:
  layoutSwipe();
  oledClear();
  y = 13;
  cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key), token_val,
                        sizeof(token_val), 0, &pageCount);
  if (memcmp(token_key, "Chain ID", 8) == 0) {
    const CosmosNetworkType *n = cosmosnetworkByChainId(token_val);
    if (n) {
      memcpy(token_key, "Chain Name", 11);
      memcpy(token_val, n->chain_name, strlen(n->chain_name) + 1);
    }
  }
  memset(desc, 0, 64);
  strcat(desc, _(token_key));
  strcat(desc, ":");
  if (index == 0) {
    sub_index = 0;
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, token_val, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  } else if (max_index == index) {
    layoutHeader(_("Sign Transaction"));
    oledDrawStringAdapter(0, 13, tx_msg[1], FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  } else {
    layoutHeader(tx_msg[0]);
    if (1 == pageCount) {
      oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
      if (oledStringWidthAdapter(desc, FONT_STANDARD) > (OLED_WIDTH - 3)) {
        oledDrawStringAdapter(0, y + 20, token_val, FONT_STANDARD);
      } else {
        oledDrawStringAdapter(0, y + 10, token_val, FONT_STANDARD);
      }
    } else {
      int lines = 3;
      if (oledStringWidthAdapter(desc, FONT_STANDARD) > (OLED_WIDTH - 3)) {
        lines--;
      }

      cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key), token_val,
                            20, 0, &max_page_count);  // 20 per line
      max_page_count++;
      if (0 == sub_index) {
        if (3 == lines) {
          oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
          cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key),
                                token_val, 20, 0, &pageCount);
          oledDrawStringAdapter(0, y + 10, token_val, FONT_STANDARD);
          cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key),
                                token_val, 20, 1, &pageCount);
          oledDrawStringAdapter(0, y + 20, token_val, FONT_STANDARD);
          cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key),
                                token_val, 20, 2, &pageCount);
          oledDrawStringAdapter(0, y + 30, token_val, FONT_STANDARD);
        } else {
          oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
          cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key),
                                token_val, 20, 0, &pageCount);
          oledDrawStringAdapter(0, y + 20, token_val, FONT_STANDARD);
          cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key),
                                token_val, 20, 1, &pageCount);
          oledDrawStringAdapter(0, y + 30, token_val, FONT_STANDARD);
        }
      } else {
        for (int i = sub_index; i < sub_index + 4; i++) {
          if (i < max_page_count - 1) {
            cosmos_parser_getItem(ctx, index, token_key, sizeof(token_key),
                                  token_val, 20, i, &pageCount);
            oledDrawStringAdapter(0, y + (i - sub_index) * 10, token_val,
                                  FONT_STANDARD);
          }
        }
      }

      if (sub_index == 0) {
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else if (sub_index == max_page_count - 4) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      }
    }
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (sub_index > 0) {
        sub_index--;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (sub_index < max_page_count - 4) {
        sub_index++;
      }
      goto refresh_menu;
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

bool cosmos_sign_tx(const CosmosSignTx *msg, const HDNode *node,
                    CosmosSignedTx *resp) {
  parser_context_t ctx_parsed_tx;

  uint8_t err = cosmos_parser_parse(&ctx_parsed_tx,
                                    (const unsigned char *)msg->raw_tx.bytes,
                                    msg->raw_tx.size);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "Json parse failed");
    layoutHome();
    return false;
  }

  err = cosmos_parser_validate(&ctx_parsed_tx);
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
