#ifndef __COSMOS_TX_DISPLAY_H__
#define __COSMOS_TX_DISPLAY_H__

#include <stdint.h>
#include "tx_parser.h"

typedef enum {
  root_item_chain_id = 0,
  root_item_account_number,
  root_item_sequence,
  root_item_msgs,
  root_item_memo,
  root_item_fee,
  root_item_tip,
} root_item_e;

bool tx_is_expert_mode(void);

const char *get_required_root_item(root_item_e i);

parser_error_t tx_display_query(uint16_t displayIdx, char *outKey,
                                uint16_t outKeyLen,
                                uint16_t *ret_value_token_index);

parser_error_t tx_display_readTx(parser_context_t *c, const uint8_t *data,
                                 size_t dataLen);

parser_error_t tx_display_numItems(uint8_t *num_items);

parser_error_t tx_display_make_friendly(void);

#endif  // __COSMOS_TX_DISPLAY_H__
