#include "parser.h"
#include <stdio.h>
#include "../util.h"
#include "bignum.h"
#include "coin.h"
#include "memzero.h"
#include "parser_impl.h"
#include "parser_txdef.h"
#include "secp256k1.h"

parser_error_t fil_parser_parse(parser_context_t *ctx, const uint8_t *data,
                                size_t dataLen) {
  CHECK_PARSER_ERR(fil_parser_init(ctx, data, dataLen))
  return _fil_read(ctx, &fil_parser_tx_obj);
}

parser_error_t fil_parser_validate(const parser_context_t *ctx) {
  CHECK_PARSER_ERR(_fil_validateTx(ctx, &fil_parser_tx_obj))

  // Iterate through all items to check that all can be shown and are valid
  uint8_t numItems = 0;
  CHECK_PARSER_ERR(fil_parser_getNumItems(ctx, &numItems));

  char tmpKey[40];
  char tmpVal[40];

  for (uint8_t idx = 0; idx < numItems; idx++) {
    uint8_t pageCount = 0;
    CHECK_PARSER_ERR(fil_parser_getItem(ctx, idx, tmpKey, sizeof(tmpKey),
                                        tmpVal, sizeof(tmpVal), 0, &pageCount))
  }

  return parser_ok;
}

parser_error_t fil_parser_getNumItems(const parser_context_t *ctx,
                                      uint8_t *num_items) {
  *num_items = _fil_getNumItems(ctx, &fil_parser_tx_obj);
  return parser_ok;
}

__Z_INLINE bool format_quantity(const bigint_t *b, uint8_t *bcd,
                                uint16_t bcdSize, char *bignum,
                                uint16_t bignumSize) {
  if (b->len < 2) {
    snprintf(bignum, bignumSize, "0");
    return true;
  }

  // first byte of b is the sign byte so we can remove this one
  bignumBigEndian_to_bcd(bcd, bcdSize, b->buffer + 1, b->len - 1);
  return bignumBigEndian_bcdprint(bignum, bignumSize, bcd, bcdSize);
}

parser_error_t parser_printParam(const parser_tx_t *tx, uint8_t paramIdx,
                                 char *outVal, uint16_t outValLen,
                                 uint8_t pageIdx, uint8_t *pageCount) {
  return _fil_printParam(tx, paramIdx, outVal, outValLen, pageIdx, pageCount);
}

__Z_INLINE parser_error_t parser_printBigIntFixedPoint(const bigint_t *b,
                                                       char *outVal,
                                                       uint16_t outValLen,
                                                       uint8_t pageIdx,
                                                       uint8_t *pageCount) {
  LESS_THAN_64_DIGIT(b->len)

  char bignum[160];
  union {
    // overlapping arrays to avoid excessive stack usage. Do not use at the same
    // time
    uint8_t bcd[80];
    char output[160 + 2];
  } overlapped;

  memzero(overlapped.bcd, sizeof(overlapped.bcd));
  memzero(bignum, sizeof(bignum));

  if (!format_quantity(b, overlapped.bcd, sizeof(overlapped.bcd), bignum,
                       sizeof(bignum))) {
    return parser_unexpected_value;
  }

  fpstr_to_str(overlapped.output, sizeof(overlapped.output), bignum,
               COIN_AMOUNT_DECIMAL_PLACES);
  pageString(outVal, outValLen, overlapped.output, pageIdx, pageCount);
  return parser_ok;
}

__Z_INLINE parser_error_t parser_printBigIntFixedPointTotal(
    const bigint_t *value, const bigint_t *gasfeecap, uint64_t gaslimit,
    char *outVal, uint16_t outValLen) {
  LESS_THAN_32_DIGIT(value->len)
  LESS_THAN_32_DIGIT(gasfeecap->len)
  bignum256 total = {0}, val = {0}, gas = {0}, limit = {0};
  uint8_t val_bytes[32] = {0};
  uint8_t gas_bytes[32] = {0};
  if (value->len > 0) {
    memcpy(val_bytes + (32 - value->len + 1), value->buffer + 1,
           value->len - 1);
  }
  memcpy(gas_bytes + (32 - gasfeecap->len + 1), gasfeecap->buffer + 1,
         gasfeecap->len - 1);

  bn_read_be(val_bytes, &val);
  bn_read_be(gas_bytes, &gas);
  bn_read_uint64(gaslimit, &limit);
  bn_multiply(&limit, &gas, &secp256k1.prime);
  bn_add(&total, &val);
  bn_add(&total, &gas);
  bn_format(&total, NULL, " FIL", 18, 0, false, 0, outVal, outValLen);

  return parser_ok;
}

parser_error_t fil_parser_getItem(const parser_context_t *ctx,
                                  uint8_t displayIdx, char *outKey,
                                  uint16_t outKeyLen, char *outVal,
                                  uint16_t outValLen, uint8_t pageIdx,
                                  uint8_t *pageCount) {
  memzero(outKey, outKeyLen);
  memzero(outVal, outValLen);
  snprintf(outKey, outKeyLen, "?");
  snprintf(outVal, outValLen, " ");
  *pageCount = 0;

  uint8_t numItems = 0;
  CHECK_PARSER_ERR(fil_parser_getNumItems(ctx, &numItems))
  CHECK_APP_CANARY()

  if (displayIdx >= numItems) {
    return parser_no_data;
  }

  if (displayIdx == 0) {
    snprintf(outKey, outKeyLen, "Amount");
    return parser_printBigIntFixedPoint(&fil_parser_tx_obj.value, outVal,
                                        outValLen, pageIdx, pageCount);
  }

  if (displayIdx == 1) {
    snprintf(outKey, outKeyLen, "Send to");
    return _fil_printAddress(&fil_parser_tx_obj.to, outVal, outValLen, pageIdx,
                             pageCount);
  }

  if (displayIdx == 2) {
    snprintf(outKey, outKeyLen, "From");
    return _fil_printAddress(&fil_parser_tx_obj.from, outVal, outValLen,
                             pageIdx, pageCount);
  }

  if (displayIdx == 3) {
    snprintf(outKey, outKeyLen, "Gas Limit");
    if (int64_to_str(outVal, outValLen, fil_parser_tx_obj.gaslimit) != NULL) {
      return parser_unexepected_error;
    }
    *pageCount = 1;
    return parser_ok;
  }

  if (displayIdx == 4) {
    snprintf(outKey, outKeyLen, "Gas Fee Cap");
    return parser_printBigIntFixedPoint(&fil_parser_tx_obj.gasfeecap, outVal,
                                        outValLen, pageIdx, pageCount);
  }

  if (displayIdx == 5) {
    snprintf(outKey, outKeyLen, "Gas Premium");
    return parser_printBigIntFixedPoint(&fil_parser_tx_obj.gaspremium, outVal,
                                        outValLen, pageIdx, pageCount);
  }

  if (displayIdx == 6) {
    snprintf(outKey, outKeyLen, "Total Amount");
    *pageCount = 1;
    return parser_printBigIntFixedPointTotal(
        &fil_parser_tx_obj.value, &fil_parser_tx_obj.gasfeecap,
        fil_parser_tx_obj.gaslimit, outVal, outValLen);
  }

  if (fil_parser_tx_obj.method != 0) {
    return parser_unexepected_error;  // Only support Transfer
  }

  return parser_ok;
}
