#ifndef __FILECOIN_PARSER_TXDEF_H__
#define __FILECOIN_PARSER_TXDEF_H__

#include <stddef.h>
#include <stdint.h>

#define CBOR_PARSER_MAX_RECURSIONS 4

#define MAX_SUPPORT_METHOD 50
#define MAX_PARAMS_BUFFER_SIZE 200

// https://github.com/filecoin-project/lotus/blob/65c669b0f2dfd8c28b96755e198b9cdaf0880df8/chain/address/address.go#L36
// https://github.com/filecoin-project/lotus/blob/65c669b0f2dfd8c28b96755e198b9cdaf0880df8/chain/address/address.go#L371-L373
// Should not be more than 64 bytes
typedef struct {
  uint8_t buffer[64];
  size_t len;
} address_t;

// https://github.com/filecoin-project/lotus/blob/3fda442bb3372c9055ec0e237c70dd30143b65d8/chain/types/bigint.go#L238-L240
typedef struct {
  // https://github.com/filecoin-project/lotus/blob/3fda442bb3372c9055ec0e237c70dd30143b65d8/chain/types/bigint.go#L17
  uint8_t buffer[129];
  size_t len;
} bigint_t;

// https://github.com/filecoin-project/lotus/blob/eb4f4675a5a765e4898ec6b005ba2e80da8e7e1a/chain/types/message.go#L24-L39
typedef struct {
  int64_t version;
  address_t to;
  address_t from;
  uint64_t nonce;
  bigint_t value;
  int64_t gaslimit;
  bigint_t gaspremium;
  bigint_t gasfeecap;
  uint64_t method;

  uint8_t numparams;
  uint8_t params[MAX_PARAMS_BUFFER_SIZE];
} parser_tx_t;

#endif
