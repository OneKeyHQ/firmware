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
#include "nervos.h"
#include <stdio.h>
#include <string.h>
#include "blake2b.h"
#include "buttons.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "schnorr_bch.h"
#include "secp256k1.h"
#include "util.h"

#define MAX_ADDRESS_LENGTH 100
#define CODE_INDEX_SECP256K1_SINGLE 0x00
#define FORMAT_TYPE_SHORT 0x01

const uint8_t *global_witness_buffer = NULL;
const char CHARSET[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";
static NervosTxRequest msg_tx_request;
static uint32_t data_total, data_left, witness_buffer_len_nervos;
static bool nervos_signing = false;
uint8_t global_hash_output[32];
HDNode *globalNode = NULL;
blake2b_state S_GLOBAL;

void ckb_hasher_init(blake2b_state *S) {
  const uint8_t personal[] = "ckb-default-hash";
  size_t outlen = 32;
  blake2b_InitPersonal(S, outlen, personal, sizeof(personal) - 1);
}

void ckb_hash(const uint8_t *message, size_t message_len, uint8_t *output) {
  blake2b_state S;
  ckb_hasher_init(&S);
  blake2b_Update(&S, message, message_len);
  blake2b_Final(&S, output, 32);
}

void ckb_blake160(const uint8_t *message, size_t message_len, char *output) {
  uint8_t hash[32];
  ckb_hash(message, message_len, hash);
  data2hexaddr(hash, 20, output);
}

uint32_t bech32_polymod(const uint8_t *values, size_t len) {
  static const uint32_t generator[] = {0x3b6a57b2, 0x26508e6d, 0x1ea119fa,
                                       0x3d4233dd, 0x2a1462b3};
  uint32_t chk = 1;
  for (size_t i = 0; i < len; ++i) {
    uint8_t top = chk >> 25;
    chk = ((chk & 0x1ffffff) << 5) ^ values[i];
    for (int j = 0; j < 5; ++j) {
      if ((top >> j) & 1) {
        chk ^= generator[j];
      }
    }
  }
  return chk;
}

void bech32_hrp_expand(const char *hrp, uint8_t *expanded) {
  size_t hrp_len = strlen(hrp);
  for (size_t i = 0; i < hrp_len; ++i) {
    expanded[i] = hrp[i] >> 5;
  }
  expanded[hrp_len] = 0;
  for (size_t i = 0; i < hrp_len; ++i) {
    expanded[hrp_len + 1 + i] = hrp[i] & 31;
  }
}

int convertbits(const uint8_t *data, size_t datalen, uint8_t *out, int frombits,
                int tobits, int pad) {
  uint32_t acc = 0;
  int bits = 0;
  size_t retlen = 0;
  const uint32_t maxv = (1 << tobits) - 1;
  const uint32_t max_acc = (1 << (frombits + tobits - 1)) - 1;
  for (size_t i = 0; i < datalen; ++i) {
    if (data[i] >> frombits) {
      return -1;  // Value out of range
    }
    acc = ((acc << frombits) | data[i]) & max_acc;
    bits += frombits;
    while (bits >= tobits) {
      bits -= tobits;
      out[retlen++] = (acc >> bits) & maxv;
    }
  }
  if (pad) {
    if (bits) {
      out[retlen++] = (acc << (tobits - bits)) & maxv;
    }
  } else if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
    return -1;  // Cannot convert
  }
  return retlen;
}

int hex_char_to_int(char c) {
  if ('0' <= c && c <= '9') return c - '0';
  if ('a' <= c && c <= 'f') return c - 'a' + 10;
  if ('A' <= c && c <= 'F') return c - 'A' + 10;
  return -1;
}

void hex_str_to_withnesss(const char *hex_str, uint8_t *withnesss,
                          size_t withnesss_len) {
  for (size_t i = 0; i < withnesss_len; ++i) {
    int hi = hex_char_to_int(hex_str[2 * i]);
    int lo = hex_char_to_int(hex_str[2 * i + 1]);
    if (hi == -1 || lo == -1) {
      printf("Invalid hex string");
      return;
    }
    withnesss[i] = (hi << 4) + lo;
  }
}

void extend_uint64(uint8_t *buffer, size_t *buffer_len, uint64_t n) {
  for (int i = 0; i < 8; i++) {
    buffer[(*buffer_len)++] = (uint8_t)(n & 0xFF);
    n >>= 8;
  }
}

// Function to initialize the global hash state
void global_hasher_init(void) { ckb_hasher_init(&S_GLOBAL); }

// Function to update the global hash state
void global_hash_update(const uint8_t *data, uint32_t data_len) {
  blake2b_Update(&S_GLOBAL, data, data_len);
}
// Function to get the global hash result
void global_hash_finalize(uint8_t *output) {
  blake2b_Final(&S_GLOBAL, output, 32);
}

