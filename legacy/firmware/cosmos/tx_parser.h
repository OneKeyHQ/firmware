#ifndef __COSMOS_TX_PARSER_H__
#define __COSMOS_TX_PARSER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "json_parser.h"

#define __Z_INLINE inline __attribute__((always_inline)) static
#define CHECK_APP_CANARY() \
  {}
#define array_length(array) (sizeof(array) / sizeof((array)[0]))
__Z_INLINE void strncpy_s(char *dst, const char *src, size_t dstSize) {
  memset(dst, 0, dstSize);
  if (dstSize > 0) {
    strncpy(dst, src, dstSize - 1);
  }
}
__Z_INLINE uint8_t fpstr_to_str(char *out, uint16_t outLen, const char *number,
                                uint8_t decimals) {
  memset(out, 0, outLen);
  size_t digits = strlen(number);

  if (decimals == 0) {
    if (digits == 0) {
      snprintf(out, outLen, "0");
      return 0;
    }

    if (outLen < digits) {
      snprintf(out, outLen, "ERR");
      return 1;
    }

    // No need for formatting
    snprintf(out, outLen, "%s", number);
    return 0;
  }

  if ((outLen < decimals + 2)) {
    snprintf(out, outLen, "ERR");
    return 1;
  }

  if (outLen < digits + 2) {
    snprintf(out, outLen, "ERR");
    return 1;
  }

  if (digits <= decimals) {
    if (outLen <= decimals + 2) {
      snprintf(out, outLen, "ERR");
      return 1;
    }

    // First part
    snprintf(out, outLen, "0.");
    out += 2;
    outLen -= 2;

    memset(out, '0', decimals - digits);
    out += decimals - digits;
    outLen -= decimals - digits;

    snprintf(out, outLen, "%s", number);
    return 0;
  }

  const size_t shift = digits - decimals;
  snprintf(out, outLen, "%s", number);
  number += shift;

  out += shift;
  outLen -= shift;

  *out++ = '.';
  outLen--;
  snprintf(out, outLen, "%s", number);
  return 0;
}

__Z_INLINE void number_inplace_trimming(char *s, uint8_t non_trimmed) {
  const size_t len = strlen(s);
  if (len == 0 || len == 1 || len > 1024) {
    return;
  }

  int16_t dec_point = -1;
  for (int16_t i = 0; i < (int16_t)len && dec_point < 0; i++) {
    if (s[i] == '.') {
      dec_point = i;
    }
  }
  if (dec_point < 0) {
    return;
  }

  const size_t limit = (size_t)dec_point + non_trimmed;
  for (size_t i = (len - 1); i > limit && s[i] == '0'; i--) {
    s[i] = 0;
  }
}

typedef enum {
  zxerr_unknown = 0b00000000,
  zxerr_ok = 0b00000011,
  zxerr_no_data = 0b00000101,
  zxerr_buffer_too_small = 0b00000110,
  zxerr_out_of_bounds = 0b00001001,
  zxerr_encoding_failed = 0b00001010,
  zxerr_invalid_crypto_settings = 0b00001100,
  zxerr_ledger_api_error = 0b00001111,
} zxerr_t;

__Z_INLINE size_t z_strlen(const char *buffer, size_t maxSize) {
  if (buffer == NULL) return 0;
  return strnlen(buffer, maxSize);
}

__Z_INLINE zxerr_t z_str3join(char *buffer, size_t bufferSize,
                              const char *prefix, const char *suffix) {
  size_t messageSize = z_strlen(buffer, bufferSize);
  size_t prefixSize = strlen(prefix);
  size_t suffixSize = strlen(suffix);
  if (prefixSize > bufferSize) prefixSize = bufferSize;
  if (suffixSize > bufferSize) suffixSize = bufferSize;

  size_t requiredSize =
      1 /* termination */ + messageSize + prefixSize + suffixSize;

  if (bufferSize < requiredSize) {
    snprintf(buffer, bufferSize, "ERR???");
    return zxerr_buffer_too_small;
  }

  if (suffixSize > 0) {
    memmove(buffer + messageSize, suffix, suffixSize);
    buffer[messageSize + suffixSize] = 0;
  }

  // shift and add prefix
  if (prefixSize > 0) {
    memmove(buffer + prefixSize, buffer, messageSize + suffixSize + 1);
    memmove(buffer, prefix, prefixSize);
  }

  return zxerr_ok;
}

