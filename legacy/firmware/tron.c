/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
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

#include <stdio.h>

#include "address.h"
#include "base58.h"
#include "crypto.h"
#include "ecdsa.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "protect.h"
#include "secp256k1.h"
#include "sha2.h"
#include "transaction.h"
#include "tron.h"
#include "util.h"

#include <pb_decode.h>
#include "tron_ui.h"

// PROTOBUF3 types
#define PROTO_TYPE_VARINT 0
#define PROTO_TYPE_STRING 2

extern int ethereum_is_canonic(uint8_t v, uint8_t signature[64]);

void tron_message_hash(const uint8_t *message, size_t message_len,
                       uint8_t hash[32]) {
  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, (const uint8_t *)"\x19" "TRON Signed Message:\n32", 24);
  sha3_Update(&ctx, message, message_len);
  keccak_Final(&ctx, hash);
}

void tron_message_sign(TronSignMessage *msg, const HDNode *node,
                       TronMessageSignature *resp) {
  uint8_t hash[32];
  uint8_t msg_hash[32];

  // hash the message
  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, msg->message.bytes, msg->message.size);
  keccak_Final(&ctx, msg_hash);

  tron_message_hash(msg_hash, 32, hash);

  uint8_t v;
  if (ecdsa_sign_digest(&secp256k1, node->private_key, hash,
                        resp->signature.bytes, &v, ethereum_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    return;
  }

  resp->signature.bytes[64] = 27 + v;
  resp->signature.size = 65;
  msg_write(MessageType_MessageType_TronMessageSignature, resp);
}

int tron_eth_2_trx_address(const uint8_t eth_address[20], char *str,
                           int strsize) {
  uint8_t address_bytes[21];
  address_bytes[0] = 0x41;  // Tron address prefix
  memcpy(&address_bytes[1], eth_address, 20);

  int r = base58_encode_check(address_bytes, sizeof(address_bytes),
                              HASHER_SHA2D, str, strsize);
  return r;
}

int add_field(uint8_t *buf, int *index, uint8_t fnumber, uint8_t ftype) {
  int ret = *index;

  if (fnumber > 15) {
    buf[*index] = fnumber << 3 | ftype;
    *index += 1;
    buf[*index] = 0x1;
    *index += 1;
  } else {
    buf[*index] = fnumber << 3 | ftype;
    *index += 1;
  }

  return *index - ret;
}

int write_varint(uint8_t *buf, int *index, uint64_t value) {
  int ret = *index;
  uint8_t byte = 0;
  uint64_t v = value;

  while (1) {
    byte = v & 0x7F;
    v = v >> 7;
    if (v == 0) {
      buf[*index] = byte;
      *index += 1;
      break;
    }
    buf[*index] = byte | 0x80;
    *index += 1;
  }

  return *index - ret;
}

int write_bytes_with_length(uint8_t *buf, int *index, uint8_t *bytes, int len) {
  int ret = *index;

  write_varint(buf, index, len);
  for (int i = 0; i < len; i++) {
    buf[*index] = bytes[i];
    *index += 1;
  }

  return *index - ret;
}

int write_bytes_without_length(uint8_t *buf, int *index, uint8_t *bytes,
                               int len) {
  int ret = *index;

  for (int i = 0; i < len; i++) {
    buf[*index] = bytes[i];
    *index += 1;
  }

  return *index - ret;
}

