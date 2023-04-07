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

#include "conflux.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "address.h"
#include "conflux_tokens.h"
#include "crypto.h"
#include "ecdsa.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "secp256k1.h"
#include "sha3.h"
#include "transaction.h"
#include "util.h"

/* maximum supported chain id.  v must fit in an uint32_t. */
#define MAX_CHAIN_ID 65535
#define MAIN_NET_PREFIX = 'cfx:'
#define TEST_NET_PREFIX = 'cfxtest:'
#define CHAIN_ID_MAINNET 1029
#define CHAIN_ID_TESTNET 1
#define VERSION_BYTE 0x00

#define CFXADDR_SUCCESS 0
#define CFXADDR_ERROR_WRONG_LENGTH 1

static bool conflux_signing = false;
static uint32_t data_total, data_left;
static ConfluxTxRequest msg_tx_request;
static CONFIDENTIAL uint8_t privkey[32];
static uint32_t chain_id;
struct SHA3_CTX keccak_ctx_cfx = {0};

void get_ethereum_format_address(uint8_t pubkeyhash[20], char *address) {
  const char *hex = "0123456789abcdef";
  // change the first nibble of pubkeyhash to 0x1
  pubkeyhash[0] = (pubkeyhash[0] & 0x0F) | 0x10;
  for (int i = 0; i < 20; i++) {
    address[i * 2] = hex[(pubkeyhash[i] >> 4) & 0x0F];
    address[i * 2 + 1] = hex[pubkeyhash[i] & 0x0F];
  }
  address[40] = '\0';
}

static uint64_t polymod(const uint8_t *v, const uint8_t in_len) {
  uint64_t c = 1;
  for (int i = 0; i < in_len; i++) {
    uint8_t c0 = c >> 35;
    c = ((c & 0x07ffffffff) << 5) ^ v[i];
    if (c0 & 0x01) c ^= 0x98f2bc8e61;
    if (c0 & 0x02) c ^= 0x79b76d99e2;
    if (c0 & 0x04) c ^= 0xf33e5fb3c4;
    if (c0 & 0x08) c ^= 0xae2eabe2a8;
    if (c0 & 0x10) c ^= 0x1e4f43e470;
  }
  return c ^ 1;
}
int get_base32_encode_address(uint8_t *in, char *out, size_t out_len,
                              uint32_t network_id, bool is_sign) {
  static char CHARSET[] = "abcdefghjkmnprstuvwxyz0123456789";

  char prefix[10];  // the longest prefix is "net" + "65535" + ':' + '\0'
  uint8_t prefix_len = 0;
  // change the first nibble of the pubkeyhash to 0x1
  if (!is_sign) {
    in[0] = (in[0] & 0x0f) | 0x10;
  }
  // initialize prefix based on network_id
  switch (network_id) {
    case CHAIN_ID_MAINNET: {
      if (out_len < 4 + 34 + 8 + 1) {
        return CFXADDR_ERROR_WRONG_LENGTH;
        return CFXADDR_ERROR_WRONG_LENGTH;
      };
      memcpy(prefix, "cfx:", 4);
      prefix_len = 4;
      break;
    }

    case CHAIN_ID_TESTNET: {
      if (out_len < (size_t)(8 + 34 + 8 + 1)) {
        return CFXADDR_ERROR_WRONG_LENGTH;
      };
      memcpy(prefix, "cfxtest:", 8);
      prefix_len = 8;
      break;
    }

    default: {
      uint8_t length = 1;
      uint16_t netid = network_id / 10;
      while (netid > 0) {
        length += 1;
        netid /= 10;
      }

      if (out_len < (size_t)(length + 34 + 8 + 1)) {
        return CFXADDR_ERROR_WRONG_LENGTH;
      };
#if !EMULATOR
      snprintf(prefix, 10, "net%ld:", network_id);
#else
      snprintf(prefix, 10, "net%d:", network_id);
#endif
      prefix_len = length + 4;
    }
  }

  // store "extended prefix"
  for (size_t ii = 0; ii < (size_t)(prefix_len - 1); ++ii) {
    out[ii] = (uint8_t)prefix[ii] & 0x1f;
  }

  out[prefix_len - 1] = 0x00;

  // convert 21 x 8bits (version byte + payload) to 34 x 5bits
  // treat as if version byte 0x00 is already in acc
  uint16_t acc = VERSION_BYTE;
  uint8_t num = 8;
  uint8_t buf_len = prefix_len;

  for (size_t ii = 0; ii < 20; ++ii) {
    acc = (acc << 8 | (uint8_t)(in[ii]));
    num += 8;

    while (num >= 5) {
      out[buf_len++] = (uint8_t)(acc >> (num - 5));
      acc &= ~(0x1f << (num - 5));
      num -= 5;
    }
  }

  out[buf_len++] = (uint8_t)(acc << (5 - num));

  // calculate checksum
  memset((uint8_t *)out + buf_len, 0x00, 8);
  uint64_t checksum = polymod((uint8_t *)out, buf_len + 8);

  // convert to characters
  for (size_t ii = 0; ii < prefix_len; ++ii) {
    out[ii] = prefix[ii];
  }

  for (size_t ii = 0; ii < 34; ++ii) {
    out[prefix_len + ii] = CHARSET[(uint8_t)out[prefix_len + ii]];
  }

  for (size_t ii = 0; ii < 8; ++ii) {
    out[prefix_len + 34 + ii] =
        CHARSET[(uint8_t)(checksum >> ((7 - ii) * 5) & 0x1f)];
  }

  out[prefix_len + 34 + 8] = '\0';

  return CFXADDR_SUCCESS;
}

