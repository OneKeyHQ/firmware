#include "substrate_dispatch_V18.h"
#include <stdint.h>
#include "../common_defs.h"
#include "substrate_strings.h"

__Z_INLINE parser_error_t _readMethod_balances_transfer_V18(
    parser_context_t* c, pd_balances_transfer_V18_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V18(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_force_transfer_V18(
    parser_context_t* c, pd_balances_force_transfer_V18_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V18(c, &m->source))
  CHECK_ERROR(_readAccountIdLookupOfT_V18(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_keep_alive_V18(
    parser_context_t* c, pd_balances_transfer_keep_alive_V18_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V18(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_all_V18(
    parser_context_t* c, pd_balances_transfer_all_V18_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V18(c, &m->dest))
  CHECK_ERROR(_readbool(c, &m->keep_alive))
  return parser_ok;
}

parser_error_t _readAccountIdLookupOfT_joy(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readAccountId_V18(c, &v->id))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_joy(
    parser_context_t* c, pd_balances_transfer_V18_t* m) {
  m->dest.value = 0;
  CHECK_ERROR(_readAccountIdLookupOfT_joy(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_force_transfer_joy(
    parser_context_t* c, pd_balances_force_transfer_V18_t* m) {
  m->source.value = 0;
  CHECK_ERROR(_readAccountIdLookupOfT_joy(c, &m->source))
  m->dest.value = 0;
  CHECK_ERROR(_readAccountIdLookupOfT_joy(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_keep_alive_joystream(
    parser_context_t* c, pd_balances_transfer_keep_alive_V18_t* m) {
  m->dest.value = 0;
  CHECK_ERROR(_readAccountIdLookupOfT_joy(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_all_joy(
    parser_context_t* c, pd_balances_transfer_all_V18_t* m) {
  m->dest.value = 0;
  CHECK_ERROR(_readAccountIdLookupOfT_joy(c, &m->dest))
  CHECK_ERROR(_readbool(c, &m->keep_alive))
  return parser_ok;
}

parser_error_t _readMethod_V18(parser_context_t* c, uint8_t moduleIdx,
                               uint8_t callIdx, pd_Method_V18_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1287: /* module 5 call 0 */
      CHECK_ERROR(_readMethod_balances_transfer_V18(
          c, &method->nested.balances_transfer_V18))
      break;
    case 1282: /* module 5 call 2 */
      CHECK_ERROR(_readMethod_balances_force_transfer_V18(
          c, &method->nested.balances_force_transfer_V18))
      break;
    case 1283: /* module 5 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_V18(
          c, &method->nested.balances_transfer_keep_alive_V18))
      break;
    case 1284: /* module 5 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_V18(
          c, &method->basic.balances_transfer_all_V18))
      break;
    default:
      return parser_unexpected_callIndex;
  }

  return parser_ok;
}

parser_error_t _readMethod_V18_kusama(parser_context_t* c, uint8_t moduleIdx,
                                      uint8_t callIdx,
                                      pd_Method_V18_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    case 1031: /* module 4 call 0 */
      CHECK_ERROR(_readMethod_balances_transfer_V18(
          c, &method->nested.balances_transfer_V18))
      break;
    case 1026: /* module 4 call 2 */
      CHECK_ERROR(_readMethod_balances_force_transfer_V18(
          c, &method->nested.balances_force_transfer_V18))
      break;
    case 1027: /* module 4 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_V18(
          c, &method->nested.balances_transfer_keep_alive_V18))
      break;
    case 1028: /* module 4 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_V18(
          c, &method->basic.balances_transfer_all_V18))
      break;
    default:
      return parser_unexpected_callIndex;
  }

  return parser_ok;
}

parser_error_t _readMethod_V18_westend(parser_context_t* c, uint8_t moduleIdx,
                                       uint8_t callIdx,
                                       pd_Method_V18_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    case 1024: /* module 4 call 0 */
      CHECK_ERROR(_readMethod_balances_transfer_V18(
          c, &method->nested.balances_transfer_V18))
      break;
    case 1026: /* module 4 call 2 */
      CHECK_ERROR(_readMethod_balances_force_transfer_V18(
          c, &method->nested.balances_force_transfer_V18))
      break;
    case 1027: /* module 4 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_V18(
          c, &method->nested.balances_transfer_keep_alive_V18))
      break;
    case 1028: /* module 4 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_V18(
          c, &method->basic.balances_transfer_all_V18))
      break;
    default:
      return parser_unexpected_callIndex;
  }

  return parser_ok;
}

parser_error_t _readMethod_astar(parser_context_t* c, uint8_t moduleIdx,
                                 uint8_t callIdx, pd_Method_V18_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    case 7943: /* module 31 call 0 */
      CHECK_ERROR(_readMethod_balances_transfer_V18(
          c, &method->nested.balances_transfer_V18))
      break;
    case 7938: /* module 31 call 2 */
      CHECK_ERROR(_readMethod_balances_force_transfer_V18(
          c, &method->nested.balances_force_transfer_V18))
      break;
    case 7939: /* module 31 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_V18(
          c, &method->nested.balances_transfer_keep_alive_V18))
      break;
    case 7940: /* module 31 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_V18(
          c, &method->basic.balances_transfer_all_V18))
      break;
    default:
      return parser_unexpected_callIndex;
  }

  return parser_ok;
}

parser_error_t _readMethod_joystream(parser_context_t* c, uint8_t moduleIdx,
                                     uint8_t callIdx, pd_Method_V18_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
      CHECK_ERROR(_readMethod_balances_transfer_joy(
          c, &method->nested.balances_transfer_V18))
      break;
    case 1283: /* module 5 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_joystream(
          c, &method->nested.balances_transfer_keep_alive_V18))
      break;
    case 1284: /* module 5 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_joy(
          c, &method->basic.balances_transfer_all_V18))
      break;
    default:
      return parser_unexpected_callIndex;
  }

  return parser_ok;
}

parser_error_t _readMethod_manta(parser_context_t* c, uint8_t moduleIdx,
                                 uint8_t callIdx, pd_Method_V18_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    case 2567:
      CHECK_ERROR(_readMethod_balances_transfer_V18(
          c, &method->nested.balances_transfer_V18))
      break;
    case 2562:
      CHECK_ERROR(_readMethod_balances_force_transfer_V18(
          c, &method->nested.balances_force_transfer_V18))
      break;
    case 2563: /* module 5 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_V18(
          c, &method->nested.balances_transfer_keep_alive_V18))
      break;
    case 2564: /* module 5 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_V18(
          c, &method->basic.balances_transfer_all_V18))
      break;
    default:
      return parser_unexpected_callIndex;
  }

  return parser_ok;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

const char* _getMethod_ModuleName_V18(uint8_t moduleIdx) {
  switch (moduleIdx) {
    case 4:
    case 5:
    case 31:
      return STR_MO_BALANCES;
    default:
      return NULL;
  }

  return NULL;
}

const char* _getMethod_Name_V18(uint8_t moduleIdx, uint8_t callIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
    case 1287:
    case 1024:
    case 1031:
    case 7936:
    case 7943:
    case 2567:
      return STR_ME_TRANSFER;
    case 1282: /* module 5 call 2 */
    case 1026:
    case 7938:
    case 2562:
      return STR_ME_FORCE_TRANSFER;
    case 1283: /* module 5 call 3 */
    case 1027:
    case 7939:
    case 2563:
      return STR_ME_TRANSFER_KEEP_ALIVE;
    case 1284: /* module 5 call 4 */
    case 1028:
    case 7940:
    case 2564:
      return STR_ME_TRANSFER_ALL;
    default:
      return _getMethod_Name_V18_ParserFull(callPrivIdx);
  }

  return NULL;
}

const char* _getMethod_Name_V18_ParserFull(uint16_t callPrivIdx) {
  switch (callPrivIdx) {
    default:
      return NULL;
  }

  return NULL;
}

uint8_t _getMethod_NumItems_V18(uint8_t moduleIdx, uint8_t callIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
    case 1287:
    case 1024:
    case 1031:
    case 7936:
    case 7943:
    case 2567:
      return 2;
    case 1282: /* module 5 call 2 */
    case 1026:
    case 7938:
    case 2562:
      return 3;
    case 1283: /* module 5 call 3 */
    case 1027:
    case 7939:
    case 2563:
      return 2;
    case 1284: /* module 5 call 4 */
    case 1028:
    case 7940:
    case 2564:
      return 2;
    default:
      return 0;
  }

  return 0;
}

const char* _getMethod_ItemName_V18(uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
    case 1287:
    case 1024:
    case 1031:
    case 7936:
    case 7943:
    case 2567:
      switch (itemIdx) {
        case 0:
          return STR_IT_amount;
        case 1:
          return STR_IT_dest;
        default:
          return NULL;
      }
    case 1282: /* module 5 call 2 */
    case 1026:
    case 7938:
    case 2562:
      switch (itemIdx) {
        case 0:
          return STR_IT_amount;
        case 1:
          return STR_IT_dest;
        case 2:
          return STR_IT_source;
        default:
          return NULL;
      }
    case 1283: /* module 5 call 3 */
    case 1027:
    case 7939:
    case 2563:
      switch (itemIdx) {
        case 0:
          return STR_IT_amount;
        case 1:
          return STR_IT_dest;
        default:
          return NULL;
      }
    case 1284: /* module 5 call 4 */
    case 1028:
    case 7940:
    case 2564:
      switch (itemIdx) {
        case 0:
          return STR_IT_dest;
        case 1:
          return STR_IT_keep_alive;
        default:
          return NULL;
      }
    default:
      return NULL;
  }

  return NULL;
}

parser_error_t _getMethod_ItemValue_V18(pd_Method_V18_t* m, uint8_t moduleIdx,
                                        uint8_t callIdx, uint8_t itemIdx,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
    case 1287: /* module 5 call 0 */
    case 1024: /* module 5 call 0 */
    case 1031: /* module 5 call 0 */
    case 7936: /* module 5 call 0 */
    case 7943:
    case 2567:
      switch (itemIdx) {
        case 0: /* balances_transfer_V18 - amount */;
          return _toStringCompactBalance(
              &m->nested.balances_transfer_V18.amount, outValue, outValueLen,
              pageIdx, pageCount);
        case 1: /* balances_transfer_V18 - dest */;
          return _toStringAccountIdLookupOfT_V18(
              &m->nested.balances_transfer_V18.dest, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1282: /* module 5 call 2 */
    case 1026: /* module 5 call 2 */
    case 7938: /* module 5 call 2 */
    case 2562:
      switch (itemIdx) {
        case 0: /* balances_force_transfer_V18 - amount */;
          return _toStringCompactBalance(
              &m->nested.balances_force_transfer_V18.amount, outValue,
              outValueLen, pageIdx, pageCount);
        case 1: /* balances_force_transfer_V18 - dest */;
          return _toStringAccountIdLookupOfT_V18(
              &m->nested.balances_force_transfer_V18.dest, outValue,
              outValueLen, pageIdx, pageCount);
        case 2: /* balances_force_transfer_V18 - source */;
          return _toStringAccountIdLookupOfT_V18(
              &m->nested.balances_force_transfer_V18.source, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1283: /* module 5 call 3 */
    case 1027: /* module 5 call 3 */
    case 7939: /* module 5 call 3 */
    case 2563:
      switch (itemIdx) {
        case 0: /* balances_transfer_keep_alive_V18 - amount */;
          return _toStringCompactBalance(
              &m->nested.balances_transfer_keep_alive_V18.amount, outValue,
              outValueLen, pageIdx, pageCount);
        case 1: /* balances_transfer_keep_alive_V18 - dest */;
          return _toStringAccountIdLookupOfT_V18(
              &m->nested.balances_transfer_keep_alive_V18.dest, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1284: /* module 5 call 4 */
    case 1028: /* module 5 call 4 */
    case 7940: /* module 5 call 4 */
    case 2564:
      switch (itemIdx) {
        case 0: /* balances_transfer_all_V18 - dest */;
          return _toStringAccountIdLookupOfT_V18(
              &m->basic.balances_transfer_all_V18.dest, outValue, outValueLen,
              pageIdx, pageCount);
        case 1: /* balances_transfer_all_V18 - keep_alive */;
          return _toStringbool(&m->basic.balances_transfer_all_V18.keep_alive,
                               outValue, outValueLen, pageCount);
        default:
          return parser_no_data;
      }

    default:
      return parser_ok;
  }

  return parser_ok;
}

bool _getMethod_ItemIsExpert_V18(uint8_t moduleIdx, uint8_t callIdx,
                                 uint8_t itemIdx) {
  (void)itemIdx;
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    default:
      return false;
  }
}

bool _getMethod_IsNestingSupported_V18(uint8_t moduleIdx, uint8_t callIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {  // Balances & BlindSign
    case 1280:            /* module 5 call 0 */
    case 1287:            /* module 5 call 0 */
    case 1024:            /* module 5 call 0 */
    case 1031:            /* module 5 call 0 */
    case 7936:            /* module 5 call 0 */
    case 7943:
    case 1282:
    case 1026:
    case 7938:
    case 1283:
    case 1027:
    case 7939:
    case 2567:
    case 2562:
    case 2563:
    case 2564:
      return true;
    default:
      return false;
  }
}
