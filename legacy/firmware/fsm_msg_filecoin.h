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

void fsm_msgFilecoinGetAddress(const FilecoinGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(FilecoinAddress);
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  uint8_t pk[65] = {0};
  /* get uncompressed public key */
  if (ecdsa_get_public_key65(node->curve->params, node->private_key, pk) != 0) {
    return;
  }
  if (!get_filecoin_addr(pk, resp)) return;

  msg_write(MessageType_MessageType_FilecoinAddress, resp);
  layoutHome();
}

void fsm_msgFilecoinSignTx(const FilecoinSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(FilecoinSignedTx);
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  if (!filecoin_sign_tx(msg, node, resp)) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Signing failed"));
    layoutHome();
    return;
  }

  msg_write(MessageType_MessageType_FilecoinSignedTx, resp);

  layoutHome();
}