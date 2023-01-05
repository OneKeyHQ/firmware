/*
 * This file is part of the OneKey project, https://onekey.so/
 *
 * Copyright (C) 2022 OneKey Team <core@onekey.so>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __COSMOS_JSON_PARSE_H__
#define __COSMOS_JSON_PARSE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../jsmn.h"

// parser_common.h
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
  // Coin Specific
  parser_json_zero_tokens,
  parser_json_too_many_tokens,  // "NOMEM: JSON string contains too many tokens"
  parser_json_incomplete_json,  // "JSON string is not complete";
  parser_json_contains_whitespace,
  parser_json_is_not_sorted,
  parser_json_missing_chain_id,
  parser_json_missing_sequence,
  parser_json_missing_fee,
  parser_json_missing_msgs,
  parser_json_missing_account_number,
  parser_json_missing_memo,
  parser_json_unexpected_error,
} parser_error_t;

typedef struct {
  const uint8_t *buffer;
  uint16_t bufferLen;
  uint16_t offset;
} parser_context_t;

/// Max number of accepted tokens in the JSON input
#define MAX_NUMBER_OF_TOKENS 96

#define ROOT_TOKEN_INDEX 0

//---------------------------------------------

// Context that keeps all the parsed data together. That includes:
//  - parsed json tokens
//  - re-created SendMsg struct with indices pointing to tokens in parsed json
typedef struct {
  uint8_t isValid;
  uint32_t numberOfTokens;
  jsmntok_t tokens[MAX_NUMBER_OF_TOKENS];
  const char *buffer;
  uint16_t bufferLen;
} parsed_json_t;

//---------------------------------------------
// NEW JSON PARSER CODE

/// Parse json to create a token representation
/// \param parsed_json
/// \param transaction
/// \param transaction_length
/// \return Error message
parser_error_t json_parse(parsed_json_t *parsed_json, const char *transaction,
                          uint16_t transaction_length);

/// Get the number of elements in the array
/// \param json
/// \param array_token_index
/// \param number of elements (out)
/// \return Error message
parser_error_t array_get_element_count(const parsed_json_t *json,
                                       uint16_t array_token_index,
                                       uint16_t *number_elements);

/// Get the token index of the nth array's element
/// \param json
/// \param array_token_index
/// \param element_index
/// \param token index
/// \return Error message
parser_error_t array_get_nth_element(const parsed_json_t *json,
                                     uint16_t array_token_index,
                                     uint16_t element_index,
                                     uint16_t *token_index);

/// Get the number of dictionary elements (key/value pairs) under given object
/// \param json
/// \param object_token_index: token index of the parent object
/// \param number of elements (out)
/// \return Error message
parser_error_t object_get_element_count(const parsed_json_t *json,
                                        uint16_t object_token_index,
                                        uint16_t *number_elements);

/// Get the token index for the nth dictionary key
/// \param json
/// \param object_token_index: token index of the parent object
/// \param object_element_index
/// \return token index (out)
/// \return Error message
parser_error_t object_get_nth_key(const parsed_json_t *json,
                                  uint16_t object_token_index,
                                  uint16_t object_element_index,
                                  uint16_t *token_index);

/// Get the token index for the nth dictionary value
/// \param json
/// \param object_token_index: token index of the parent object
/// \param object_element_index
/// \return token index (out))
/// \return Error message
parser_error_t object_get_nth_value(const parsed_json_t *json,
                                    uint16_t object_token_index,
                                    uint16_t object_element_index,
                                    uint16_t *token_index);

/// Get the token index of the value that matches the given key
/// \param json
/// \param object_token_index: token index of the parent object
/// \param key_name: key name of the wanted value
/// \return Error message
parser_error_t object_get_value(const parsed_json_t *json,
                                uint16_t object_token_index,
                                const char *key_name, uint16_t *token_index);

/// Validate json transaction
/// \param parsed_transacton
/// \param transaction
/// \return
parser_error_t tx_validate(parsed_json_t *json);

#endif  // __COSMOS_JSON_PARSE_H__
