#ifndef __ALGO_PARSER_H__
#define __ALGO_PARSER_H__

#include "parser_impl.h"

const char *parser_getErrorDescription(parser_error_t err);
const char *parser_getMsgPackTypeDescription(uint8_t type);

//// parses a tx buffer
parser_error_t parser_parse(parser_context_t *ctx, const uint8_t *data,
                            size_t dataLen, parser_tx_t *tx_obj);

//// verifies tx fields
parser_error_t parser_validate(parser_context_t *ctx);

//// returns the number of items in the current parsing context
parser_error_t parser_getNumItems(uint8_t *num_items);

// retrieves a readable output for each field / page
parser_error_t parser_getItem(parser_context_t *ctx, uint8_t displayIdx,
                              char *outKey, uint16_t outKeyLen, char *outVal,
                              uint16_t outValLen, uint8_t pageIdx,
                              uint8_t *pageCount);

parser_error_t getItem(uint8_t index, uint8_t *displayIdx);

#endif
