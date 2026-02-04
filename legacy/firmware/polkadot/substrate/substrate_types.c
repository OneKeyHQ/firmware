#include "../bignum.h"
#include "../common_defs.h"
#include "../parser_common.h"
#include "../parser_impl.h"
#include "substrate_coin.h"
#include "substrate_dispatch.h"

extern char __polkadot_ticker[8];
extern uint16_t __polkadot_decimal;
extern char polkadot_network[32];

#define IS_PRINTABLE(c) (c >= 0x20 && c <= 0x7e)

parser_error_t _readbool(parser_context_t* c, pd_bool_t* v) {
  return _preadUInt8(c, v);
}

parser_error_t _readu8(parser_context_t* c, pd_u8_t* v) {
  return _preadUInt8(c, v);
}

parser_error_t _readu16(parser_context_t* c, pd_u16_t* v) {
  return _preadUInt16(c, v);
}

parser_error_t _readu32(parser_context_t* c, pd_u32_t* v) {
  return _preadUInt32(c, v);
}

parser_error_t _readu64(parser_context_t* c, pd_u64_t* v) {
  return _preadUInt64(c, v);
}

parser_error_t _readBlockNumber(parser_context_t* c, pd_BlockNumber_t* v) {
  return _preadUInt32(c, v);
}

parser_error_t _readCompactu32(parser_context_t* c, pd_Compactu32_t* v) {
  return _readCompactInt(c, v);
}

parser_error_t _readCompactu64(parser_context_t* c, pd_Compactu64_t* v) {
  return _readCompactInt(c, v);
}

