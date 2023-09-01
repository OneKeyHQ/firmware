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

void fsm_msgKaspaGetAddress(const KaspaGetAddress *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(KaspaAddress);

  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;
  hdnode_fill_public_key(node);

  CHECK_PARAM(msg->has_schema && (strcmp(msg->schema, "schnorr") == 0 ||
                                  strcmp(msg->schema, "ecdsa") == 0),
              "Invalid schema");

  if (strcmp(msg->schema, "schnorr") == 0) {
    kaspa_get_address(node->public_key + 1, 32, msg->prefix, resp->address);
  } else {
    kaspa_get_address(node->public_key, 33, msg->prefix, resp->address);
  }

  if (msg->has_show_display && msg->show_display) {
    char desc[16] = {0};
    strcat(desc, "Kaspa");
    strcat(desc, _("Address:"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }
  msg_write(MessageType_MessageType_KaspaAddress, resp);
  layoutHome();
}

#define SIGN_DYNAMIC                                                        \
  HDNode *node = fsm_getDerivedNode(SECP256K1_NAME, msg->address_n,         \
                                    msg->address_n_count, NULL);            \
  if (!node) return;                                                        \
  hdnode_fill_public_key(node);                                             \
  if (input_count > 1) {                                                    \
    RESP_INIT(KaspaTxInputRequest);                                         \
    resp->request_index = input_index++;                                    \
    resp->has_signature = true;                                             \
    kaspa_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, \
                       resp->signature.bytes, &resp->signature.size);       \
    msg_write(MessageType_MessageType_KaspaTxInputRequest, resp);           \
  } else {                                                                  \
    RESP_INIT(KaspaSignedTx);                                               \
    kaspa_sign_sighash(node, msg->raw_message.bytes, msg->raw_message.size, \
                       resp->signature.bytes, &resp->signature.size);       \
    msg_write(MessageType_MessageType_KaspaSignedTx, resp);                 \
    kaspa_signing_abort();                                                  \
  }

void fsm_msgKaspaSignTx(const KaspaSignTx *msg) {
  CHECK_INITIALIZED

  CHECK_PIN
  CHECK_PARAM(msg->has_input_count && msg->input_count >= 1,
              "Invalid input count");
  CHECK_PARAM(msg->has_schema && (strcmp(msg->schema, "schnorr") == 0 ||
                                  strcmp(msg->schema, "ecdsa") == 0),
              "Invalid schema");

  kaspa_signing_init(msg);
  SIGN_DYNAMIC;
}

void fsm_msgKaspaTxInputAck(const KaspaTxInputAck *msg) { SIGN_DYNAMIC; }
