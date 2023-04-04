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

#include "ada.h"
#include "base58.h"
#include "bip39.h"
#include "buttons.h"
#include "cardano.h"
#include "cbor.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "segwit_addr.h"
#include "sha3.h"
#include "util.h"

struct AdaSigner ada_signer;
static CardanoTxItemAck ada_msg_item_ack;
static CardanoSignTxFinished ada_msg_sign_tx_finished;
extern int convert_bits(uint8_t *out, size_t *outlen, int outbits,
                        const uint8_t *in, size_t inlen, int inbits, int pad);

bool fsm_getCardanoIcaruNode(HDNode *node, const uint32_t *address_n,
                             size_t address_n_count, uint32_t *fingerprint) {
  int res;
  char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
  char passphrase[MAX_PASSPHRASE_LEN + 1] = {0};
  if (!config_getMnemonic(mnemonic, sizeof(mnemonic))) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Mnemonic dismissed"));
    return false;
  }
  if (!protectPassphrase(passphrase)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Passphrase dismissed"));
    return false;
  }

  uint8_t mnemonic_bits[64] = {0};
  int mnemonic_bits_len = mnemonic_to_bits(mnemonic, mnemonic_bits);
  if (mnemonic_bits_len == 0 || mnemonic_bits_len % 33 != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Invalid mnemonic"));
    return false;
  }
  int entropy_len = mnemonic_bits_len - mnemonic_bits_len / 33;
  int mnemonic_bytes_used = 0;

  // Exclude checksum (original Icarus spec)
  mnemonic_bytes_used = entropy_len / 8;

  uint8_t icarus_secret[96] = {0};
  secret_from_entropy_cardano_icarus((const uint8_t *)passphrase,
                                     strlen(passphrase), mnemonic_bits,
                                     mnemonic_bytes_used, icarus_secret, NULL);

  res = hdnode_from_secret_cardano(icarus_secret, node);
  if (res != 1) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Unexpected failure in constructing cardano node"));
    return false;
  }
  if (hdnode_private_ckd_cached(node, address_n, address_n_count,
                                fingerprint) == 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Failed to derive private key"));
    return false;
  }
  hdnode_fill_public_key(node);
  return true;
}

bool validate_network_info(int network_id, int protocol_magic) {
  /*
    We are only concerned about checking that both network_id and protocol_magic
    belong to the mainnet or that both belong to a testnet. We don't need to
    check for consistency between various testnets (at least for now).
  */
  bool is_mainnet_network_id = network_id != 0;
  bool is_mainnet_protocol_magic = protocol_magic == 764824073;
  if (is_mainnet_network_id != is_mainnet_protocol_magic) {
    return false;
  }

  return true;
}

static void to_seq(uint32_t x, uint8_t *out, int *bytes_generated) {
  int i, j;
  for (i = 5; i > 0; i--) {
    if (x & 127UL << i * 7) break;
  }
  for (j = 0; j <= i; j++) {
    out[j] = ((x >> ((i - j) * 7)) & 127) | 128;
  }

  out[i] ^= 128;
  if (NULL != bytes_generated) {
    *bytes_generated = j;
  }
}

static uint32_t reflect(uint32_t ref, char ch) {
  uint32_t value = 0;

  for (int i = 1; i < (ch + 1); i++) {
    if (ref & 1) value |= 1 << (ch - i);
    ref >>= 1;
  }

  return value;
}

uint32_t crc32(uint8_t *buf, uint32_t len) {
  uint32_t result = 0xFFFFFFFF;
  uint32_t m_Table[256];

  uint32_t ulPolynomial = 0x04C11DB7;

  for (int i = 0; i <= 0xFF; i++) {
    m_Table[i] = reflect(i, 8) << 24;
    for (int j = 0; j < 8; j++)
      m_Table[i] =
          (m_Table[i] << 1) ^ (m_Table[i] & (1 << 31) ? ulPolynomial : 0);
    m_Table[i] = reflect(m_Table[i], 32);
  }

  while (len--) result = (result >> 8) ^ m_Table[(result & 0xFF) ^ *buf++];

  result ^= 0xFFFFFFFF;

  return result;
}

