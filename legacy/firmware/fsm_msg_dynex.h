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
#include "monero/monero.h"

void fsm_msgDnxGetAddress(const DnxGetAddress* msg) {
  CHECK_INITIALIZED

  CHECK_PIN
  CHECK_PARAM(
      msg->address_n_count == 5 && msg->address_n[1] == (29538 | 0x80000000),
      "Invalid address path");
  RESP_INIT(DnxAddress);

  HDNode* node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;
  dnx_get_address(resp->address, node->private_key, false);
  resp->has_address = true;
  if (msg->has_show_display && msg->show_display) {
    char desc[12] = {0};
    strcat(desc, "Dnx ");
    strcat(desc, _("Address"));
    if (!fsm_layoutAddress(resp->address, NULL, desc, false, 0, msg->address_n,
                           msg->address_n_count, true, NULL, 0, 0, NULL)) {
      return;
    }
  }

  msg_write(MessageType_MessageType_DnxAddress, resp);
  layoutHome();
}
void fsm_msgDnxSignTx(const DnxSignTx* msg) {
  CHECK_INITIALIZED

  CHECK_PIN
  CHECK_PARAM(!msg->has_payment_id ||
                  (msg->has_payment_id && msg->payment_id.size == 32),
              "Invalid payment id");
  CHECK_PARAM(msg->inputs_count >= 1 && msg->inputs_count <= MAX_INPUTS_COUNT,
              "Invalid number of inputs");
  CHECK_PARAM(
      msg->address_n_count == 5 && msg->address_n[1] == (29538 | 0x80000000),
      "Invalid address path");
  HDNode* node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;
  dnx_singing_init(msg);
  if (!layoutDnxTxInfo(msg, node)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Action cancelled");
    dnx_singing_abort();
    return;
  }
  uint8_t tx_header[3] = {0};
  tx_header[0] = 1;                  // version
  tx_header[1] = 0;                  // unlock time
  tx_header[2] = msg->inputs_count;  // input counts
  xmr_hasher_update(&dnx_hasher_context, tx_header, sizeof(tx_header));
  RESP_INIT(DnxInputRequest);
  resp->has_request_index = true;
  resp->request_index = ++dnx_input_index;
  resp->has_tx_key = true;
  resp->tx_key.has_ephemeral_tx_sec_key = true;
  resp->tx_key.has_ephemeral_tx_pub_key = true;
  bignum256modm scalar = {0};
  xmr_random_scalar(scalar);
  uint8_t key_seed[32] = {0};
  encodeint(key_seed, scalar);
  ge25519 tx_pub_key = {0};
  ge25519_set_neutral(&tx_pub_key);
  generate_keys(key_seed, &tx_pub_key, tx_sec_key);
  resp->tx_key.ephemeral_tx_sec_key.size = 32;
  resp->tx_key.ephemeral_tx_pub_key.size = 32;
  encodeint(resp->tx_key.ephemeral_tx_sec_key.bytes, tx_sec_key);
  encodepoint(tx_pub_key_bytes, &tx_pub_key);
  memcpy(resp->tx_key.ephemeral_tx_pub_key.bytes, tx_pub_key_bytes, 32);
  msg_write(MessageType_MessageType_DnxInputRequest, resp);
}
void fsm_msgDnxInputAck(const DnxInputAck* msg) {
  if (!dnx_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Not in Dynex signing mode");
    layoutHome();
    return;
  };
  if (dnx_input_index <= dnx_inputs_count) {
    ++dnx_input_index;
    RESP_INIT(DnxInputRequest);
    resp->has_computed_key_image = true;
    if (dnx_input_index > dnx_inputs_count) {
      resp->has_request_index = false;
      key_image_synced = true;
    } else {
      resp->has_request_index = true;
      resp->request_index = dnx_input_index;
    }
    ge25519 tx_pub_key = {0};
    ge25519_set_neutral(&tx_pub_key);
    decodepoint(&tx_pub_key, msg->tx_pubkey.bytes);
    resp->computed_key_image.has_key_image = true;
    resp->computed_key_image.key_image.size = 32;
    int amount_size = xmr_size_varint(msg->amount);
    int global_index_size = xmr_size_varint(msg->global_index);
    uint8_t buff[2 + amount_size + global_index_size + 32];
    buff[0] = 2;                                           // input type tag
    xmr_write_varint(buff + 1, amount_size, msg->amount);  // amount
    buff[1 + amount_size] = 1;  // outputIndexes vector size
    xmr_write_varint(buff + 2 + amount_size, global_index_size,
                     msg->global_index);  // globalOutputIndex
    dnx_generate_key_image(&tx_pub_key, &spend_pub_key, spend_sec_key,
                           msg->prev_index, view_sec_key,
                           resp->computed_key_image.key_image.bytes,
                           ephemeral_keys[dnx_input_index - 2]);
    memcpy(key_images[dnx_input_index - 2],
           resp->computed_key_image.key_image.bytes, 32);
    memcpy(output_keys[dnx_input_index - 2], msg->prev_out_pubkey.bytes, 32);
    memcpy(buff + 2 + amount_size + global_index_size,
           resp->computed_key_image.key_image.bytes, 32);
    xmr_hasher_update(&dnx_hasher_context, buff, sizeof(buff));
    total_input_amounts += msg->amount;
    msg_write(MessageType_MessageType_DnxInputRequest, resp);
  } else {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Dnx image key already synced");
    dnx_singing_abort();
    return;
  }
}
void fsm_msgDnxRTSigsRequest(const DnxRTSigsRequest* msg) {
  (void)msg;
  if (!dnx_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Not in Dynex signing mode");
    layoutHome();
    return;
  };
  if (!key_image_synced) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Dnx key image not synced");
    dnx_singing_abort();
    return;
  }
  if (total_input_amounts < amounts + fee) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "insufficient funds");
    dnx_singing_abort();
    return;
  }
  RESP_INIT(DnxSignedTx);
  uint64_t change_amount = total_input_amounts - amounts - fee;
  bool has_change = change_amount > 0;
  int size_out_amounts = xmr_size_varint(amounts);
  uint8_t buff[2 + size_out_amounts + 32];
  buff[0] = has_change ? 2 : 1;                           // output count
  xmr_write_varint(buff + 1, size_out_amounts, amounts);  // output amount
  buff[1 + size_out_amounts] = 2;                         // type of ouput

  uint8_t data[64] = {0};
  dnx_base58_addr_decode_check(destination, sizeof(destination) - 1, data,
                               sizeof(data));
  ge25519 destination_view_pub_key = {0};
  ge25519_set_neutral(&destination_view_pub_key);
  ge25519 destination_spend_pub_key = {0};
  ge25519_set_neutral(&destination_spend_pub_key);
  decodepoint(&destination_spend_pub_key, data);
  decodepoint(&destination_view_pub_key, data + 32);
  resp->output_keys_count = has_change ? 2 : 1;
  resp->output_keys[0].size = 32;
  dnx_generate_output_key(resp->output_keys[0].bytes, 0, tx_sec_key,
                          &destination_view_pub_key,
                          &destination_spend_pub_key);
  memcpy(buff + 2 + size_out_amounts, resp->output_keys->bytes, 32);
  xmr_hasher_update(&dnx_hasher_context, buff, sizeof(buff));
  if (has_change) {
    int size_change_amounts = xmr_size_varint(change_amount);
    uint8_t buff1[1 + size_change_amounts + 32];
    xmr_write_varint(buff1, size_change_amounts,
                     change_amount);  // output amount
    buff1[size_change_amounts] = 2;   // type of ouput
    resp->output_keys[1].size = 32;
    dnx_generate_output_key(resp->output_keys[1].bytes, 1, tx_sec_key,
                            &view_pub_key, &spend_pub_key);
    memcpy(buff1 + 1 + size_change_amounts, resp->output_keys[1].bytes, 32);
    xmr_hasher_update(&dnx_hasher_context, buff1, sizeof(buff1));
  }
  uint8_t extra[240] = {0};
  int extra_size = 0;
  if (has_payment_id) {
    extra[extra_size++] = 2;
    extra[extra_size++] = 33;
    extra[extra_size++] = 0;
    memcpy(extra + extra_size, payment_id, 32);
    extra_size += 32;
  }
  extra[extra_size++] = 1;  // tx tag public key
  memcpy(extra + extra_size, tx_pub_key_bytes, 32);
  extra_size += 32;
  extra[extra_size++] = 4;  // from_addr tag
  encodepoint(extra + extra_size, &spend_pub_key);
  extra_size += 32;
  encodepoint(extra + extra_size, &view_pub_key);
  extra_size += 32;
  extra[extra_size++] = 5;  // to_addr tag
  encodepoint(extra + extra_size, &destination_spend_pub_key);
  extra_size += 32;
  encodepoint(extra + extra_size, &destination_view_pub_key);
  extra_size += 32;
  extra[extra_size++] = 6;  // amount tag
  memcpy(extra + extra_size, (uint8_t*)&amounts, sizeof(amounts));
  extra_size += sizeof(amounts);
  extra[extra_size++] = 7;  // tx tag sec key
  encodeint(extra + extra_size, tx_sec_key);
  extra_size += 32;
  int size = xmr_size_varint(extra_size);
  uint8_t size_buffer[size];
  xmr_write_varint(size_buffer, size, extra_size);
  xmr_hasher_update(&dnx_hasher_context, size_buffer, size);
  xmr_hasher_update(&dnx_hasher_context, extra, extra_size);
  uint8_t prefix_hash[32] = {0};
  xmr_hasher_final(&dnx_hasher_context, prefix_hash);
  resp->signatures_count = dnx_inputs_count;
  bignum256modm sigs[1][2];
  for (size_t i = 0; i < dnx_inputs_count; i++) {
    ge25519 output_key_selected[1] = {0};
    decodepoint(output_key_selected, output_keys[i]);
    ge25519 key_image = {0};
    decodepoint(&key_image, key_images[i]);
    bignum256modm eph_key = {0};
    decodeint(eph_key, ephemeral_keys[i]);
    generate_ring_signature(prefix_hash, &key_image, 1, output_key_selected,
                            eph_key, 0, sigs);
    resp->signatures[i].size = 64;
    encodeint(resp->signatures[i].bytes, sigs[0][0]);
    encodeint(resp->signatures[i].bytes + 32, sigs[0][1]);
  }
  msg_write(MessageType_MessageType_DnxSignedTx, resp);
  dnx_singing_abort();
  return;
}
