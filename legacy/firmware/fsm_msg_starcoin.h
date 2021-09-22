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

  const HDNode *node =
      starcoin_deriveNode(msg->address_n, msg->address_n_count);

  if (!node) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive private key"));
    return;
  }

  resp->has_address = true;
  resp->address[0] = '0';
  resp->address[1] = 'x';
  starcoin_get_address_from_public_key(node->public_key + 1, resp->address + 2);

  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutAddress(resp->address, _("Address:"), false, 0,
                           msg->address_n, msg->address_n_count, true, NULL, 0,
                           NULL)) {
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

  const HDNode *node =
      starcoin_deriveNode(msg->address_n, msg->address_n_count);
  if (!node) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive private key"));
    return;
  }

  resp->has_public_key = true;
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

  const HDNode *node =
      starcoin_deriveNode(msg->address_n, msg->address_n_count);
  if (!node) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive private key"));
    return;
  }

  starcoin_sign_tx(msg, node, resp);
  if (!resp->has_signature) {
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

  const HDNode *node =
      starcoin_deriveNode(msg->address_n, msg->address_n_count);
  if (!node) return;

  starcoin_sign_message(node, msg, resp);

  msg_write(MessageType_MessageType_StarcoinMessageSignature, resp);

  layoutHome();
}

void fsm_msgStarcoinVerifyMessage(const StarcoinVerifyMessage *msg) {
  if (starcoin_verify_message(msg)) {
    char address[MAX_STARCOIN_ADDRESS_SIZE] = {'0', 'x'};
    starcoin_get_address_from_public_key(msg->public_key.bytes, address + 2);
    layoutVerifyAddress(NULL, address);

    if (!protectButton(ButtonRequestType_ButtonRequest_Other, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
    layoutVerifyMessage(msg->message.bytes, msg->message.size);

    if (!protectButton(ButtonRequestType_ButtonRequest_Other, false)) {
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