static inline void hash_data(const uint8_t *buf, size_t size) {
  sha3_Update(&keccak_ctx_cfx, buf, size);
}

/*
 * Push an RLP encoded length to the hash buffer.
 */
static void hash_rlp_length(uint32_t length, uint8_t firstbyte) {
  uint8_t buf[4] = {0};
  if (length == 1 && firstbyte <= 0x7f) {
    /* empty length header */
  } else if (length <= 55) {
    buf[0] = 0x80 + length;
    hash_data(buf, 1);
  } else if (length <= 0xff) {
    buf[0] = 0xb7 + 1;
    buf[1] = length;
    hash_data(buf, 2);
  } else if (length <= 0xffff) {
    buf[0] = 0xb7 + 2;
    buf[1] = length >> 8;
    buf[2] = length & 0xff;
    hash_data(buf, 3);
  } else {
    buf[0] = 0xb7 + 3;
    buf[1] = length >> 16;
    buf[2] = length >> 8;
    buf[3] = length & 0xff;
    hash_data(buf, 4);
  }
}

/*
 * Push an RLP encoded list length to the hash buffer.
 */
static void hash_rlp_list_length(uint32_t length) {
  uint8_t buf[4] = {0};
  if (length <= 55) {
    buf[0] = 0xc0 + length;
    hash_data(buf, 1);
  } else if (length <= 0xff) {
    buf[0] = 0xf7 + 1;
    buf[1] = length;
    hash_data(buf, 2);
  } else if (length <= 0xffff) {
    buf[0] = 0xf7 + 2;
    buf[1] = length >> 8;
    buf[2] = length & 0xff;
    hash_data(buf, 3);
  } else {
    buf[0] = 0xf7 + 3;
    buf[1] = length >> 16;
    buf[2] = length >> 8;
    buf[3] = length & 0xff;
    hash_data(buf, 4);
  }
}

/*
 * Push an RLP encoded length field and data to the hash buffer.
 */
static void hash_rlp_field(const uint8_t *buf, size_t size) {
  hash_rlp_length(size, buf[0]);
  hash_data(buf, size);
}

/*
 * Push an RLP encoded number to the hash buffer.
 * Conflux yellow paper says to convert to big endian and strip leading zeros.
 */
static void hash_rlp_number(uint32_t number) {
  if (!number) {
    return;
  }
  uint8_t data[4] = {0};
  data[0] = (number >> 24) & 0xff;
  data[1] = (number >> 16) & 0xff;
  data[2] = (number >> 8) & 0xff;
  data[3] = (number)&0xff;
  int offset = 0;
  while (!data[offset]) {
    offset++;
  }
  hash_rlp_field(data + offset, 4 - offset);
}

/*
 * Calculate the number of bytes needed for an RLP length header.
 * NOTE: supports up to 16MB of data (how unlikely...)
 * FIXME: improve
 */
