#ifndef __POLKADOT_TX_H__
#define __POLKADOT_TX_H__

#include "coin.h"
#include "common_defs.h"
#include "parser_common.h"

parser_error_t polkadot_tx_parse(const uint8_t *data, size_t dataLen,
                                 bool has_preset_address_type,
                                 uint16_t preset_address_type);

/// Return the number of items in the transaction
zxerr_t polkadot_tx_getNumItems(uint8_t *num_items);

/// Gets an specific item from the transaction (including paging)
zxerr_t polkadot_tx_getItem(int8_t displayIdx, char *outKey, uint16_t outKeyLen,
                            char *outValue, uint16_t outValueLen,
                            uint8_t pageIdx, uint8_t *pageCount);

#endif
