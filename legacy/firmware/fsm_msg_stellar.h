/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2018 ZuluCrypto <zulucrypto@protonmail.com>
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

static bool fsm_stellarCheckPath(uint32_t address_n_count,
                                 const uint32_t *address_n) {
  if (stellar_path_check(address_n_count, address_n)) {
    return true;
  }

  if (config_getSafetyCheckLevel() == SafetyCheckLevel_Strict) {
    fsm_sendFailure(FailureType_Failure_DataError, "Forbidden key path");
    return false;
  }

  return fsm_layoutPathWarning(address_n_count, address_n);
}

void fsm_msgStellarGetAddress(const StellarGetAddress *msg) {
  RESP_INIT(StellarAddress);

  CHECK_INITIALIZED

  CHECK_PIN

  if (!fsm_stellarCheckPath(msg->address_n_count, msg->address_n)) {
    layoutHome();
    return;
  }

  HDNode *node = stellar_deriveNode(msg->address_n, msg->address_n_count);
  if (!node) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    "Failed to derive private key");
    layoutHome();
    return;
  }
  hdnode_fill_public_key(node);
  stellar_publicAddressAsStr(node->public_key + 1, resp->address,
                             sizeof(resp->address));

  if (msg->has_show_display && msg->show_display) {
    char desc[16] = {0};
    snprintf(desc, 16, "%s %s", "XLM", _("Address"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_StellarAddress, resp);

  layoutHome();
}

void fsm_msgStellarSignTx(const StellarSignTx *msg) {
  CHECK_INITIALIZED
  CHECK_PIN

  if (!fsm_stellarCheckPath(msg->address_n_count, msg->address_n)) {
    layoutHome();
    return;
  }

  if (!stellar_signingInit(msg)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    "Failed to derive private key");
    layoutHome();
    return;
  }

  // Confirm transaction basics
  stellar_layoutTransactionSummary(msg);

  // Respond with a request for the first operation
  RESP_INIT(StellarTxOpRequest);

  msg_write(MessageType_MessageType_StellarTxOpRequest, resp);
}

#define GO_AHEAD                                                   \
  do {                                                             \
    if (stellar_allOperationsConfirmed()) {                        \
      RESP_INIT(StellarSignedTx);                                  \
      stellar_fillSignedTx(resp);                                  \
      msg_write(MessageType_MessageType_StellarSignedTx, resp);    \
      layoutHome();                                                \
    } else {                                                       \
      RESP_INIT(StellarTxOpRequest);                               \
      msg_write(MessageType_MessageType_StellarTxOpRequest, resp); \
    }                                                              \
  } while (0);

void fsm_msgStellarCreateAccountOp(const StellarCreateAccountOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmCreateAccountOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarPaymentOp(const StellarPaymentOp *msg) {
  CHECK_UNLOCKED

  // This will display additional dialogs to the user
  if (!stellar_confirmPaymentOp(msg)) return;

  // Last operation was confirmed, send a StellarSignedTx
  GO_AHEAD
}

void fsm_msgStellarPathPaymentStrictReceiveOp(
    const StellarPathPaymentStrictReceiveOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmPathPaymentStrictReceiveOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarPathPaymentStrictSendOp(
    const StellarPathPaymentStrictSendOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmPathPaymentStrictSendOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarManageBuyOfferOp(const StellarManageBuyOfferOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmManageBuyOfferOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarManageSellOfferOp(const StellarManageSellOfferOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmManageSellOfferOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarCreatePassiveSellOfferOp(
    const StellarCreatePassiveSellOfferOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmCreatePassiveSellOfferOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarSetOptionsOp(const StellarSetOptionsOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmSetOptionsOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarChangeTrustOp(const StellarChangeTrustOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmChangeTrustOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarAllowTrustOp(const StellarAllowTrustOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmAllowTrustOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarAccountMergeOp(const StellarAccountMergeOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmAccountMergeOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarManageDataOp(const StellarManageDataOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmManageDataOp(msg)) return;

  GO_AHEAD
}

void fsm_msgStellarBumpSequenceOp(const StellarBumpSequenceOp *msg) {
  CHECK_UNLOCKED

  if (!stellar_confirmBumpSequenceOp(msg)) return;

  GO_AHEAD
}
