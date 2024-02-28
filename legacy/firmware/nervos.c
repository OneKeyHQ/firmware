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
#include "secp256k1.h"
#include "schnorr_bch.h"

#define MAX_ADDRESS_LENGTH 100
#define CODE_INDEX_SECP256K1_SINGLE 0x00
#define FORMAT_TYPE_SHORT 0x01


const char CHARSET[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

void bytes_to_hex_str(const uint8_t *bytes, size_t bytes_len, char *hex_str) {
  const char hex_chars[] = "0123456789abcdef";
  for (size_t i = 0; i < bytes_len; i++) {
    hex_str[2 * i] = hex_chars[(bytes[i] >> 4) & 0x0F];
    hex_str[2 * i + 1] = hex_chars[bytes[i] & 0x0F];
  }
  hex_str[2 * bytes_len] = '\0';  // Null-terminate the string
}

void ckb_blake160(const uint8_t *public_key, size_t public_key_len,
                    char *output) {

  uint8_t hash[32];  
  blake2b_state S;
  const uint8_t personal[] = "ckb-default-hash";
  size_t outlen = 32;  

  if (blake2b_InitPersonal(&S, outlen, personal, sizeof(personal) - 1) != 0) {return;}
  if (blake2b_Update(&S, public_key, public_key_len) != 0) {return;}
  if (blake2b_Final(&S, hash, outlen) != 0) {return;}

  char hash_hex[2 * 20 + 1];  
  bytes_to_hex_str(hash, 20, hash_hex);
  snprintf(output, 2 * 20 + 3, "%s", hash_hex);  
}



uint32_t bech32_polymod(const uint8_t *values, size_t len) {
    static const uint32_t generator[] = {0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3};
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


int convertbits(const uint8_t *data, size_t datalen, uint8_t *out, int frombits, int tobits, int pad) {
    uint32_t acc = 0;
    int bits = 0;
    size_t retlen = 0;
    const uint32_t maxv = (1 << tobits) - 1;
    const uint32_t max_acc = (1 << (frombits + tobits - 1)) - 1;
    for (size_t i = 0; i < datalen; ++i) {
        if (data[i] >> frombits) {
            return -1; // Value out of range
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
        return -1; // Cannot convert
    }
    return retlen; 
}

int hex_char_to_int(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return -1;
}

void hex_str_to_bytes(const char *hex_str, uint8_t *bytes, size_t bytes_len) {
    for (size_t i = 0; i < bytes_len; ++i) {
        int hi = hex_char_to_int(hex_str[2*i]);
        int lo = hex_char_to_int(hex_str[2*i + 1]);
        if (hi == -1 || lo == -1) {
            printf("Invalid hex string");
            return;
        }
        bytes[i] = (hi << 4) + lo;
    }
}



void nervos_get_address_from_public_key(const uint8_t *public_key,
                                        char *address,const char *network) {
    size_t public_key_len = 33;
    char output[43];  
    ckb_blake160(public_key, public_key_len, output);  //hash160 calculation
    uint8_t payload[22];  // format type(1) + code index(1) + hash(20)
    payload[0] = FORMAT_TYPE_SHORT;
    payload[1] = CODE_INDEX_SECP256K1_SINGLE;
    hex_str_to_bytes(output, payload + 2, 20); 

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

    uint32_t polymod = bech32_polymod(values, values_len) ^ 1;  //polymod calculation
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



void nervos_sign_sighash(HDNode *node, const uint8_t *raw_message,
                         uint32_t raw_message_len, uint8_t *signature,
                         pb_size_t *signature_len) 
{
    uint8_t v1;
    uint8_t sig[64]; 
    if (ecdsa_sign_digest(&secp256k1, node->private_key, raw_message,sig, &v1,NULL) != 0) {
          fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));                 
        }
    memcpy(signature, sig, 64);
    signature[64] = v1;
    *signature_len = 65;
}