void nervos_get_address_from_public_key(const uint8_t *public_key,
                                        char *address, const char *network) {
  size_t public_key_len = 33;
  char output[43];
  ckb_blake160(public_key, public_key_len, output);  // hash160 calculation
  uint8_t payload[22];  // format type(1) + code index(1) + hash(20)
  payload[0] = FORMAT_TYPE_SHORT;
  payload[1] = CODE_INDEX_SECP256K1_SINGLE;
  hex_str_to_withnesss(output, payload + 2, 20);

  uint8_t data_part[36];
  convertbits(payload, 22, data_part, 8, 5, 1);
  size_t data_len = sizeof(data_part) / sizeof(data_part[0]);

  uint8_t expanded[7];
  bech32_hrp_expand(network, expanded);
  size_t hrp_exp_len = strlen(network) * 2 + 1;

  uint8_t values[49];
  memcpy(values, expanded, hrp_exp_len);
  memcpy(values + hrp_exp_len, data_part, data_len);
  size_t values_len = hrp_exp_len + data_len;

  for (int i = 0; i < 6; ++i) {
    values[values_len + i] = 0;
  }
  values_len += 6;

  uint32_t polymod =
      bech32_polymod(values, values_len) ^ 1;  // polymod calculation
  uint8_t checksum[6];
  for (int i = 0; i < 6; ++i) {
    checksum[i] = (polymod >> 5 * (5 - i)) & 31;
  }
  size_t combined_len = 36 + 6;
  uint8_t combined[combined_len];
  memcpy(combined, data_part, 36);
  memcpy(combined + 36, checksum, 6);
  sprintf(address, "%s1", network);
  for (size_t i = 0; i < combined_len; ++i) {
    address[strlen(network) + 1 + i] = CHARSET[combined[i]];
  }
  address[strlen(network) + 1 + combined_len] = '\0';
}

void nervos_sign_sighash(HDNode *node, const uint8_t *data_initial_chunk,
                         uint32_t data_initial_chunk_len,
                         const uint8_t *witness_buffer,
                         uint32_t witness_buffer_len, uint8_t *signature,
                         pb_size_t *signature_len) {
  uint8_t hash_output[32];
  ckb_hash(data_initial_chunk, data_initial_chunk_len, hash_output);
  blake2b_state S;
  ckb_hasher_init(&S);
  blake2b_Update(&S, hash_output, 32);

  uint8_t buffer[8];
  size_t buffer_len = 0;
  extend_uint64(buffer, &buffer_len, witness_buffer_len);
  blake2b_Update(&S, buffer, 8);
  blake2b_Update(&S, witness_buffer, witness_buffer_len);
  uint8_t output[32];
  blake2b_Final(&S, output, 32);

  uint8_t v1;
  uint8_t sig[64];

  if (ecdsa_sign_digest(&secp256k1, node->private_key, output, sig, &v1,
                        NULL) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
  }

  memcpy(signature, sig, 64);
  signature[64] = v1;
  *signature_len = 65;
}

static void send_request_chunk(void) {
  msg_tx_request.has_data_length = true;
  msg_tx_request.data_length = data_left <= 1024 ? data_left : 1024;
  msg_write(MessageType_MessageType_NervosTxRequest, &msg_tx_request);
}

// In the case of multi-package, initialize first
void nervos_sign_sighash_init(HDNode *node, const uint8_t *data_initial_chunk,
                              uint32_t data_initial_chunk_len,
                              const uint8_t *witness_buffer,
                              uint32_t witness_buffer_len,
                              uint32_t data_length) {
  nervos_signing = true;
  globalNode = malloc(sizeof(HDNode));

  if (globalNode != NULL) {
    memcpy(globalNode, node, sizeof(HDNode));
  }
  uint8_t *temp = malloc(witness_buffer_len);
  memcpy(temp, witness_buffer, witness_buffer_len);
  global_witness_buffer = temp;
  witness_buffer_len_nervos = witness_buffer_len;
  data_total = data_length;
  data_left = data_total - data_initial_chunk_len;
  global_hasher_init();
  global_hash_update(data_initial_chunk, data_initial_chunk_len);
  send_request_chunk();
}

void nervos_signing_txack(const NervosTxAck *tx) {
  if (!nervos_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    _("Not in nervos signing mode"));
    layoutHome();
    return;
  }
  if (tx->data_chunk.size > data_left) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Too much data"));
    nervos_signing_abort();
    return;
  }
  if (data_left > 0 && tx->data_chunk.size == 0) {
    fsm_sendFailure(FailureType_Failure_DataError,
                    _("Empty data chunk received"));
    nervos_signing_abort();
    return;
  }
  global_hash_update(tx->data_chunk.bytes, tx->data_chunk.size);
  data_left -= tx->data_chunk.size;
  if (data_left > 0) {
    send_request_chunk();
  } else {
    global_hash_finalize(global_hash_output);
    send_signature();
  }
}

void send_signature(void) {
  NervosSignedTx tx_resp;
  blake2b_state S;
  ckb_hasher_init(&S);
  blake2b_Update(&S, global_hash_output, 32);
  uint8_t buffer[8];
  size_t buffer_len = 0;
  debugInt(witness_buffer_len_nervos);

  extend_uint64(buffer, &buffer_len, witness_buffer_len_nervos);
  blake2b_Update(&S, buffer, 8);
  blake2b_Update(&S, global_witness_buffer, witness_buffer_len_nervos);
  uint8_t output[32];
  blake2b_Final(&S, output, 32);
  uint8_t v1;
  uint8_t sig[64];
  if (ecdsa_sign_digest(&secp256k1, globalNode->private_key, output, sig, &v1,
                        NULL) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
  }
  memcpy(tx_resp.signature.bytes, sig, 64);
  tx_resp.signature.bytes[64] = v1;
  tx_resp.signature.size = 65;
  msg_write(MessageType_MessageType_NervosSignedTx, &tx_resp);
  layoutHome();
}

void nervos_signing_abort(void) {
  if (globalNode != NULL) {
    free(globalNode);
    globalNode = NULL;
  }
  if (global_witness_buffer != NULL) {
    free((void *)global_witness_buffer);
    global_witness_buffer = NULL;
  }
  data_total = 0;
  data_left = 0;
  witness_buffer_len_nervos = 0;
  nervos_signing = false;
  global_witness_buffer = NULL;
  memset(global_hash_output, 0, sizeof(global_hash_output));
  memset(&S_GLOBAL, 0, sizeof(S_GLOBAL));
  memset(&msg_tx_request, 0, sizeof(msg_tx_request));
}