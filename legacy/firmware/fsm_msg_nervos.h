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

void fsm_msgNervosGetAddress(const NervosGetAddress *msg) {
  CHECK_INITIALIZED
  CHECK_PIN
  RESP_INIT(NervosAddress);
  if (strcmp(msg->network, "ckt") != 0 && strcmp(msg->network, "ckb") != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("network format error"));
    layoutHome();
    return;
  }
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  hdnode_fill_public_key(node);
  if (!node) return;
  nervos_get_address_from_public_key(node->public_key, resp->address,
                                     msg->network);

  if (msg->has_show_display && msg->show_display) {
    char desc[16] = {0};
    strcat(desc, "Nervos");
    strcat(desc, _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, true, 0, msg->address_n,
                           msg->address_n_count, false, NULL, 0, 0, NULL)) {
      return;
    }
  }
  msg_write(MessageType_MessageType_NervosAddress, resp);
  layoutHome();
}

#define SIGN_DYNAMIC_NERVOS                                                  \
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,          \
                                    msg->address_n_count, NULL);             \
  if (!node) return;                                                         \
  hdnode_fill_public_key(node);                                              \
  if (input_count_nervos > 1) {                                              \
    RESP_INIT(NervosTxInputRequest);                                         \
    resp->request_index = input_index_nervos++;                              \
    resp->has_signature = true;                                              \
    nervos_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, \
                        global_witness_buffer, witness_buffer_len_nervos,    \
                        resp->signature.bytes, &resp->signature.size);       \
    msg_write(MessageType_MessageType_NervosTxInputRequest, resp);           \
  } else {                                                                   \
    RESP_INIT(NervosSignedTx);                                               \
    nervos_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, \
                        global_witness_buffer, witness_buffer_len_nervos,    \
                        resp->signature.bytes, &resp->signature.size);       \
    msg_write(MessageType_MessageType_NervosSignedTx, resp);                 \
    nervos_signing_abort();                                                  \
  }                                                                          \
  layoutHome();

void fsm_msgNervosSignTx(const NervosSignTx *msg) {
  CHECK_INITIALIZED
  CHECK_PIN
  if (strcmp(msg->network, "ckt") != 0 && strcmp(msg->network, "ckb") != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("network format error"));
    layoutHome();
    return;
  }
  nervos_signing_init(msg);
  SIGN_DYNAMIC_NERVOS;
}

void fsm_msgNervosTxInputAck(const NervosTxInputAck *msg) {
  SIGN_DYNAMIC_NERVOS;
}