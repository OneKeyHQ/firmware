#ifndef __FILECOIN_PARSER_H__
#define __FILECOIN_PARSER_H__

#include <stdbool.h>
#include "parser_impl.h"

#define LESS_THAN_64_DIGIT(num_digit) \
  if (num_digit > 64) return parser_value_out_of_range;

extern bool bignumBigEndian_bcdprint(char *outBuffer, uint16_t outBufferLen,
                                     const uint8_t *bcdIn, uint16_t bcdInLen);
extern void bignumBigEndian_to_bcd(uint8_t *bcdOut, uint16_t bcdOutLen,
                                   const uint8_t *binValue,
                                   uint16_t binValueLen);

const char *fil_parser_getErrorDescription(parser_error_t err);

//// parses a tx buffer
parser_error_t fil_parser_parse(parser_context_t *ctx, const uint8_t *data,
                                size_t dataLen);

//// verifies tx fields
parser_error_t fil_parser_validate(const parser_context_t *ctx);

//// returns the number of items in the current parsing context
parser_error_t fil_parser_getNumItems(const parser_context_t *ctx,
                                      uint8_t *num_items);

// retrieves a readable output for each field / page
parser_error_t fil_parser_getItem(const parser_context_t *ctx,
                                  uint8_t displayIdx, char *outKey,
                                  uint16_t outKeyLen, char *outVal,
                                  uint16_t outValLen, uint8_t pageIdx,
                                  uint8_t *pageCount);

#endif
