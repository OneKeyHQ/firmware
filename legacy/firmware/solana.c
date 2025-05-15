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
#include "util.h"

static const uint8_t _SIGN_DOMAIN[] =
    "\xff"
    "solana offchain";
static const uint8_t _SIGNER_COUNT = 1;
#define _APPLICATION_DOMAIN_LENGTH (32)
#define _PREAMBLE_LENGTH                                                       \
  (16 + 1 + _APPLICATION_DOMAIN_LENGTH + 1 + 1 + _SIGNER_COUNT * SIZE_PUBKEY + \
   2)
#define _PREAMBLE_LENGTH_LEDGER (16 + 1 + 1 + 2)
#define _MAX_MESSAGE_LENGTH_WITH_PREAMBLE (1232)
#define _MAX_MESSAGE_LENGTH \
  (_MAX_MESSAGE_LENGTH_WITH_PREAMBLE - _PREAMBLE_LENGTH)
#define _MAX_MESSAGE_LENGTH_LEDGER \
  (_MAX_MESSAGE_LENGTH_WITH_PREAMBLE - _PREAMBLE_LENGTH_LEDGER)

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
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid message");
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
      fsm_sendFailure(FailureType_Failure_DataError, "Invalid params");
      return;
    }
  }
  transaction_summary_reset();
  PrintConfig print_config = {.header = header,
                              .expert_mode = true,
                              .signer_pubkey = &header.pubkeys[0]};
  if (process_message_body(parser.buffer, parser.buffer_length,
                           &print_config)) {
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
        fsm_sendFailure(FailureType_Failure_DataError, "Parse error");
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
                 resp->signature.bytes);
    resp->signature.size = 64;
  } else {
    fsm_sendFailure(FailureType_Failure_DataError, "Parse error");
    return;
  }
  msg_write(MessageType_MessageType_SolanaSignedTx, resp);
}

//=====================Offchain Message Signing=====================
/**
 * Prepare the message for signing
 */
static void prepare_message(const SolanaSignOffChainMessage *msg,
                            const uint8_t *public_key, BufferWriter *writer) {
  write_bytes(_SIGN_DOMAIN, 16, writer);
  write_bytes((uint8_t *)&msg->message_version, 1, writer);
  if (msg->has_application_domain) {
    write_bytes(msg->application_domain.bytes, _APPLICATION_DOMAIN_LENGTH,
                writer);
  }
  write_bytes((uint8_t *)&msg->message_format, 1, writer);
  if (msg->has_application_domain) {
    write_bytes(&_SIGNER_COUNT, 1, writer);
    write_bytes(public_key, SIZE_PUBKEY, writer);
  }
  write_bytes((uint8_t *)&msg->message.size, 2, writer);
  write_bytes(msg->message.bytes, msg->message.size, writer);
}

void solana_sign_offchain_message(const SolanaSignOffChainMessage *msg,
                                  const HDNode *node,
                                  SolanaMessageSignature *resp) {
  BufferWriter writer = {0};
  uint32_t preamble_length =
      msg->has_application_domain ? _PREAMBLE_LENGTH : _PREAMBLE_LENGTH_LEDGER;
  uint8_t message[msg->message.size + preamble_length];
  init_buffer_writer(&writer, message, sizeof(message));
  prepare_message(msg, node->public_key + 1, &writer);
  // sign message
  ed25519_sign(message, sizeof(message), node->private_key,
               resp->signature.bytes);
  resp->signature.size = 64;
}

bool solana_sanitize_offchain_message(const SolanaSignOffChainMessage *msg) {
  if (msg->has_application_domain &&
      msg->application_domain.size != _APPLICATION_DOMAIN_LENGTH) {
    fsm_sendFailure(FailureType_Failure_DataError,
                    "Application domain must be 32 bytes");
    return false;
  }
  if (msg->has_application_domain && msg->message.size > _MAX_MESSAGE_LENGTH) {
    fsm_sendFailure(FailureType_Failure_DataError, "Message is too long");
    return false;
  }
  if (msg->message_version != SolanaOffChainMessageVersion_MESSAGE_VERSION_0) {
    fsm_sendFailure(FailureType_Failure_DataError, "Message version must be 0");
    return false;
  }
  switch (msg->message_format) {
    case SolanaOffChainMessageFormat_V0_RESTRICTED_ASCII:
      if (!is_valid_ascii(msg->message.bytes, msg->message.size)) {
        fsm_sendFailure(
            FailureType_Failure_DataError,
            "Message format 0 must contain only printable characters");
        return false;
      }
      break;
    case SolanaOffChainMessageFormat_V0_LIMITED_UTF8:
      if (!is_valid_utf8(msg->message.bytes, msg->message.size)) {
        fsm_sendFailure(FailureType_Failure_DataError,
                        "Message format 1 must be a valid UTF-8 string");
        return false;
      }
      break;
    default:
      fsm_sendFailure(FailureType_Failure_DataError,
                      "Message format must be 0 or 1");
      return false;
  }
  return true;
}
