#ifndef __POLKADOT_PARSER_IMPL_H__
#define __POLKADOT_PARSER_IMPL_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "parser_common.h"

// Checks that there are at least SIZE bytes available in the buffer
#define CTX_CHECK_AVAIL(CTX, SIZE)                                    \
  if ((CTX) == NULL || ((CTX)->offset + (SIZE)) > (CTX)->bufferLen) { \
    return parser_unexpected_buffer_end;                              \
  }

#define CTX_CHECK_AND_ADVANCE(CTX, SIZE) \
  CTX_CHECK_AVAIL((CTX), (SIZE))         \
  (CTX)->offset += (SIZE);

// Checks function input is valid
#define CHECK_INPUT()      \
  if (v == NULL) {         \
    return parser_no_data; \
  }                        \
  CTX_CHECK_AVAIL(         \
      c, 1)  // Checks that there is something available in the buffer

#define CLEAN_AND_CHECK()           \
  memset(outValue, 0, outValueLen); \
  if (v == NULL) {                  \
    *pageCount = 0;                 \
    return parser_no_data;          \
  }

#define GEN_DEF_READARRAY(SIZE)    \
  v->_ptr = c->buffer + c->offset; \
  CTX_CHECK_AND_ADVANCE(c, SIZE)   \
  return parser_ok;

#define GEN_DEF_TOSTRING_ARRAY(SIZE)                                 \
  CLEAN_AND_CHECK();                                                 \
  if (v->_ptr == NULL || outValueLen == 0)                           \
    return parser_unexpected_buffer_end;                             \
  const uint16_t outLenNormalized = (outValueLen - 1) / 2;           \
  if (outLenNormalized == 0) return parser_unexpected_buffer_end;    \
  *pageCount = (SIZE) / outLenNormalized;                            \
  if ((SIZE) % outLenNormalized != 0) *pageCount += 1;               \
  const uint16_t pageOffset = pageIdx * outLenNormalized;            \
  uint16_t loopmax = outLenNormalized;                               \
  if (loopmax > (SIZE)-pageOffset) loopmax = (SIZE)-pageOffset;      \
  for (uint16_t i = 0; i < loopmax; i++) {                           \
    const uint16_t offset = i << 1u;                                 \
    const uint8_t *c = v->_ptr + pageOffset;                         \
    snprintf(outValue + offset, outValueLen - offset, "%02x", c[i]); \
  }                                                                  \
  return parser_ok;

#define GEN_DEF_TOSTRING_ENUM(NAME)        \
  (*pageCount)++;                          \
  if (pageIdx == 0) {                      \
    snprintf(outValue, outValueLen, NAME); \
    return parser_ok;                      \
  }                                        \
  pageIdx--;

#define GEN_DEC_READFIX_UNSIGNED(BITS) \
  parser_error_t _preadUInt##BITS(parser_context_t *ctx, uint##BITS##_t *value);
