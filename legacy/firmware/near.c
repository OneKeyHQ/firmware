/*
 * This file is part of the OneKey project, https://onekey.so/
 *
 * Copyright (C) 2022 OneKey Team <core@onekey.so>
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

#include "near.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "sha3.h"
#include "util.h"

typedef enum {
  at_create_account,
  at_deploy_contract,
  at_function_call,
  at_transfer,
  at_stake,
  at_add_key,
  at_delete_key,
  at_delete_account,
  at_mul_action,
  at_last_value = at_mul_action
} action_type_t;

void near_get_address_from_public_key(const uint8_t *public_key,
                                      char *address) {
  const char *hex = "0123456789abcdef";
  for (uint32_t i = 0; i < 32; i++) {
    address[i * 2] = hex[(public_key[i] >> 4) & 0xF];
    address[i * 2 + 1] = hex[public_key[i] & 0xF];
  }
  address[32 * 2] = 0;
}

/*
 Adapted from https://en.wikipedia.org/wiki/Double_dabble#C_implementation
 Returns: length of resulting string or -1 for error
*/
static int format_long_int_amount(size_t input_size, char *input,
                                  size_t output_size, char *output) {
  // NOTE: Have to copy to have word-aligned array (otherwise crashing on read)
  // Lots of time has been lost debugging this, make sure to avoid unaligned RAM
  // access (as compiler in BOLOS SDK won't)
  uint16_t aligned_amount[8];
  memcpy(aligned_amount, input, 16);
  // Convert size in bytes into words
  size_t n = input_size / 2;

  size_t nbits = 16 * n;       /* length of arr in bits */
  size_t nscratch = nbits / 3; /* length of scratch in bytes */
  if (nscratch >= output_size) {
    // Output buffer is too small
    output[0] = '\0';
    return -1;
  }

  char *scratch = output;

  size_t i, j, k;
  size_t smin = nscratch - 2; /* speed optimization */

  for (i = 0; i < n; ++i) {
    for (j = 0; j < 16; ++j) {
      /* This bit will be shifted in on the right. */
      int shifted_in = (aligned_amount[n - i - 1] & (1 << (15 - j))) ? 1 : 0;

      /* Add 3 everywhere that scratch[k] >= 5. */
      for (k = smin; k < nscratch; ++k) {
        scratch[k] += (scratch[k] >= 5) ? 3 : 0;
      }

      /* Shift scratch to the left by one position. */
      if (scratch[smin] >= 8) {
        smin -= 1;
      }
      for (k = smin; k < nscratch - 1; ++k) {
        scratch[k] <<= 1;
        scratch[k] &= 0xF;
        scratch[k] |= (scratch[k + 1] >= 8);
      }

      /* Shift in the new bit from arr. */
      scratch[nscratch - 1] <<= 1;
      scratch[nscratch - 1] &= 0xF;
      scratch[nscratch - 1] |= shifted_in;
    }
  }

  /* Remove leading zeros from the scratch space. */
  for (k = 0; k < nscratch - 1; ++k) {
    if (scratch[k] != 0) {
      break;
    }
  }
  nscratch -= k;
  memmove(scratch, scratch + k, nscratch + 1);

  /* Convert the scratch space from BCD digits to ASCII. */
  for (k = 0; k < nscratch; ++k) {
    scratch[k] += '0';
  }

  /* Resize and return */
  memmove(output, scratch, nscratch + 1);
  return nscratch;
}

static int format_long_decimal_amount(size_t input_size, char *input,
                                      size_t output_size, char *output,
                                      int nomination) {
  int len = format_long_int_amount(input_size, input, output_size, output);
  if (len < 0 || (size_t)len + 2 > output_size) {
    // Output buffer is too small
    output[0] = '\0';
    return -1;
  }

  if (len <= nomination) {
    // < 1.0
    memmove(output + 2 + (nomination - len), output, len);
    /* coverity[bad_memset] */
    memset(output + 2, '0', (nomination - len));
    output[0] = '0';
    output[1] = '.';
    len = nomination + 2;
  } else {
    // >= 1.0
    int int_len = len - nomination;
    memmove(output + int_len + 1, output + int_len, nomination);
    output[int_len] = '.';
    len = len + 1;
  }

  // Remove trailing zeros
  output[len] = '0';
  while (len > 0 && output[len] == '0') {
    output[len--] = 0;
  }

  // Remove trailing dot
  if (output[len] == '.') {
    output[len] = 0;
  }

  return len;
}

static int check_overflow(const NearSignTx *msg, uint32_t processed,
                          uint32_t size) {
  if (size > msg->raw_tx.size || processed + size > msg->raw_tx.size) {
    return -1;
  }
  return 0;
}

static int borsh_read_uint8(const NearSignTx *msg, uint32_t *processed,
                            uint8_t *n) {
  if (check_overflow(msg, *processed, 1)) {
    return -1;
  }
  *n = *((uint8_t *)&msg->raw_tx.bytes[*processed]);
  *processed += 1;
  return 0;
}

static int borsh_read_uint32(const NearSignTx *msg, uint32_t *processed,
                             uint32_t *n) {
  if (check_overflow(msg, *processed, 4)) {
    return -1;
  }
  *n = *((uint32_t *)&msg->raw_tx.bytes[*processed]);
  *processed += 4;
  return 0;
}

static int borsh_read_buffer(const NearSignTx *msg, uint32_t *buffer_len,
                             const uint8_t **buffer, uint32_t *processed) {
  if (borsh_read_uint32(msg, processed, buffer_len)) {
    return -1;
  }
  if (check_overflow(msg, *processed, *buffer_len)) {
    return -1;
  }
  *buffer = &msg->raw_tx.bytes[*processed];
  *processed += *buffer_len;
  return 0;
}

