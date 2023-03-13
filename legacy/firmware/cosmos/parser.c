#include "parser.h"
#include <stdio.h>
#include "../cosmos_networks.h"
#include "../gettext.h"
#include "coin.h"
#include "tx_display.h"
#include "tx_parser.h"

static char cosmos_chain_id[128] = {0};

parser_error_t cosmos_parser_parse(parser_context_t *ctx, const uint8_t *data,
                                   size_t dataLen) {
  CHECK_PARSER_ERR(tx_display_readTx(ctx, data, dataLen))
  extraDepthLevel = false;
  return parser_ok;
}

parser_error_t cosmos_parser_validate(const parser_context_t *ctx) {
  CHECK_PARSER_ERR(tx_validate(&parser_tx_obj.json))

  // Iterate through all items to check that all can be shown and are valid
  uint8_t numItems = 0;
  CHECK_PARSER_ERR(cosmos_parser_getNumItems(ctx, &numItems))

  char tmpKey[64];
  char tmpVal[64];

  for (uint8_t idx = 0; idx < numItems; idx++) {
    uint8_t pageCount = 0;
    CHECK_PARSER_ERR(cosmos_parser_getItem(ctx, idx, tmpKey, sizeof(tmpKey),
                                           tmpVal, sizeof(tmpVal), 0,
                                           &pageCount))
    if (memcmp(tmpKey, "Chain ID", 8) == 0) {
      memcpy(cosmos_chain_id, tmpVal, strlen(tmpVal) + 1);
    }
  }

  return parser_ok;
}

parser_error_t cosmos_parser_getNumItems(const parser_context_t *ctx
                                         __attribute__((unused)),
                                         uint8_t *num_items) {
  *num_items = 0;
  return tx_display_numItems(num_items);
}

__Z_INLINE bool parser_areEqual(uint16_t tokenIdx, char *expected) {
  if (parser_tx_obj.json.tokens[tokenIdx].type != JSMN_STRING) {
    return false;
  }

  int32_t len = parser_tx_obj.json.tokens[tokenIdx].end -
                parser_tx_obj.json.tokens[tokenIdx].start;
  if (len < 0) {
    return false;
  }

  if (strlen(expected) != (size_t)len) {
    return false;
  }

  const char *p = parser_tx_obj.tx + parser_tx_obj.json.tokens[tokenIdx].start;
  for (int32_t i = 0; i < len; i++) {
    if (expected[i] != *(p + i)) {
      return false;
    }
  }

  return true;
}

__Z_INLINE bool parser_isAmount(char *key) {
  if (strcmp(key, "fee/amount") == 0) {
    return true;
  }

  if (strcmp(key, "msgs/inputs/coins") == 0) {
    return true;
  }

  if (strcmp(key, "msgs/outputs/coins") == 0) {
    return true;
  }

  if (strcmp(key, "msgs/value/inputs/coins") == 0) {
    return true;
  }

  if (strcmp(key, "msgs/value/outputs/coins") == 0) {
    return true;
  }

  if (strcmp(key, "msgs/value/amount") == 0) {
    return true;
  }

  if (strcmp(key, "tip/amount") == 0) {
    return true;
  }

  return false;
}

__Z_INLINE bool is_default_denom_base(const char *denom, uint8_t denom_len) {
  if (tx_is_expert_mode()) {
    return false;
  }

  if (strlen(COIN_DEFAULT_DENOM_BASE) != denom_len) {
    return false;
  }

  if (memcmp(denom, COIN_DEFAULT_DENOM_BASE, denom_len) == 0) return true;

  return false;
}

__Z_INLINE parser_error_t parser_formatAmountItem(uint16_t amountToken,
                                                  char *outVal,
                                                  uint16_t outValLen,
                                                  uint8_t pageIdx,
                                                  uint8_t *pageCount) {
  *pageCount = 0;

  uint16_t numElements;
  CHECK_PARSER_ERR(
      array_get_element_count(&parser_tx_obj.json, amountToken, &numElements))

  if (numElements == 0) {
    *pageCount = 1;
    snprintf(outVal, outValLen, _("none"));
    return parser_ok;
  }

  if (numElements != 4) {
    return parser_unexpected_field;
  }

  if (parser_tx_obj.json.tokens[amountToken].type != JSMN_OBJECT) {
    return parser_unexpected_field;
  }

  if (!parser_areEqual(amountToken + 1u, "amount")) {
    return parser_unexpected_field;
  }

  if (!parser_areEqual(amountToken + 3u, "denom")) {
    return parser_unexpected_field;
  }

  char bufferUI[160];
  char tmpDenom[COIN_DENOM_MAXSIZE];
  char tmpAmount[COIN_AMOUNT_MAXSIZE];
  memset(tmpDenom, 0, sizeof tmpDenom);
  memset(tmpAmount, 0, sizeof(tmpAmount));
  memset(outVal, 0, outValLen);
  memset(bufferUI, 0, sizeof(bufferUI));

  const char *amountPtr =
      parser_tx_obj.tx + parser_tx_obj.json.tokens[amountToken + 2].start;
  if (parser_tx_obj.json.tokens[amountToken + 2].start < 0) {
    return parser_unexpected_buffer_end;
  }

  const int32_t amountLen = parser_tx_obj.json.tokens[amountToken + 2].end -
                            parser_tx_obj.json.tokens[amountToken + 2].start;
  const char *denomPtr =
      parser_tx_obj.tx + parser_tx_obj.json.tokens[amountToken + 4].start;
  const int32_t denomLen = parser_tx_obj.json.tokens[amountToken + 4].end -
                           parser_tx_obj.json.tokens[amountToken + 4].start;

  if (denomLen <= 0 || denomLen >= COIN_DENOM_MAXSIZE) {
    return parser_unexpected_error;
  }
  if (amountLen <= 0 || amountLen >= COIN_AMOUNT_MAXSIZE) {
    return parser_unexpected_error;
  }

  const size_t totalLen = amountLen + denomLen + 2;
  if (sizeof(bufferUI) < totalLen) {
    return parser_unexpected_buffer_end;
  }

  // Extract amount and denomination
  memcpy(tmpDenom, denomPtr, denomLen);
  memcpy(tmpAmount, amountPtr, amountLen);

  snprintf(bufferUI, sizeof(bufferUI), "%s ", tmpAmount);
  // If denomination has been recognized format and replace
  if (is_default_denom_base(denomPtr, denomLen)) {
    if (fpstr_to_str(bufferUI, sizeof(bufferUI), tmpAmount,
                     COIN_DEFAULT_DENOM_FACTOR) != 0) {
      return parser_unexpected_error;
    }
    number_inplace_trimming(bufferUI, 0);
    snprintf(tmpDenom, sizeof(tmpDenom), " %s", COIN_DEFAULT_DENOM_REPR);
  } else {
    const CosmosNetworkType *n = cosmosnetworkByChainId(cosmos_chain_id);
    if (n) {
      if (memcmp(tmpDenom, n->coin_minimal_denom,
                 strlen(n->coin_minimal_denom)) == 0) {
        if (fpstr_to_str(bufferUI, sizeof(bufferUI), tmpAmount, n->decimals) !=
            0) {
          return parser_unexpected_error;
        }
        number_inplace_trimming(bufferUI, 0);
        snprintf(tmpDenom, sizeof(tmpDenom), " %s", n->coin_denom);
      }
    }
  }

  const size_t len = strlen(bufferUI);
  if (bufferUI[len - 1] == '.') bufferUI[len - 1] = '\0';
  z_str3join(bufferUI, sizeof(bufferUI), "", tmpDenom);
  pageString(outVal, outValLen, bufferUI, pageIdx, pageCount);

  return parser_ok;
}