static int rlp_calculate_length(int length, uint8_t firstbyte) {
  if (length == 1 && firstbyte <= 0x7f) {
    return 1;
  } else if (length <= 55) {
    return 1 + length;
  } else if (length <= 0xff) {
    return 2 + length;
  } else if (length <= 0xffff) {
    return 3 + length;
  } else {
    return 4 + length;
  }
}

static int rlp_calculate_number_length(uint32_t number) {
  if (number <= 0x7f) {
    return 1;
  } else if (number <= 0xff) {
    return 2;
  } else if (number <= 0xffff) {
    return 3;
  } else if (number <= 0xffffff) {
    return 4;
  } else {
    return 5;
  }
}

static void send_request_chunk(void) {
  int progress = 1000 - (data_total > 1000000 ? data_left / (data_total / 800)
                                              : data_left * 800 / data_total);
  layoutProgressAdapter(_("Signing"), progress);
  msg_tx_request.has_data_length = true;
  msg_tx_request.data_length = data_left <= 1024 ? data_left : 1024;
  msg_write(MessageType_MessageType_ConfluxTxRequest, &msg_tx_request);
}

static int conflux_is_canonic(uint8_t v, uint8_t signature[64]) {
  (void)signature;
  return (v & 2) == 0;
}

static void send_signature(void) {
  uint8_t hash[32] = {0}, sig[64] = {0};
  uint8_t v = 0;
  layoutProgressAdapter(_("Signing"), 1000);

  keccak_Final(&keccak_ctx_cfx, hash);
  if (ecdsa_sign_digest(&secp256k1, privkey, hash, sig, &v,
                        conflux_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    conflux_signing_abort();
    return;
  }

  memzero(privkey, sizeof(privkey));

  /* Send back the result */
  msg_tx_request.has_data_length = false;

  msg_tx_request.has_signature_v = true;
  msg_tx_request.signature_v = v;

  msg_tx_request.has_signature_r = true;
  msg_tx_request.signature_r.size = 32;
  memcpy(msg_tx_request.signature_r.bytes, sig, 32);

  msg_tx_request.has_signature_s = true;
  msg_tx_request.signature_s.size = 32;
  memcpy(msg_tx_request.signature_s.bytes, sig + 32, 32);

  msg_write(MessageType_MessageType_ConfluxTxRequest, &msg_tx_request);

  conflux_signing_abort();
}
/* Format a 256 bit number (amount in drip) into a human readable format
 * using standard Conflux units.
 * The buffer must be at least 25 bytes.
 */
static void confluxFormatAmount(const bignum256 *amnt,
                                const ConfluxTokenType *token, char *buf,
                                int buflen) {
  bignum256 bn1e9 = {0};
  bn_read_uint32(1000000000, &bn1e9);
  const char *suffix = NULL;
  int decimals = 18;
  if (token == ConfluxUnknownToken) {
    strlcpy(buf, "Unknown token value", buflen);
    return;
  } else if (token != NULL) {
    suffix = token->symbol;
    decimals = token->decimals;
  } else if (bn_is_less(amnt, &bn1e9)) {
    suffix = " Drip";
    decimals = 0;
  } else {
    suffix = " CFX";
    bn_format(amnt, NULL, suffix, decimals, 0, false, 0, buf, buflen);
    // Take 5 decimal places(ref coinmarkercap).
    // Note that there may be a loss of precision between the real price value
    // and what is shown to the user
    char *dot_start = strchr(buf, '.');
    if (dot_start) {
      char *dot_end = strchr(dot_start, ' ');
      if ((dot_end - dot_start) > 6) {
        snprintf(dot_start + 6, 5, "%s", " CFX");
      }
    }
    return;
  }
  bn_format(amnt, NULL, suffix, decimals, 0, false, 0, buf, buflen);
}

static void layoutConfluxConfirmTx(uint8_t *to, uint32_t to_len,
                                   const uint8_t *value, uint32_t value_len,
                                   const ConfluxTokenType *token,
                                   uint32_t chain_ids) {
  bignum256 val = {0};
  uint8_t pad_val[32] = {0};
  memzero(pad_val, sizeof(pad_val));
  memcpy(pad_val + (32 - value_len), value, value_len);
  bn_read_be(pad_val, &val);

  char amount[36] = {0};
  if (token == NULL) {
    if (bn_is_zero(&val)) {
      strcpy(amount, _("message"));
    } else {
      confluxFormatAmount(&val, NULL, amount, sizeof(amount));
    }
  } else {
    confluxFormatAmount(&val, token, amount, sizeof(amount));
  }

  char _to1[30] = "to ____________";
  char _to2[30] = "______________";
  char _to3[] = "________________?";

  if (to_len) {
    char to_str[52] = {0};
    get_base32_encode_address(to, to_str, sizeof(to_str), chain_ids, true);
    if (oledStringWidthAdapter(amount, FONT_STANDARD) > (OLED_WIDTH - 20)) {
      memcpy(_to1 + 3, to_str, 20);
      memcpy(_to2, to_str + 20, strlen(to_str) - 20);
    } else {
      memcpy(_to1 + 3, to_str, 16);
      memcpy(_to2, to_str + 16, strlen(to_str) - 32);
      memcpy(_to3, to_str + (strlen(to_str) - 16), 16);
    }

  } else {
    strlcpy(_to1, _("to new contract?"), sizeof(_to1));
    strlcpy(_to2, "", sizeof(_to2));
    strlcpy(_to3, "", sizeof(_to3));
  }
  if (oledStringWidthAdapter(amount, FONT_STANDARD) > (OLED_WIDTH - 20)) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Send"), amount, NULL, _to1, _to2, NULL);
  } else {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Send"), amount, _to1, _to2, _to3, NULL);
  }
}

