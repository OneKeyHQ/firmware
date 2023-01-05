#ifndef __FILECOIN_PARSER_IMPL_H__
#define __FILECOIN_PARSER_IMPL_H__

#include "common_defs.h"
#include "parser_txdef.h"

extern parser_tx_t fil_parser_tx_obj;

parser_error_t fil_parser_init(parser_context_t *ctx, const uint8_t *buffer,
                               uint16_t bufferSize);

parser_error_t _fil_read(const parser_context_t *c, parser_tx_t *v);

parser_error_t _fil_validateTx(const parser_context_t *c, const parser_tx_t *v);

parser_error_t _fil_printParam(const parser_tx_t *tx, uint8_t paramIdx,
                               char *outVal, uint16_t outValLen,
                               uint8_t pageIdx, uint8_t *pageCount);

parser_error_t _fil_printAddress(const address_t *a, char *outVal,
                                 uint16_t outValLen, uint8_t pageIdx,
                                 uint8_t *pageCount);

uint8_t _fil_getNumItems(const parser_context_t *c, const parser_tx_t *v);

parser_error_t fil_checkMethod(uint64_t methodValue);

#endif
