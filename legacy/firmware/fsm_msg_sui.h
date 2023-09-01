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

void fsm_msgSuiGetAddress(const SuiGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(SuiAddress);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);
  resp->has_address = true;

  sui_get_address_from_public_key(node->public_key + 1, resp->address);

  if (msg->has_show_display && msg->show_display) {
    char desc[16] = {0};
    strcat(desc, "Sui");
    strcat(desc, _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_SuiAddress, resp);
  layoutHome();
}

void fsm_msgSuiSignTx(const SuiSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(SuiSignedTx);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  sui_sign_tx(msg, node, resp);

  layoutHome();
}

void fsm_msgSuiSignMessage(SuiSignMessage *msg) {
  RESP_INIT(SuiMessageSignature);

  CHECK_INITIALIZED

  CHECK_PIN

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  sui_get_address_from_public_key(node->public_key + 1, resp->address);

  if (!fsm_layoutSignMessage("Sui", resp->address, msg->message.bytes,
                             msg->message.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  sui_message_sign(msg, node, resp);
  layoutHome();
}