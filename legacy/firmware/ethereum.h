/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2016 Alex Beregszaszi <alex@rtfs.hu>
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

#ifndef __ETHEREUM_H__
#define __ETHEREUM_H__

#include <stdbool.h>
#include <stdint.h>
#include "bip32.h"
#include "messages-ethereum.pb.h"

#define CHAIN_ID_UNKNOWN UINT64_MAX

void ethereum_signing_init(const EthereumSignTx *msg, const HDNode *node);
void ethereum_signing_init_eip1559(const EthereumSignTxEIP1559 *msg,
                                   const HDNode *node);
void ethereum_signing_abort(void);
void ethereum_signing_txack(const EthereumTxAck *msg);

void ethereum_message_sign(const EthereumSignMessage *msg, const HDNode *node,
                           EthereumMessageSignature *resp);
int ethereum_message_verify(const EthereumVerifyMessage *msg);
void ethereum_typed_hash_sign(const EthereumSignTypedHash *msg,
                              const HDNode *node,
                              EthereumTypedDataSignature *resp);
bool ethereum_parse(const char *address, uint8_t pubkeyhash[20]);
void ethereum_message_sign_eip712(const EthereumSignMessageEIP712 *msg,
                                  const HDNode *node,
                                  EthereumMessageSignature *resp);
const char *get_chain_name(uint32_t chain_id);

// To reduce space, only some EVM networks are supported
#define ASSIGN_ETHEREUM_NAME(chain_name, chain_id) \
  switch (chain_id) {                              \
    case 1:                                        \
      chain_name = "Ethereum";                     \
      break;                                       \
    case 4:                                        \
      chain_name = "Rinkeby";                      \
      break;                                       \
    case 10:                                       \
      chain_name = "Optimistic";                   \
      break;                                       \
    case 56:                                       \
      chain_name = "BSC";                          \
      break;                                       \
    case 61:                                       \
      chain_name = "Ethereum Classic";             \
      break;                                       \
    case 66:                                       \
      chain_name = "OKXChain";                     \
      break;                                       \
    case 128:                                      \
      chain_name = "Huobi ECO";                    \
      break;                                       \
    case 137:                                      \
      chain_name = "Polygon";                      \
      break;                                       \
    case 42161:                                    \
      chain_name = "Arbitrum One";                 \
      break;                                       \
    case 42170:                                    \
      chain_name = "Arbitrum Nova";                \
      break;                                       \
    default:                                       \
      chain_name = "Unknown Chain";                \
      break; /* unknown chain */                   \
  }

bool ethereum_path_check(uint32_t address_n_count, const uint32_t *address_n,
                         bool pubkey_export, uint64_t chain);
#endif