bool derive_bytes(const CardanoAddressParametersType *address_parameters,
                  uint32_t network_id, uint32_t protocol_magic,
                  uint8_t *address_bytes, int *address_len) {
  int address_bytes_len = 0;
  uint8_t public_key[32] = {0};
  if (!validate_network_info(network_id, protocol_magic)) return false;
  if (address_parameters->address_type == CardanoAddressType_BYRON) {
    uint8_t extpubkey[64] = {0};
    HDNode node = {0};
    uint32_t fingerprint;
    fsm_getCardanoIcaruNode(&node, address_parameters->address_n,
                            address_parameters->address_n_count, &fingerprint);
    memcpy(extpubkey, node.public_key + 1, 32);
    memcpy(extpubkey + 32, node.chain_code, 32);

    // [0, [0, extpubkey], address_attributes]
    uint8_t cbor_buffer[64 + 10] = {0};
    size_t cbor_index = 0;
    cbor_index += cbor_writeToken(CBOR_TYPE_ARRAY, 3, cbor_buffer + cbor_index,
                                  74 - cbor_index);
    cbor_index += cbor_writeToken(CBOR_TYPE_UNSIGNED, 0,
                                  cbor_buffer + cbor_index, 74 - cbor_index);
    cbor_index += cbor_writeToken(CBOR_TYPE_ARRAY, 2, cbor_buffer + cbor_index,
                                  74 - cbor_index);
    cbor_index += cbor_writeToken(CBOR_TYPE_UNSIGNED, 0,
                                  cbor_buffer + cbor_index, 74 - cbor_index);
    cbor_index += cbor_writeToken(CBOR_TYPE_BYTES, 64, cbor_buffer + cbor_index,
                                  74 - cbor_index);
    memcpy(cbor_buffer + cbor_index, extpubkey, 64);
    cbor_index += 64;
    if (protocol_magic == MAINNET_PROTOCOL_MAGIC) {  // MAINNET
      cbor_index += cbor_writeToken(CBOR_TYPE_MAP, 0, cbor_buffer + cbor_index,
                                    74 - cbor_index);
    } else {
      cbor_index += cbor_writeToken(CBOR_TYPE_MAP, 1, cbor_buffer + cbor_index,
                                    74 - cbor_index);
      cbor_index += cbor_writeToken(CBOR_TYPE_UNSIGNED, 2,
                                    cbor_buffer + cbor_index, 74 - cbor_index);
      uint8_t scratch[10] = {0};
      size_t scratchSize =
          cbor_writeToken(CBOR_TYPE_UNSIGNED, protocol_magic, scratch, 10);
      cbor_index += cbor_writeToken(CBOR_TYPE_BYTES, scratchSize,
                                    cbor_buffer + cbor_index, 74 - cbor_index);
      memcpy(cbor_buffer + cbor_index, scratch, scratchSize);
      cbor_index += scratchSize;
    }

    // cborBuffer is hashed twice. First by sha3_256 and then by blake2b_224
    uint8_t sha_hash[32] = {0};
    uint8_t hash[ADDRESS_KEY_HASH_SIZE] = {0};
    struct SHA3_CTX ctx = {0};
    sha3_256_Init(&ctx);
    sha3_Update(&ctx, cbor_buffer, cbor_index);
    sha3_Final(&ctx, sha_hash);
    blake2b(sha_hash, 32, hash, ADDRESS_KEY_HASH_SIZE);

    // [address_root, address_attributes, address_type]
    memset(cbor_buffer, 0, 74);
    cbor_index = 0;
    cbor_index += cbor_writeToken(CBOR_TYPE_ARRAY, 3, cbor_buffer + cbor_index,
                                  74 - cbor_index);
    cbor_index += cbor_writeToken(CBOR_TYPE_BYTES, ADDRESS_KEY_HASH_SIZE,
                                  cbor_buffer + cbor_index, 74 - cbor_index);
    memcpy(cbor_buffer + cbor_index, hash, ADDRESS_KEY_HASH_SIZE);
    cbor_index += ADDRESS_KEY_HASH_SIZE;
    if (protocol_magic == MAINNET_PROTOCOL_MAGIC) {  // MAINNET
      cbor_index += cbor_writeToken(CBOR_TYPE_MAP, 0, cbor_buffer + cbor_index,
                                    74 - cbor_index);
    } else {
      cbor_index += cbor_writeToken(CBOR_TYPE_MAP, 1, cbor_buffer + cbor_index,
                                    74 - cbor_index);
      cbor_index += cbor_writeToken(CBOR_TYPE_UNSIGNED, 2,
                                    cbor_buffer + cbor_index, 74 - cbor_index);
      uint8_t scratch[10] = {0};
      size_t scratchSize =
          cbor_writeToken(CBOR_TYPE_UNSIGNED, protocol_magic, scratch, 10);
      cbor_index += cbor_writeToken(CBOR_TYPE_BYTES, scratchSize,
                                    cbor_buffer + cbor_index, 74 - cbor_index);
      memcpy(cbor_buffer + cbor_index, scratch, scratchSize);
      cbor_index += scratchSize;
    }
    cbor_index += cbor_writeToken(CBOR_TYPE_UNSIGNED, 0,
                                  cbor_buffer + cbor_index, 74 - cbor_index);

    // Array[
    //     tag(24):bytes(rawAddress),
    //     crc32(rawAddress)
    // ]
    address_bytes_len +=
        cbor_writeToken(CBOR_TYPE_ARRAY, 2, address_bytes + address_bytes_len,
                        128 - address_bytes_len);
    address_bytes_len += cbor_writeToken(
        CBOR_TYPE_TAG, CBOR_TAG_EMBEDDED_CBOR_BYTE_STRING,
        address_bytes + address_bytes_len, 128 - address_bytes_len);
    address_bytes_len += cbor_writeToken(CBOR_TYPE_BYTES, cbor_index,
                                         address_bytes + address_bytes_len,
                                         128 - address_bytes_len);
    memcpy(address_bytes + address_bytes_len, cbor_buffer, cbor_index);
    address_bytes_len += cbor_index;
    uint32_t checksum = crc32(cbor_buffer, cbor_index);
    address_bytes_len += cbor_writeToken(CBOR_TYPE_UNSIGNED, checksum,
                                         address_bytes + address_bytes_len,
                                         128 - address_bytes_len);
  } else {
    // _create_header
    address_bytes[0] = address_parameters->address_type << 4 | network_id;
    address_bytes_len = 1;

    // _get_payment_part
    if (address_parameters->address_n_count > 0) {
      HDNode node = {0};
      uint32_t fingerprint;
      fsm_getCardanoIcaruNode(&node, address_parameters->address_n,
                              address_parameters->address_n_count,
                              &fingerprint);
      memcpy(public_key, node.public_key + 1, 32);

      uint8_t hash[ADDRESS_KEY_HASH_SIZE] = {0};
      blake2b(public_key, 32, hash, ADDRESS_KEY_HASH_SIZE);
      memcpy(address_bytes + address_bytes_len, hash, ADDRESS_KEY_HASH_SIZE);
      address_bytes_len += ADDRESS_KEY_HASH_SIZE;
    } else if (address_parameters->has_script_payment_hash) {
      memcpy(address_bytes + address_bytes_len,
             address_parameters->script_payment_hash.bytes, SCRIPT_HASH_SIZE);
      address_bytes_len += SCRIPT_HASH_SIZE;
    }

    // _get_staking_part
    if (address_parameters->has_staking_key_hash) {
      memcpy(address_bytes + address_bytes_len,
             address_parameters->staking_key_hash.bytes, ADDRESS_KEY_HASH_SIZE);
      address_bytes_len += ADDRESS_KEY_HASH_SIZE;
    } else if (address_parameters->address_n_staking_count > 0) {
      HDNode node = {0};
      uint32_t fingerprint;
      fsm_getCardanoIcaruNode(&node, address_parameters->address_n_staking,
                              address_parameters->address_n_staking_count,
                              &fingerprint);
      memcpy(public_key, node.public_key + 1, 32);
      uint8_t hash[ADDRESS_KEY_HASH_SIZE] = {0};
      blake2b(public_key, 32, hash, ADDRESS_KEY_HASH_SIZE);
      memcpy(address_bytes + address_bytes_len, hash, ADDRESS_KEY_HASH_SIZE);
      address_bytes_len += ADDRESS_KEY_HASH_SIZE;
    } else if (address_parameters->has_script_staking_hash) {
      memcpy(address_bytes + address_bytes_len,
             address_parameters->script_staking_hash.bytes, SCRIPT_HASH_SIZE);
      address_bytes_len += SCRIPT_HASH_SIZE;
    } else if (address_parameters->has_certificate_pointer) {
      int bytes_generated;
      to_seq(address_parameters->certificate_pointer.block_index,
             address_bytes + address_bytes_len, &bytes_generated);
      address_bytes_len += bytes_generated;
      to_seq(address_parameters->certificate_pointer.tx_index,
             address_bytes + address_bytes_len, &bytes_generated);
      address_bytes_len += bytes_generated;
      to_seq(address_parameters->certificate_pointer.certificate_index,
             address_bytes + address_bytes_len, &bytes_generated);
      address_bytes_len += bytes_generated;
    }
  }

  *address_len = address_bytes_len;
  return true;
}

