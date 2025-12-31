#ifndef __POLKADOT_PARSER_COMMON_H__
#define __POLKADOT_PARSER_COMMON_H__

#include <stddef.h>
#include <stdint.h>
#include "parser_txdef.h"

#define CHECK_PARSER_ERR(__CALL)          \
  {                                       \
    parser_error_t __err = __CALL;        \
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
  // Coin specific
  parser_unexpected_address_type,
  parser_spec_not_supported,
  parser_tx_version_not_supported,
  parser_not_allowed,
  parser_not_supported,
  parser_unexpected_buffer_end,
  parser_unexpected_value,
  parser_value_out_of_range,
  parser_value_too_many_bytes,
  parser_unexpected_module,
  parser_unexpected_callIndex,
  parser_unexpected_unparsed_bytes,
  parser_print_not_supported,
  parser_tx_nesting_not_supported,
  parser_tx_nesting_limit_reached,
  parser_tx_call_vec_too_large,
} parser_error_t;

typedef struct {
  const uint8_t *buffer;
  uint16_t bufferLen;
  uint16_t offset;
  parser_tx_t *tx_obj;
  bool has_preset_address_type;
  uint16_t preset_address_type;
} parser_context_t;

#endif
