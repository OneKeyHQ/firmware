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

void fsm_msgPolkadotGetAddress(PolkadotGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(PolkadotAddress);

  HDNode node = {0};
  uint8_t ledger_secret[96 + 1];
  int res;

  const uint8_t *seed = config_getSeed();
  if (seed == NULL) {
    return;
  }
  res = secret_from_seed_cardano_ledger(seed, 64, ledger_secret);
  if (res != 1) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Unexpected failure in Ledger derivation"));
    return;
  }
  res = hdnode_from_secret_cardano(ledger_secret, &node);
  if (res != 1) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Unexpected failure in constructing polkadot node"));
    return;
  }

  if (hdnode_private_ckd_cached(&node, msg->address_n, msg->address_n_count,
                                NULL) == 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive private key"));
    return;
  }
  ed25519_publickey(node.private_key, node.public_key + 1);

  data2hexaddr(node.public_key + 1, 32, resp->public_key);
  resp->has_address = true;
  resp->has_public_key = true;
  polkadot_get_address_from_public_key(node.public_key + 1, resp->address,
                                       msg->prefix);

  if (msg->has_show_display && msg->show_display) {
    char desc[32] = {0};
    strcat(desc, msg->network);
    desc[0] = desc[0] - ('a' - 'A');
    strcat(desc, " ");
    strcat(desc, _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_PolkadotAddress, resp);
  layoutHome();
}

void fsm_msgPolkadotSignTx(const PolkadotSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(PolkadotSignedTx);

  HDNode node = {0};
  uint8_t ledger_secret[96 + 1];
  int res;

  const uint8_t *seed = config_getSeed();
  if (seed == NULL) {
    return;
  }
  res = secret_from_seed_cardano_ledger(seed, 64, ledger_secret);
  if (res != 1) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Unexpected failure in Ledger derivation"));
    return;
  }

  res = hdnode_from_secret_cardano(ledger_secret, &node);
  if (res != 1) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Unexpected failure in constructing Polkadot node"));
    return;
  }

  if (hdnode_private_ckd_cached(&node, msg->address_n, msg->address_n_count,
                                NULL) == 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive private key"));
    return;
  }
  ed25519_publickey(node.private_key, node.public_key + 1);

  if (!polkadot_sign_tx(msg, &node, resp)) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Signing failed"));
    layoutHome();
    return;
  }
  msg_write(MessageType_MessageType_PolkadotSignedTx, resp);
  layoutHome();
}