bool ada_get_address(const CardanoGetAddress *msg, char *address) {
  uint8_t address_bytes[128] = {0};
  int address_bytes_len = 0;
  derive_bytes(&msg->address_parameters, msg->network_id, msg->protocol_magic,
               address_bytes, &address_bytes_len);

  // encode_human_readable
  if (msg->address_parameters.address_type == CardanoAddressType_BYRON) {
    base58_encode(address_bytes, address_bytes_len, address, 128);
  } else {
    uint8_t data[128] = {0};
    size_t datalen = 0;
    char hrp[16] = {8};
    if ((msg->address_parameters.address_type == CardanoAddressType_REWARD) ||
        (msg->address_parameters.address_type ==
         CardanoAddressType_REWARD_SCRIPT)) {
      if (msg->network_id != 0) {  // is_mainnet
        memcpy(hrp, HRP_REWARD_ADDRESS, 5);
      } else {
        memcpy(hrp, HRP_TESTNET_REWARD_ADDRESS, 10);
      }
    } else {
      if (msg->network_id != 0) {  // is_mainnet
        memcpy(hrp, HRP_ADDRESS, 4);
      } else {
        memcpy(hrp, HRP_TESTNET_ADDRESS, 9);
      }
    }
    convert_bits(data, &datalen, 5, address_bytes, address_bytes_len, 8, 1);
    if (1 !=
        bech32_encode(address, hrp, data, datalen, BECH32_ENCODING_BECH32)) {
      return false;
    }
  }

  return true;
}

#define BUILDER_APPEND_CBOR(type, value) \
  blake2b_256_append_cbor_tx_body(&ada_signer.ctx, type, value)

#define BUILDER_APPEND_DATA(buffer, bufferSize) \
  blake2b_256_append_buffer_tx_body(&ada_signer.ctx, buffer, bufferSize)

void blake2b_256_append_buffer_tx_body(BLAKE2B_CTX *ctx, const uint8_t *buffer,
                                       size_t bufferSize) {
  blake2b_Update(ctx, buffer, bufferSize);
}

void blake2b_256_append_cbor_tx_body(BLAKE2B_CTX *ctx, uint8_t type,
                                     uint64_t value) {
  uint8_t buffer[10] = {0};
  size_t size = cbor_writeToken(type, value, buffer, 10);
  blake2b_Update(ctx, buffer, size);
}

// ============================== input ==============================

void txHashBuilder_enterInputs(void) {
  if (ada_signer.state != TX_HASH_BUILDER_INIT) return;
  {
    // Enter inputs
    BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_INPUTS);
    BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, ada_signer.remainingInputs);
  }
  ada_signer.state = TX_HASH_BUILDER_IN_INPUTS;
  ada_signer.tx_dict_items_count--;
}

void cbor_append_txInput(const uint8_t *utxoHashBuffer, size_t utxoHashSize,
                         uint32_t utxoIndex) {
  // Array(2)[
  //    Bytes[hash],
  //    Unsigned[index]
  // ]
  BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, 2);
  {
    BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, utxoHashSize);
    BUILDER_APPEND_DATA(utxoHashBuffer, utxoHashSize);
  }
  { BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, utxoIndex); }
}

void txHashBuilder_addInput(const CardanoTxInput *input) {
  ada_signer.remainingInputs--;
  cbor_append_txInput(input->prev_hash.bytes, input->prev_hash.size,
                      input->prev_index);
}

// ============================== output ==============================

void txHashBuilder_enterOutputs(void) {
  {
    // Enter outputs
    BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_OUTPUTS);
    BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, ada_signer.remainingOutputs);
  }
  ada_signer.state = TX_HASH_BUILDER_IN_OUTPUTS;
  ada_signer.tx_dict_items_count--;
}

bool get_bytes_unsafe(const char *address, uint8_t *address_bytes,
                      int *address_len) {
  char hrp[16] = {0};
  size_t res;
  uint8_t dbuf[128] = {0};
  int r = bech32_decode(hrp, dbuf, &res, address);
  if (r == BECH32_ENCODING_NONE) {
    res = *address_len;
    uint8_t d[*address_len];
    if (b58tobin(d, &res, address) != true) {
      *address_len = 0;
      return false;
    }
    memcpy(address_bytes, d + (*address_len - res), res);
    *address_len = res;
  } else {
    size_t len = 0;
    if (!convert_bits(address_bytes, &len, 8, dbuf, res, 5, 0)) return false;
    *address_len = len;
  }

  return true;
}

