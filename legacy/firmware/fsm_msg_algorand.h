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

void fsm_msgAlgorandGetAddress(AlgorandGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(AlgorandAddress);
  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  resp->has_address = true;
  algorand_get_address_from_public_key(node->public_key + 1, resp->address);

  if (msg->has_show_display && msg->show_display) {
    char desc[20] = {0};
    snprintf(desc, 20, "%s %s", "Algorand", _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_AlgorandAddress, resp);
  layoutHome();
}

void fsm_msgAlgorandSignTx(const AlgorandSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(AlgorandSignedTx);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  if (!algorand_sign_tx(msg, node, resp)) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Signing failed"));
    layoutHome();
    return;
  }
  msg_write(MessageType_MessageType_AlgorandSignedTx, resp);
  layoutHome();
}
