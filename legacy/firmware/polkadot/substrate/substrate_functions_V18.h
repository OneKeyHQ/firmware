#ifndef __POLKADOT_SUBSTRATE_FUNCTIONS_V18_H__
#define __POLKADOT_SUBSTRATE_FUNCTIONS_V18_H__

#include <stddef.h>
#include <stdint.h>
#include "../parser_common.h"
#include "substrate_methods_V18.h"
#include "substrate_types_V18.h"

parser_error_t _readAccountId_V18(parser_context_t* c, pd_AccountId_V18_t* v);
parser_error_t _readAccountIdLookupOfT_V18(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V18_t* v);
parser_error_t _readCompactAccountIndex_V18(parser_context_t* c,
                                            pd_CompactAccountIndex_V18_t* v);

//////////
parser_error_t _toStringAccountIdLookupOfT_V18(
    const pd_AccountIdLookupOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);
parser_error_t _toStringAccountId_V18(const pd_AccountId_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);
parser_error_t _toStringCompactAccountIndex_V18(
    const pd_CompactAccountIndex_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);
parser_error_t _toStringNetworkId_V18(const pd_NetworkId_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

#endif