static bool layoutOutput(const CardanoTxOutput *output) {
  bool ret;
  uint8_t key = KEY_NULL;
  char desc[32] = {0};
  char str_amount[32] = {0};
  const char **tx_msg = format_tx_message("Cardano");

  if (output->asset_groups_count > 0) {
    oledClear();
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(
        0, 13, _("The following transaction output contains tokens."),
        FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
    oledRefresh();
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        ret = true;
        break;
      }
      if (key == KEY_CANCEL) {
        return false;
      }
    }
  }

  oledClear();
  layoutHeader(tx_msg[0]);
  bn_format_uint64(output->amount, NULL, " ADA", 6, 0, false, ',', str_amount,
                   sizeof(str_amount));
  strcat(desc, _("Amount"));
  strcat(desc, ":");
  oledDrawStringAdapter(0, 13, desc, FONT_STANDARD);
  oledDrawStringAdapter(0, 13 + 10, str_amount, FONT_STANDARD);
  layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  oledRefresh();
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      ret = true;
      break;
    }
    if (key == KEY_CANCEL) {
      return false;
    }
  }
  if (output->has_address) {
    uint32_t rowlen = 21, addrlen = strlen(output->address);
    int index = 0, rowcount = addrlen / rowlen + 1;
    if (rowcount > 3) {
      const char **str =
          split_message((const uint8_t *)output->address, addrlen, rowlen);

    refresh_addr:
      oledClear_ex();
      layoutHeader(tx_msg[0]);

      if (0 == index) {
        oledDrawStringAdapter(0, 13, _("Send to:"), FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[0], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[2], FONT_STANDARD);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      } else {
        oledDrawStringAdapter(0, 13, str[index - 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 1 * 10, str[index], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 2 * 10, str[index + 1], FONT_STANDARD);
        oledDrawStringAdapter(0, 13 + 3 * 10, str[index + 2], FONT_STANDARD);
        if (index == rowcount - 3) {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
        } else {
          oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_up);
          oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                         &bmp_bottom_middle_arrow_down);
        }
      }

      // scrollbar
      int i, bar_start = 12, bar_end = 52;
      int bar_heght = 40 - 2 * (rowcount - 4);
      for (i = bar_start; i < bar_end; i += 2) {  // 40 pixel
        oledDrawPixel(OLED_WIDTH - 1, i);
      }
      for (i = bar_start + 2 * ((int)index);
           i < (bar_start + bar_heght + 2 * ((int)index - 1)) - 1; i++) {
        oledDrawPixel(OLED_WIDTH - 1, i);
        oledDrawPixel(OLED_WIDTH - 2, i);
      }

      layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
      layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);

      oledRefresh();
      key = protectWaitKey(0, 0);
      switch (key) {
        case KEY_UP:
          if (index > 0) {
            index--;
          }
          goto refresh_addr;
        case KEY_DOWN:
          if (index < rowcount - 3) {
            index++;
          }
          goto refresh_addr;
        case KEY_CONFIRM:
          return true;
        case KEY_CANCEL:
          return false;
        default:
          goto refresh_addr;
      }
    } else {
      oledClear();
      layoutHeader(tx_msg[0]);
      oledDrawStringAdapter(0, 13, _("Send to:"), FONT_STANDARD);
      oledDrawStringAdapter(0, 13 + 10, output->address, FONT_STANDARD);
      layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
      layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
      oledRefresh();
      while (1) {
        key = protectWaitKey(0, 0);
        if (key == KEY_CONFIRM) {
          ret = true;
          break;
        }
        if (key == KEY_CANCEL) {
          ret = false;
          break;
        }
      }
    }
  }
  return ret;
}

static bool layoutFinal(void) {
  uint8_t key = KEY_NULL;
  const char **tx_msg = format_tx_message("Cardano");

  oledClear();
  layoutHeader(_("Sign Transaction"));
  oledDrawStringAdapter(0, 13, tx_msg[1], FONT_STANDARD);
  layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  oledRefresh();
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      return true;
      break;
    }
    if (key == KEY_CANCEL) {
      return false;
    }
  }
}

static bool layoutFee(void) {
  uint8_t key = KEY_NULL;
  char desc[32] = {0};
  char str_amount[32] = {0};
  const char **tx_msg = format_tx_message("Cardano");

  oledClear();
  layoutHeader(tx_msg[0]);
  strcat(desc, _("Fee"));
  strcat(desc, ":");
  bn_format_uint64(ada_signer.signertx.fee, NULL, " ADA", 6, 0, false, ',',
                   str_amount, sizeof(str_amount));
  oledDrawStringAdapter(0, 13, desc, FONT_STANDARD);
  oledDrawStringAdapter(0, 13 + 10, str_amount, FONT_STANDARD);

  layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  oledRefresh();
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      break;
    }
    if (key == KEY_CANCEL) {
      return false;
    }
  }
  return layoutFinal();
}

bool txHashBuilder_addOutput(const CardanoTxOutput *output) {
  if (!layoutOutput(output)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }
  ada_signer.remainingOutputs--;
  int output_items_count = 2;
  if (output->has_datum_hash) output_items_count++;
  if (output->has_inline_datum_size && (output->inline_datum_size > 0))
    output_items_count++;
  if (output->has_reference_script_size && (output->reference_script_size > 0))
    output_items_count++;
  if (output->format == CardanoTxOutputSerializationFormat_ARRAY_LEGACY) {
    BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, output_items_count);

    uint8_t address_bytes[128] = {0};
    int address_bytes_len = 64;
    if (output->has_address_parameters) {
      derive_bytes(&output->address_parameters, ada_signer.signertx.network_id,
                   ada_signer.signertx.protocol_magic, address_bytes,
                   &address_bytes_len);
    } else {
      get_bytes_unsafe(output->address, address_bytes, &address_bytes_len);
    }
    BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, address_bytes_len);
    BUILDER_APPEND_DATA(address_bytes, address_bytes_len);
    // Output structure is: [address, amount, datum_hash?]
    if (0 == output->asset_groups_count) {
      BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, output->amount);
      if (output->has_datum_hash) {
        BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, output->datum_hash.size);
        BUILDER_APPEND_DATA(output->datum_hash.bytes, output->datum_hash.size);
      }

      ada_signer.outputState = TX_OUTPUT_FINISHED;
    } else {
      // Output structure is: [address, [amount, asset_groups], datum_hash?]
      BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, 2);
      BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, output->amount);
      BUILDER_APPEND_CBOR(CBOR_TYPE_MAP, output->asset_groups_count);

      ada_signer.outputState = TX_OUTPUT_TOP_LEVEL_DATA;
    }
  } else if (output->format == CardanoTxOutputSerializationFormat_MAP_BABBAGE) {
    return false;  // unsupport
  } else {
    return false;
  }

  return true;
}

bool txHashBuilder_addAssetGroup(const CardanoAssetGroup *msg) {
  BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, msg->policy_id.size);
  BUILDER_APPEND_DATA(msg->policy_id.bytes, msg->policy_id.size);
  BUILDER_APPEND_CBOR(CBOR_TYPE_MAP, msg->tokens_count);

  memcpy(ada_signer.policy_id, msg->policy_id.bytes, msg->policy_id.size);
  ada_signer.policy_id_size = msg->policy_id.size;
  return true;
}

bool txHashBuilder_addToken(const CardanoToken *msg) {
  BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, msg->asset_name_bytes.size);
  BUILDER_APPEND_DATA(msg->asset_name_bytes.bytes, msg->asset_name_bytes.size);
  if (msg->has_amount) {
    BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, msg->amount);
  }

  uint8_t message_digest[20];
  char fingerprint[128] = {0};
  uint8_t data[65] = {0};
  size_t datalen = 0;
  bool ret = false;
  char amount[32] = {0};
  uint8_t key = KEY_NULL, index = 0;
  const char **tx_msg = format_tx_message("Cardano");

  BLAKE2B_CTX ctx;
  blake2b_Init(&ctx, 20);
  blake2b_Update(&ctx, ada_signer.policy_id, ada_signer.policy_id_size);
  blake2b_Update(&ctx, msg->asset_name_bytes.bytes, msg->asset_name_bytes.size);
  blake2b_Final(&ctx, message_digest, 20);

  // bech32_encode
  convert_bits(data, &datalen, 5, message_digest, 20, 8, 1);
  bech32_encode(fingerprint, "asset", data, datalen, BECH32_ENCODING_BECH32);

  uint2str(msg->amount, amount);

