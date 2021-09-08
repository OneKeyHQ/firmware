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

void fsm_msgSolanaGetAddress(const SolanaGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(SolanaAddress);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  resp->has_address = true;
  hdnode_fill_public_key(node);
  solana_get_address_from_public_key(&node->public_key[1], resp->address);

  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutAddress(resp->address, _("Address:"), true, 0,
                           msg->address_n, msg->address_n_count, false, NULL, 0,
                           NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_SolanaAddress, resp);

  layoutHome();
}
void fsm_msgSolanaSignTx(const SolanaSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(SolanaSignedTx);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  hdnode_fill_public_key(node);

  solana_sign_tx(msg, node, resp);

  msg_write(MessageType_MessageType_SolanaSignedTx, resp);

  layoutHome();
}
