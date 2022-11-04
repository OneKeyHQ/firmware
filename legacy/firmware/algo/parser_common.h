#ifndef __ALGO_PARSER_COMMON_H__
#define __ALGO_PARSER_COMMON_H__

#include <stddef.h>
#include <stdint.h>
#include "parser_txdef.h"

#define CHECK_ERROR(__CALL)               \
  {                                       \
    parser_error_t __err = __CALL;        \
    CHECK_APP_CANARY()                    \
    if (__err != parser_ok) return __err; \
  }

typedef enum {
  // Generic errors
  parser_ok = 0,
  parser_no_data,
  parser_init_context_empty,
  parser_display_idx_out_of_range,
  parser_display_page_out_of_range,
  parser_unexpected_error,
  // Coin generic
  parser_unexpected_type,
  parser_unexpected_method,
  parser_unexpected_buffer_end,
  parser_unexpected_value,
  parser_unexpected_number_items,
  parser_unexpected_version,
  parser_unexpected_characters,
  parser_unexpected_field,
  parser_duplicated_field,
  parser_value_out_of_range,
  parser_invalid_address,
  parser_unexpected_chain,
  parser_missing_field,
  parser_query_no_results,

  paser_unknown_transaction,

  parser_key_not_found,

  // Msgpack specific
  parser_msgpack_unexpected_type,
  parser_msgpack_unexpected_key,

  parser_msgpack_map_type_expected,  // 33
  parser_msgpack_map_type_not_supported,

  parser_msgpack_str_type_expected,
  parser_msgpack_str_type_not_supported,
  parser_msgpack_str_too_big,

  parser_msgpack_bin_type_expected,
  parser_msgpack_bin_type_not_supported,
  parser_msgpack_bin_unexpected_size,

  parser_msgpack_int_type_expected,

  parser_msgpack_bool_type_expected,

  parser_msgpack_array_unexpected_size,
  parser_msgpack_array_too_big,
  parser_msgpack_array_type_expected,

} parser_error_t;

typedef struct {
  const uint8_t *buffer;
  uint16_t bufferLen;
  uint16_t offset;
  parser_tx_t *parser_tx_obj;
} parser_context_t;

#endif