refresh_layout:
  oledClear();
  layoutHeader(tx_msg[0]);

  if (0 == index) {
    oledDrawStringAdapter(0, 13, _("Asset Fingerprint:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10, fingerprint, FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 30, _("Token Amount:"), FONT_STANDARD);
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_down);
  } else {
    oledDrawStringAdapter(0, 13, fingerprint, FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 20, _("Token Amount:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 30, amount, FONT_STANDARD);
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_up);
  }

  layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
      }
      goto refresh_layout;
    case KEY_DOWN:
      if (index < 1) {
        index++;
      }
      goto refresh_layout;
    case KEY_CONFIRM:
      ret = true;
      break;
    case KEY_CANCEL:
      ret = false;
      break;
    default:
      break;
  }
  if (!ret) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }

  return true;
}

// ============================== FEE ==============================

void txHashBuilder_addFee(uint64_t fee) {
  // add fee item into the main tx body map
  BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_FEE);
  BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, fee);

  ada_signer.state = TX_HASH_BUILDER_IN_FEE;
  ada_signer.tx_dict_items_count--;
}

// ============================== TTL ==============================

void txHashBuilder_addTtl(uint64_t ttl) {
  BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_TTL);
  BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, ttl);

  ada_signer.state = TX_HASH_BUILDER_IN_TTL;
  ada_signer.tx_dict_items_count--;
}

static void get_public_key_hash(const uint32_t *address_n,
                                size_t address_n_count,
                                uint8_t hash[ADDRESS_KEY_HASH_SIZE]) {
  HDNode node = {0};
  uint32_t fingerprint;
  uint8_t public_key[32] = {0};
  fsm_getCardanoIcaruNode(&node, address_n, address_n_count, &fingerprint);
  memcpy(public_key, node.public_key + 1, 32);
  blake2b(public_key, 32, hash, ADDRESS_KEY_HASH_SIZE);
}

// ============================== Certificate ==============================

static bool layoutCertificate(const CardanoTxCertificate *cert) {
  uint8_t key = KEY_NULL;
  const char **tx_msg = format_tx_message("Cardano");

  oledClear();
  layoutHeader(tx_msg[0]);
  oledDrawStringAdapter(0, 13, _("Transaction Type:"), FONT_STANDARD);
  if (cert->type == CardanoCertificateType_STAKE_REGISTRATION) {
    oledDrawStringAdapter(0, 13 + 10, _("Stake key registration"),
                          FONT_STANDARD);
  } else if (cert->type == CardanoCertificateType_STAKE_DEREGISTRATION) {
    oledDrawStringAdapter(0, 13 + 10, _("Stake deregistration"), FONT_STANDARD);
  } else if (cert->type == CardanoCertificateType_STAKE_DELEGATION) {
    oledDrawStringAdapter(0, 13 + 10, _("Stake delegation"), FONT_STANDARD);
  } else if (cert->type == CardanoCertificateType_STAKE_POOL_REGISTRATION) {
    oledDrawStringAdapter(0, 13 + 10, _("Pool registration"), FONT_STANDARD);
  }

  layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  oledRefresh();
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      break;
    }
    if (key == KEY_CANCEL) {
      return false;
    }
  }

  // account
  if (cert->path_count > 0) {
    oledClear();
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(0, 13, _("Account:"), FONT_STANDARD);
    oledDrawString(0, 13 + 10,
                   address_n_str(cert->path, cert->path_count, true),
                   FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
    oledRefresh();
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
      if (key == KEY_CANCEL) {
        return false;
      }
    }
  }
  if (cert->has_pool) {
    char pool[65] = {0};
    uint8_t data[65] = {0};
    size_t datalen = 0;
    convert_bits(data, &datalen, 5, cert->pool.bytes, cert->pool.size, 8, 1);
    bech32_encode(pool, "pool", data, datalen, BECH32_ENCODING_BECH32);
    oledClear();
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(0, 13, _("To Pool:"), FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 10, pool, FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
    oledRefresh();
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
      if (key == KEY_CANCEL) {
        return false;
      }
    }
  }
  return true;
}

bool txHashBuilder_addCertificate(const CardanoTxCertificate *cert) {
  if (!layoutCertificate(cert)) {
    return false;
  }

  if (ada_signer.state != TX_HASH_BUILDER_IN_CERTIFICATES) {
    // enter Certificate
    BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_CERTIFICATES);
    BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, ada_signer.remainingCertificates);

    ada_signer.state = TX_HASH_BUILDER_IN_CERTIFICATES;
    ada_signer.tx_dict_items_count--;
  }

  if ((cert->type == CardanoCertificateType_STAKE_REGISTRATION) ||
      (cert->type == CardanoCertificateType_STAKE_DEREGISTRATION)) {
    BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, 2);
    { BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, cert->type); }
    {
      BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, 2);
      {
        if (cert->has_key_hash) {
          BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, 0);

          BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, cert->key_hash.size);
          BUILDER_APPEND_DATA(cert->key_hash.bytes, cert->key_hash.size);
        } else if (cert->path_count > 0) {
          BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, 0);

          uint8_t hash[ADDRESS_KEY_HASH_SIZE] = {0};
          get_public_key_hash(cert->path, cert->path_count, hash);
          BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, ADDRESS_KEY_HASH_SIZE);
          BUILDER_APPEND_DATA(hash, ADDRESS_KEY_HASH_SIZE);
        } else if (cert->has_script_hash) {
          BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, 1);

          BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, cert->script_hash.size);
          BUILDER_APPEND_DATA(cert->script_hash.bytes, cert->script_hash.size);
        } else {
          return false;
        }
      }
    }
  } else if (cert->type == CardanoCertificateType_STAKE_DELEGATION) {
    BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, 3);
    { BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, cert->type); }
    {
      BUILDER_APPEND_CBOR(CBOR_TYPE_ARRAY, 2);
      {
        if (cert->has_key_hash) {
          BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, 0);

          BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, cert->key_hash.size);
          BUILDER_APPEND_DATA(cert->key_hash.bytes, cert->key_hash.size);
        } else if (cert->path_count > 0) {
          BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, 0);

          uint8_t hash[ADDRESS_KEY_HASH_SIZE] = {0};
          get_public_key_hash(cert->path, cert->path_count, hash);
          BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, ADDRESS_KEY_HASH_SIZE);
          BUILDER_APPEND_DATA(hash, ADDRESS_KEY_HASH_SIZE);
        } else if (cert->has_script_hash) {
          BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, 1);

          BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, cert->script_hash.size);
          BUILDER_APPEND_DATA(cert->script_hash.bytes, cert->script_hash.size);
        } else {
          return false;
        }
      }
    }
    {
      if (cert->has_pool) {
        BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, cert->pool.size);
        BUILDER_APPEND_DATA(cert->pool.bytes, cert->pool.size);
      } else {
        BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, 0);
      }
    }
  } else if (cert->type == CardanoCertificateType_STAKE_POOL_REGISTRATION) {
    return false;
  } else {
    return false;
  }

  ada_signer.remainingCertificates--;
  return true;
}