static void layoutConfluxData(const uint8_t *data, uint32_t len,
                              uint32_t total_len) {
  char hexdata[3][17] = {0};
  char summary[20] = {0};
  uint32_t printed = 0;
  for (int i = 0; i < 3; i++) {
    uint32_t linelen = len - printed;
    if (linelen > 8) {
      linelen = 8;
    }
    data2hex(data, linelen, hexdata[i]);
    data += linelen;
    printed += linelen;
  }

  strcpy(summary, "...          bytes");
  char *p = summary + 11;
  uint32_t number = total_len;
  while (number > 0) {
    *p-- = '0' + number % 10;
    number = number / 10;
  }
  char *summarystart = summary;
  if (total_len == printed) summarystart = summary + 4;

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Transaction data:"), hexdata[0], hexdata[1], hexdata[2],
                    summarystart, NULL);
}

static void layoutConfluxFee(const uint8_t *value, uint32_t value_len,
                             const uint8_t *gas_price, uint32_t gas_price_len,
                             const uint8_t *gas_limit, uint32_t gas_limit_len,
                             bool is_token) {
  bignum256 val = {0}, gas = {0};
  uint8_t pad_val[32] = {0};
  char tx_value[36] = {0};
  char gas_value[32] = {0};

  memzero(tx_value, sizeof(tx_value));
  memzero(gas_value, sizeof(gas_value));

  memzero(pad_val, sizeof(pad_val));
  memcpy(pad_val + (32 - gas_price_len), gas_price, gas_price_len);
  bn_read_be(pad_val, &val);

  memzero(pad_val, sizeof(pad_val));
  memcpy(pad_val + (32 - gas_limit_len), gas_limit, gas_limit_len);
  bn_read_be(pad_val, &gas);
  bn_multiply(&val, &gas, &secp256k1.prime);

  confluxFormatAmount(&gas, NULL, gas_value, sizeof(gas_value));

  memzero(pad_val, sizeof(pad_val));
  memcpy(pad_val + (32 - value_len), value, value_len);
  bn_read_be(pad_val, &val);

  if (bn_is_zero(&val)) {
    strcpy(tx_value, is_token ? _("token") : _("message"));
  } else {
    confluxFormatAmount(&val, NULL, tx_value, sizeof(tx_value));
  }
  if (oledStringWidthAdapter(tx_value, FONT_STANDARD) > (OLED_WIDTH - 20)) {
    char buf[64] = {0};
    strcat(buf, _("paying up to"));
    strcat(buf, _(" "));
    strcat(buf, gas_value);
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Really send"), tx_value, NULL, buf, _("for gas?"),
                      NULL);
  } else {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Really send"), tx_value, _("paying up to"), gas_value,
                      _("for gas?"), NULL);
  }
}

