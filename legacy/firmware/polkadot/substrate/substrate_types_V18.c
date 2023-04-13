#include <stddef.h>
#include <stdint.h>
#include "substrate_coin.h"
#include "substrate_dispatch_V18.h"

#include "../bignum.h"
#include "../common_defs.h"
#include "../parser_impl.h"
#include "util.h"

parser_error_t _readAccountIdLookupOfT_V18(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Id
      CHECK_ERROR(_readAccountId_V18(c, &v->id))
      break;
    case 1:  // Index
      CHECK_ERROR(_readCompactAccountIndex_V18(c, &v->index))
      break;
    case 2:  // Raw
      CHECK_ERROR(_substrate_readBytes(c, &v->raw))
      break;
    case 3:  // Address32
      GEN_DEF_READARRAY(32)
      break;
    case 4:  // Address20
      GEN_DEF_READARRAY(20)
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readAccountId_V18(parser_context_t* c,
                                  pd_AccountId_V18_t* v){GEN_DEF_READARRAY(32)}

parser_error_t _readCompactAccountIndex_V18(parser_context_t* c,
                                            pd_CompactAccountIndex_V18_t* v) {
  return _readCompactInt(c, &v->value);
}

parser_error_t _readTupleAccountIdData_V18(parser_context_t* c,
                                           pd_TupleAccountIdData_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readAccountId_V18(c, &v->id));
  CHECK_ERROR(_readData(c, &v->data));
  return parser_ok;
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

parser_error_t _toStringAccountIdLookupOfT_V18(
    const pd_AccountIdLookupOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Id
      CHECK_ERROR(_toStringAccountId_V18(&v->id, outValue, outValueLen, pageIdx,
                                         pageCount))
      break;
    case 1:  // Index
      CHECK_ERROR(_toStringCompactAccountIndex_V18(
          &v->index, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 2:  // Raw
      CHECK_ERROR(
          _toStringBytes(&v->raw, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 3:  // Address32
    {
      GEN_DEF_TOSTRING_ARRAY(32)
    }
    case 4:  // Address20
    {
      GEN_DEF_TOSTRING_ARRAY(20)
    }
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringAccountId_V18(const pd_AccountId_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringPubkeyAsAddress(v->_ptr, outValue, outValueLen, pageIdx,
                                  pageCount);
}

parser_error_t _toStringCompactAccountIndex_V18(
    const pd_CompactAccountIndex_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringCompactInt(&v->value, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _toStringNetworkId_V18(const pd_NetworkId_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // Any
      snprintf(outValue, outValueLen, "Any");
      break;
    case 1:  // Named
      CHECK_ERROR(
          _toStringBytes(&v->named, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 2:  // Polkadot
      snprintf(outValue, outValueLen, "Polkadot");
      break;
    case 3:  // Kusama
      snprintf(outValue, outValueLen, "Kusama");
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}
