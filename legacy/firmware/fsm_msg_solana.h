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
#undef COIN_TYPE
#define COIN_TYPE 501
void fsm_msgSolanaGetAddress(const SolanaGetAddress *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, ED25519_NAME, true),
              "Invalid path");
  CHECK_PIN

  RESP_INIT(SolanaAddress);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  solana_get_address_from_public_key(&node->public_key[1], resp->address);

  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutAddress(resp->address, _("Address:"), false, 0,
                           msg->address_n, msg->address_n_count, false, NULL, 0,
                           0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_SolanaAddress, resp);

  layoutHome();
}
void fsm_msgSolanaSignTx(const SolanaSignTx *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, ED25519_NAME, true),
              "Invalid path");
  CHECK_PIN

  RESP_INIT(SolanaSignedTx);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  solana_sign_tx(msg, node, resp);

  layoutHome();
}

void fsm_msgSolanaSignOffChainMessage(const SolanaSignOffChainMessage *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, ED25519_NAME, true),
              "Invalid path");
  if (!solana_sanitize_offchain_message(msg)) {
    layoutHome();
    return;
  }
  CHECK_PIN

  RESP_INIT(SolanaMessageSignature);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  // address
  char address[45] = {0};
  solana_get_address_from_public_key(node->public_key + 1, address);
  layoutVerifyAddress(NULL, address);
  if (!protectButton(ButtonRequestType_ButtonRequest_Other, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "user cancelled");
    layoutHome();
    return;
  }
  if (!fsm_layoutSignMessage(msg->message.bytes, msg->message.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "user cancelled");
    layoutHome();
    return;
  }
  solana_sign_offchain_message(msg, node, resp);
  resp->has_public_key = true;
  resp->public_key.size = 32;
  memcpy(resp->public_key.bytes, node->public_key + 1, 32);
  msg_write(MessageType_MessageType_SolanaMessageSignature, resp);
  layoutHome();
}

void fsm_msgSolanaSignUnsafeMessage(const SolanaSignUnsafeMessage *msg) {
  CHECK_INITIALIZED

  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, ED25519_NAME, true),
              "Invalid path");
  Parser parser = {msg->message.bytes, msg->message.size};
  MessageHeader header;
  if (parse_message_header(&parser, &header) == 0) {
    fsm_sendFailure(FailureType_Failure_DataError,
                    "Valid transaction message format is not allowed");
    layoutHome();
    return;
  }
  CHECK_PIN

  RESP_INIT(SolanaMessageSignature);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;
  hdnode_fill_public_key(node);
  layoutDialogSwipe(
      &bmp_icon_warning, _("Abort"), _("Continue"), NULL,
      _("Risk of phishing. Proceed only if you trust the source."), NULL, NULL,
      NULL, NULL, NULL);
  uint8_t key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "user cancelled");
    layoutHome();
    return;
  }
  // address
  char address[45] = {0};
  solana_get_address_from_public_key(node->public_key + 1, address);
  layoutVerifyAddress(NULL, address);
  if (!protectButton(ButtonRequestType_ButtonRequest_Other, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "user cancelled");
    layoutHome();
    return;
  }
  if (!fsm_layoutSignMessage(msg->message.bytes, msg->message.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "user cancelled");
    layoutHome();
    return;
  }
  ed25519_sign(msg->message.bytes, msg->message.size, node->private_key,
               resp->signature.bytes);
  resp->signature.size = 64;
  resp->has_public_key = false;
  msg_write(MessageType_MessageType_SolanaMessageSignature, resp);
  layoutHome();
}
