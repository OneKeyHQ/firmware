#ifndef __ALGO_PARSER_IMPL_H__
#define __ALGO_PARSER_IMPL_H__

#include "parser_common.h"
#include "parser_txdef.h"

// Checks that there are at least SIZE bytes available in the buffer
#define CTX_CHECK_AVAIL(CTX, SIZE)                                    \
  if ((CTX) == NULL || ((CTX)->offset + (SIZE)) > (CTX)->bufferLen) { \
    return parser_unexpected_buffer_end;                              \
  }

#define CTX_CHECK_AND_ADVANCE(CTX, SIZE) \
  CTX_CHECK_AVAIL((CTX), (SIZE))         \
  (CTX)->offset += (SIZE);

#define DEF_READARRAY(SIZE)        \
  v->_ptr = c->buffer + c->offset; \
  CTX_CHECK_AND_ADVANCE(c, SIZE)   \
  return parser_ok;

#define DEF_READFIX_UNSIGNED(BITS) \
  parser_error_t _readUInt##BITS(parser_context_t *ctx, uint##BITS##_t *value)
#define DEC_READFIX_UNSIGNED(BITS)                                             \
  parser_error_t _readUInt##BITS(parser_context_t *ctx,                        \
                                 uint##BITS##_t *value) {                      \
    if (value == NULL) return parser_no_data;                                  \
    *value = 0u;                                                               \
    for (uint8_t i = 0u; i < (BITS##u >> 3u); i++, ctx->offset++) {            \
      if (ctx->offset >= ctx->bufferLen) return parser_unexpected_buffer_end;  \
      *value = (*value << 8) | (uint##BITS##_t) * (ctx->buffer + ctx->offset); \
    }                                                                          \
    return parser_ok;                                                          \
  }

parser_error_t _readBytes(parser_context_t *c, uint8_t *buff, uint16_t bufLen);

parser_error_t parser_init(parser_context_t *ctx, const uint8_t *buffer,
                           uint16_t bufferSize);

uint8_t _getNumItems(void);
uint8_t _getCommonNumItems(void);
uint8_t _getTxNumItems(void);

parser_error_t _read(parser_context_t *c, parser_tx_t *v);

parser_error_t _readMapSize(parser_context_t *c, uint16_t *mapItems);
parser_error_t _readArraySize(parser_context_t *c, uint8_t *mapItems);
parser_error_t _readString(parser_context_t *c, uint8_t *buff,
                           uint16_t buffLen);
parser_error_t _readInteger(parser_context_t *c, uint64_t *value);
parser_error_t _readBool(parser_context_t *c, uint8_t *value);
parser_error_t _readArray_args(parser_context_t *c, uint8_t args[][MAX_ARGLEN],
                               size_t args_len[], uint8_t *argsSize,
                               uint8_t maxArgs);
parser_error_t _readBinFixed(parser_context_t *c, uint8_t *buff,
                             uint16_t bufferLen);
parser_error_t _findKey(parser_context_t *c, const char *key);

parser_error_t _getAccount(parser_context_t *c, uint8_t *account,
                           uint8_t account_idx, uint8_t num_accounts);
parser_error_t _getAppArg(parser_context_t *c, uint8_t **args,
                          uint16_t *args_len, uint8_t args_idx,
                          uint16_t max_args_len, uint8_t max_array_len);

DEF_READFIX_UNSIGNED(8);
DEF_READFIX_UNSIGNED(16);
DEF_READFIX_UNSIGNED(32);
DEF_READFIX_UNSIGNED(64);

#endif