// ============================== Withdrawal ==============================

bool txHashBuilder_addWithdrawal(const CardanoTxWithdrawal *wdr) {
  if (ada_signer.state != TX_HASH_BUILDER_IN_WITHDRAWALS) {
    // enter Certificate
    BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_WITHDRAWALS);
    BUILDER_APPEND_CBOR(CBOR_TYPE_MAP, ada_signer.remainingWithdrawals);

    ada_signer.state = TX_HASH_BUILDER_IN_WITHDRAWALS;
    ada_signer.tx_dict_items_count--;
  }

  uint8_t address_bytes[128] = {0};
  int address_bytes_len = 0;
  CardanoAddressParametersType address_parameters;
  memset(&address_parameters, 0, sizeof(CardanoAddressParametersType));
  if (wdr->path_count > 0) {
    address_parameters.address_type = CardanoAddressType_REWARD;
    memcpy(address_parameters.address_n_staking, wdr->path,
           wdr->path_count * 4);
    address_parameters.address_n_staking_count = wdr->path_count;
  }

  if (wdr->has_key_hash) {
    address_parameters.address_type = CardanoAddressType_REWARD;
    memcpy(address_parameters.staking_key_hash.bytes, wdr->key_hash.bytes, 28);
    address_parameters.staking_key_hash.size = 28;
    address_parameters.has_staking_key_hash = true;
  }
  if (wdr->has_script_hash) {
    address_parameters.address_type = CardanoAddressType_REWARD_SCRIPT;
    memcpy(address_parameters.script_staking_hash.bytes, wdr->script_hash.bytes,
           28);
    address_parameters.script_staking_hash.size = 28;
    address_parameters.has_script_staking_hash = true;
  }
  derive_bytes(&address_parameters, ada_signer.signertx.network_id,
               ada_signer.signertx.protocol_magic, address_bytes,
               &address_bytes_len);

  BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, address_bytes_len);
  BUILDER_APPEND_DATA(address_bytes, address_bytes_len);
  BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, wdr->amount);

  ada_signer.remainingWithdrawals--;
  return true;
}

// ============================== AuxiliaryData ==============================

bool txHashBuilder_addAuxiliaryData(const CardanoTxAuxiliaryData *au) {
  CardanoTxAuxiliaryDataSupplement au_data_sup;

  memset(&au_data_sup, 0, sizeof(CardanoTxAuxiliaryDataSupplement));

  if (au->has_hash) {
    au_data_sup.type = CardanoTxAuxiliaryDataSupplementType_NONE;
    au_data_sup.has_auxiliary_data_hash = false;
    au_data_sup.has_governance_signature = false;
    BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_AUX_DATA);
    BUILDER_APPEND_CBOR(CBOR_TYPE_BYTES, au->hash.size);
    BUILDER_APPEND_DATA(au->hash.bytes, au->hash.size);
  } else if (au->has_governance_registration_parameters) {
    return false;  // unsupport
  } else {
    return false;
  }

  ada_signer.state = TX_HASH_BUILDER_IN_AUX_DATA;
  ada_signer.tx_dict_items_count--;

  // CardanoTxHostAck
  msg_write(MessageType_MessageType_CardanoTxAuxiliaryDataSupplement,
            &au_data_sup);

  return true;
}

