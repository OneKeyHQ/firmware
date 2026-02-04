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
#undef COIN_TYPE
#define COIN_TYPE 29223
void fsm_msgNexaGetAddress(const NexaGetAddress *msg) {
  CHECK_INITIALIZED
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,
                                    COIN_TYPE, SECP256K1_NAME, true),
              "Invalid path");
  CHECK_PIN

  RESP_INIT(NexaAddress);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;
  hdnode_fill_public_key(node);

  nexa_get_address(node->public_key, 33, msg->prefix, resp->address);

  if (msg->has_show_display && msg->show_display) {
    char desc[16] = {0};
    strcat(desc, "Nexa ");
    strcat(desc, _("Address"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }
  resp->public_key.size = 33;
  memcpy(resp->public_key.bytes, node->public_key, 33);
  msg_write(MessageType_MessageType_NexaAddress, resp);
  layoutHome();
}

#define SIGN_DYNAMIC_NEXA                                                  \
  CHECK_PARAM(fsm_common_path_check(msg->address_n, msg->address_n_count,  \
                                    COIN_TYPE, SECP256K1_NAME, true),      \
              "Invalid path");                                             \
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,        \
                                    msg->address_n_count, NULL);           \
  if (!node) return;                                                       \
  hdnode_fill_public_key(node);                                            \
  if (input_count_nexa > 1) {                                              \
    RESP_INIT(NexaTxInputRequest);                                         \
    resp->request_index = input_index_nexa++;                              \
    resp->has_signature = true;                                            \
    nexa_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, \
                      resp->signature.bytes, &resp->signature.size);       \
    msg_write(MessageType_MessageType_NexaTxInputRequest, resp);           \
  } else {                                                                 \
    RESP_INIT(NexaSignedTx);                                               \
    nexa_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, \
                      resp->signature.bytes, &resp->signature.size);       \
    msg_write(MessageType_MessageType_NexaSignedTx, resp);                 \
    nexa_signing_abort();                                                  \
  }

void fsm_msgNexaSignTx(const NexaSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN
  CHECK_PARAM(msg->has_input_count && msg->input_count >= 1,
              "Invalid input count");

  nexa_signing_init(msg);
  SIGN_DYNAMIC_NEXA;
}

void fsm_msgNexaTxInputAck(const NexaTxInputAck *msg) { SIGN_DYNAMIC_NEXA; }