#define GEN_DEF_READFIX_UNSIGNED(BITS)                                        \
  parser_error_t _preadUInt##BITS(parser_context_t *ctx,                      \
                                  uint##BITS##_t *value) {                    \
    if (value == NULL) return parser_no_data;                                 \
    *value = 0u;                                                              \
    for (uint8_t i = 0u; i < (BITS##u >> 3u); i++, ctx->offset++) {           \
      if (ctx->offset >= ctx->bufferLen) return parser_unexpected_buffer_end; \
      *value += (uint##BITS##_t) * (ctx->buffer + ctx->offset) << (8u * i);   \
    }                                                                         \
    return parser_ok;                                                         \
  }

GEN_DEC_READFIX_UNSIGNED(8);
GEN_DEC_READFIX_UNSIGNED(16);
GEN_DEC_READFIX_UNSIGNED(32);
GEN_DEC_READFIX_UNSIGNED(64);

#define GEN_DEF_READVECTOR(TYPE)                                              \
  pd_##TYPE##_t dummy;                                                        \
  compactInt_t clen;                                                          \
  CHECK_PARSER_ERR(_readCompactInt(c, &clen));                                \
  CHECK_PARSER_ERR(_getValue(&clen, &v->_len));                               \
  v->_ptr = c->buffer + c->offset;                                            \
  v->_lenBuffer = c->offset;                                                  \
  for (uint64_t i = 0; i < v->_len; i++) CHECK_ERROR(_read##TYPE(c, &dummy)); \
  v->_lenBuffer = c->offset - v->_lenBuffer;                                  \
  return parser_ok;

#define GEN_DEF_READVECTOR_VERSION(TYPE, VERSION)     \
  pd_##TYPE##_V##VERSION##_t dummy;                   \
  compactInt_t clen;                                  \
  CHECK_PARSER_ERR(_readCompactInt(c, &clen));        \
  CHECK_PARSER_ERR(_getValue(&clen, &v->_len));       \
  v->_ptr = c->buffer + c->offset;                    \
  v->_lenBuffer = c->offset;                          \
  for (uint64_t i = 0; i < v->_len; i++)              \
    CHECK_ERROR(_read##TYPE##_V##VERSION(c, &dummy)); \
  v->_lenBuffer = c->offset - v->_lenBuffer;          \
  return parser_ok;

#define GEN_DEF_READVECTOR_ITEM(VEC, TYPE, INDEX, VALUE)                                       \
  parser_context_t ctx;                                                                        \
  polkadot_parser_init(&ctx, (VEC)._ptr, (VEC)._lenBuffer);                                    \
  compactInt_t clen;                                                                           \
  CHECK_PARSER_ERR(_readCompactInt(&ctx, &clen));                                              \
  if ((INDEX) >= (VEC)._len) return parser_no_data;                                            \
  for (uint64_t i = 0; i < (VEC)._len; i++) CHECK_PARSER_ERR(_read_cro_##TYPE(&ctx, &(VALUE));  \
    return parser_ok;

#define GEN_DEF_TOSTRING_VECTOR(TYPE)                                       \
  CLEAN_AND_CHECK()                                                         \
  /* count number of pages, then output specific */                         \
  *pageCount = 0;                                                           \
  pd_##TYPE##_t tmp;                                                        \
  parser_context_t ctx;                                                     \
  uint8_t chunkPageCount;                                                   \
  uint16_t currentPage, currentTotalPage = 0;                               \
  if (v->_len == 0) {                                                       \
    *pageCount = 1;                                                         \
    snprintf(outValue, outValueLen, "<Empty>");                             \
    return parser_ok;                                                       \
  }                                                                         \
  /* We need to do it twice because there is no memory to keep intermediate \
   * results*/                                                              \
  /* First count*/                                                          \
  polkadot_parser_init(&ctx, v->_ptr, v->_lenBuffer);                       \
  for (uint16_t i = 0; i < v->_len; i++) {                                  \
    CHECK_ERROR(_read##TYPE(&ctx, &tmp));                                   \
    CHECK_ERROR(                                                            \
        _toString##TYPE(&tmp, outValue, outValueLen, 0, &chunkPageCount));  \
    (*pageCount) += chunkPageCount;                                         \
  }                                                                         \
  /* Then iterate until we can print the corresponding chunk*/              \
  polkadot_parser_init(&ctx, v->_ptr, v->_lenBuffer);                       \
  for (uint16_t i = 0; i < v->_len; i++) {                                  \
    CHECK_ERROR(_read##TYPE(&ctx, &tmp));                                   \
    chunkPageCount = 1;                                                     \
    currentPage = 0;                                                        \
    while (currentPage < chunkPageCount) {                                  \
      CHECK_ERROR(_toString##TYPE(&tmp, outValue, outValueLen, currentPage, \
                                  &chunkPageCount));                        \
      if (currentTotalPage == pageIdx) {                                    \
        return parser_ok;                                                   \
      }                                                                     \
      currentPage++;                                                        \
      currentTotalPage++;                                                   \
    }                                                                       \
  };                                                                        \
  return parser_print_not_supported;

parser_error_t polkadot_parser_init(parser_context_t *ctx,
                                    const uint8_t *buffer, uint16_t bufferSize);

parser_error_t _readCompactInt(parser_context_t *c, compactInt_t *v);

parser_error_t _readCompactBalance(parser_context_t *c, pd_CompactBalance_t *v);

parser_error_t _getValue(const compactInt_t *c, uint64_t *v);

parser_error_t _readCallIndex(parser_context_t *c, pd_CallIndex_t *v);

parser_error_t _readEra(parser_context_t *c, pd_ExtrinsicEra_t *v);

parser_error_t _polkadot_readTx(parser_context_t *c, parser_tx_t *v,
                                bool mode_enabled);
// parser_error_t _checkVersions(parser_context_t *c);

uint16_t getAddressType(void);
void setAddressType(uint16_t addressType);
parser_error_t _readCompactIndex(parser_context_t *c, pd_CompactIndex_t *v);

int8_t detectNetworkMetadata(void);

parser_error_t _toStringCompactInt(const compactInt_t *c, uint8_t decimalPlaces,
                                   bool trimTrailingZeros, char postfix[],
                                   char prefix[], char *outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t *pageCount);

parser_error_t _toStringCompactIndex(const pd_CompactIndex_t *v, char *outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t *pageCount);

parser_error_t _toStringPubkeyAsAddress(const uint8_t *pubkey, char *outValue,
                                        uint16_t outValueLen, uint8_t pageIdx,
                                        uint8_t *pageCount);

parser_error_t _toStringCompactBalance(const pd_CompactBalance_t *v,
                                       char *outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t *pageCount);

#endif