bool hash_stage() {
  switch (ada_signer.state) {
    case TX_HASH_BUILDER_INIT:
      msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
      txHashBuilder_enterInputs();
      break;
    case TX_HASH_BUILDER_IN_INPUTS:
      if (0 == ada_signer.remainingInputs) {
        ada_signer.state = TX_HASH_BUILDER_IN_OUTPUTS;
        txHashBuilder_enterOutputs();
      }
      if (ada_signer.state != TX_HASH_BUILDER_IN_OUTPUTS) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_OUTPUTS:
      if (ada_signer.remainingOutputs > 0 ||
          (ada_signer.remainingOutputs == 0 &&
           ada_signer.outputState != TX_OUTPUT_FINISHED)) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_FEE:
      txHashBuilder_addFee(ada_signer.signertx.fee);
      if (0 == ada_signer.tx_dict_items_count) break;
      //-fallthrough
    case TX_HASH_BUILDER_IN_TTL:
      ada_signer.state = TX_HASH_BUILDER_IN_TTL;
      if (ada_signer.signertx.has_ttl) {
        txHashBuilder_addTtl(ada_signer.signertx.ttl);
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_CERTIFICATES:
      if (ada_signer.remainingCertificates > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_WITHDRAWALS:
      if (ada_signer.remainingWithdrawals > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_AUX_DATA:
      if (ada_signer.signertx.has_auxiliary_data &&
          ada_signer.tx_dict_items_count > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_VALIDITY_INTERVAL_START:
      if (ada_signer.signertx.has_validity_interval_start) {
        BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED,
                            TX_BODY_KEY_VALIDITY_INTERVAL_START);
        BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED,
                            ada_signer.signertx.validity_interval_start);
        ada_signer.signertx.has_validity_interval_start = false;
        ada_signer.state = TX_HASH_BUILDER_IN_VALIDITY_INTERVAL_START;
        ada_signer.tx_dict_items_count--;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_MINT:
      if (ada_signer.remainingMintingAssetGroupsCount > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_SCRIPT_DATA_HASH:
      if (ada_signer.signertx.has_script_data_hash) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_COLLATERAL_INPUTS:
      if (ada_signer.remainingCollateralInputs > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_REQUIRED_SIGNERS:
      if (ada_signer.remainingRequiredSigners > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_NETWORK_ID:
      if (ada_signer.signertx.has_include_network_id &&
          ada_signer.signertx.include_network_id) {
        BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_NETWORK_ID);
        BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, ada_signer.signertx.network_id);
        ada_signer.signertx.has_include_network_id = false;
        ada_signer.state = TX_HASH_BUILDER_IN_COLLATERAL_OUTPUT;
        ada_signer.tx_dict_items_count--;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_COLLATERAL_OUTPUT:
      if (ada_signer.remainingCollateralInputs > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_TOTAL_COLLATERAL:
      if (ada_signer.signertx.has_total_collateral) {
        BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED, TX_BODY_KEY_TOTAL_COLLATERAL);
        BUILDER_APPEND_CBOR(CBOR_TYPE_UNSIGNED,
                            ada_signer.signertx.total_collateral);
        ada_signer.signertx.has_total_collateral = false;
        ada_signer.tx_dict_items_count--;
      }
      //-fallthrough
    case TX_HASH_BUILDER_IN_REFERENCE_INPUTS:
      if (ada_signer.remainingReferenceInputs > 0) {
        msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
        break;
      }
      //-fallthrough
    case TX_HASH_BUILDER_FINISHED:
      ada_signer.state = TX_HASH_BUILDER_FINISHED;
      break;
    default:
      return false;
  }

  if (!ada_signer.is_finished &&
      0 == ada_signer.tx_dict_items_count) {  // finish
    if ((ada_signer.remainingCertificates == 0) &&
        (ada_signer.remainingCollateralInputs == 0) &&
        (ada_signer.remainingRequiredSigners == 0) &&
        (ada_signer.remainingReferenceInputs == 0)) {
      blake2b_Final(&ada_signer.ctx, ada_signer.digest, 32);
      if (!layoutFee()) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled,
                        "Signing cancelled");
        layoutHome();
        return false;
      }
      msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
      ada_signer.is_finished = true;
    }
  }

  return true;
}

bool _processs_tx_init(CardanoSignTxInit *msg) {
  if (msg->signing_mode != CardanoTxSigningMode_ORDINARY_TRANSACTION) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Only support ORDINARY TRANSACTION"));
    return false;
  }
  memset(&ada_signer, 0, sizeof(struct AdaSigner));
  memcpy(&ada_signer.signertx, msg, sizeof(CardanoSignTxInit));

  // _validate_tx_init
  if ((msg->fee > LOVELACE_MAX_SUPPLY) ||
      (msg->total_collateral > LOVELACE_MAX_SUPPLY) ||
      (msg->has_total_collateral)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Invalid tx signing request"));
    return false;
  }
  if (!validate_network_info(msg->network_id, msg->protocol_magic))
    return false;
  // if((msg->collateral_inputs_count != 0) || (msg->has_total_collateral) ||
  // (msg->has_reference_inputs_count) ) {
  //   fsm_sendFailure(FailureType_Failure_ProcessError,
  //                   _("Invalid tx signing request"));
  //   return false;
  // }

  if (0 != blake2b_Init(&ada_signer.ctx, 32)) return false;
  // Inputs, outputs and fee are mandatory, count the number of optional fields
  // present.
  ada_signer.tx_dict_items_count = 3;
  if (ada_signer.signertx.has_ttl) ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.certificates_count > 0)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.withdrawals_count > 0)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.has_auxiliary_data) ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.has_validity_interval_start)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.minting_asset_groups_count > 0)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.has_include_network_id &&
      ada_signer.signertx.include_network_id)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.has_script_data_hash)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.collateral_inputs_count > 0)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.required_signers_count > 0)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.has_has_collateral_return &&
      ada_signer.signertx.has_collateral_return) {
    ada_signer.tx_dict_items_count++;
  }
  if (ada_signer.signertx.has_total_collateral)
    ada_signer.tx_dict_items_count++;
  if (ada_signer.signertx.reference_inputs_count > 0)
    ada_signer.tx_dict_items_count++;
  blake2b_256_append_cbor_tx_body(&ada_signer.ctx, CBOR_TYPE_MAP,
                                  ada_signer.tx_dict_items_count);

  ada_signer.state = TX_HASH_BUILDER_INIT;
  ada_signer.remainingInputs = ada_signer.signertx.inputs_count;
  ada_signer.remainingOutputs = ada_signer.signertx.outputs_count;
  ada_signer.remainingWithdrawals = ada_signer.signertx.withdrawals_count;
  ada_signer.remainingCertificates = ada_signer.signertx.certificates_count;
  ada_signer.remainingCollateralInputs =
      ada_signer.signertx.collateral_inputs_count;
  ada_signer.remainingRequiredSigners =
      ada_signer.signertx.required_signers_count;
  ada_signer.remainingMintingAssetGroupsCount =
      ada_signer.signertx.minting_asset_groups_count;

  if (ada_signer.signertx.has_reference_inputs_count)
    ada_signer.remainingReferenceInputs =
        ada_signer.signertx.reference_inputs_count;

  ada_signer.is_finished = false;
  return hash_stage();
}

void cardano_txack(void) {
  if (ada_signer.state == TX_HASH_BUILDER_IN_AUX_DATA) {
    msg_write(MessageType_MessageType_CardanoTxItemAck, &ada_msg_item_ack);
    ada_signer.state = TX_HASH_BUILDER_IN_VALIDITY_INTERVAL_START;
    hash_stage();
  } else if (ada_signer.state != TX_SIGN_FINISHED) {
    ada_signer.state = TX_SIGN_FINISHED;
    CardanoTxBodyHash resp;
    memset(&resp, 0, sizeof(CardanoTxBodyHash));
    resp.tx_hash.size = 32;
    memcpy(resp.tx_hash.bytes, ada_signer.digest, 32);
    msg_write(MessageType_MessageType_CardanoTxBodyHash, &resp);
  } else {
    msg_write(MessageType_MessageType_CardanoSignTxFinished,
              &ada_msg_sign_tx_finished);
  }
}

