#include "tx.h"
#include <string.h>
#include "parser.h"
#include "parser_txdef.h"

static parser_tx_t tx_obj = {0};
static parser_context_t ctx_parsed_tx = {0};

parser_error_t polkadot_tx_parse(const uint8_t *data, size_t dataLen,
                                 bool has_preset_address_type,
                                 uint16_t preset_address_type) {
  ctx_parsed_tx.has_preset_address_type = has_preset_address_type;
  ctx_parsed_tx.preset_address_type = preset_address_type;
  uint8_t err = polkadot_parser_parse(&ctx_parsed_tx, data, dataLen, &tx_obj);
  if (err != parser_ok) {
    return err;
  }
  err = polkadot_parser_validate(&ctx_parsed_tx);

  if (err != parser_ok) {
    return err;
  }

  return parser_ok;
}

zxerr_t polkadot_tx_getNumItems(uint8_t *num_items) {
  parser_error_t err = polkadot_parser_getNumItems(&ctx_parsed_tx, num_items);

  if (err != parser_ok) {
    return zxerr_no_data;
  }

  return zxerr_ok;
}

zxerr_t polkadot_tx_getItem(int8_t displayIdx, char *outKey, uint16_t outKeyLen,
                            char *outVal, uint16_t outValLen, uint8_t pageIdx,
                            uint8_t *pageCount) {
  uint8_t numItems = 0;

  CHECK_ZXERR(polkadot_tx_getNumItems(&numItems))

  if (displayIdx < 0 || displayIdx > numItems) {
    return zxerr_no_data;
  }

  parser_error_t err =
      polkadot_parser_getItem(&ctx_parsed_tx, displayIdx, outKey, outKeyLen,
                              outVal, outValLen, pageIdx, pageCount);

  // Convert error codes
  if (err == parser_no_data || err == parser_display_idx_out_of_range ||
      err == parser_display_page_out_of_range)
    return zxerr_no_data;

  if (err != parser_ok) return zxerr_unknown;

  return zxerr_ok;
}
