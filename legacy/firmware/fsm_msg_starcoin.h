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

void fsm_msgStarcoinGetAddress(const StarcoinGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(StarcoinAddress);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  resp->has_address = true;
  resp->address[0] = '0';
  resp->address[1] = 'x';
  starcoin_get_address_from_public_key(node->public_key + 1, resp->address + 2);

  if (msg->has_show_display && msg->show_display) {
    char desc[20] = {0};
    snprintf(desc, 20, "%s %s", "Starcoin", _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_StarcoinAddress, resp);
  layoutHome();
}

void fsm_msgStarcoinGetPublicKey(const StarcoinGetPublicKey *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(StarcoinPublicKey);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  resp->public_key.size = 32;

  if (msg->has_show_display && msg->show_display) {
    layoutPublicKey(&node->public_key[1]);
    if (!protectButton(ButtonRequestType_ButtonRequest_PublicKey, true)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }

  memcpy(&resp->public_key.bytes, &node->public_key[1], resp->public_key.size);

  msg_write(MessageType_MessageType_StarcoinPublicKey, resp);
  layoutHome();
}

void fsm_msgStarcoinSignTx(const StarcoinSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(StarcoinSignedTx);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  if (!starcoin_sign_tx(msg, node, resp)) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Signing failed"));
    layoutHome();
    return;
  }
  msg_write(MessageType_MessageType_StarcoinSignedTx, resp);
  layoutHome();
}

void fsm_msgStarcoinSignMessage(const StarcoinSignMessage *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(StarcoinMessageSignature);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  char address[MAX_STARCOIN_ADDRESS_SIZE] = {'0', 'x'};
  hdnode_fill_public_key(node);
  starcoin_get_address_from_public_key(node->public_key + 1, address + 2);
  if (!fsm_layoutSignMessage("Starcoin", address, msg->message.bytes,
                             msg->message.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  hdnode_fill_public_key(node);
  if (starcoin_sign_message(node, msg, resp)) {
    msg_write(MessageType_MessageType_StarcoinMessageSignature, resp);
    layoutHome();
  }
}

void fsm_msgStarcoinVerifyMessage(const StarcoinVerifyMessage *msg) {
  if (starcoin_verify_message(msg)) {
    char address[MAX_STARCOIN_ADDRESS_SIZE] = {'0', 'x'};
    starcoin_get_address_from_public_key(msg->public_key.bytes, address + 2);
    layoutVerifyAddress(NULL, address);

    if (!fsm_layoutVerifyMessage("Starcoin", address, msg->message.bytes,
                                 msg->message.size)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }

    fsm_sendSuccess(_("Message verified"));
  } else {
    fsm_sendFailure(FailureType_Failure_DataError, _("Invalid signature"));
  }

  layoutHome();
}
