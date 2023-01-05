#ifndef __POLKADOT_TX_H__
#define __POLKADOT_TX_H__

#include "coin.h"
#include "common_defs.h"

/// Parse message stored in transaction buffer
/// This function should be called as soon as full buffer data is loaded.
/// \return It returns NULL if data is valid or error message otherwise.
const char *polkadot_tx_parse(const uint8_t *data, size_t dataLen);

/// Return the number of items in the transaction
zxerr_t polkadot_tx_getNumItems(uint8_t *num_items);

/// Gets an specific item from the transaction (including paging)
zxerr_t polkadot_tx_getItem(int8_t displayIdx, char *outKey, uint16_t outKeyLen,
                            char *outValue, uint16_t outValueLen,
                            uint8_t pageIdx, uint8_t *pageCount);

#endif