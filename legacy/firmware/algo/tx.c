#include "tx.h"
#include <string.h>
#include "defs.h"
#include "parser.h"

static parser_tx_t parser_tx_obj;
static parser_context_t ctx_parsed_tx;

const char *tx_parse(const uint8_t *data, size_t dataLen) {
  memzero(&parser_tx_obj, sizeof(parser_tx_obj));

  uint8_t err = parser_parse(&ctx_parsed_tx, data, dataLen, &parser_tx_obj);

  CHECK_APP_CANARY()

  if (err != parser_ok) {
    return parser_getErrorDescription(err);
  }

  err = parser_validate(&ctx_parsed_tx);
  CHECK_APP_CANARY()

  if (err != parser_ok) {
    return parser_getErrorDescription(err);
  }

  return NULL;
}

zxerr_t tx_getNumItems(uint8_t *num_items) {
  parser_error_t err = parser_getNumItems(num_items);
  if (err != parser_ok) {
    return zxerr_unknown;
  }
  return zxerr_ok;
}

zxerr_t tx_getItem(int8_t displayIdx, char *outKey, uint16_t outKeyLen,
                   char *outVal, uint16_t outValLen, uint8_t pageIdx,
                   uint8_t *pageCount) {
  uint8_t numItems = 0;

  CHECK_ZXERR(tx_getNumItems(&numItems))

  if (displayIdx > numItems) {
    return zxerr_no_data;
  }

  parser_error_t err =
      parser_getItem(&ctx_parsed_tx, displayIdx, outKey, outKeyLen, outVal,
                     outValLen, pageIdx, pageCount);

  // Convert error codes
  if (err == parser_no_data || err == parser_display_idx_out_of_range ||
      err == parser_display_page_out_of_range)
    return zxerr_no_data;

  if (err != parser_ok) return zxerr_unknown;

  return zxerr_ok;
}
