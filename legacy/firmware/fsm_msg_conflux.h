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

void fsm_msgConfluxSignTx(ConfluxSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  conflux_signing_init(msg, node);
}

void fsm_msgConfluxTxAck(const ConfluxTxAck *msg) {
  conflux_signing_txack(msg);
}

void fsm_msgConfluxGetAddress(const ConfluxGetAddress *msg) {
  RESP_INIT(ConfluxAddress);

  CHECK_INITIALIZED

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  uint8_t pubkeyhash[20];

  if (!hdnode_get_ethereum_pubkeyhash(node, pubkeyhash)) return;

  resp->has_address = true;

  int failed = get_base32_encode_address(pubkeyhash, resp->address,
                                         sizeof(resp->address), msg->chain_id);
  if (failed) {
    fsm_sendFailure(FailureType_Failure_DataError, "Get address failed");
    return;
  }
  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutAddress(resp->address, _("Address:"), true, 0,
                           msg->address_n, msg->address_n_count, false, NULL, 0,
                           NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_ConfluxAddress, resp);
  layoutHome();
}

void fsm_msgConfluxSignMessage(const ConfluxSignMessage *msg) {
  RESP_INIT(ConfluxMessageSignature);

  CHECK_INITIALIZED

  layoutSignMessage(msg->message.bytes, msg->message.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  conflux_message_sign(msg, node, resp);
  layoutHome();
}
void fsm_msgConfluxSignMessageCIP23(const ConfluxSignMessageCIP23 *msg) {
  RESP_INIT(ConfluxMessageSignature);

  CHECK_INITIALIZED

  if (msg->domain_hash.size != 32 || msg->message_hash.size != 32) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "data length error");
    return;
  }

  if (!config_getCoinSwitch(COIN_SWITCH_CFX_CIP23)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("CIP23 blind sign is disabled"));
    return;
  }

  layoutSignMessage_ex("DomainSeparator Hash?", msg->domain_hash.bytes,
                       msg->domain_hash.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  layoutSignMessage_ex("Messages Hash?", msg->message_hash.bytes,
                       msg->message_hash.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) {
    fsm_sendFailure(FailureType_Failure_DataError, NULL);
    return;
  }

  conflux_message_sign_cip23(msg, node, resp);
  layoutHome();
}