__Z_INLINE parser_error_t parser_formatAmount(uint16_t amountToken,
                                              char *outVal, uint16_t outValLen,
                                              uint8_t pageIdx,
                                              uint8_t *pageCount) {
  *pageCount = 0;
  if (parser_tx_obj.json.tokens[amountToken].type != JSMN_ARRAY) {
    return parser_formatAmountItem(amountToken, outVal, outValLen, pageIdx,
                                   pageCount);
  }

  uint8_t totalPages = 0;
  bool showItemSet = false;
  uint8_t showPageIdx = pageIdx;
  uint16_t showItemTokenIdx = 0;

  uint16_t numberAmounts;
  CHECK_PARSER_ERR(
      array_get_element_count(&parser_tx_obj.json, amountToken, &numberAmounts))

  // Count total subpagesCount and calculate correct page and TokenIdx
  for (uint16_t i = 0; i < numberAmounts; i++) {
    uint16_t itemTokenIdx;
    uint8_t subpagesCount;

    CHECK_PARSER_ERR(array_get_nth_element(&parser_tx_obj.json, amountToken, i,
                                           &itemTokenIdx));
    CHECK_PARSER_ERR(parser_formatAmountItem(itemTokenIdx, outVal, outValLen, 0,
                                             &subpagesCount));
    totalPages += subpagesCount;

    if (!showItemSet) {
      if (showPageIdx < subpagesCount) {
        showItemSet = true;
        showItemTokenIdx = itemTokenIdx;
      } else {
        showPageIdx -= subpagesCount;
      }
    }
  }
  *pageCount = totalPages;
  if (pageIdx > totalPages) {
    return parser_unexpected_value;
  }

  if (totalPages == 0) {
    *pageCount = 1;
    snprintf(outVal, outValLen, _("none"));
    return parser_ok;
  }

  uint8_t dummy;
  return parser_formatAmountItem(showItemTokenIdx, outVal, outValLen,
                                 showPageIdx, &dummy);
}

parser_error_t cosmos_parser_getItem(const parser_context_t *ctx,
                                     uint8_t displayIdx, char *outKey,
                                     uint16_t outKeyLen, char *outVal,
                                     uint16_t outValLen, uint8_t pageIdx,
                                     uint8_t *pageCount) {
  *pageCount = 0;

  char tmpKey[100];

  memset(outKey, 0, outKeyLen);
  memset(outVal, 0, outValLen);

  uint8_t numItems;
  CHECK_PARSER_ERR(cosmos_parser_getNumItems(ctx, &numItems))
  CHECK_APP_CANARY()

  if (numItems == 0) {
    return parser_unexpected_number_items;
  }

  if (displayIdx >= numItems) {
    return parser_display_idx_out_of_range;
  }

  uint16_t ret_value_token_index = 0;
  CHECK_PARSER_ERR(tx_display_query(displayIdx, tmpKey, sizeof(tmpKey),
                                    &ret_value_token_index))
  CHECK_APP_CANARY()
  snprintf(outKey, outKeyLen, "%s", tmpKey);

  if (parser_isAmount(tmpKey)) {
    CHECK_PARSER_ERR(parser_formatAmount(ret_value_token_index, outVal,
                                         outValLen, pageIdx, pageCount))
  } else {
    CHECK_PARSER_ERR(tx_getToken(ret_value_token_index, outVal, outValLen,
                                 pageIdx, pageCount))
  }
  CHECK_APP_CANARY()

  CHECK_PARSER_ERR(tx_display_make_friendly())
  CHECK_APP_CANARY()

  snprintf(outKey, outKeyLen, "%s", tmpKey);
  CHECK_APP_CANARY()

  return parser_ok;
}
