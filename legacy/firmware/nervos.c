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

#include "nervos.h"
#include "crypto.h"
#include "base32.h"
#include "buttons.h"
#include "config.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "sha3.h"
#include "sha512.h"
#include "segwit_addr.h"
#include "util.h"

#define HASHTYPE_DATA                      (0)
#define HASHTYPE_TYPE                      (1)
#define HASHTYPE_INVALID                   (2)


extern int convert_bits(uint8_t *out, size_t *outlen, int outbits,
                        const uint8_t *in, size_t inlen, int inbits, int pad);

const uint8_t defaultCodeHash[] = {0x9b, 0xd7, 0xe0, 0x6f, 0x3e, 0xcf, 0x4b, 0xe0, 0xf2, 0xfc, 0xd2,
                                   0x18, 0x8b, 0x23, 0xf1, 0xb9, 0xfc, 0xc8, 0x8e, 0x5d, 0x4b, 0x65,
                                   0xa8, 0x63, 0x7b, 0x17, 0x72, 0x3b, 0xbd, 0xa3, 0xcc, 0xe8};

bool nervos_generate_ckb_address(char *output,uint8_t *pubkeyhash,
                      char *hrp,uint32_t hashtype){
  /* Full Payload Format
   *            1        32         1         20
   * payload = 0x00 | code_hash | hash_type | args
  */
  uint8_t payload[54] = {0};
  // format type
  payload[0] = 0x00;
  // code_hash
  memcpy(payload + 1, defaultCodeHash, sizeof(defaultCodeHash));
  // hash_type
  payload[33] = hashtype;

  if (hashtype >= HASHTYPE_INVALID){
    return false;
  }
  
  //SECP256K1/blake160
  BLAKE2B_CTX ctx;
  blake2b_Init(&ctx, 20);
  blake2b_Update(&ctx, pubkeyhash, 20);
  blake2b_Final(&ctx, payload + 34, 20);

  uint8_t dataPart[54];
  size_t dataPartSize = 0;
  convert_bits(dataPart, &dataPartSize, 5, payload, sizeof(payload), 8, 1);
  if (1 != bech32_encode(output, hrp, dataPart, dataPartSize, BECH32_ENCODING_BECH32M)) {
    return false;
  }                        
  return true;
}



static bool nervos_path_check_bip44(uint32_t address_n_count,
                                      const uint32_t *address_n,
                                      bool pubkey_export) {
  bool valid = (address_n_count >= 3);
  valid = valid && (address_n[0] == (PATH_HARDENED | 44));
  valid = valid && (address_n[1] & PATH_HARDENED);
  valid = valid && (address_n[2] & PATH_HARDENED);
  valid = valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);

  uint32_t path_slip44 = address_n[1] & PATH_UNHARDEN_MASK;
  valid = valid && (path_slip44 == 309);

  if (pubkey_export) {
    // m/44'/coin_type'/account'/*
    return valid;
  }

  if (address_n_count == 3) {
    // SEP-0005 for non-UTXO-based currencies, defined by Stellar:
    // https://github.com/stellar/stellar-protocol/blob/master/ecosystem/sep-0005.md
    // m/44'/coin_type'/account'
    return valid;
  }

  if (address_n_count == 4) {
    // Also to support "Ledger Live" legacy paths
    // https://github.com/trezor/trezor-firmware/issues/1749
    // m/44'/coin_type'/0'/account
    valid = valid && (address_n[2] == (PATH_HARDENED | 0));
    valid = valid && (address_n[3] <= PATH_MAX_ACCOUNT);
    return valid;
  }

  valid = valid && (address_n_count == 5);
  valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
  valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);

  return valid;
}

bool nervos_path_check(uint32_t address_n_count,const uint32_t *address_n, bool pubkey_export){
  if (address_n_count == 0) {
    return false;
  }
  if (address_n[0] == (PATH_HARDENED | 44)) {
    return nervos_path_check_bip44(address_n_count, address_n, pubkey_export);
  }
  return false;
}