// static int borsh_read_fixed_buffer(const NearSignTx *msg, uint32_t
// buffer_len, const uint8_t **buffer, uint32_t *processed) {
//     if (check_overflow(msg, *processed, buffer_len)) {
//         return -1;
//     }
//     *buffer = &msg->raw_tx.bytes[*processed];
//     *processed += buffer_len;
//     return 0;
// }

static int borsh_skip(const NearSignTx *msg, uint32_t *processed, uint32_t c) {
  if (check_overflow(msg, *processed, c)) {
    return -1;
  }
  *processed += c;
  return 0;
}

static void strcpy_ellipsis(size_t dst_size, char *dst, size_t src_size,
                            const char *src) {
  if (dst_size >= src_size + 1) {
    memcpy(dst, src, src_size);
    dst[src_size] = 0;
    return;
  }

  memcpy(dst, src, dst_size);
  size_t ellipsis_start = dst_size >= 4 ? dst_size - 4 : 0;
  for (size_t i = ellipsis_start; i < dst_size; i++) {
    dst[i] = '.';
  }
  dst[dst_size - 1] = 0;
  return;
}

// Parse the transaction details for the user to approve
static int parse_transaction(const NearSignTx *msg, uint32_t *processed,
                             char *receiver) {
  char singer[65] = {0};
  char *var_name = NULL;
  uint32_t len = 0;

  // singer
  if (borsh_read_buffer(msg, &len, (const uint8_t **)&var_name, processed)) {
    return -1;
  }
  if (var_name == NULL || len != 64) {
    return -1;
  }
  strcpy_ellipsis(sizeof(singer), singer, len, var_name);

  // public key
  if (borsh_skip(msg, processed, 33)) {
    return -1;
  }
  // nonce
  if (borsh_skip(msg, processed, 8)) {
    return -1;
  }
  // receiver
  if (borsh_read_buffer(msg, &len, (const uint8_t **)&var_name, processed)) {
    return -1;
  }
  if (var_name == NULL || len != 64) {
    return -1;
  }
  strcpy_ellipsis(sizeof(singer), singer, len, var_name);
  memcpy(receiver, singer, 65);

  // block hash
  if (borsh_skip(msg, processed, 32)) {
    return -1;
  }

  // actions
  uint32_t actions_len;
  if (borsh_read_uint32(msg, processed, &actions_len)) {
    return -1;
  }

  if (actions_len != 1) {
    return at_mul_action;
  }
  // TODO: Parse more than one action

  // action type
  uint8_t action_type;
  if (borsh_read_uint8(msg, processed, &action_type)) {
    return -1;
  }

  return action_type;
}

bool near_sign_tx(const NearSignTx *msg, const HDNode *node,
                  NearSignedTx *resp) {
  uint32_t processed = 0;
  char receiver[65] = {0};
  char amount[64] = {0};
  char address[65] = {0};
  char *var_name;
  near_get_address_from_public_key(node->public_key + 1, address);

  int action_type = parse_transaction(msg, &processed, receiver);
  switch (action_type) {
    case at_create_account:
      break;
    case at_deploy_contract:
      break;
    case at_function_call:
      break;
    case at_transfer:
      var_name = (char *)&msg->raw_tx.bytes[processed];
      processed += 16;
      if (-1 == format_long_decimal_amount(16, var_name, sizeof(amount), amount,
                                           24)) {
        return false;
      }
      break;
    case at_stake:
      break;
    case at_add_key:
      break;
    case at_delete_key:
      break;
    case at_delete_account:
      break;
    case at_mul_action:
      break;
    default:
      return false;
  }
  if (at_transfer == action_type) {
    char _to1[70] = "to ____________";
    char _amount_str[32] = {0};
    int i, receiver_len = strlen(receiver);
    bool has_dot = false;
    int len = strlen(amount);
    // retain 5 decimal places
    for (i = 0; i < len; i++) {
      _amount_str[i] = amount[i];
      if (amount[i] == '.') {
        has_dot = true;
        if ((len - i) >= 6) {
          memcpy(_amount_str + i, amount + i, 6);
          i += 6;
        } else {
          memcpy(_amount_str + i, amount + i, len - i);
          i += len - i;
        }
        break;
      }
    }
    len = strlen(_amount_str);
    if (0 == strncmp(_amount_str, "0.00000", 7)) {
      memcpy(_amount_str, "< 0.00001 NEAR", 15);
    } else if (has_dot) {  // rstrip("0") & rstrip(".")
      for (i = len - 1; i >= 0; i--) {
        if (_amount_str[i] == '0') {
          _amount_str[i] = 0;
        } else if (_amount_str[i] == '.') {
          _amount_str[i] = 0;
          break;
        } else {
          break;
        }
      }
      memcpy(_amount_str + strlen(_amount_str), " NEAR", 6);
    } else {
      memcpy(_amount_str + len, " NEAR", 6);
    }
    memcpy(_to1 + 3, receiver, receiver_len);
    memcpy(_to1 + 3 + receiver_len, "?", 2);

    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Send"), _amount_str, _to1, NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      return false;
    }
  } else {
    if (!layoutBlindSign("Near", address)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
      layoutHome();
      return false;
    }
  }

  // hash the tx
  uint8_t hash[32];
  SHA256_CTX ctx;
  sha256_Init(&ctx);
  sha256_Update(&ctx, msg->raw_tx.bytes, msg->raw_tx.size);
  sha256_Final(&ctx, hash);

  ed25519_sign(hash, 32, node->private_key, resp->signature.bytes);

  resp->signature.size = 64;
  return true;
}
