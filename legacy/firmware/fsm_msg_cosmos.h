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

void fsm_msgCosmosGetAddress(CosmosGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(CosmosAddress);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  resp->has_address = true;
  if (!msg->has_hrp) {
    memcpy(msg->hrp, "cosmos", 6);  // default cosmos
  }
  if (!cosmos_get_address(resp->address, node->public_key, msg->hrp)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to get address"));
    layoutHome();
    return;
  }

  if (msg->has_show_display && msg->show_display) {
    char desc[32] = {0};
    const CosmosNetworkType *n = cosmosnetworkByHrp(msg->hrp);
    if (n) {
      strcat(desc, n->chain_name);
    } else {
      strcat(desc, "Cosmos");
    }
    strcat(desc, " ");
    strcat(desc, _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_CosmosAddress, resp);
  layoutHome();
}

void fsm_msgCosmosSignTx(const CosmosSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(CosmosSignedTx);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (hdnode_fill_public_key(node) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive public key"));
    layoutHome();
    return;
  }

  if (!cosmos_sign_tx(msg, node, resp)) {
    return;
  }
  msg_write(MessageType_MessageType_CosmosSignedTx, resp);

  layoutHome();
}