/*
 * RLP fields:
 * - nonce (0 .. 32)
 * - gas_price (0 .. 32)
 * - gas_limit (0 .. 32)
 * - to (0, 20)
 * - value (0 .. 32)
 * - storage_limit (0 .. 32)
 * - epoch_height (0 .. 32)
 * - chain_id (0 .. 32)
 * - data (0 ..)
 */

static bool conflux_signing_check(const ConfluxSignTx *msg) {
  if (!msg->has_gas_price || !msg->has_gas_limit || !msg->has_epoch_height ||
      !msg->has_storage_limit) {
    return false;
  }

  size_t tolen = msg->has_to ? strlen(msg->to) : 0;

  if (tolen != 42 && tolen != 40 && tolen != 0) {
    /* Address has wrong length */
    return false;
  }

  // sending transaction to address 0 (contract creation) without a data field
  if (tolen == 0 && (!msg->has_data_length || msg->data_length == 0)) {
    return false;
  }

  if (msg->gas_price.size + msg->gas_limit.size + msg->storage_limit.size >
      30) {
    // sanity check that fee doesn't overflow
    return false;
  }

  return true;
}

void conflux_signing_init(ConfluxSignTx *msg, const HDNode *node) {
  conflux_signing = true;
  sha3_256_Init(&keccak_ctx_cfx);

  memzero(&msg_tx_request, sizeof(ConfluxTxRequest));
  /* set fields to 0, to avoid conditions later */
  if (!msg->has_value) msg->value.size = 0;
  if (!msg->has_data_initial_chunk) msg->data_initial_chunk.size = 0;
  bool toset;
  uint8_t pubkeyhash[20] = {0};
  if (msg->has_to && conflux_parse(msg->to, pubkeyhash)) {
    toset = true;
  } else {
    msg->to[0] = 0;
    toset = false;
    memzero(pubkeyhash, sizeof(pubkeyhash));
  }
  if (!msg->has_nonce) msg->nonce.size = 0;

  if (msg->has_chain_id) {
    if (msg->chain_id < 1 || msg->chain_id > MAX_CHAIN_ID) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Chain Id out of bounds"));
      conflux_signing_abort();
      return;
    }
    chain_id = msg->chain_id;
  } else {
    // cfx mainnet
    chain_id = CHAIN_ID_MAINNET;
  }

  if (msg->has_data_length && msg->data_length > 0) {
    if (!msg->has_data_initial_chunk || msg->data_initial_chunk.size == 0) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Data length provided, but no initial chunk"));
      conflux_signing_abort();
      return;
    }
    /* Our encoding only supports transactions up to 2^24 bytes.  To
     * prevent exceeding the limit we use a stricter limit on data length.
     */
    if (msg->data_length > 16000000) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Data length exceeds limit"));
      conflux_signing_abort();
      return;
    }
    data_total = msg->data_length;
  } else {
    data_total = 0;
  }
  if (msg->data_initial_chunk.size > data_total) {
    fsm_sendFailure(FailureType_Failure_DataError,
                    _("Invalid size of initial chunk"));
    conflux_signing_abort();
    return;
  }

  // safety checks
  if (!conflux_signing_check(msg)) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Safety check failed"));
    conflux_signing_abort();
    return;
  }
  const ConfluxTokenType *token = NULL;

  // detect ERC-20 like token
  if (toset && msg->value.size == 0 && data_total == 68 &&
      msg->data_initial_chunk.size == 68 &&
      memcmp(msg->data_initial_chunk.bytes,
             "\xa9\x05\x9c\xbb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
             16) == 0) {
    char to_str[52] = {0};
    get_base32_encode_address(pubkeyhash, to_str, sizeof(to_str), msg->chain_id,
                              true);
    token = tokenByAddress(to_str);
  }

  if (token != NULL) {
    layoutConfluxConfirmTx(msg->data_initial_chunk.bytes + 16, 20,
                           msg->data_initial_chunk.bytes + 36, 32, token,
                           msg->chain_id);
  } else {
    if (toset) {
      layoutConfluxConfirmTx(pubkeyhash, 20, msg->value.bytes, msg->value.size,
                             NULL, msg->chain_id);
    } else {
      layoutConfluxConfirmTx(pubkeyhash, 0, msg->value.bytes, msg->value.size,
                             NULL, msg->chain_id);
    }
  }

  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    conflux_signing_abort();
    return;
  }

  if (data_total > 0) {
    layoutConfluxData(msg->data_initial_chunk.bytes,
                      msg->data_initial_chunk.size, data_total);
    if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      conflux_signing_abort();
      return;
    }
  }

  if (token != NULL) {
    layoutConfluxFee(msg->value.bytes, msg->value.size, msg->gas_price.bytes,
                     msg->gas_price.size, msg->gas_limit.bytes,
                     msg->gas_limit.size, true);
  } else {
    layoutConfluxFee(msg->value.bytes, msg->value.size, msg->gas_price.bytes,
                     msg->gas_price.size, msg->gas_limit.bytes,
                     msg->gas_limit.size, false);
  }
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    conflux_signing_abort();
    return;
  }

  /* Stage 1: Calculate total RLP length */
  uint32_t rlp_length = 0;

  layoutProgressAdapter(_("Signing"), 0);

  rlp_length += rlp_calculate_length(msg->nonce.size, msg->nonce.bytes[0]);
  rlp_length +=
      rlp_calculate_length(msg->gas_price.size, msg->gas_price.bytes[0]);
  rlp_length +=
      rlp_calculate_length(msg->gas_limit.size, msg->gas_limit.bytes[0]);
  rlp_length += rlp_calculate_length(toset ? 20 : 0, pubkeyhash[0]);
  rlp_length += rlp_calculate_length(msg->value.size, msg->value.bytes[0]);
  rlp_length += rlp_calculate_length(msg->storage_limit.size,
                                     msg->storage_limit.bytes[0]);
  rlp_length +=
      rlp_calculate_length(msg->epoch_height.size, msg->epoch_height.bytes[0]);
  rlp_length +=
      rlp_calculate_length(data_total, msg->data_initial_chunk.bytes[0]);

  rlp_length += rlp_calculate_number_length(chain_id);

  /* Stage 2: Store header fields */
  hash_rlp_list_length(rlp_length);

  layoutProgressAdapter(_("Signing"), 100);

  hash_rlp_field(msg->nonce.bytes, msg->nonce.size);
  hash_rlp_field(msg->gas_price.bytes, msg->gas_price.size);
  hash_rlp_field(msg->gas_limit.bytes, msg->gas_limit.size);
  hash_rlp_field(pubkeyhash, toset ? 20 : 0);
  hash_rlp_field(msg->value.bytes, msg->value.size);
  hash_rlp_field(msg->storage_limit.bytes, msg->storage_limit.size);
  hash_rlp_field(msg->epoch_height.bytes, msg->epoch_height.size);
  hash_rlp_number(chain_id);
  hash_rlp_length(data_total, msg->data_initial_chunk.bytes[0]);
  hash_data(msg->data_initial_chunk.bytes, msg->data_initial_chunk.size);
  data_left = data_total - msg->data_initial_chunk.size;

  memcpy(privkey, node->private_key, 32);

  if (data_left > 0) {
    send_request_chunk();
  } else {
    send_signature();
  }
}

