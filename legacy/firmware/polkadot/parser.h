#ifndef __POLKADOT_PARSER_H__
#define __POLKADOT_PARSER_H__

#include "parser_common.h"
#include "parser_impl.h"

const char *polkadot_parser_getErrorDescription(parser_error_t err);

//// parses a tx buffer
parser_error_t polkadot_parser_parse(parser_context_t *ctx, const uint8_t *data,
                                     size_t dataLen, parser_tx_t *tx_obj);

//// verifies tx fields
parser_error_t polkadot_parser_validate(const parser_context_t *ctx);

//// returns the number of items in the current parsing context
parser_error_t polkadot_parser_getNumItems(const parser_context_t *ctx,
                                           uint8_t *num_items);

// retrieves a readable output for each field / page
parser_error_t polkadot_parser_getItem(const parser_context_t *ctx,
                                       uint8_t displayIdx, char *outKey,
                                       uint16_t outKeyLen, char *outVal,
                                       uint16_t outValLen, uint8_t pageIdx,
                                       uint8_t *pageCount);

#endif
