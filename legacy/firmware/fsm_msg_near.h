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

void fsm_msgNearGetAddress(NearGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NearAddress);

  // m/44h/397h/0h
  if ((msg->address_n_count != 3) || (msg->address_n[0] != 0x8000002c) ||
      (msg->address_n[1] != 0x8000018d)) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Invalid Path"));
    layoutHome();
    return;
  }

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  resp->has_address = true;
  near_get_address_from_public_key(node->public_key + 1, resp->address);

  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutAddress(resp->address, _("Address:"), false, 0,
                           msg->address_n, msg->address_n_count, true, NULL, 0,
                           0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_NearAddress, resp);
  layoutHome();
}

void fsm_msgNearSignTx(const NearSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(NearSignedTx);

  // m/44h/397h/0h
  if ((msg->address_n_count != 3) || (msg->address_n[0] != 0x8000002c) ||
      (msg->address_n[1] != 0x8000018d)) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Invalid Path"));
    layoutHome();
    return;
  }

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  if (!near_sign_tx(msg, node, resp)) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Signing failed"));
    layoutHome();
    return;
  }
  msg_write(MessageType_MessageType_NearSignedTx, resp);
  layoutHome();
}
