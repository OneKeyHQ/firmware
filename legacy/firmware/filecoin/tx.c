#include "tx.h"
#include <string.h>
#include "parser.h"

parser_context_t fil_ctx_parsed_tx;

const char *fil_tx_parse(const uint8_t *data, size_t dataLen) {
  uint8_t err = fil_parser_parse(&fil_ctx_parsed_tx, data, dataLen);

  if (err != parser_ok) {
    return fil_parser_getErrorDescription(err);
  }

  err = fil_parser_validate(&fil_ctx_parsed_tx);
  CHECK_APP_CANARY()

  if (err != parser_ok) {
    return fil_parser_getErrorDescription(err);
  }

  return NULL;
}

zxerr_t fil_tx_getNumItems(uint8_t *num_items) {
  parser_error_t err = fil_parser_getNumItems(&fil_ctx_parsed_tx, num_items);

  if (err != parser_ok) {
    return zxerr_no_data;
  }

  return zxerr_ok;
}

zxerr_t fil_tx_getItem(int8_t displayIdx, char *outKey, uint16_t outKeyLen,
                       char *outVal, uint16_t outValLen, uint8_t pageIdx,
                       uint8_t *pageCount) {
  uint8_t numItems = 0;

  CHECK_ZXERR(fil_tx_getNumItems(&numItems))

  if (displayIdx < 0 || displayIdx > numItems) {
    return zxerr_no_data;
  }

  parser_error_t err =
      fil_parser_getItem(&fil_ctx_parsed_tx, displayIdx, outKey, outKeyLen,
                         outVal, outValLen, pageIdx, pageCount);

  // Convert error codes
  if (err == parser_no_data || err == parser_display_idx_out_of_range ||
      err == parser_display_page_out_of_range)
    return zxerr_no_data;

  if (err != parser_ok) return zxerr_unknown;

  return zxerr_ok;
}