__Z_INLINE void pageStringExt(char *outValue, uint16_t outValueLen,
                              const char *inValue, uint16_t inValueLen,
                              uint8_t pageIdx, uint8_t *pageCount) {
  memset(outValue, 0, outValueLen);
  *pageCount = 0;

  outValueLen--;  // leave space for NULL termination
  if (outValueLen == 0) {
    return;
  }

  if (inValueLen == 0) {
    return;
  }

  *pageCount = (uint8_t)(inValueLen / outValueLen);
  const uint16_t lastChunkLen = (inValueLen % outValueLen);

  if (lastChunkLen > 0) {
    (*pageCount)++;
  }

  if (pageIdx < *pageCount) {
    if (lastChunkLen > 0 && pageIdx == *pageCount - 1) {
      memcpy(outValue, inValue + (pageIdx * outValueLen), lastChunkLen);
    } else {
      memcpy(outValue, inValue + (pageIdx * outValueLen), outValueLen);
    }
  }
}

__Z_INLINE void pageString(char *outValue, uint16_t outValueLen,
                           const char *inValue, uint8_t pageIdx,
                           uint8_t *pageCount) {
  pageStringExt(outValue, outValueLen, inValue, (uint16_t)strlen(inValue),
                pageIdx, pageCount);
}

// parser_txdef.h
typedef struct {
  // These are internal values used for tracking the state of the query/search
  uint16_t _item_index_current;

  // maximum json tree level. Beyond this tree depth, key/values are flattened
  uint8_t max_level;

  // maximum tree traversal depth. This limits possible stack overflow issues
  uint8_t max_depth;

  // Index of the item to retrieve
  int16_t item_index;
  // Chunk of the item to retrieve (assuming partitioning based on out_val_len
  // chunks)
  int16_t page_index;

  // These fields (out_*) are where query results are placed
  char *out_key;
  uint16_t out_key_len;
  char *out_val;
  int16_t out_val_len;
} tx_query_t;

typedef struct {
  // Buffer to the original tx blob
  const char *tx;

  // parsed data (tokens, etc.)
  parsed_json_t json;

  // internal flags
  struct {
    bool cache_valid : 1;
    bool msg_type_grouping : 1;  // indicates if msg type grouping is enabled
    bool msg_from_grouping : 1;  // indicates if msg from grouping is enabled
    bool msg_from_grouping_hide_all : 1;  // indicates if msg from grouping
                                          // should hide all
  } flags;

  // indicates that N identical msg_type fields have been detected
  uint8_t filter_msg_type_count;
  int32_t filter_msg_type_valid_idx;

  // indicates that N identical msg_from fields have been detected
  uint8_t filter_msg_from_count;
  int32_t filter_msg_from_valid_idx;
  const char *own_addr;

  // current tx query
  tx_query_t query;
} parser_tx_t;

// parser_impl.h
typedef struct {
  char str1[50];
  char str2[50];
} key_subst_t;

extern parser_tx_t parser_tx_obj;

parser_error_t cosmos_parser_init(parser_context_t *ctx, const uint8_t *buffer,
                                  size_t bufferSize);

parser_error_t _cosmos_readTx(parser_context_t *c, parser_tx_t *v);

// tx_parser.h
#define MAX_RECURSION_DEPTH 6
#define MULTISEND_KEY_IDX 9
extern bool extraDepthLevel;

#define INIT_QUERY_CONTEXT(_KEY, _KEY_LEN, _VAL, _VAL_LEN, _PAGE_IDX, \
                           _MAX_LEVEL)                                \
  parser_tx_obj.query._item_index_current = 0;                        \
  parser_tx_obj.query.max_depth = MAX_RECURSION_DEPTH;                \
  parser_tx_obj.query.max_level = _MAX_LEVEL;                         \
                                                                      \
  parser_tx_obj.query.item_index = 0;                                 \
  parser_tx_obj.query.page_index = (_PAGE_IDX);                       \
                                                                      \
  memset(_KEY, 0, (_KEY_LEN));                                        \
  memset(_VAL, 0, (_VAL_LEN));                                        \
  parser_tx_obj.query.out_key = _KEY;                                 \
  parser_tx_obj.query.out_val = _VAL;                                 \
  parser_tx_obj.query.out_key_len = (_KEY_LEN);                       \
  parser_tx_obj.query.out_val_len = (_VAL_LEN);

parser_error_t tx_traverse_find(uint16_t root_token_index,
                                uint16_t *ret_value_token_index);

// Traverses transaction data and fills tx_context
parser_error_t tx_traverse(int16_t root_token_index, uint8_t *numChunks);

// Retrieves the value for the corresponding token index. If the value goes
// beyond val_len, the chunk_idx will be used
parser_error_t tx_getToken(uint16_t token_index, char *out_val,
                           uint16_t out_val_len, uint8_t pageIdx,
                           uint8_t *pageCount);

__Z_INLINE bool is_msg_type_field(char *field_name) {
  return strcmp(field_name, "msgs/type") == 0;
}

__Z_INLINE bool is_msg_from_field(char *field_name) {
  return strcmp(field_name, "msgs/value/delegator_address") == 0;
}

#endif  //__COSMOS_TX_PARSER_H__
