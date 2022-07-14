/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2018 Pavol Rusnak <stick@satoshilabs.com>
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

void fsm_msgTronSignMessage(TronSignMessage *msg) {
  RESP_INIT(TronMessageSignature);

  CHECK_INITIALIZED

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  char signer_str[36];
  uint8_t eth_address[20];
  if (!hdnode_get_ethereum_pubkeyhash(node, eth_address)) return;
  tron_eth_2_trx_address(eth_address, signer_str, sizeof(signer_str));

  layoutVerifyAddress(NULL, signer_str);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  if (is_valid_ascii(msg->message.bytes, msg->message.size)) {
    if (!fsm_layoutSignMessage(msg->message.bytes, msg->message.size)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  } else {
    unsigned char buf[65];
    size_t len = msg->message.size >= 32 ? 32 : msg->message.size;
    data2hex(msg->message.bytes, len, (char *)buf);
    if (!fsm_layoutSignMessage(buf, len * 2)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }

  resp->address.size = strlen(signer_str);
  memcpy(resp->address.bytes, signer_str, resp->address.size);

  tron_message_sign(msg, node, resp);
  layoutHome();
}

void fsm_msgTronGetAddress(TronGetAddress *msg) {
  RESP_INIT(TronAddress);

  CHECK_INITIALIZED

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  uint8_t eth_address[20];
  if (!hdnode_get_ethereum_pubkeyhash(node, eth_address)) return;
  tron_eth_2_trx_address(eth_address, resp->address, sizeof(resp->address));
  resp->has_address = 1;

  if (msg->has_show_display && msg->show_display) {
    char desc[16];
    strlcpy(desc, _("Address:"), sizeof(desc));

    bool qrcode = false;
    for (;;) {
      layoutAddress(resp->address, desc, qrcode, false, msg->address_n,
                    msg->address_n_count, false);
      if (protectButton(ButtonRequestType_ButtonRequest_Address, false)) {
        break;
      }
      qrcode = !qrcode;
    }
  }

  msg_write(MessageType_MessageType_TronAddress, resp);
  layoutHome();
}

void fsm_msgTronSignTx(TronSignTx *msg) {
  RESP_INIT(TronSignedTx);

  CHECK_INITIALIZED

  CHECK_PIN

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  char signer_str[36];
  uint8_t eth_address[20];
  if (!hdnode_get_ethereum_pubkeyhash(node, eth_address)) return;
  tron_eth_2_trx_address(eth_address, signer_str, sizeof(signer_str));

  if (tron_sign_tx(msg, signer_str, node, resp)) {
    msg_write(MessageType_MessageType_TronSignedTx, resp);
  }

  layoutHome();
}
