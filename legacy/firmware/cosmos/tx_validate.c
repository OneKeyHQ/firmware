#include "../jsmn.h"
#include "json_parser.h"

const char whitespaces[] = {
    0x20,  // space ' '
    0x0c,  // form_feed '\f'
    0x0a,  // line_feed, '\n'
    0x0d,  // carriage_return, '\r'
    0x09,  // horizontal_tab, '\t'
    0x0b   // vertical_tab, '\v'
};

int8_t is_space(char c) {
  for (uint32_t i = 0; i < sizeof(whitespaces); i++) {
    if (whitespaces[i] == c) {
      return 1;
    }
  }
  return 0;
}

int8_t contains_whitespace(parsed_json_t *json) {
  int start = 0;
  const int last_element_index = json->tokens[0].end;

  // Starting at token 1 because token 0 contains full tx
  for (uint32_t i = 1; i < json->numberOfTokens; i++) {
    if (json->tokens[i].type != JSMN_UNDEFINED) {
      const int end = json->tokens[i].start;
      for (int j = start; j < end; j++) {
        if (is_space(json->buffer[j]) == 1) {
          return 1;
        }
      }
      start = json->tokens[i].end + 1;
    } else {
      return 0;
    }
  }
  while (start < last_element_index && json->buffer[start] != '\0') {
    if (is_space(json->buffer[start])) {
      return 1;
    }
    start++;
  }
  return 0;
}

int8_t is_sorted(uint16_t first_index, uint16_t second_index,
                 parsed_json_t *json) {
  char first[256] = {0};
  char second[256] = {0};

  size_t size = json->tokens[first_index].end - json->tokens[first_index].start;
  if (size >= sizeof(first)) {
    return 0;
  }

  strncpy(first, json->buffer + json->tokens[first_index].start, size);
  first[size] = '\0';

  size = json->tokens[second_index].end - json->tokens[second_index].start;
  if (size >= sizeof(second)) return 0;

  strncpy(second, json->buffer + json->tokens[second_index].start, size);
  second[size] = '\0';

  if (strcmp(first, second) <= 0) {
    return 1;
  }

  return 0;
}

int8_t dictionaries_sorted(parsed_json_t *json) {
  for (uint32_t i = 0; i < json->numberOfTokens; i++) {
    if (json->tokens[i].type == JSMN_OBJECT) {
      uint16_t count;

      if (object_get_element_count(json, i, &count) != parser_ok) {
        return 0;
      }

      if (count > 1) {
        uint16_t prev_token_index;
        if (object_get_nth_key(json, i, 0, &prev_token_index) != parser_ok) {
          return 0;
        }

        for (int j = 1; j < count; j++) {
          uint16_t next_token_index;

          if (object_get_nth_key(json, i, j, &next_token_index) != parser_ok) {
            return 0;
          }

          if (!is_sorted(prev_token_index, next_token_index, json)) {
            return 0;
          }
          prev_token_index = next_token_index;
        }
      }
    }
  }
  return 1;
}

parser_error_t tx_validate(parsed_json_t *json) {
  if (contains_whitespace(json) == 1) {
    return parser_json_contains_whitespace;
  }

  if (dictionaries_sorted(json) != 1) {
    return parser_json_is_not_sorted;
  }

  uint16_t token_index;
  parser_error_t err;

  err = object_get_value(json, 0, "chain_id", &token_index);
  if (err != parser_ok) return parser_json_missing_chain_id;

  err = object_get_value(json, 0, "sequence", &token_index);
  if (err != parser_ok) return parser_json_missing_sequence;

  err = object_get_value(json, 0, "fee", &token_index);
  if (err != parser_ok) return parser_json_missing_fee;

  err = object_get_value(json, 0, "msgs", &token_index);
  if (err != parser_ok) return parser_json_missing_msgs;

  err = object_get_value(json, 0, "account_number", &token_index);
  if (err != parser_ok) return parser_json_missing_account_number;

  err = object_get_value(json, 0, "memo", &token_index);
  if (err != parser_ok) return parser_json_missing_memo;

  return parser_ok;
}
