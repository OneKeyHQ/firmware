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

#include "json_parser.h"
#include "../jsmn.h"

#define MEMMOVE memmove
#define MEMSET memset
#define MEMCPY memcpy
#define MEMCMP memcmp
#define MEMCPY_NV memcpy
#define PIC(x) (x)
#define EQUALS(_P, _Q, _LEN) \
  (MEMCMP((const void *)PIC(_P), (const void *)PIC(_Q), (_LEN)) == 0)

parser_error_t json_parse(parsed_json_t *parsed_json, const char *buffer,
                          uint16_t bufferLen) {
  jsmn_parser parser;
  jsmn_init(&parser);

  memset(parsed_json, 0, sizeof(parsed_json_t));
  parsed_json->buffer = buffer;
  parsed_json->bufferLen = bufferLen;

  int32_t num_tokens =
      jsmn_parse(&parser, parsed_json->buffer, parsed_json->bufferLen,
                 parsed_json->tokens, MAX_NUMBER_OF_TOKENS);

  if (num_tokens < 0) {
    switch (num_tokens) {
      case JSMN_ERROR_NOMEM:
        return parser_json_too_many_tokens;
      case JSMN_ERROR_INVAL:
        return parser_unexpected_characters;
      case JSMN_ERROR_PART:
        return parser_json_incomplete_json;
      default:
        return parser_json_unexpected_error;
    }
  }

  parsed_json->numberOfTokens = 0;
  parsed_json->isValid = 0;

  // Parsing error
  if (num_tokens <= 0) {
    return parser_json_zero_tokens;
  }

  // We cannot support if number of tokens exceeds the limit
  if (num_tokens > MAX_NUMBER_OF_TOKENS) {
    return parser_json_too_many_tokens;
  }

  parsed_json->numberOfTokens = num_tokens;
  parsed_json->isValid = true;

  return parser_ok;
}

parser_error_t array_get_element_count(const parsed_json_t *json,
                                       uint16_t array_token_index,
                                       uint16_t *number_elements) {
  *number_elements = 0;
  if (array_token_index > json->numberOfTokens) {
    return parser_no_data;
  }

  jsmntok_t array_token = json->tokens[array_token_index];
  uint16_t token_index = array_token_index;
  uint16_t prev_element_end = array_token.start;
  while (true) {
    token_index++;
    if (token_index >= json->numberOfTokens) {
      break;
    }
    jsmntok_t current_token = json->tokens[token_index];
    if (current_token.start > array_token.end) {
      break;
    }
    if (current_token.start <= prev_element_end) {
      continue;
    }
    prev_element_end = current_token.end;
    (*number_elements)++;
  }

  return parser_ok;
}

parser_error_t array_get_nth_element(const parsed_json_t *json,
                                     uint16_t array_token_index,
                                     uint16_t element_index,
                                     uint16_t *token_index) {
  if (array_token_index > json->numberOfTokens) {
    return parser_no_data;
  }

  jsmntok_t array_token = json->tokens[array_token_index];
  *token_index = array_token_index;

  uint16_t element_count = 0;
  uint16_t prev_element_end = array_token.start;
  while (true) {
    (*token_index)++;
    if (*token_index >= json->numberOfTokens) {
      break;
    }
    jsmntok_t current_token = json->tokens[*token_index];
    if (current_token.start > array_token.end) {
      break;
    }
    if (current_token.start <= prev_element_end) {
      continue;
    }
    prev_element_end = current_token.end;
    if (element_count == element_index) {
      return parser_ok;
    }
    element_count++;
  }

  return parser_no_data;
}

parser_error_t object_get_element_count(const parsed_json_t *json,
                                        uint16_t object_token_index,
                                        uint16_t *element_count) {
  *element_count = 0;
  if (object_token_index > json->numberOfTokens) {
    return parser_no_data;
  }

  jsmntok_t object_token = json->tokens[object_token_index];
  uint16_t token_index = object_token_index;
  uint16_t prev_element_end = object_token.start;
  token_index++;
  while (true) {
    if (token_index >= json->numberOfTokens) {
      break;
    }
    jsmntok_t key_token = json->tokens[token_index++];
    jsmntok_t value_token = json->tokens[token_index];
    if (key_token.start > object_token.end) {
      break;
    }
    if (key_token.start <= prev_element_end) {
      continue;
    }
    prev_element_end = value_token.end;
    (*element_count)++;
  }

  return parser_ok;
}

parser_error_t object_get_nth_key(const parsed_json_t *json,
                                  uint16_t object_token_index,
                                  uint16_t object_element_index,
                                  uint16_t *token_index) {
  *token_index = object_token_index;
  if (object_token_index > json->numberOfTokens) {
    return parser_no_data;
  }

  jsmntok_t object_token = json->tokens[object_token_index];
  uint16_t element_count = 0;
  uint16_t prev_element_end = object_token.start;
  (*token_index)++;
  while (true) {
    if (*token_index >= json->numberOfTokens) {
      break;
    }
    jsmntok_t key_token = json->tokens[(*token_index)++];
    jsmntok_t value_token = json->tokens[*token_index];
    if (key_token.start > object_token.end) {
      break;
    }
    if (key_token.start <= prev_element_end) {
      continue;
    }
    prev_element_end = value_token.end;
    if (element_count == object_element_index) {
      (*token_index)--;
      return parser_ok;
    }
    element_count++;
  }

  return parser_no_data;
}

parser_error_t object_get_nth_value(const parsed_json_t *json,
                                    uint16_t object_token_index,
                                    uint16_t object_element_index,
                                    uint16_t *key_index) {
  if (object_token_index > json->numberOfTokens) {
    return parser_no_data;
  }

  CHECK_PARSER_ERR(object_get_nth_key(json, object_token_index,
                                      object_element_index, key_index))
  (*key_index)++;

  return parser_ok;
}

parser_error_t object_get_value(const parsed_json_t *json,
                                uint16_t object_token_index,
                                const char *key_name, uint16_t *token_index) {
  if (object_token_index > json->numberOfTokens) {
    return parser_no_data;
  }

  const jsmntok_t object_token = json->tokens[object_token_index];

  *token_index = object_token_index;
  int prev_element_end = object_token.start;
  (*token_index)++;

  while (*token_index < json->numberOfTokens) {
    const jsmntok_t key_token = json->tokens[*token_index];
    (*token_index)++;
    const jsmntok_t value_token = json->tokens[*token_index];

    if (key_token.start > object_token.end) {
      break;
    }
    if (key_token.start <= prev_element_end) {
      continue;
    }
    prev_element_end = value_token.end;

    if (((uint16_t)strlen(key_name)) == (key_token.end - key_token.start)) {
      if (EQUALS(key_name, json->buffer + key_token.start,
                 key_token.end - key_token.start)) {
        return parser_ok;
      }
    }
  }

  return parser_no_data;
}
