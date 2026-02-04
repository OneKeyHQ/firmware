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
#undef COIN_TYPE
#define COIN_TYPE 195
void fsm_msgTronSignMessage(TronSignMessage *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, SECP256K1_NAME, true),
              "Invalid path");
  CHECK_PIN
  RESP_INIT(TronMessageSignature);

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  char signer_str[36];
  uint8_t eth_address[20];
  if (!hdnode_get_ethereum_pubkeyhash(node, eth_address)) return;
  tron_eth_2_trx_address(eth_address, signer_str, sizeof(signer_str));

  if (!fsm_layoutSignMessage("Tron", signer_str, msg->message.bytes,
                             msg->message.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  resp->address.size = strlen(signer_str);
  memcpy(resp->address.bytes, signer_str, resp->address.size);

  tron_message_sign(msg, node, resp);
  layoutHome();
}

void fsm_msgTronGetAddress(TronGetAddress *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, SECP256K1_NAME, true),
              "Invalid path");
  CHECK_PIN
  RESP_INIT(TronAddress);

  const HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                          msg->address_n_count, NULL);
  if (!node) return;

  uint8_t eth_address[20];
  if (!hdnode_get_ethereum_pubkeyhash(node, eth_address)) return;
  tron_eth_2_trx_address(eth_address, resp->address, sizeof(resp->address));
  resp->has_address = 1;

  if (msg->has_show_display && msg->show_display) {
    char desc[16] = {0};
    strcat(desc, "Tron ");
    strcat(desc, _("Address"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_TronAddress, resp);
  layoutHome();
}

void fsm_msgTronSignTx(TronSignTx *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, SECP256K1_NAME, true),
              "Invalid path");
  CHECK_PIN
  RESP_INIT(TronSignedTx);
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