parser_error_t _readCallImpl(parser_context_t* c, pd_Call_t* v,
                             pd_MethodNested_t* m) {
  // If it's the first Call, store a pointer to it
  if (c->tx_obj->nestCallIdx._ptr == NULL) {
    c->tx_obj->nestCallIdx._ptr = c->buffer + c->offset;
    c->tx_obj->nestCallIdx._lenBuffer = c->bufferLen - c->offset;
  } else {
    // If _ptr is not null, and landed here, means we're inside a nested call.
    // We stored the pointer to the first Call and now we store
    // the pointer to the 'next' Call.
    if (c->tx_obj->nestCallIdx._nextPtr == NULL) {
      c->tx_obj->nestCallIdx._nextPtr = c->buffer + c->offset;
    }
  }

  // To keep track on how many nested Calls we have
  c->tx_obj->nestCallIdx.slotIdx++;
  if (c->tx_obj->nestCallIdx.slotIdx > MAX_CALL_NESTING_SIZE) {
    return parser_tx_nesting_limit_reached;
  }

  CHECK_ERROR(_readCallIndex(c, &v->callIndex));

  if (!_getMethod_IsNestingSupported(c->tx_obj->transactionVersion,
                                     v->callIndex.moduleIdx,
                                     v->callIndex.idx)) {
    return parser_tx_nesting_not_supported;
  }

  // Read and check the contained method on this Call
  CHECK_ERROR(
      _readMethod(c, v->callIndex.moduleIdx, v->callIndex.idx, (pd_Method_t*)m))

  // The instance of 'v' corresponding to the upper call on the stack (persisted
  // variable) will end up having the pointer to the first Call and to the
  // 'next' one if exists.
  v->_txVerPtr = &c->tx_obj->transactionVersion;
  v->nestCallIdx._lenBuffer = c->tx_obj->nestCallIdx._lenBuffer;
  v->nestCallIdx._ptr = c->tx_obj->nestCallIdx._ptr;
  v->nestCallIdx._nextPtr = c->tx_obj->nestCallIdx._nextPtr;

  return parser_ok;
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
parser_error_t _readCompactu128(parser_context_t* c, pd_Compactu128_t* v) {
  return _readCompactInt(c, v);
}

parser_error_t _readBalance(parser_context_t* c,
                            pd_Balance_t* v){GEN_DEF_READARRAY(16)}

parser_error_t _readBalanceOf(parser_context_t* c, pd_BalanceOf_t* v) {
  return _readBalance(c, &v->value);
}

#include "util.h"
parser_error_t p_readBytes(parser_context_t* c, pd_Bytes_t* v) {
  CHECK_INPUT()

  compactInt_t clen;
  CHECK_ERROR(_readCompactInt(c, &clen))
  CHECK_ERROR(_getValue(&clen, &v->_len))

  v->_ptr = c->buffer + c->offset;
  CTX_CHECK_AND_ADVANCE(c, v->_len);
  return parser_ok;
}

parser_error_t _readCall(parser_context_t* c, pd_Call_t* v) {
  pd_MethodNested_t _method;
  if (c->tx_obj->nestCallIdx.isTail) {
    c->tx_obj->nestCallIdx.isTail = false;
    v->nestCallIdx.isTail = true;
  } else {
    v->nestCallIdx.isTail = false;
  }

  CHECK_ERROR(_readCallImpl(c, v, &_method))
  if (c->tx_obj->nestCallIdx._ptr != NULL &&
      c->tx_obj->nestCallIdx._nextPtr != NULL) {
    v->nestCallIdx._ptr = c->tx_obj->nestCallIdx._ptr;
    v->nestCallIdx._nextPtr = c->tx_obj->nestCallIdx._nextPtr;
  }
  v->nestCallIdx.slotIdx = c->tx_obj->nestCallIdx.slotIdx;
  return parser_ok;
}

parser_error_t _readProposal(parser_context_t* c, pd_Proposal_t* v) {
  return _readCall(c, &v->call);
}

parser_error_t _readVecCall(parser_context_t* c, pd_VecCall_t* v) {
  compactInt_t clen;
  pd_Call_t dummy;
  CHECK_PARSER_ERR(_readCompactInt(c, &clen));
  CHECK_PARSER_ERR(_getValue(&clen, &v->_len));

  if (v->_len > MAX_CALL_VEC_SIZE) {
    return parser_tx_call_vec_too_large;
  }

  v->_ptr = c->buffer + c->offset;
  v->_lenBuffer = c->offset;
  if (v->_len == 0) {
    return parser_unexpected_buffer_end;
  }

  for (uint64_t i = 0; i < v->_len; i++) {
    c->tx_obj->nestCallIdx.slotIdx = 0;
    CHECK_ERROR(_readCall(c, &dummy))
  }
  v->_lenBuffer = c->offset - v->_lenBuffer;
  v->callTxVersion = c->tx_obj->transactionVersion;

  return parser_ok;
}

parser_error_t _readH256(parser_context_t* c,
                         pd_H256_t* v){GEN_DEF_READARRAY(32)}

parser_error_t
    _readHash(parser_context_t* c, pd_Hash_t* v){GEN_DEF_READARRAY(32)}

parser_error_t
    _readVecu32(parser_context_t* c, pd_Vecu32_t* v){GEN_DEF_READVECTOR(u32)}

parser_error_t
    _readVecu8(parser_context_t* c, pd_Vecu8_t* v){GEN_DEF_READVECTOR(u8)}

parser_error_t _readOptionu32(parser_context_t* c, pd_Optionu32_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readu32(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readData(parser_context_t* c, pd_Data_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  if (v->value <= 1) {
    return parser_ok;
  } else if (v->value <= 37) {
    const uint8_t length = v->value <= 32 ? v->value - 1 : 32;
    GEN_DEF_READARRAY(length)
  } else {
    return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readu8_array_20(parser_context_t* c,
                                pd_u8_array_20_t* v){GEN_DEF_READARRAY(20)}

parser_error_t _readTupleDataData(parser_context_t* c, pd_TupleDataData_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readData(c, &v->data1))
  CHECK_ERROR(_readData(c, &v->data2))
  return parser_ok;
}

parser_error_t _readOptionu8_array_20(parser_context_t* c,
                                      pd_Optionu8_array_20_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readu8_array_20(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readVecTupleDataData(parser_context_t* c,
                                     pd_VecTupleDataData_t* v){
    GEN_DEF_READVECTOR(TupleDataData)}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

parser_error_t _toStringbool(const pd_bool_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  switch (*v) {
    case 0:
      snprintf(outValue, outValueLen, "False");
      return parser_ok;
    case 1:
      snprintf(outValue, outValueLen, "True");
      return parser_ok;
  }

  return parser_not_supported;
}

parser_error_t _toStringu8(const pd_u8_t* v, char* outValue,
                           uint16_t outValueLen, uint8_t pageIdx,
                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  char bufferUI[50];

  uint64_to_str(bufferUI, sizeof(bufferUI), *v);
  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _toStringu16(const pd_u16_t* v, char* outValue,
                            uint16_t outValueLen, uint8_t pageIdx,
                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  char bufferUI[50];

  uint64_to_str(bufferUI, sizeof(bufferUI), *v);
  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _toStringu32(const pd_u32_t* v, char* outValue,
                            uint16_t outValueLen, uint8_t pageIdx,
                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  char bufferUI[100];

  uint64_to_str(bufferUI, sizeof(bufferUI), *v);
  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _toStringu64(const pd_u64_t* v, char* outValue,
                            uint16_t outValueLen, uint8_t pageIdx,
                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  char bufferUI[100];

  uint64_to_str(bufferUI, sizeof(bufferUI), *v);
  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _toStringBlockNumber(const pd_BlockNumber_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  return _toStringu32(v, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringCompactu32(const pd_Compactu32_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringCompactInt(v, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _toStringCompactu64(const pd_Compactu64_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringCompactInt(v, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

parser_error_t _toStringCompactu128(const pd_Compactu128_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  return _toStringCompactInt(v, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _substrate_toStringBalance(const pd_Balance_t* v, char* outValue,
                                          uint16_t outValueLen, uint8_t pageIdx,
                                          uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  char bufferUI[200];
  memset(outValue, 0, outValueLen);
  memset(bufferUI, 0, sizeof(bufferUI));
  *pageCount = 1;

  uint8_t bcdOut[100];
  const uint16_t bcdOutLen = sizeof(bcdOut);

  bignumLittleEndian_to_bcd(bcdOut, bcdOutLen, v->_ptr, 16);
  if (!bignumLittleEndian_bcdprint(bufferUI, sizeof(bufferUI), bcdOut,
                                   bcdOutLen)) {
    return parser_unexpected_buffer_end;
  }

  // Format number
  if (intstr_to_fpstr_inplace(bufferUI, sizeof(bufferUI), __polkadot_decimal) ==
      0) {
    return parser_unexpected_value;
  }

  number_inplace_trimming(bufferUI, 0);
  const size_t len = strlen(bufferUI);
  if (bufferUI[len - 1] == '.') bufferUI[len - 1] = '\0';
  if (z_str3join(bufferUI, sizeof(bufferUI), "", __polkadot_ticker) !=
      zxerr_ok) {
    return parser_print_not_supported;
  }

  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _substrate_toStringBalanceOf(const pd_BalanceOf_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  return _substrate_toStringBalance(&v->value, outValue, outValueLen, pageIdx,
                                    pageCount);
}

parser_error_t _toStringBytes(const pd_Bytes_t* v, char* outValue,
                              uint16_t outValueLen, uint8_t pageIdx,
                              uint8_t* pageCount) {
  GEN_DEF_TOSTRING_ARRAY(v->_len);
}

parser_error_t _toStringCall(const pd_Call_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t pageIdx,
                             uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;

  parser_context_t ctx;

  const uint8_t* buffer;
  if (v->nestCallIdx.isTail) {
    buffer = v->nestCallIdx._ptr;
  } else {
    buffer = v->nestCallIdx._nextPtr;
  }

  polkadot_parser_init(&ctx, buffer, v->nestCallIdx._lenBuffer);
  parser_tx_t _txObj;

  pd_Call_t _call;
  _call.nestCallIdx.isTail = false;

  ctx.tx_obj = &_txObj;
  _txObj.transactionVersion = *v->_txVerPtr;

  ctx.tx_obj->nestCallIdx._ptr = NULL;
  ctx.tx_obj->nestCallIdx._nextPtr = NULL;
  ctx.tx_obj->nestCallIdx._lenBuffer = 0;
  ctx.tx_obj->nestCallIdx.slotIdx = 0;
  ctx.tx_obj->nestCallIdx.isTail = false;

  // Read the Call, so we get the contained Method
  parser_error_t err =
      _readCallImpl(&ctx, &_call, (pd_MethodNested_t*)&_txObj.method);
  if (err != parser_ok) {
    return err;
  }

  // Get num items of this current Call
  uint8_t callNumItems = _getMethod_NumItems(
      *v->_txVerPtr, v->callIndex.moduleIdx, v->callIndex.idx);

  // Count how many pages this call has (including nested ones if they exists)
  for (uint8_t i = 0; i < callNumItems; i++) {
    uint8_t itemPages = 0;
    _getMethod_ItemValue(*v->_txVerPtr, &_txObj.method,
                         _call.callIndex.moduleIdx, _call.callIndex.idx, i,
                         outValue, outValueLen, 0, &itemPages);
    (*pageCount) += itemPages;
  }

  if (pageIdx == 0) {
    snprintf(outValue, outValueLen, "%s",
             _getMethod_Name(*v->_txVerPtr, v->callIndex.moduleIdx,
                             v->callIndex.idx));
    return parser_ok;
  }

  pageIdx--;

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  for (uint8_t i = 0; i < callNumItems; i++) {
    uint8_t itemPages = 0;
    _getMethod_ItemValue(*v->_txVerPtr, &_txObj.method, v->callIndex.moduleIdx,
                         v->callIndex.idx, i, outValue, outValueLen, 0,
                         &itemPages);

    if (pageIdx < itemPages) {
      uint8_t tmp;
      _getMethod_ItemValue(*v->_txVerPtr, &_txObj.method,
                           v->callIndex.moduleIdx, v->callIndex.idx, i,
                           outValue, outValueLen, pageIdx, &tmp);
      return parser_ok;
    }

    pageIdx -= itemPages;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringProposal(const pd_Proposal_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t pageIdx,
                                 uint8_t* pageCount) {
  return _toStringCall(&v->call, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringVecCall(const pd_VecCall_t* v, char* outValue,
                                uint16_t outValueLen, uint8_t pageIdx,
                                uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  /* count number of pages, then output specific */
  *pageCount = 0;
  uint8_t chunkPageCount;
  uint16_t currentPage, currentTotalPage = 0;
  /* We need to do it twice because there is no memory to keep intermediate
   * results*/
  /* First count*/
  parser_context_t ctx;
  polkadot_parser_init(&ctx, v->_ptr, v->_lenBuffer);
  parser_tx_t _txObj;
  pd_Call_t _call;
  ctx.tx_obj = &_txObj;
  _txObj.transactionVersion = v->callTxVersion;
  _call._txVerPtr = &v->callTxVersion;
  _call.nestCallIdx.isTail = true;

  ctx.tx_obj->nestCallIdx.slotIdx = 0;
  ctx.tx_obj->nestCallIdx._lenBuffer = 0;
  ctx.tx_obj->nestCallIdx._ptr = NULL;
  ctx.tx_obj->nestCallIdx._nextPtr = NULL;
  ctx.tx_obj->nestCallIdx.isTail = true;

  for (uint16_t i = 0; i < v->_len; i++) {
    ctx.tx_obj->nestCallIdx._ptr = NULL;
    ctx.tx_obj->nestCallIdx._nextPtr = NULL;
    ctx.tx_obj->nestCallIdx.slotIdx = 0;
    CHECK_ERROR(
        _readCallImpl(&ctx, &_call, (pd_MethodNested_t*)&_txObj.method));
    CHECK_ERROR(
        _toStringCall(&_call, outValue, outValueLen, 0, &chunkPageCount));
    (*pageCount) += chunkPageCount;
  }

  /* Then iterate until we can print the corresponding chunk*/
  polkadot_parser_init(&ctx, v->_ptr, v->_lenBuffer);
  for (uint16_t i = 0; i < v->_len; i++) {
    ctx.tx_obj->nestCallIdx._ptr = NULL;
    ctx.tx_obj->nestCallIdx._nextPtr = NULL;
    ctx.tx_obj->nestCallIdx.slotIdx = 0;
    CHECK_ERROR(
        _readCallImpl(&ctx, &_call, (pd_MethodNested_t*)&_txObj.method));
    chunkPageCount = 1;
    currentPage = 0;
    while (currentPage < chunkPageCount) {
      CHECK_ERROR(_toStringCall(&_call, outValue, outValueLen, currentPage,
                                &chunkPageCount));
      if (currentTotalPage == pageIdx) {
        return parser_ok;
      }
      currentPage++;
      currentTotalPage++;
    }
  }

  return parser_print_not_supported;
}

parser_error_t _toStringH256(const pd_H256_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t pageIdx,
                             uint8_t* pageCount) {
  GEN_DEF_TOSTRING_ARRAY(32);
}

parser_error_t _toStringHash(const pd_Hash_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t pageIdx,
                             uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(32)}

parser_error_t
    _toStringVecu32(const pd_Vecu32_t* v, char* outValue, uint16_t outValueLen,
                    uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(u32);
}

parser_error_t _toStringVecu8(const pd_Vecu8_t* v, char* outValue,
                              uint16_t outValueLen, uint8_t pageIdx,
                              uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(u8);
}

parser_error_t _toStringOptionu32(const pd_Optionu32_t* v, char* outValue,
                                  uint16_t outValueLen, uint8_t pageIdx,
                                  uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(
        _toStringu32(&v->contained, outValue, outValueLen, pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringu8_array_20(const pd_u8_array_20_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(20)}

parser_error_t
    _toStringData(const pd_Data_t* v, char* outValue, uint16_t outValueLen,
                  uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  if (v->value == 0) {
    snprintf(outValue, outValueLen, "None");
    return parser_ok;
  } else if (v->value == 1) {
    snprintf(outValue, outValueLen, "Empty raw");
    return parser_ok;
  } else if (v->value > 37) {
    return parser_unexpected_value;
  }
  const uint8_t length = v->value <= 32 ? v->value - 1 : 32;
  bool allPrintable = true;
  if (v->value <= 33) {
    for (uint8_t i = 0; i < length; i++) {
      allPrintable &= IS_PRINTABLE(v->_ptr[i]);
    }
  }
  if (v->value <= 33 && allPrintable) {
    char bufferUI[40] = {0};
    snprintf(bufferUI, length + 1, "%s", v->_ptr);  // it counts null terminator
    pageString(outValue, outValueLen, (const char*)bufferUI, pageIdx,
               pageCount);
  } else {
    GEN_DEF_TOSTRING_ARRAY(length)
  }
  return parser_ok;
}

parser_error_t _toStringOptionu8_array_20(const pd_Optionu8_array_20_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringu8_array_20(&v->contained, outValue, outValueLen,
                                     pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringVecTupleDataData(const pd_VecTupleDataData_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(TupleDataData);
}

parser_error_t _toStringTupleDataData(const pd_TupleDataData_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringData(&v->data1, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(_toStringData(&v->data2, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringData(&v->data1, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(
        _toStringData(&v->data2, outValue, outValueLen, pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
parser_error_t _readAccountId(parser_context_t* c,
                              pd_AccountId_t* v){GEN_DEF_READARRAY(32)}

parser_error_t
    _readCompactAccountIndex(parser_context_t* c, pd_CompactAccountIndex_t* v) {
  return _readCompactInt(c, &v->value);
}
parser_error_t _readTupleAccountIdData(parser_context_t* c,
                                       pd_TupleAccountIdData_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readAccountId(c, &v->id));
  CHECK_ERROR(_readData(c, &v->data));
  return parser_ok;
}

parser_error_t _readAccountIdLookupOfT(parser_context_t* c,
                                       pd_AccountIdLookupOfT_t* v) {
  CHECK_INPUT()
  if (!(strcmp(polkadot_network, "joystream") == 0 ||
        strcmp(polkadot_network, "hydration") == 0)) {
    CHECK_ERROR(_preadUInt8(c, &v->value))
  }
  switch (v->value) {
    case 0:  // Id
      CHECK_ERROR(_readAccountId(c, &v->id))
      break;
    case 1:  // Index
      CHECK_ERROR(_readCompactAccountIndex(c, &v->index))
      break;
    case 2:  // Raw
      CHECK_ERROR(p_readBytes(c, &v->raw))
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

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

parser_error_t _toStringAccountId(const pd_AccountId_t* v, char* outValue,
                                  uint16_t outValueLen, uint8_t pageIdx,
                                  uint8_t* pageCount) {
  return _toStringPubkeyAsAddress(v->_ptr, outValue, outValueLen, pageIdx,
                                  pageCount);
}

parser_error_t _toStringCompactAccountIndex(const pd_CompactAccountIndex_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  return _toStringCompactInt(&v->value, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _toStringNetworkIdV2(const pd_NetworkIdV2_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
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

parser_error_t _toStringu8_array_32(const pd_u8_array_32_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(32)} parser_error_t
    _toStringByFork(const pd_ByFork_t* v, char* outValue, uint16_t outValueLen,
                    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(
      _toStringu64(&v->blockNumber, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(
      _toStringu8_array_32(&v->blockHash, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx >= *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringu64(&v->blockNumber, outValue, outValueLen, pageIdx,
                             &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringu8_array_32(&v->blockHash, outValue, outValueLen,
                                     pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}
parser_error_t _toStringNetworkIdV3(const pd_NetworkIdV3_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // ByGenesis
      CHECK_ERROR(_toStringu8_array_32(&v->byGenesis, outValue, outValueLen,
                                       pageIdx, pageCount))
      break;
    case 1:  // ByFork
      CHECK_ERROR(_toStringByFork(&v->byFork, outValue, outValueLen, pageIdx,
                                  pageCount))
      break;
    case 7:  // Ethereum
      CHECK_ERROR(_toStringCompactu64(&v->chainId, outValue, outValueLen,
                                      pageIdx, pageCount))
      break;

    case 2:  // Polkadot
      snprintf(outValue, outValueLen, "Polkadot");
      break;
    case 3:  // Kusama
      snprintf(outValue, outValueLen, "Kusama");
      break;
    case 4:  // Westend
      snprintf(outValue, outValueLen, "Westend");
      break;
    case 5:  // Rococo
      snprintf(outValue, outValueLen, "Rococo");
      break;
    case 6:  // Wococo
      snprintf(outValue, outValueLen, "Wococo");
      break;
    case 8:  // BitcoinCore
      snprintf(outValue, outValueLen, "BitcoinCore");
      break;
    case 9:  // BitcoinCash
      snprintf(outValue, outValueLen, "BitcoinCash");
      break;
    default:
      return parser_not_supported;
  }
  return parser_ok;
}
parser_error_t _toStringAccountIdLookupOfT(const pd_AccountIdLookupOfT_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Id
      CHECK_ERROR(
          _toStringAccountId(&v->id, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 1:  // Index
      CHECK_ERROR(_toStringCompactAccountIndex(&v->index, outValue, outValueLen,
                                               pageIdx, pageCount))
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