void conflux_signing_txack(const ConfluxTxAck *tx) {
  if (!conflux_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    _("Not in Conflux signing mode"));
    layoutHome();
    return;
  }

  if (tx->data_chunk.size > data_left) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Too much data"));
    conflux_signing_abort();
    return;
  }

  if (data_left > 0 && (!tx->has_data_chunk || tx->data_chunk.size == 0)) {
    fsm_sendFailure(FailureType_Failure_DataError,
                    _("Empty data chunk received"));
    conflux_signing_abort();
    return;
  }

  hash_data(tx->data_chunk.bytes, tx->data_chunk.size);

  data_left -= tx->data_chunk.size;

  if (data_left > 0) {
    send_request_chunk();
  } else {
    send_signature();
  }
}

void conflux_signing_abort(void) {
  if (conflux_signing) {
    memzero(privkey, sizeof(privkey));
    layoutHome();
    conflux_signing = false;
  }
}

static void conflux_message_hash(const uint8_t *message, size_t message_len,
                                 uint8_t hash[32]) {
  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, (const uint8_t *)"\x19" "Conflux Signed Message:\n", 25);
  uint8_t c = 0;
  if (message_len >= 1000000000) {
    c = '0' + message_len / 1000000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 100000000) {
    c = '0' + message_len / 100000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 10000000) {
    c = '0' + message_len / 10000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 1000000) {
    c = '0' + message_len / 1000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 100000) {
    c = '0' + message_len / 100000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 10000) {
    c = '0' + message_len / 10000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 1000) {
    c = '0' + message_len / 1000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 100) {
    c = '0' + message_len / 100 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 10) {
    c = '0' + message_len / 10 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  c = '0' + message_len % 10;
  sha3_Update(&ctx, &c, 1);
  sha3_Update(&ctx, message, message_len);
  keccak_Final(&ctx, hash);
}

