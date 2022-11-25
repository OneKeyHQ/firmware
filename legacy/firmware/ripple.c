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
#include "ripemd160.h"
#include "ripple.h"
#include "secp256k1.h"
#include "sha2.h"
#include "transaction.h"
#include "util.h"

static const char bas_e58_alphabet[] = {
    'r', 'p', 's', 'h', 'n', 'a', 'f', '3', '9', 'w', 'B', 'U', 'D', 'N', 'E',
    'G', 'H', 'J', 'K', 'L', 'M', '4', 'P', 'Q', 'R', 'S', 'T', '7', 'V', 'W',
    'X', 'Y', 'Z', '2', 'b', 'c', 'd', 'e', 'C', 'g', '6', '5', 'j', 'k', 'm',
    '8', 'o', 'F', 'q', 'i', '1', 't', 'u', 'v', 'A', 'x', 'y', 'z'};

static size_t xrp_encode_base58_address(const uint8_t *in, size_t inlen,
                                        char *out, size_t outlen) {
  unsigned char buffer[26 * 138 / 100 + 1] = {0};
  size_t i = 0, j;
  size_t start_at, stop_at;
  size_t zero_count = 0;
  size_t output_size;

  while ((zero_count < inlen) && (in[zero_count] == 0)) {
    ++zero_count;
  }

  output_size = (inlen - zero_count) * 138 / 100 + 1;
  stop_at = output_size - 1;
  for (start_at = zero_count; start_at < inlen; start_at++) {
    int carry = in[start_at];
    for (j = output_size - 1; (int)j >= 0; j--) {
      carry += 256 * buffer[j];
      buffer[j] = carry % 58;
      carry /= 58;

      if (j <= stop_at - 1 && carry == 0) {
        break;
      }
    }
    stop_at = j;
  }

  j = 0;
  while (j < output_size && buffer[j] == 0) {
    j += 1;
  }

  memset(out, bas_e58_alphabet[0], zero_count);

  i = zero_count;
  while (j < output_size) {
    out[i++] = bas_e58_alphabet[buffer[j++]];
  }
  outlen = i;

  return outlen;
}

bool get_ripple_address(const uint8_t pubkey[33], char *address) {
  uint8_t hash[32];
  uint8_t addr[21 + 4] = {0};
  SHA256_CTX ctx;
  sha256_Init(&ctx);
  sha256_Update(&ctx, pubkey, 33);
  sha256_Final(&ctx, hash);

  ripemd160(hash, 32, addr + 1);

  sha256_Init(&ctx);
  sha256_Update(&ctx, addr, 21);
  sha256_Final(&ctx, hash);
  sha256_Init(&ctx);
  sha256_Update(&ctx, hash, 32);
  sha256_Final(&ctx, hash);

  memcpy(addr + 21, hash, 4);
  xrp_encode_base58_address(addr, 25, address, 36);
  return true;
}

static void write_type(uint8_t *buf, int *index, uint8_t type, uint8_t key) {
  if (key <= 0xf) {
    buf[*index] = type << 4 | key;
    *index += 1;
  } else {
    buf[*index] = type << 4;
    *index += 1;
    buf[*index] = key;
    *index += 1;
  }
}

void serialize_int32(unsigned char *buffer, int *index, int32_t value) {
  /* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char.
   */
  buffer[*index] = value >> 24;
  *index += 1;
  buffer[*index] = value >> 16;
  *index += 1;
  buffer[*index] = value >> 8;
  *index += 1;
  buffer[*index] = value;
  *index += 1;
}

void serialize_int16(uint8_t *buffer, int *index, int16_t value) {
  buffer[*index] = value >> 8;
  *index += 1;
  buffer[*index] = value;
  *index += 1;
}

void serialize_amount(unsigned char *buffer, int *index, int64_t value) {
  buffer[*index] = value >> 56;
  buffer[*index] &= 0x7F;  // clear first bit to indicate XRP
  buffer[*index] |= 0x40;  // set second bit to indicate positive number
  *index += 1;
  buffer[*index] = value >> 48;
  *index += 1;
  buffer[*index] = value >> 40;
  *index += 1;
  buffer[*index] = value >> 32;
  *index += 1;
  buffer[*index] = value >> 24;
  *index += 1;
  buffer[*index] = value >> 16;
  *index += 1;
  buffer[*index] = value >> 8;
  *index += 1;
  buffer[*index] = value;
  *index += 1;
}

static bool write_varint(uint8_t *buf, int *index, uint32_t value) {
  if (value < 192) {
    buf[*index] = value;
    *index += 1;
  } else if (value <= 12480) {
    value -= 193;
    buf[*index] = 193 + (value >> 8);
    *index += 1;
    buf[*index] = value & 0xff;
    *index += 1;
  } else if (value <= 918744) {
    value -= 12481;
    buf[*index] = 241 + (value >> 16);
    *index += 1;
    buf[*index] = (value >> 8) & 0xff;
    *index += 1;
    buf[*index] = value & 0xff;
    *index += 1;
  } else {
    return false;
  }
  return true;
}

static bool write_bytes_varint(uint8_t *buf, int *index, uint8_t *bytes,
                               int len) {
  write_varint(buf, index, len);
  memcpy(buf + (*index), bytes, len);
  *index += len;
  if (*index > 1024) {
    return false;
  }
  return true;
}