int pack_contract(TronSignTx *msg, uint8_t *buf, int *index,
                  const char *owner_address) {
  // Pack Tron Proto3 Contract
  // See: https://github.com/tronprotocol/protocol/blob/master/core/Tron.proto
  // and
  // https://github.com/tronprotocol/protocol/blob/master/core/contract/smart_contract.proto

  int ret = *index, len = 0, cmessage_len = 0, cmessage_index = 0, capi_len = 0,
      capi_index = 0;
  uint8_t cmessage[512] = {0};
  uint8_t capi[64] = {0};
  uint8_t addr_raw[MAX_ADDR_RAW_SIZE] = {0};

  add_field(buf, index, 1, PROTO_TYPE_VARINT);
  if (msg->contract.has_transfer_contract) {
    capi_len += add_field(capi, &capi_index, 1, PROTO_TYPE_STRING);
    capi_len += write_bytes_with_length(
        capi, &capi_index,
        (uint8_t *)"type.googleapis.com/protocol.TransferContract", 45);

    write_varint(buf, index, 1);

    cmessage_len += add_field(cmessage, &cmessage_index, 1, PROTO_TYPE_STRING);
    len = base58_decode_check(owner_address, HASHER_SHA2D, addr_raw,
                              MAX_ADDR_RAW_SIZE);
    cmessage_len +=
        write_bytes_with_length(cmessage, &cmessage_index, addr_raw, len);

    cmessage_len += add_field(cmessage, &cmessage_index, 2, PROTO_TYPE_STRING);
    len = base58_decode_check(msg->contract.transfer_contract.to_address,
                              HASHER_SHA2D, addr_raw, MAX_ADDR_RAW_SIZE);
    cmessage_len +=
        write_bytes_with_length(cmessage, &cmessage_index, addr_raw, len);
    cmessage_len += add_field(cmessage, &cmessage_index, 3, PROTO_TYPE_VARINT);
    cmessage_len += write_varint(cmessage, &cmessage_index,
                                 msg->contract.transfer_contract.amount);
  }

  if (msg->contract.has_trigger_smart_contract) {
    capi_len += add_field(capi, &capi_index, 1, PROTO_TYPE_STRING);
    capi_len += write_bytes_with_length(
        capi, &capi_index,
        (uint8_t *)"type.googleapis.com/protocol.TriggerSmartContract", 49);

    write_varint(buf, index, 31);

    cmessage_len += add_field(cmessage, &cmessage_index, 1, PROTO_TYPE_STRING);
    len = base58_decode_check(owner_address, HASHER_SHA2D, addr_raw,
                              MAX_ADDR_RAW_SIZE);
    cmessage_len +=
        write_bytes_with_length(cmessage, &cmessage_index, addr_raw, len);

    cmessage_len += add_field(cmessage, &cmessage_index, 2, PROTO_TYPE_STRING);
    len = base58_decode_check(
        msg->contract.trigger_smart_contract.contract_address, HASHER_SHA2D,
        addr_raw, MAX_ADDR_RAW_SIZE);
    cmessage_len +=
        write_bytes_with_length(cmessage, &cmessage_index, addr_raw, len);

    if (msg->contract.trigger_smart_contract.call_value) {
      cmessage_len +=
          add_field(cmessage, &cmessage_index, 3, PROTO_TYPE_VARINT);
      cmessage_len +=
          write_varint(cmessage, &cmessage_index,
                       msg->contract.trigger_smart_contract.call_value);
    }

    // Contract data
    cmessage_len += add_field(cmessage, &cmessage_index, 4, PROTO_TYPE_STRING);
    cmessage_len +=
        write_bytes_with_length(cmessage, &cmessage_index,
                                msg->contract.trigger_smart_contract.data.bytes,
                                msg->contract.trigger_smart_contract.data.size);
    if (msg->contract.trigger_smart_contract.call_token_value) {
      cmessage_len +=
          add_field(cmessage, &cmessage_index, 5, PROTO_TYPE_VARINT);
      cmessage_len +=
          write_varint(cmessage, &cmessage_index,
                       msg->contract.trigger_smart_contract.call_token_value);
      cmessage_len +=
          add_field(cmessage, &cmessage_index, 6, PROTO_TYPE_VARINT);
      cmessage_len +=
          write_varint(cmessage, &cmessage_index,
                       msg->contract.trigger_smart_contract.asset_id);
    }
  }

  uint8_t tmp[8] = {0};
  int cmessage_varint_len = 0;
  write_varint(tmp, &cmessage_varint_len, cmessage_len);

  add_field(buf, index, 2, PROTO_TYPE_STRING);
  write_varint(buf, index, capi_len + cmessage_len + 1 + cmessage_varint_len);
  write_bytes_without_length(buf, index, capi, capi_len);
  add_field(buf, index, 2, PROTO_TYPE_STRING);
  write_varint(buf, index, cmessage_len);
  write_bytes_without_length(buf, index, cmessage, cmessage_len);

  return *index - ret;
}

