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

void fsm_msgCardanoGetPublicKey(CardanoGetPublicKey *msg) {
  RESP_INIT(CardanoPublicKey);

  CHECK_INITIALIZED

  CHECK_PIN

  if (msg->derivation_type != CardanoDerivationType_ICARUS) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Only support ICARUS scheme"));
    return;
  }
  HDNode node = {0};
  uint32_t fingerprint;
  fsm_getCardanoIcaruNode(&node, msg->address_n, msg->address_n_count,
                          &fingerprint);

  resp->node.depth = node.depth;
  resp->node.fingerprint = fingerprint;
  resp->node.child_num = node.child_num;
  resp->node.chain_code.size = 32;
  memcpy(resp->node.chain_code.bytes, node.chain_code, 32);
  resp->node.has_private_key = false;
  resp->node.public_key.size = 32;
  memcpy(resp->node.public_key.bytes, node.public_key + 1, 32);

  uint8_t xpub[64] = {0};
  memcpy(xpub, node.public_key + 1, 32);
  memcpy(xpub + 32, node.chain_code, 32);
  data2hexaddr(xpub, 64, resp->xpub);

  msg_write(MessageType_MessageType_CardanoPublicKey, resp);
  layoutHome();
}

void fsm_msgCardanoGetAddress(CardanoGetAddress *msg) {
  RESP_INIT(CardanoAddress);

  CHECK_INITIALIZED

  CHECK_PIN

  if (msg->derivation_type != CardanoDerivationType_ICARUS) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Only support ICARUS scheme"));
    return;
  }

  if (!ada_get_address(msg, resp->address)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Deriving address failed"));
    layoutHome();
    return;
  }
  if (msg->has_show_display && msg->show_display) {
    char desc[20] = {0};
    char addr_type[32] = {0};
    snprintf(desc, 20, "Cardano %s", _("Address:"));
    if (msg->address_parameters.address_type == CardanoAddressType_BASE) {
      snprintf(addr_type, 32, "Base %s", _("Address:"));
    } else if (msg->address_parameters.address_type ==
               CardanoAddressType_REWARD) {
      snprintf(addr_type, 32, "Reward %s", _("Address:"));
    }
    if (msg->address_parameters.address_n_count > 0) {
      if (!fsm_layoutAddress(resp->address, addr_type, desc, false, 0,
                             msg->address_parameters.address_n,
                             msg->address_parameters.address_n_count, true,
                             NULL, 0, 0, NULL)) {
        layoutHome();
        return;
      }
    } else {
      if (!fsm_layoutAddress(resp->address, addr_type, desc, false, 0,
                             msg->address_parameters.address_n_staking,
                             msg->address_parameters.address_n_staking_count,
                             true, NULL, 0, 0, NULL)) {
        layoutHome();
        return;
      }
    }
  }

  msg_write(MessageType_MessageType_CardanoAddress, resp);
  layoutHome();
}

void fsm_msgCardanoTxWitnessRequest(CardanoTxWitnessRequest *msg) {
  RESP_INIT(CardanoTxWitnessResponse);
  cardano_txwitness(msg, resp);
  msg_write(MessageType_MessageType_CardanoTxWitnessResponse, resp);
  layoutHome();
}

void fsm_msgCardanoTxHostAck(void) { cardano_txack(); }

void fsm_msgCardanoSignTxInit(CardanoSignTxInit *msg) {
  if (!_processs_tx_init(msg)) {
    layoutHome();
  }
}

void fsm_msgCardanoTxInput(CardanoTxInput *msg) {
  txHashBuilder_addInput(msg);
  hash_stage();
}

void fsm_msgCardanoTxOutput(CardanoTxOutput *msg) {
  txHashBuilder_addOutput(msg);
  hash_stage();
}

void fsm_msgCardanoAssetGroup(CardanoAssetGroup *msg) {
  txHashBuilder_addAssetGroup(msg);
  hash_stage();
}

void fsm_msgCardanoToken(CardanoToken *msg) {
  txHashBuilder_addToken(msg);
  hash_stage();
}

void fsm_msgCardanoTxCertificate(CardanoTxCertificate *msg) {
  if (!txHashBuilder_addCertificate(msg)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Invalid Certificate request"));
    layoutHome();
    return;
  }
  hash_stage();
}

void fsm_msgCardanoTxWithdrawal(CardanoTxWithdrawal *msg) {
  if (!txHashBuilder_addWithdrawal(msg)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Invalid Withdrawal request"));
    layoutHome();
    return;
  }
  hash_stage();
}

void fsm_msgCardanoTxAuxiliaryData(CardanoTxAuxiliaryData *msg) {
  if (!txHashBuilder_addAuxiliaryData(msg)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Invalid AuxiliaryData request"));
    layoutHome();
    return;
  }
}

void fsm_msgCardanoPoolOwner(CardanoPoolOwner *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoPoolRelayParameters(
    CardanoPoolRelayParameters *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoGetNativeScriptHash(void) {  // unsupport
}
void fsm_msgCardanoTxMint(CardanoTxMint *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoTxCollateralInput(
    CardanoTxCollateralInput *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoTxRequiredSigner(
    CardanoTxRequiredSigner *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoTxInlineDatumChunk(
    CardanoTxInlineDatumChunk *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoTxReferenceScriptChunk(
    CardanoTxReferenceScriptChunk *msg) {  // unsupport
  (void)msg;
}
void fsm_msgCardanoTxReferenceInput(
    CardanoTxReferenceInput *msg) {  // unsupport
  (void)msg;
}

void fsm_msgCardanoSignMessage(CardanoSignMessage *msg) {
  RESP_INIT(CardanoMessageSignature);

  CHECK_INITIALIZED

  CHECK_PIN

  if ((msg->network_id != 0) && (msg->network_id != 1)) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Invalid Networ ID"));
    return;
  }
  HDNode node = {0};
  uint32_t fingerprint;
  bool res = fsm_getCardanoIcaruNode(&node, msg->address_n,
                                     msg->address_n_count, &fingerprint);
  if (!res) return;

  if (!ada_sign_messages(&node, msg, resp)) {
    return;
  }

  msg_write(MessageType_MessageType_CardanoMessageSignature, resp);
  layoutHome();
}
