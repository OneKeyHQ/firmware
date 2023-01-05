#ifndef __POLKADOT_PARSER_TXDEF_H__
#define __POLKADOT_PARSER_TXDEF_H__

#include <stddef.h>
#include <stdint.h>
#include "substrate/substrate_methods.h"

#define MAX_CALL_NESTING_SIZE 2
#define MAX_CALL_VEC_SIZE 5

typedef struct {
  pd_CallIndex_t callIndex;
  pd_Method_t method;

  pd_ExtrinsicEra_t era;
  pd_CompactIndex_t nonce;
  pd_CompactBalance_t tip;
  uint32_t specVersion;
  uint32_t transactionVersion;

  pd_Hash_t genesisHash;
  pd_Hash_t blockHash;

  pd_NestCallIdx_t nestCallIdx;
} parser_tx_t;

#endif