void serialize(TronSignTx *msg, uint8_t *buf, int *index,
               const char *owner_address) {
  // transaction parameters
  add_field(buf, index, 1, PROTO_TYPE_STRING);
  write_bytes_with_length(buf, index, msg->ref_block_bytes.bytes,
                          msg->ref_block_bytes.size);
  add_field(buf, index, 4, PROTO_TYPE_STRING);
  write_bytes_with_length(buf, index, msg->ref_block_hash.bytes,
                          msg->ref_block_hash.size);
  add_field(buf, index, 8, PROTO_TYPE_VARINT);
  write_varint(buf, index, msg->expiration);
  if (msg->has_data) {
    add_field(buf, index, 10, PROTO_TYPE_STRING);
    write_bytes_with_length(buf, index, (uint8_t *)msg->data,
                            strlen(msg->data));
  }

  // add Contract
  add_field(buf, index, 11, PROTO_TYPE_STRING);
  int current_index = *index, contract_len = 0;
  contract_len = pack_contract(msg, buf, index, owner_address);
  *index = current_index;
  write_varint(buf, index, contract_len);
  pack_contract(msg, buf, index, owner_address);

  // add timestamp
  add_field(buf, index, 14, PROTO_TYPE_VARINT);
  write_varint(buf, index, msg->timestamp);
  // add fee_limit if any
  if (msg->has_fee_limit) {
    add_field(buf, index, 18, PROTO_TYPE_VARINT);
    write_varint(buf, index, msg->fee_limit);
  }
}

bool tron_sign_tx(TronSignTx *msg, const char *owner_address,
                  const HDNode *node, TronSignedTx *resp) {
  ConstTronTokenPtr token = NULL;
  uint64_t amount = 0;
  uint8_t value_bytes[32];
  char to_str[36];

  int index = 0;
  uint8_t *raw = resp->serialized_tx.bytes;

  if (msg->contract.has_transfer_contract) {
    if (msg->contract.transfer_contract.has_amount) {
      amount = msg->contract.transfer_contract.amount;
    }
    if (msg->contract.transfer_contract.has_to_address) {
      memcpy(to_str, msg->contract.transfer_contract.to_address,
             strlen(msg->contract.transfer_contract.to_address));
    }
  } else if (msg->contract.has_trigger_smart_contract) {
    if (!msg->contract.trigger_smart_contract.has_data) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Invalid Tron contract call data"));
      return false;
    }
    if (msg->contract.trigger_smart_contract.data.size < 4) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Invalid Tron contract call data"));
      return false;
    }

    // parse chunk data as TRC20 transfer
    static uint8_t TRANSFER_SIG[4] = {0xa9, 0x05, 0x9c, 0xbb};
    // check method sig
    if (memcmp(TRANSFER_SIG, msg->contract.trigger_smart_contract.data.bytes,
               4)) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Invalid Tron contract call data"));
      return false;
    }
    if (msg->contract.trigger_smart_contract.data.size != 68) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Invalid TRC20 transfer method arguments data size"));
      return false;
    }

    token = get_tron_token_by_address(
        msg->contract.trigger_smart_contract.contract_address);

    if (tron_eth_2_trx_address(
            &msg->contract.trigger_smart_contract.data.bytes[4 + 12], to_str,
            sizeof(to_str)) < 34) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Failed to encode to Tron address"));
      return false;
    }

    memcpy(value_bytes,
           &msg->contract.trigger_smart_contract.data.bytes[4 + 32], 32);
  } else {
    fsm_sendFailure(FailureType_Failure_DataError, "unsupported contract type");
    return false;
  }

  serialize(msg, raw, &index, owner_address);

  // display tx info and ask user to confirm
  layoutTronConfirmTx(to_str, amount, value_bytes, token);
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    return false;
  }
  if (msg->has_fee_limit) {
    layoutTronFee(amount, value_bytes, token, msg->fee_limit);
    if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      return false;
    }
  }

  // hash the tx
  uint8_t hash[32];
  SHA256_CTX ctx;
  sha256_Init(&ctx);
  sha256_Update(&ctx, raw, index);
  sha256_Final(&ctx, hash);

  // sign tx hash
  uint8_t v;
  if (ecdsa_sign_digest(&secp256k1, node->private_key, hash,
                        resp->signature.bytes, &v, ethereum_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    return false;
  }

  // fill response
  resp->signature.bytes[64] = 27 + v;
  resp->signature.size = 65;
  resp->has_serialized_tx = 1;
  resp->serialized_tx.size = index;

  return true;
}

void tron_format_amount(const uint64_t amount, char *buf, int buflen) {
  char str_amount[12] = {0};
  bn_format_uint64(amount, NULL, NULL, 6, 0, false, str_amount,
                   sizeof(str_amount));
  snprintf(buf, buflen, "%s TRX", str_amount);
}

void tron_format_token_amount(const bignum256 *amnt, ConstTronTokenPtr token,
                              char *buf, int buflen) {
  if (token == NULL) {
    strlcpy(buf, "Unknown token value", buflen);
    return;
  }
  bn_format(amnt, NULL, token->ticker, token->decimals, 0, false, buf, buflen);
}