void conflux_message_sign(const ConfluxSignMessage *msg, const HDNode *node,
                          ConfluxMessageSignature *resp) {
  uint8_t pubkeyhash[20] = {0};
  if (!hdnode_get_ethereum_pubkeyhash(node, pubkeyhash)) {
    return;
  }

  resp->has_address = true;
  resp->address[0] = '0';
  resp->address[1] = 'x';
  get_ethereum_format_address(pubkeyhash, &resp->address[2]);
  uint8_t hash[32] = {0};
  conflux_message_hash(msg->message.bytes, msg->message.size, hash);

  uint8_t v = 0;
  if (ecdsa_sign_digest(&secp256k1, node->private_key, hash,
                        resp->signature.bytes, &v, conflux_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    return;
  }

  resp->has_signature = true;
  resp->signature.bytes[64] = v;
  resp->signature.size = 65;
  msg_write(MessageType_MessageType_ConfluxMessageSignature, resp);
}

bool conflux_parse(const char *address, uint8_t pubkeyhash[20]) {
  memzero(pubkeyhash, 20);
  size_t len = strlen(address);
  if (len == 40) {
    // do nothing
  } else if (len == 42) {
    // check for "0x" prefix and strip it when required
    if (address[0] != '0') return false;
    if (address[1] != 'x' && address[1] != 'X') return false;
    address += 2;
    len -= 2;
  } else {
    return false;
  }
  for (size_t i = 0; i < len; i++) {
    if (address[i] >= '0' && address[i] <= '9') {
      pubkeyhash[i / 2] |= (address[i] - '0') << ((1 - (i % 2)) * 4);
    } else if (address[i] >= 'a' && address[i] <= 'f') {
      pubkeyhash[i / 2] |= ((address[i] - 'a') + 10) << ((1 - (i % 2)) * 4);
    } else if (address[i] >= 'A' && address[i] <= 'F') {
      pubkeyhash[i / 2] |= ((address[i] - 'A') + 10) << ((1 - (i % 2)) * 4);
    } else {
      return false;
    }
  }
  return true;
}

void conflux_message_sign_cip23(const ConfluxSignMessageCIP23 *msg,
                                const HDNode *node,
                                ConfluxMessageSignature *resp) {
  uint8_t pubkeyhash[20] = {0};
  if (!hdnode_get_ethereum_pubkeyhash(node, pubkeyhash)) {
    return;
  }

  resp->has_address = true;
  resp->address[0] = '0';
  resp->address[1] = 'x';
  get_ethereum_format_address(pubkeyhash, resp->address + 2);

  uint8_t hash[32] = {0};
  struct SHA3_CTX ctx = {0};

  sha3_256_Init(&ctx);
  sha3_Update(&ctx, (const uint8_t *)"\x19\x01",
              2);  // CIP23 magic is the same as EIP712
  sha3_Update(&ctx, msg->domain_hash.bytes, msg->domain_hash.size);
  sha3_Update(&ctx, msg->message_hash.bytes, msg->message_hash.size);
  keccak_Final(&ctx, hash);

  uint8_t v = 0;
  if (ecdsa_sign_digest(&secp256k1, node->private_key, hash,
                        resp->signature.bytes, &v, conflux_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    return;
  }

  resp->has_signature = true;
  resp->signature.bytes[64] = v;
  resp->signature.size = 65;
  msg_write(MessageType_MessageType_ConfluxMessageSignature, resp);
}