static bool serialize(const RippleSignTx *msg, uint8_t *raw, int *index,
                      uint8_t *publickey, uint8_t *sig, size_t sig_len) {
  int transaction_type = 0;  // only support ["Payment": 0]
  uint8_t FIELD_TYPE_INT16 = 1;
  uint8_t FIELD_TYPE_INT32 = 2;
  uint8_t FIELD_TYPE_AMOUNT = 6;
  uint8_t FIELD_TYPE_VL = 7;
  uint8_t FIELD_TYPE_ACCOUNT = 8;
  char signer_str[36] = {0};
  get_ripple_address(publickey, signer_str);
  // must be sorted numerically first by type and then by name
  // type
  write_type(raw, index, FIELD_TYPE_INT16, 2);
  serialize_int16(raw, index, transaction_type);

  // flags
  write_type(raw, index, FIELD_TYPE_INT32, 2);
  serialize_int32(raw, index, msg->flags | 0x80000000);

  // sequence
  write_type(raw, index, FIELD_TYPE_INT32, 4);
  serialize_int32(raw, index, msg->sequence);

  // destinationTag
  write_type(raw, index, FIELD_TYPE_INT32, 14);
  serialize_int32(raw, index, msg->payment.destination_tag);

  // lastLedgerSequence
  write_type(raw, index, FIELD_TYPE_INT32, 27);
  serialize_int32(raw, index, msg->last_ledger_sequence);

  // amount
  write_type(raw, index, FIELD_TYPE_AMOUNT, 1);
  serialize_amount(raw, index, msg->payment.amount);

  // fee
  write_type(raw, index, FIELD_TYPE_AMOUNT, 8);
  serialize_amount(raw, index, msg->fee);

  // signingPubKey
  write_type(raw, index, FIELD_TYPE_VL, 3);
  write_bytes_varint(raw, index, publickey, 33);

  // txnSignature
  if (sig) {
    write_type(raw, index, FIELD_TYPE_VL, 4);
    write_bytes_varint(raw, index, sig, sig_len);
  }

  // account
  write_type(raw, index, FIELD_TYPE_ACCOUNT, 1);
  uint8_t d[25] = {0};
  size_t res = 25;
  int i, j;
  char b_addr[36] = {0};
  int r_addr_len = strlen(signer_str);
  for (i = 0; i < r_addr_len; i++) {
    for (j = 0; j < 59; j++) {
      if (bas_e58_alphabet[j] == signer_str[i]) {
        break;
      }
    }
    b_addr[i] = b58digits_ordered[j];
  }
  if (b58tobin(d, &res, b_addr) != true) {
    return false;
  }
  write_bytes_varint(raw, index, d + 1, 20);

  // destination
  write_type(raw, index, FIELD_TYPE_ACCOUNT, 3);
  memset(d, 0, 25);
  memset(b_addr, 0, 36);
  r_addr_len = strlen(msg->payment.destination);
  for (i = 0; i < r_addr_len; i++) {
    for (j = 0; j < 59; j++) {
      if (bas_e58_alphabet[j] == msg->payment.destination[i]) {
        break;
      }
    }
    b_addr[i] = b58digits_ordered[j];
  }
  if (b58tobin(d, &res, b_addr) != true) {
    return false;
  }
  write_bytes_varint(raw, index, d + 1, 20);
  return true;
}

static int ripple_is_canonic(uint8_t v, uint8_t signature[64]) {
  (void)signature;
  return (v & 2) == 0;
}

void ripple_format_amount(const uint64_t amount, char *buf, int buflen) {
  char str_amount[12] = {0};
  bn_format_uint64(amount, NULL, NULL, 6, 0, false, str_amount,
                   sizeof(str_amount));
  snprintf(buf, buflen, "%s XRP", str_amount);
}

void layoutRippleConfirmTx(const char *to_str, const uint64_t value) {
  char amount[60];
  ripple_format_amount(value, amount, sizeof(amount));

  char _to1[] = "to   ________";
  char _to2[] = "_____________";
  char _to3[] = "_____________?";

  memcpy(_to1 + 5, to_str, 8);
  memcpy(_to2, to_str + 8, 13);
  memcpy(_to3, to_str + 21, 13);
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Send"), amount, _to1, _to2, _to3, NULL);
}

void layoutRippleFee(const uint64_t value, const uint64_t fee) {
  char gas_value[32];
  char amount[60];
  ripple_format_amount(fee, gas_value, sizeof(gas_value));
  ripple_format_amount(value, amount, sizeof(amount));

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Really send"), amount, _("and limit max fee to"),
                    gas_value, _("?"), NULL);
}

bool ripple_sign_tx(const RippleSignTx *msg, HDNode *node,
                    RippleSignedTx *resp) {
  int index = 4;
  uint8_t net_prefix[] = {0x53, 0x54, 0x58, 0x00};
  uint8_t *raw = resp->serialized_tx.bytes;
  memcpy(raw, net_prefix, 4);
  serialize(msg, raw, &index, node->public_key, NULL, 0);

  layoutRippleConfirmTx(msg->payment.destination, msg->payment.amount);
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    return false;
  }
  layoutRippleFee(msg->payment.amount, msg->fee);
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    return false;
  }

  // hash the tx
  uint8_t hash[64];
  SHA512_CTX ctx;
  sha512_Init(&ctx);
  sha512_Update(&ctx, raw, index);
  sha512_Final(&ctx, hash);
  memset(hash + 32, 0, 32);

  // sign tx hash
  uint8_t v;
  uint8_t sig[65] = {0};
  if (ecdsa_sign_digest(&secp256k1, node->private_key, hash, sig, &v,
                        ripple_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    return false;
  }

  resp->signature.size = ecdsa_sig_to_der(sig, resp->signature.bytes);
  index = 0;
  serialize(msg, resp->serialized_tx.bytes, &index, node->public_key,
            resp->signature.bytes, resp->signature.size);
  resp->serialized_tx.size = index;

  return true;
}