bool cardano_txwitness(CardanoTxWitnessRequest *msg,
                       CardanoTxWitnessResponse *resp) {
  HDNode node = {0};
  uint32_t fingerprint;
  fsm_getCardanoIcaruNode(&node, msg->path, msg->path_count, &fingerprint);
  resp->pub_key.size = 32;
  memcpy(resp->pub_key.bytes, node.public_key + 1, 32);
  ed25519_public_key pk = {0};
  ed25519_publickey_ext(node.private_key_extension, pk);

  ed25519_sign_ext(ada_signer.digest, 32, node.private_key,
                   node.private_key_extension, resp->signature.bytes);
  resp->signature.size = 64;
  if ((msg->path[0] == 2147483692) &&
      (msg->path[1] == 2147485463)) {  // BYRON_ROOT = [44,1815...]
    resp->type = CardanoTxWitnessType_BYRON_WITNESS;
    resp->has_chain_code = true;
    resp->chain_code.size = 32;
    memcpy(resp->chain_code.bytes, node.chain_code, 32);
  } else {
    resp->type = CardanoTxWitnessType_SHELLEY_WITNESS;
    resp->has_chain_code = false;
  }

  return true;
}

bool ada_sign_messages(const HDNode *node, CardanoSignMessage *msg,
                       CardanoMessageSignature *resp) {
  uint8_t data[1024 + 128] = {0};
  uint8_t sig_structure[1024 + 128] = {0};
  uint8_t phdr_encoded[128] = {0};
  uint8_t verification_key[32] = {0};
  uint8_t hash[29] = {0};
  uint8_t sig[64];
  int data_index = 0, phdr_encoded_index = 0, sig_structure_index = 0;
  size_t size = 0;
  ed25519_publickey(node->private_key, verification_key);
  blake2b(verification_key, 32, hash + 1, 28);
  hash[0] =
      0x61;  // header = (KEY_NONE << 4 | msg.network_id).to_bytes(1, "big")

  // phdr={
  //     1: -8, # Algorithm: EdDSA,
  //     "address": header + verification_key_hash
  // }
  uint8_t buffer[10] = {0};
  size = cbor_writeToken(CBOR_TYPE_MAP, 2, buffer, 10);
  memcpy(phdr_encoded + phdr_encoded_index, buffer, size);
  phdr_encoded_index += size;

  size = cbor_writeToken(CBOR_TYPE_UNSIGNED, 1, buffer, 10);
  memcpy(phdr_encoded + phdr_encoded_index, buffer, size);
  phdr_encoded_index += size;

  size = cbor_writeToken(CBOR_TYPE_NEGATIVE, -8, buffer, 10);
  memcpy(phdr_encoded + phdr_encoded_index, buffer, size);
  phdr_encoded_index += size;

  size = cbor_writeToken(CBOR_TYPE_TEXT, 7, buffer, 10);
  memcpy(phdr_encoded + phdr_encoded_index, buffer, size);
  phdr_encoded_index += size;
  memcpy(phdr_encoded + phdr_encoded_index, "address", 7);
  phdr_encoded_index += 7;

  size = cbor_writeToken(CBOR_TYPE_BYTES, 29, buffer, 10);
  memcpy(phdr_encoded + phdr_encoded_index, buffer, size);
  phdr_encoded_index += size;
  memcpy(phdr_encoded + phdr_encoded_index, hash, 29);
  phdr_encoded_index += 29;

  size = cbor_writeToken(CBOR_TYPE_ARRAY, 4, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;

  size = cbor_writeToken(CBOR_TYPE_BYTES, phdr_encoded_index, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  memcpy(data + data_index, phdr_encoded, phdr_encoded_index);
  data_index += phdr_encoded_index;

  // Sign1Message.uhdr  = {"hashed": False}
  data[data_index++] = 0xa1;
  size = cbor_writeToken(CBOR_TYPE_TEXT, 6, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  memcpy(data + data_index, "hashed", 6);
  data_index += 6;
  data[data_index++] = 0xf4;

  // Sign1Message.payload
  size = cbor_writeToken(CBOR_TYPE_BYTES, msg->message.size, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  memcpy(data + data_index, msg->message.bytes, msg->message.size);
  data_index += msg->message.size;

  // Signature1
  size = cbor_writeToken(CBOR_TYPE_ARRAY, 4, buffer, 10);
  memcpy(sig_structure + sig_structure_index, buffer, size);
  sig_structure_index += size;

  size = cbor_writeToken(CBOR_TYPE_TEXT, 10, buffer, 10);
  memcpy(sig_structure + sig_structure_index, buffer, size);
  sig_structure_index += size;
  memcpy(sig_structure + sig_structure_index, "Signature1", 10);
  sig_structure_index += 10;

  size = cbor_writeToken(CBOR_TYPE_BYTES, phdr_encoded_index, buffer, 10);
  memcpy(sig_structure + sig_structure_index, buffer, size);
  sig_structure_index += size;
  memcpy(sig_structure + sig_structure_index, phdr_encoded, phdr_encoded_index);
  sig_structure_index += phdr_encoded_index;

  size = cbor_writeToken(CBOR_TYPE_BYTES, 0, buffer, 10);
  memcpy(sig_structure + sig_structure_index, buffer, size);
  sig_structure_index += size;

  size = cbor_writeToken(CBOR_TYPE_BYTES, msg->message.size, buffer, 10);
  memcpy(sig_structure + sig_structure_index, buffer, size);
  sig_structure_index += size;
  memcpy(sig_structure + sig_structure_index, msg->message.bytes,
         msg->message.size);
  sig_structure_index += msg->message.size;

  ed25519_sign(sig_structure, sig_structure_index, node->private_key, sig);

  size = cbor_writeToken(CBOR_TYPE_BYTES, 64, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  memcpy(data + data_index, sig, 64);
  data_index += 64;

  memcpy(resp->signature.bytes, data, data_index);
  resp->signature.size = data_index;

  /*
      key_to_return = {
        1: 1, # KpKty: KtyOKP,
        3: -8, # KpAlg: EdDSA,
        -1: 6, # OKPKpCurve: Ed25519,
        -2: verification_key,  # OKPKpX: public key
    }
  */
  data_index = 0;
  size = cbor_writeToken(CBOR_TYPE_MAP, 4, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_UNSIGNED, 1, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_UNSIGNED, 1, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_UNSIGNED, 3, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_NEGATIVE, -8, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_NEGATIVE, -1, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_UNSIGNED, 6, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  size = cbor_writeToken(CBOR_TYPE_NEGATIVE, -2, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;

  size = cbor_writeToken(CBOR_TYPE_BYTES, 32, buffer, 10);
  memcpy(data + data_index, buffer, size);
  data_index += size;
  memcpy(data + data_index, verification_key, 32);
  data_index += 32;

  memcpy(resp->key.bytes, data, data_index);
  resp->key.size = data_index;

  return true;
}
