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

#include "solana.h"
#include "buttons.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "sol/message.h"
#include "sol/parser.h"
#include "sol/printer.h"
#include "sol/transaction_summary.h"

void solana_get_address_from_public_key(const uint8_t *public_key,
                                        char *address) {
  encode_base58(public_key, SIZE_PUBKEY, address, BASE58_PUBKEY_LENGTH);
}

void solana_sign_tx(const SolanaSignTx *msg, const HDNode *node,
                    SolanaSignedTx *resp) {
  Parser parser = {msg->raw_tx.bytes, msg->raw_tx.size};
  MessageHeader header;
  if (parse_message_header(&parser, &header)) {
    // This is not a valid Solana message
    fsm_sendFailure(FailureType_Failure_DataError, _("Invalid message"));
    return;
  } else {
    uint8_t signer_pubkey[SIZE_PUBKEY];
    memcpy(signer_pubkey, &node->public_key[1], SIZE_PUBKEY);
    size_t signer_count = header.pubkeys_header.num_required_signatures;
    size_t i;
    for (i = 0; i < signer_count; i++) {
      const Pubkey *pubkey = &header.pubkeys[i];
      if (memcmp(pubkey, signer_pubkey, PUBKEY_SIZE) == 0) {
        break;
      }
    }
    if (i >= signer_count) {
      fsm_sendFailure(FailureType_Failure_DataError, _("Invalid params"));
      return;
    }
  }
  transaction_summary_reset();
  if (process_message_body(parser.buffer, parser.buffer_length, &header)) {
    if (config_getCoinSwitch(COIN_SWITCH_SOLANA)) {
      SummaryItem *item = transaction_summary_primary_item();
      summary_item_set_string(item, "Unrecognized", "format");
      struct SHA3_CTX ctx = {0};
      sha3_256_Init(&ctx);
      sha3_Update(&ctx, parser.buffer, parser.buffer_length);
      Hash UnrecognizedMessageHash;
      keccak_Final(&ctx, (uint8_t *)&UnrecognizedMessageHash);

      item = transaction_summary_general_item();
      summary_item_set_hash(item, "Message Hash", &UnrecognizedMessageHash);
    } else {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Please confirm the BlindSign enabled"));
      return;
    }
  }
  // Set fee-payer if it hasn't already been resolved by
  // the transaction printer
  transaction_summary_set_fee_payer_pubkey(&header.pubkeys[0]);

  enum SummaryItemKind summary_step_kinds[MAX_TRANSACTION_SUMMARY_ITEMS];
  size_t num_summary_steps = 0;
  size_t steps = 0;
  uint8_t steps_list[MAX_TRANSACTION_SUMMARY_ITEMS];
  if (transaction_summary_finalize(summary_step_kinds, &num_summary_steps) ==
      0) {
    for (size_t i = 0; i < num_summary_steps; i++) {
      if (transaction_summary_display_item(i, DisplayFlagAll)) {
        fsm_sendFailure(FailureType_Failure_DataError, _("Parse error"));
        layoutHome();
        return;
      } else {
        char *title = G_transaction_summary_title;
        char *text = G_transaction_summary_text;

        if (strcmp(title, "Create token acct") == 0) {
          continue;
        } else if (strcmp(title, "From mint") == 0) {
          continue;
        } else if (strcmp(title, "From") == 0) {
          continue;
        } else if (strcmp(title, "To") == 0) {
          continue;
        } else if (strcmp(title, "Funded by") == 0) {
          continue;
        } else if (strcmp(title, "Owner") == 0) {
          title = "From";
        } else if (strcmp(title, "Owned by") == 0) {
          title = "To";
        } else if (strcmp(title, "Transfer tokens") == 0 ||
                   strcmp(title, "Transfer") == 0) {
          title = "Amount";
        }

        char desc[64];
        memset(desc, 0, sizeof(desc));
        strcat(desc, _(title));
        strcat(desc, ":");

        steps_list[steps++] = i;

        layoutDialogAdapter_ex(
            NULL, &bmp_btn_cancel, _("REJECT"),
            i < num_summary_steps - 1 ? &bmp_button_forward : &bmp_btn_confirm,
            i < num_summary_steps - 1 ? _("NEXT") : _("APPROVE"), NULL, NULL,
            desc, _(text), NULL, NULL, NULL);
        uint8_t key;
      button_scan:
        key = protectButtonValue(ButtonRequestType_ButtonRequest_ProtectCall,
                                 false, true, 0);
        if (key == KEY_CANCEL) {
          fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
          return;
        } else if (key == KEY_UP) {
          if (steps == 1) {
            goto button_scan;
          } else {
            i = steps_list[steps - 2] - 1;
            steps -= 2;
          }
        } else if (key == KEY_DOWN) {
          if (i == num_summary_steps - 1) {
            goto button_scan;
          }
        }
      }
    }
    ed25519_sign(msg->raw_tx.bytes, msg->raw_tx.size, node->private_key,
                 &node->public_key[1], resp->signature.bytes);
    resp->has_signature = true;
    resp->signature.size = 64;
  } else {
    fsm_sendFailure(FailureType_Failure_DataError, _("Parse error"));
    return;
  }
  msg_write(MessageType_MessageType_SolanaSignedTx, resp);
}
