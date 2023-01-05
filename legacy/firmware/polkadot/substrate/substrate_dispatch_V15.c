#include "substrate_dispatch_V15.h"
#include <stdint.h>
#include "../common_defs.h"
#include "substrate_strings.h"

__Z_INLINE parser_error_t _readMethod_balances_transfer_V15(
    parser_context_t* c, pd_balances_transfer_V15_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_force_transfer_V15(
    parser_context_t* c, pd_balances_force_transfer_V15_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->source))
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_keep_alive_V15(
    parser_context_t* c, pd_balances_transfer_keep_alive_V15_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->dest))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_balances_transfer_all_V15(
    parser_context_t* c, pd_balances_transfer_all_V15_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->dest))
  CHECK_ERROR(_readbool(c, &m->keep_alive))
  return parser_ok;
}

__Z_INLINE parser_error_t
_readMethod_staking_bond_V15(parser_context_t* c, pd_staking_bond_V15_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->controller))
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  CHECK_ERROR(_readRewardDestination_V15(c, &m->payee))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_bond_extra_V15(
    parser_context_t* c, pd_staking_bond_extra_V15_t* m) {
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_unbond_V15(
    parser_context_t* c, pd_staking_unbond_V15_t* m) {
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_withdraw_unbonded_V15(
    parser_context_t* c, pd_staking_withdraw_unbonded_V15_t* m) {
  CHECK_ERROR(_readu32(c, &m->num_slashing_spans))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_validate_V15(
    parser_context_t* c, pd_staking_validate_V15_t* m) {
  CHECK_ERROR(_readValidatorPrefs_V15(c, &m->prefs))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_nominate_V15(
    parser_context_t* c, pd_staking_nominate_V15_t* m) {
  CHECK_ERROR(_readVecAccountIdLookupOfT_V15(c, &m->targets))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_chill_V15(void) {
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_set_payee_V15(
    parser_context_t* c, pd_staking_set_payee_V15_t* m) {
  CHECK_ERROR(_readRewardDestination_V15(c, &m->payee))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_set_controller_V15(
    parser_context_t* c, pd_staking_set_controller_V15_t* m) {
  CHECK_ERROR(_readAccountIdLookupOfT_V15(c, &m->controller))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_payout_stakers_V15(
    parser_context_t* c, pd_staking_payout_stakers_V15_t* m) {
  CHECK_ERROR(_readAccountId_V15(c, &m->validator_stash))
  CHECK_ERROR(_readEraIndex_V15(c, &m->era))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_staking_rebond_V15(
    parser_context_t* c, pd_staking_rebond_V15_t* m) {
  CHECK_ERROR(_readCompactBalance(c, &m->amount))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_session_set_keys_V15(
    parser_context_t* c, pd_session_set_keys_V15_t* m) {
  CHECK_ERROR(_readKeys_V15(c, &m->keys))
  CHECK_ERROR(_substrate_readBytes(c, &m->proof))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_session_purge_keys_V15(void) {
  return parser_ok;
}

__Z_INLINE parser_error_t
_readMethod_utility_batch_V15(parser_context_t* c, pd_utility_batch_V15_t* m) {
  CHECK_ERROR(_readVecCall(c, &m->calls))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_utility_batch_all_V15(
    parser_context_t* c, pd_utility_batch_all_V15_t* m) {
  CHECK_ERROR(_readVecCall(c, &m->calls))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_utility_force_batch_V15(
    parser_context_t* c, pd_utility_force_batch_V15_t* m) {
  CHECK_ERROR(_readVecCall(c, &m->calls))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_create_V15(
    parser_context_t* c, pd_crowdloan_create_V15_t* m) {
  CHECK_ERROR(_readCompactu32(c, &m->index))
  CHECK_ERROR(_readCompactu128(c, &m->cap))
  CHECK_ERROR(_readCompactu32(c, &m->first_period))
  CHECK_ERROR(_readCompactu32(c, &m->last_period))
  CHECK_ERROR(_readCompactu32(c, &m->end))
  CHECK_ERROR(_readOptionMultiSigner_V15(c, &m->verifier))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_contribute_V15(
    parser_context_t* c, pd_crowdloan_contribute_V15_t* m) {
  CHECK_ERROR(_readCompactu32(c, &m->index))
  CHECK_ERROR(_readCompactu128(c, &m->amount))
  CHECK_ERROR(_readOptionMultiSignature_V15(c, &m->signature))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_withdraw_V15(
    parser_context_t* c, pd_crowdloan_withdraw_V15_t* m) {
  CHECK_ERROR(_readAccountId_V15(c, &m->who))
  CHECK_ERROR(_readCompactu32(c, &m->index))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_refund_V15(
    parser_context_t* c, pd_crowdloan_refund_V15_t* m) {
  CHECK_ERROR(_readCompactu32(c, &m->index))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_dissolve_V15(
    parser_context_t* c, pd_crowdloan_dissolve_V15_t* m) {
  CHECK_ERROR(_readCompactu32(c, &m->index))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_edit_V15(
    parser_context_t* c, pd_crowdloan_edit_V15_t* m) {
  CHECK_ERROR(_readCompactu32(c, &m->index))
  CHECK_ERROR(_readCompactu128(c, &m->cap))
  CHECK_ERROR(_readCompactu32(c, &m->first_period))
  CHECK_ERROR(_readCompactu32(c, &m->last_period))
  CHECK_ERROR(_readCompactu32(c, &m->end))
  CHECK_ERROR(_readOptionMultiSigner_V15(c, &m->verifier))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_add_memo_V15(
    parser_context_t* c, pd_crowdloan_add_memo_V15_t* m) {
  CHECK_ERROR(_readParaId_V15(c, &m->index))
  CHECK_ERROR(_readVecu8(c, &m->memo))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_poke_V15(
    parser_context_t* c, pd_crowdloan_poke_V15_t* m) {
  CHECK_ERROR(_readParaId_V15(c, &m->index))
  return parser_ok;
}

__Z_INLINE parser_error_t _readMethod_crowdloan_contribute_all_V15(
    parser_context_t* c, pd_crowdloan_contribute_all_V15_t* m) {
  CHECK_ERROR(_readCompactu32(c, &m->index))
  CHECK_ERROR(_readOptionMultiSignature_V15(c, &m->signature))
  return parser_ok;
}

parser_error_t _readMethod_V15(parser_context_t* c, uint8_t moduleIdx,
                               uint8_t callIdx, pd_Method_V15_t* method) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;
  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
      CHECK_ERROR(_readMethod_balances_transfer_V15(
          c, &method->nested.balances_transfer_V15))
      break;
    case 1282: /* module 5 call 2 */
      CHECK_ERROR(_readMethod_balances_force_transfer_V15(
          c, &method->nested.balances_force_transfer_V15))
      break;
    case 1283: /* module 5 call 3 */
      CHECK_ERROR(_readMethod_balances_transfer_keep_alive_V15(
          c, &method->nested.balances_transfer_keep_alive_V15))
      break;
    case 1284: /* module 5 call 4 */
      CHECK_ERROR(_readMethod_balances_transfer_all_V15(
          c, &method->basic.balances_transfer_all_V15))
      break;
    case 1792: /* module 7 call 0 */
      CHECK_ERROR(
          _readMethod_staking_bond_V15(c, &method->basic.staking_bond_V15))
      break;
    case 1793: /* module 7 call 1 */
      CHECK_ERROR(_readMethod_staking_bond_extra_V15(
          c, &method->basic.staking_bond_extra_V15))
      break;
    case 1794: /* module 7 call 2 */
      CHECK_ERROR(
          _readMethod_staking_unbond_V15(c, &method->basic.staking_unbond_V15))
      break;
    case 1795: /* module 7 call 3 */
      CHECK_ERROR(_readMethod_staking_withdraw_unbonded_V15(
          c, &method->basic.staking_withdraw_unbonded_V15))
      break;
    case 1796: /* module 7 call 4 */
      CHECK_ERROR(_readMethod_staking_validate_V15(
          c, &method->basic.staking_validate_V15))
      break;
    case 1797: /* module 7 call 5 */
      CHECK_ERROR(_readMethod_staking_nominate_V15(
          c, &method->basic.staking_nominate_V15))
      break;
    case 1798: /* module 7 call 6 */
      CHECK_ERROR(_readMethod_staking_chill_V15())
      break;
    case 1799: /* module 7 call 7 */
      CHECK_ERROR(_readMethod_staking_set_payee_V15(
          c, &method->basic.staking_set_payee_V15))
      break;
    case 1800: /* module 7 call 8 */
      CHECK_ERROR(_readMethod_staking_set_controller_V15(
          c, &method->basic.staking_set_controller_V15))
      break;
    case 1810: /* module 7 call 18 */
      CHECK_ERROR(_readMethod_staking_payout_stakers_V15(
          c, &method->basic.staking_payout_stakers_V15))
      break;
    case 1811: /* module 7 call 19 */
      CHECK_ERROR(
          _readMethod_staking_rebond_V15(c, &method->basic.staking_rebond_V15))
      break;
    case 2304: /* module 9 call 0 */
      CHECK_ERROR(_readMethod_session_set_keys_V15(
          c, &method->basic.session_set_keys_V15))
      break;
    case 2305: /* module 9 call 1 */
      CHECK_ERROR(_readMethod_session_purge_keys_V15())
      break;
    case 6656: /* module 26 call 0 */
      CHECK_ERROR(
          _readMethod_utility_batch_V15(c, &method->basic.utility_batch_V15))
      break;
    case 6658: /* module 26 call 2 */
      CHECK_ERROR(_readMethod_utility_batch_all_V15(
          c, &method->basic.utility_batch_all_V15))
      break;
    case 6660: /* module 26 call 4 */
      CHECK_ERROR(_readMethod_utility_force_batch_V15(
          c, &method->basic.utility_force_batch_V15))
      break;
    case 18688: /* module 73 call 0 */
      CHECK_ERROR(_readMethod_crowdloan_create_V15(
          c, &method->basic.crowdloan_create_V15))
      break;
    case 18689: /* module 73 call 1 */
      CHECK_ERROR(_readMethod_crowdloan_contribute_V15(
          c, &method->basic.crowdloan_contribute_V15))
      break;
    case 18690: /* module 73 call 2 */
      CHECK_ERROR(_readMethod_crowdloan_withdraw_V15(
          c, &method->basic.crowdloan_withdraw_V15))
      break;
    case 18691: /* module 73 call 3 */
      CHECK_ERROR(_readMethod_crowdloan_refund_V15(
          c, &method->basic.crowdloan_refund_V15))
      break;
    case 18692: /* module 73 call 4 */
      CHECK_ERROR(_readMethod_crowdloan_dissolve_V15(
          c, &method->basic.crowdloan_dissolve_V15))
      break;
    case 18693: /* module 73 call 5 */
      CHECK_ERROR(
          _readMethod_crowdloan_edit_V15(c, &method->basic.crowdloan_edit_V15))
      break;
    case 18694: /* module 73 call 6 */
      CHECK_ERROR(_readMethod_crowdloan_add_memo_V15(
          c, &method->basic.crowdloan_add_memo_V15))
      break;
    case 18695: /* module 73 call 7 */
      CHECK_ERROR(
          _readMethod_crowdloan_poke_V15(c, &method->basic.crowdloan_poke_V15))
      break;
    case 18696: /* module 73 call 8 */
      CHECK_ERROR(_readMethod_crowdloan_contribute_all_V15(
          c, &method->basic.crowdloan_contribute_all_V15))
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

const char* _getMethod_ModuleName_V15(uint8_t moduleIdx) {
  switch (moduleIdx) {
    case 5:
      return STR_MO_BALANCES;
    case 7:
      return STR_MO_STAKING;
    case 9:
      return STR_MO_SESSION;
    case 26:
      return STR_MO_UTILITY;
    case 73:
      return STR_MO_CROWDLOAN;
    default:
      return NULL;
  }

  return NULL;
}

const char* _getMethod_Name_V15(uint8_t moduleIdx, uint8_t callIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
      return STR_ME_TRANSFER;
    case 1282: /* module 5 call 2 */
      return STR_ME_FORCE_TRANSFER;
    case 1283: /* module 5 call 3 */
      return STR_ME_TRANSFER_KEEP_ALIVE;
    case 1284: /* module 5 call 4 */
      return STR_ME_TRANSFER_ALL;
    case 1792: /* module 7 call 0 */
      return STR_ME_BOND;
    case 1793: /* module 7 call 1 */
      return STR_ME_BOND_EXTRA;
    case 1794: /* module 7 call 2 */
      return STR_ME_UNBOND;
    case 1795: /* module 7 call 3 */
      return STR_ME_WITHDRAW_UNBONDED;
    case 1796: /* module 7 call 4 */
      return STR_ME_VALIDATE;
    case 1797: /* module 7 call 5 */
      return STR_ME_NOMINATE;
    case 1798: /* module 7 call 6 */
      return STR_ME_CHILL;
    case 1799: /* module 7 call 7 */
      return STR_ME_SET_PAYEE;
    case 1800: /* module 7 call 8 */
      return STR_ME_SET_CONTROLLER;
    case 1810: /* module 7 call 18 */
      return STR_ME_PAYOUT_STAKERS;
    case 1811: /* module 7 call 19 */
      return STR_ME_REBOND;
    case 2304: /* module 9 call 0 */
      return STR_ME_SET_KEYS;
    case 2305: /* module 9 call 1 */
      return STR_ME_PURGE_KEYS;
    case 6656: /* module 26 call 0 */
      return STR_ME_BATCH;
    case 6658: /* module 26 call 2 */
      return STR_ME_BATCH_ALL;
    case 6660: /* module 26 call 4 */
      return STR_ME_FORCE_BATCH;
    case 18688: /* module 73 call 0 */
      return STR_ME_CREATE;
    case 18689: /* module 73 call 1 */
      return STR_ME_CONTRIBUTE;
    case 18690: /* module 73 call 2 */
      return STR_ME_WITHDRAW;
    case 18691: /* module 73 call 3 */
      return STR_ME_REFUND;
    case 18692: /* module 73 call 4 */
      return STR_ME_DISSOLVE;
    case 18693: /* module 73 call 5 */
      return STR_ME_EDIT;
    case 18694: /* module 73 call 6 */
      return STR_ME_ADD_MEMO;
    case 18695: /* module 73 call 7 */
      return STR_ME_POKE;
    case 18696: /* module 73 call 8 */
      return STR_ME_CONTRIBUTE_ALL;
    default:
      return _getMethod_Name_V15_ParserFull(callPrivIdx);
  }

  return NULL;
}

const char* _getMethod_Name_V15_ParserFull(uint16_t callPrivIdx) {
  switch (callPrivIdx) {
    default:
      return NULL;
  }

  return NULL;
}

uint8_t _getMethod_NumItems_V15(uint8_t moduleIdx, uint8_t callIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
      return 2;
    case 1282: /* module 5 call 2 */
      return 3;
    case 1283: /* module 5 call 3 */
      return 2;
    case 1284: /* module 5 call 4 */
      return 2;
    case 1792: /* module 7 call 0 */
      return 3;
    case 1793: /* module 7 call 1 */
      return 1;
    case 1794: /* module 7 call 2 */
      return 1;
    case 1795: /* module 7 call 3 */
      return 1;
    case 1796: /* module 7 call 4 */
      return 1;
    case 1797: /* module 7 call 5 */
      return 1;
    case 1798: /* module 7 call 6 */
      return 0;
    case 1799: /* module 7 call 7 */
      return 1;
    case 1800: /* module 7 call 8 */
      return 1;
    case 1810: /* module 7 call 18 */
      return 2;
    case 1811: /* module 7 call 19 */
      return 1;
    case 2304: /* module 9 call 0 */
      return 2;
    case 2305: /* module 9 call 1 */
      return 0;
    case 6656: /* module 26 call 0 */
      return 1;
    case 6658: /* module 26 call 2 */
      return 1;
    case 6660: /* module 26 call 4 */
      return 1;
    case 18688: /* module 73 call 0 */
      return 6;
    case 18689: /* module 73 call 1 */
      return 3;
    case 18690: /* module 73 call 2 */
      return 2;
    case 18691: /* module 73 call 3 */
      return 1;
    case 18692: /* module 73 call 4 */
      return 1;
    case 18693: /* module 73 call 5 */
      return 6;
    case 18694: /* module 73 call 6 */
      return 2;
    case 18695: /* module 73 call 7 */
      return 1;
    case 18696: /* module 73 call 8 */
      return 2;
    default:
      return 0;
  }

  return 0;
}

const char* _getMethod_ItemName_V15(uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
      switch (itemIdx) {
        case 0:
          return STR_IT_dest;
        case 1:
          return STR_IT_amount;
        default:
          return NULL;
      }
    case 1282: /* module 5 call 2 */
      switch (itemIdx) {
        case 0:
          return STR_IT_source;
        case 1:
          return STR_IT_dest;
        case 2:
          return STR_IT_amount;
        default:
          return NULL;
      }
    case 1283: /* module 5 call 3 */
      switch (itemIdx) {
        case 0:
          return STR_IT_dest;
        case 1:
          return STR_IT_amount;
        default:
          return NULL;
      }
    case 1284: /* module 5 call 4 */
      switch (itemIdx) {
        case 0:
          return STR_IT_dest;
        case 1:
          return STR_IT_keep_alive;
        default:
          return NULL;
      }
    case 1792: /* module 7 call 0 */
      switch (itemIdx) {
        case 0:
          return STR_IT_controller;
        case 1:
          return STR_IT_amount;
        case 2:
          return STR_IT_payee;
        default:
          return NULL;
      }
    case 1793: /* module 7 call 1 */
      switch (itemIdx) {
        case 0:
          return STR_IT_amount;
        default:
          return NULL;
      }
    case 1794: /* module 7 call 2 */
      switch (itemIdx) {
        case 0:
          return STR_IT_amount;
        default:
          return NULL;
      }
    case 1795: /* module 7 call 3 */
      switch (itemIdx) {
        case 0:
          return STR_IT_num_slashing_spans;
        default:
          return NULL;
      }
    case 1796: /* module 7 call 4 */
      switch (itemIdx) {
        case 0:
          return STR_IT_prefs;
        default:
          return NULL;
      }
    case 1797: /* module 7 call 5 */
      switch (itemIdx) {
        case 0:
          return STR_IT_targets;
        default:
          return NULL;
      }
    case 1798: /* module 7 call 6 */
      switch (itemIdx) {
        default:
          return NULL;
      }
    case 1799: /* module 7 call 7 */
      switch (itemIdx) {
        case 0:
          return STR_IT_payee;
        default:
          return NULL;
      }
    case 1800: /* module 7 call 8 */
      switch (itemIdx) {
        case 0:
          return STR_IT_controller;
        default:
          return NULL;
      }
    case 1810: /* module 7 call 18 */
      switch (itemIdx) {
        case 0:
          return STR_IT_validator_stash;
        case 1:
          return STR_IT_era;
        default:
          return NULL;
      }
    case 1811: /* module 7 call 19 */
      switch (itemIdx) {
        case 0:
          return STR_IT_amount;
        default:
          return NULL;
      }
    case 2304: /* module 9 call 0 */
      switch (itemIdx) {
        case 0:
          return STR_IT_keys;
        case 1:
          return STR_IT_proof;
        default:
          return NULL;
      }
    case 2305: /* module 9 call 1 */
      switch (itemIdx) {
        default:
          return NULL;
      }
    case 6656: /* module 26 call 0 */
      switch (itemIdx) {
        case 0:
          return STR_IT_calls;
        default:
          return NULL;
      }
    case 6658: /* module 26 call 2 */
      switch (itemIdx) {
        case 0:
          return STR_IT_calls;
        default:
          return NULL;
      }
    case 6660: /* module 26 call 4 */
      switch (itemIdx) {
        case 0:
          return STR_IT_calls;
        default:
          return NULL;
      }
    case 18688: /* module 73 call 0 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        case 1:
          return STR_IT_cap;
        case 2:
          return STR_IT_first_period;
        case 3:
          return STR_IT_last_period;
        case 4:
          return STR_IT_end;
        case 5:
          return STR_IT_verifier;
        default:
          return NULL;
      }
    case 18689: /* module 73 call 1 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        case 1:
          return STR_IT_amount;
        case 2:
          return STR_IT_signature;
        default:
          return NULL;
      }
    case 18690: /* module 73 call 2 */
      switch (itemIdx) {
        case 0:
          return STR_IT_who;
        case 1:
          return STR_IT_index;
        default:
          return NULL;
      }
    case 18691: /* module 73 call 3 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        default:
          return NULL;
      }
    case 18692: /* module 73 call 4 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        default:
          return NULL;
      }
    case 18693: /* module 73 call 5 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        case 1:
          return STR_IT_cap;
        case 2:
          return STR_IT_first_period;
        case 3:
          return STR_IT_last_period;
        case 4:
          return STR_IT_end;
        case 5:
          return STR_IT_verifier;
        default:
          return NULL;
      }
    case 18694: /* module 73 call 6 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        case 1:
          return STR_IT_memo;
        default:
          return NULL;
      }
    case 18695: /* module 73 call 7 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        default:
          return NULL;
      }
    case 18696: /* module 73 call 8 */
      switch (itemIdx) {
        case 0:
          return STR_IT_index;
        case 1:
          return STR_IT_signature;
        default:
          return NULL;
      }
    default:
      return NULL;
  }

  return NULL;
}

parser_error_t _getMethod_ItemValue_V15(pd_Method_V15_t* m, uint8_t moduleIdx,
                                        uint8_t callIdx, uint8_t itemIdx,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1280: /* module 5 call 0 */
      switch (itemIdx) {
        case 0: /* balances_transfer_V15 - dest */;
          return _toStringAccountIdLookupOfT_V15(
              &m->nested.balances_transfer_V15.dest, outValue, outValueLen,
              pageIdx, pageCount);
        case 1: /* balances_transfer_V15 - amount */;
          return _toStringCompactBalance(
              &m->nested.balances_transfer_V15.amount, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1282: /* module 5 call 2 */
      switch (itemIdx) {
        case 0: /* balances_force_transfer_V15 - source */;
          return _toStringAccountIdLookupOfT_V15(
              &m->nested.balances_force_transfer_V15.source, outValue,
              outValueLen, pageIdx, pageCount);
        case 1: /* balances_force_transfer_V15 - dest */;
          return _toStringAccountIdLookupOfT_V15(
              &m->nested.balances_force_transfer_V15.dest, outValue,
              outValueLen, pageIdx, pageCount);
        case 2: /* balances_force_transfer_V15 - amount */;
          return _toStringCompactBalance(
              &m->nested.balances_force_transfer_V15.amount, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1283: /* module 5 call 3 */
      switch (itemIdx) {
        case 0: /* balances_transfer_keep_alive_V15 - dest */;
          return _toStringAccountIdLookupOfT_V15(
              &m->nested.balances_transfer_keep_alive_V15.dest, outValue,
              outValueLen, pageIdx, pageCount);
        case 1: /* balances_transfer_keep_alive_V15 - amount */;
          return _toStringCompactBalance(
              &m->nested.balances_transfer_keep_alive_V15.amount, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1284: /* module 5 call 4 */
      switch (itemIdx) {
        case 0: /* balances_transfer_all_V15 - dest */;
          return _toStringAccountIdLookupOfT_V15(
              &m->basic.balances_transfer_all_V15.dest, outValue, outValueLen,
              pageIdx, pageCount);
        case 1: /* balances_transfer_all_V15 - keep_alive */;
          return _toStringbool(&m->basic.balances_transfer_all_V15.keep_alive,
                               outValue, outValueLen, pageCount);
        default:
          return parser_no_data;
      }
    case 1792: /* module 7 call 0 */
      switch (itemIdx) {
        case 0: /* staking_bond_V15 - controller */;
          return _toStringAccountIdLookupOfT_V15(
              &m->basic.staking_bond_V15.controller, outValue, outValueLen,
              pageIdx, pageCount);
        case 1: /* staking_bond_V15 - amount */;
          return _toStringCompactBalance(&m->basic.staking_bond_V15.amount,
                                         outValue, outValueLen, pageIdx,
                                         pageCount);
        case 2: /* staking_bond_V15 - payee */;
          return _toStringRewardDestination_V15(
              &m->basic.staking_bond_V15.payee, outValue, outValueLen, pageIdx,
              pageCount);
        default:
          return parser_no_data;
      }
    case 1793: /* module 7 call 1 */
      switch (itemIdx) {
        case 0: /* staking_bond_extra_V15 - amount */;
          return _toStringCompactBalance(
              &m->basic.staking_bond_extra_V15.amount, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1794: /* module 7 call 2 */
      switch (itemIdx) {
        case 0: /* staking_unbond_V15 - amount */;
          return _toStringCompactBalance(&m->basic.staking_unbond_V15.amount,
                                         outValue, outValueLen, pageIdx,
                                         pageCount);
        default:
          return parser_no_data;
      }
    case 1795: /* module 7 call 3 */
      switch (itemIdx) {
        case 0: /* staking_withdraw_unbonded_V15 - num_slashing_spans */;
          return _toStringu32(
              &m->basic.staking_withdraw_unbonded_V15.num_slashing_spans,
              outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1796: /* module 7 call 4 */
      switch (itemIdx) {
        case 0: /* staking_validate_V15 - prefs */;
          return _toStringValidatorPrefs_V15(
              &m->basic.staking_validate_V15.prefs, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1797: /* module 7 call 5 */
      switch (itemIdx) {
        case 0: /* staking_nominate_V15 - targets */;
          return _toStringVecAccountIdLookupOfT_V15(
              &m->basic.staking_nominate_V15.targets, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1798: /* module 7 call 6 */
      switch (itemIdx) {
        default:
          return parser_no_data;
      }
    case 1799: /* module 7 call 7 */
      switch (itemIdx) {
        case 0: /* staking_set_payee_V15 - payee */;
          return _toStringRewardDestination_V15(
              &m->basic.staking_set_payee_V15.payee, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1800: /* module 7 call 8 */
      switch (itemIdx) {
        case 0: /* staking_set_controller_V15 - controller */;
          return _toStringAccountIdLookupOfT_V15(
              &m->basic.staking_set_controller_V15.controller, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 1810: /* module 7 call 18 */
      switch (itemIdx) {
        case 0: /* staking_payout_stakers_V15 - validator_stash */;
          return _toStringAccountId_V15(
              &m->basic.staking_payout_stakers_V15.validator_stash, outValue,
              outValueLen, pageIdx, pageCount);
        case 1: /* staking_payout_stakers_V15 - era */;
          return _toStringEraIndex_V15(&m->basic.staking_payout_stakers_V15.era,
                                       outValue, outValueLen, pageIdx,
                                       pageCount);
        default:
          return parser_no_data;
      }
    case 1811: /* module 7 call 19 */
      switch (itemIdx) {
        case 0: /* staking_rebond_V15 - amount */;
          return _toStringCompactBalance(&m->basic.staking_rebond_V15.amount,
                                         outValue, outValueLen, pageIdx,
                                         pageCount);
        default:
          return parser_no_data;
      }
    case 2304: /* module 9 call 0 */
      switch (itemIdx) {
        case 0: /* session_set_keys_V15 - keys */;
          return _toStringKeys_V15(&m->basic.session_set_keys_V15.keys,
                                   outValue, outValueLen, pageIdx, pageCount);
        case 1: /* session_set_keys_V15 - proof */;
          return _toStringBytes(&m->basic.session_set_keys_V15.proof, outValue,
                                outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 2305: /* module 9 call 1 */
      switch (itemIdx) {
        default:
          return parser_no_data;
      }
    case 6656: /* module 26 call 0 */
      switch (itemIdx) {
        case 0: /* utility_batch_V15 - calls */;
          return _toStringVecCall(&m->basic.utility_batch_V15.calls, outValue,
                                  outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 6658: /* module 26 call 2 */
      switch (itemIdx) {
        case 0: /* utility_batch_all_V15 - calls */;
          return _toStringVecCall(&m->basic.utility_batch_all_V15.calls,
                                  outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 6660: /* module 26 call 4 */
      switch (itemIdx) {
        case 0: /* utility_force_batch_V15 - calls */;
          return _toStringVecCall(&m->basic.utility_force_batch_V15.calls,
                                  outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18688: /* module 73 call 0 */
      switch (itemIdx) {
        case 0: /* crowdloan_create_V15 - index */;
          return _toStringCompactu32(&m->basic.crowdloan_create_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 1: /* crowdloan_create_V15 - cap */;
          return _toStringCompactu128(&m->basic.crowdloan_create_V15.cap,
                                      outValue, outValueLen, pageIdx,
                                      pageCount);
        case 2: /* crowdloan_create_V15 - first_period */;
          return _toStringCompactu32(
              &m->basic.crowdloan_create_V15.first_period, outValue,
              outValueLen, pageIdx, pageCount);
        case 3: /* crowdloan_create_V15 - last_period */;
          return _toStringCompactu32(&m->basic.crowdloan_create_V15.last_period,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 4: /* crowdloan_create_V15 - end */;
          return _toStringCompactu32(&m->basic.crowdloan_create_V15.end,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 5: /* crowdloan_create_V15 - verifier */;
          return _toStringOptionMultiSigner_V15(
              &m->basic.crowdloan_create_V15.verifier, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18689: /* module 73 call 1 */
      switch (itemIdx) {
        case 0: /* crowdloan_contribute_V15 - index */;
          return _toStringCompactu32(&m->basic.crowdloan_contribute_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 1: /* crowdloan_contribute_V15 - amount */;
          return _toStringCompactu128(&m->basic.crowdloan_contribute_V15.amount,
                                      outValue, outValueLen, pageIdx,
                                      pageCount);
        case 2: /* crowdloan_contribute_V15 - signature */;
          return _toStringOptionMultiSignature_V15(
              &m->basic.crowdloan_contribute_V15.signature, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18690: /* module 73 call 2 */
      switch (itemIdx) {
        case 0: /* crowdloan_withdraw_V15 - who */;
          return _toStringAccountId_V15(&m->basic.crowdloan_withdraw_V15.who,
                                        outValue, outValueLen, pageIdx,
                                        pageCount);
        case 1: /* crowdloan_withdraw_V15 - index */;
          return _toStringCompactu32(&m->basic.crowdloan_withdraw_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18691: /* module 73 call 3 */
      switch (itemIdx) {
        case 0: /* crowdloan_refund_V15 - index */;
          return _toStringCompactu32(&m->basic.crowdloan_refund_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18692: /* module 73 call 4 */
      switch (itemIdx) {
        case 0: /* crowdloan_dissolve_V15 - index */;
          return _toStringCompactu32(&m->basic.crowdloan_dissolve_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18693: /* module 73 call 5 */
      switch (itemIdx) {
        case 0: /* crowdloan_edit_V15 - index */;
          return _toStringCompactu32(&m->basic.crowdloan_edit_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 1: /* crowdloan_edit_V15 - cap */;
          return _toStringCompactu128(&m->basic.crowdloan_edit_V15.cap,
                                      outValue, outValueLen, pageIdx,
                                      pageCount);
        case 2: /* crowdloan_edit_V15 - first_period */;
          return _toStringCompactu32(&m->basic.crowdloan_edit_V15.first_period,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 3: /* crowdloan_edit_V15 - last_period */;
          return _toStringCompactu32(&m->basic.crowdloan_edit_V15.last_period,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 4: /* crowdloan_edit_V15 - end */;
          return _toStringCompactu32(&m->basic.crowdloan_edit_V15.end, outValue,
                                     outValueLen, pageIdx, pageCount);
        case 5: /* crowdloan_edit_V15 - verifier */;
          return _toStringOptionMultiSigner_V15(
              &m->basic.crowdloan_edit_V15.verifier, outValue, outValueLen,
              pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18694: /* module 73 call 6 */
      switch (itemIdx) {
        case 0: /* crowdloan_add_memo_V15 - index */;
          return _toStringParaId_V15(&m->basic.crowdloan_add_memo_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        case 1: /* crowdloan_add_memo_V15 - memo */;
          return _toStringVecu8(&m->basic.crowdloan_add_memo_V15.memo, outValue,
                                outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18695: /* module 73 call 7 */
      switch (itemIdx) {
        case 0: /* crowdloan_poke_V15 - index */;
          return _toStringParaId_V15(&m->basic.crowdloan_poke_V15.index,
                                     outValue, outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    case 18696: /* module 73 call 8 */
      switch (itemIdx) {
        case 0: /* crowdloan_contribute_all_V15 - index */;
          return _toStringCompactu32(
              &m->basic.crowdloan_contribute_all_V15.index, outValue,
              outValueLen, pageIdx, pageCount);
        case 1: /* crowdloan_contribute_all_V15 - signature */;
          return _toStringOptionMultiSignature_V15(
              &m->basic.crowdloan_contribute_all_V15.signature, outValue,
              outValueLen, pageIdx, pageCount);
        default:
          return parser_no_data;
      }
    default:
      return parser_ok;
  }

  return parser_ok;
}

bool _getMethod_ItemIsExpert_V15(uint8_t moduleIdx, uint8_t callIdx,
                                 uint8_t itemIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 1795:  // Staking:Withdraw Unbonded
      switch (itemIdx) {
        case 0:  // Num slashing spans
          return true;
        default:
          return false;
      }
    case 1807:  // Staking:Force unstake
      switch (itemIdx) {
        case 1:  // Num slashing spans
          return true;
        default:
          return false;
      }
    case 1812:  // Staking:Reap stash
      switch (itemIdx) {
        case 1:  // Num slashing spans
          return true;
        default:
          return false;
      }
    case 9988:  // NominationPools:Pool withdraw unbonded
      switch (itemIdx) {
        case 1:  // Num slashing spans
          return true;
        default:
          return false;
      }
    case 9989:  // NominationPools:Withdraw Unbonded
      switch (itemIdx) {
        case 1:  // Num slashing spans
          return true;
        default:
          return false;
      }

    default:
      return false;
  }
}

bool _getMethod_IsNestingSupported_V15(uint8_t moduleIdx, uint8_t callIdx) {
  uint16_t callPrivIdx = ((uint16_t)moduleIdx << 8u) + callIdx;

  switch (callPrivIdx) {
    case 2560:   // Preimage:Note preimage
    case 2561:   // Preimage:Unnote preimage
    case 2562:   // Preimage:Request preimage
    case 2563:   // Preimage:Unrequest preimage
    case 768:    // Timestamp:Set
    case 1024:   // Indices:Claim
    case 1025:   // Indices:Transfer
    case 1026:   // Indices:Free
    case 1027:   // Indices:Force transfer
    case 1028:   // Indices:Freeze
    case 1284:   // Balances:Transfer all
    case 1285:   // Balances:Force unreserve
    case 1792:   // Staking:Bond
    case 1793:   // Staking:Bond extra
    case 1794:   // Staking:Unbond
    case 1795:   // Staking:Withdraw Unbonded
    case 1796:   // Staking:Validate
    case 1797:   // Staking:Nominate
    case 1798:   // Staking:Chill
    case 1799:   // Staking:Set payee
    case 1800:   // Staking:Set controller
    case 1801:   // Staking:Set validator count
    case 1802:   // Staking:Increase validator count
    case 1804:   // Staking:Force no eras
    case 1805:   // Staking:Force new era
    case 1806:   // Staking:Set invulnerables
    case 1807:   // Staking:Force unstake
    case 1808:   // Staking:Force new era always
    case 1809:   // Staking:Cancel deferred slash
    case 1810:   // Staking:Payout stakers
    case 1811:   // Staking:Rebond
    case 1812:   // Staking:Reap stash
    case 1813:   // Staking:Kick
    case 1815:   // Staking:Chill other
    case 1816:   // Staking:Force apply min commission
    case 2304:   // Session:Set keys
    case 2305:   // Session:Purge keys
    case 2818:   // Grandpa:Note stalled
    case 3598:   // Democracy:Note preimage
    case 3599:   // Democracy:Note preimage operational
    case 3600:   // Democracy:Note imminent preimage
    case 3601:   // Democracy:Note imminent preimage operational
    case 3840:   // Council:Set members
    case 3841:   // Council:Execute
    case 3842:   // Council:Propose
    case 3843:   // Council:Vote
    case 3844:   // Council:Close
    case 3845:   // Council:Disapprove proposal
    case 4096:   // TechnicalCommittee:Set members
    case 4097:   // TechnicalCommittee:Execute
    case 4098:   // TechnicalCommittee:Propose
    case 4099:   // TechnicalCommittee:Vote
    case 4100:   // TechnicalCommittee:Close
    case 4101:   // TechnicalCommittee:Disapprove proposal
    case 4352:   // PhragmenElection:Vote
    case 4353:   // PhragmenElection:Remove voter
    case 4354:   // PhragmenElection:Submit candidacy
    case 4356:   // PhragmenElection:Remove member
    case 4357:   // PhragmenElection:Clean defunct voters
    case 4608:   // TechnicalMembership:Add member
    case 4609:   // TechnicalMembership:Remove member
    case 4610:   // TechnicalMembership:Swap member
    case 4611:   // TechnicalMembership:Reset members
    case 4612:   // TechnicalMembership:Change key
    case 4613:   // TechnicalMembership:Set prime
    case 4614:   // TechnicalMembership:Clear prime
    case 4864:   // Treasury:Propose spend
    case 4865:   // Treasury:Reject proposal
    case 4866:   // Treasury:Approve proposal
    case 4867:   // Treasury:Spend
    case 4868:   // Treasury:Remove approval
    case 6144:   // Claims:Claim
    case 6146:   // Claims:Claim attest
    case 6147:   // Claims:Attest
    case 6148:   // Claims:Move claim
    case 6400:   // Vesting:Vest
    case 6401:   // Vesting:Vest other
    case 6402:   // Vesting:Vested transfer
    case 6403:   // Vesting:Force vested transfer
    case 6404:   // Vesting:Merge schedules
    case 6656:   // Utility:Batch
    case 6658:   // Utility:Batch all
    case 6660:   // Utility:Force batch
    case 7168:   // Identity:Add registrar
    case 7171:   // Identity:Clear identity
    case 7172:   // Identity:Request judgement
    case 7173:   // Identity:Cancel request
    case 7174:   // Identity:Set fee
    case 7175:   // Identity:Set account id
    case 7178:   // Identity:Kill identity
    case 7181:   // Identity:Remove sub
    case 7182:   // Identity:Quit sub
    case 7425:   // Proxy:Add proxy
    case 7426:   // Proxy:Remove proxy
    case 7427:   // Proxy:Remove proxies
    case 7428:   // Proxy:Create pure
    case 7429:   // Proxy:Kill pure
    case 7433:   // Proxy:Proxy announced
    case 8704:   // Bounties:Propose bounty
    case 8705:   // Bounties:Approve bounty
    case 8706:   // Bounties:Propose curator
    case 8707:   // Bounties:Unassign curator
    case 8708:   // Bounties:Accept curator
    case 8709:   // Bounties:Award bounty
    case 8710:   // Bounties:Claim bounty
    case 8711:   // Bounties:Close bounty
    case 8712:   // Bounties:Extend bounty expiry
    case 9728:   // ChildBounties:Add child bounty
    case 9729:   // ChildBounties:Propose curator
    case 9730:   // ChildBounties:Accept curator
    case 9731:   // ChildBounties:Unassign curator
    case 9732:   // ChildBounties:Award child bounty
    case 9733:   // ChildBounties:Claim child bounty
    case 9734:   // ChildBounties:Close child bounty
    case 8960:   // Tips:Report awesome
    case 8961:   // Tips:Retract tip
    case 8962:   // Tips:Tip new
    case 8963:   // Tips:Tip
    case 8964:   // Tips:Close tip
    case 8965:   // Tips:Slash tip
    case 9220:   // ElectionProviderMultiPhase:Governance fallback
    case 9472:   // VoterList:Rebag
    case 9473:   // VoterList:Put in front of
    case 9984:   // NominationPools:Join
    case 9985:   // NominationPools:Bond extra
    case 9986:   // NominationPools:Claim payout
    case 9987:   // NominationPools:Unbond
    case 9988:   // NominationPools:Pool withdraw unbonded
    case 9989:   // NominationPools:Withdraw Unbonded
    case 9990:   // NominationPools:Create
    case 9991:   // NominationPools:Nominate
    case 9992:   // NominationPools:Set state
    case 9993:   // NominationPools:Set metadata
    case 9994:   // NominationPools:Set configs
    case 9995:   // NominationPools:Update roles
    case 9996:   // NominationPools:Chill
    case 10240:  // FastUnstake:Register fast unstake
    case 10241:  // FastUnstake:Deregister
    case 10242:  // FastUnstake:Control
    case 13056:  // Configuration:Set validation upgrade cooldown
    case 13057:  // Configuration:Set validation upgrade delay
    case 13058:  // Configuration:Set code retention period
    case 13059:  // Configuration:Set max code size
    case 13060:  // Configuration:Set max pov size
    case 13061:  // Configuration:Set max head data size
    case 13062:  // Configuration:Set parathread cores
    case 13063:  // Configuration:Set parathread retries
    case 13064:  // Configuration:Set group rotation frequency
    case 13065:  // Configuration:Set chain availability period
    case 13066:  // Configuration:Set thread availability period
    case 13067:  // Configuration:Set scheduling lookahead
    case 13068:  // Configuration:Set max validators per core
    case 13069:  // Configuration:Set max validators
    case 13071:  // Configuration:Set dispute post conclusion acceptance period
    case 13072:  // Configuration:Set dispute max spam slots
    case 13073:  // Configuration:Set dispute conclusion by time out period
    case 13074:  // Configuration:Set no show slots
    case 13075:  // Configuration:Set n delay tranches
    case 13076:  // Configuration:Set zeroth delay tranche width
    case 13077:  // Configuration:Set needed approvals
    case 13078:  // Configuration:Set relay vrf modulo samples
    case 13079:  // Configuration:Set max upward queue count
    case 13080:  // Configuration:Set max upward queue size
    case 13081:  // Configuration:Set max downward message size
    case 13082:  // Configuration:Set ump service total weight
    case 13083:  // Configuration:Set max upward message size
    case 13084:  // Configuration:Set max upward message num per candidate
    case 13085:  // Configuration:Set hrmp open request ttl
    case 13086:  // Configuration:Set hrmp sender deposit
    case 13087:  // Configuration:Set hrmp recipient deposit
    case 13088:  // Configuration:Set hrmp channel max capacity
    case 13089:  // Configuration:Set hrmp channel max total size
    case 13090:  // Configuration:Set hrmp max parachain inbound channels
    case 13091:  // Configuration:Set hrmp max parathread inbound channels
    case 13092:  // Configuration:Set hrmp channel max message size
    case 13093:  // Configuration:Set hrmp max parachain outbound channels
    case 13094:  // Configuration:Set hrmp max parathread outbound channels
    case 13095:  // Configuration:Set hrmp max message num per candidate
    case 13096:  // Configuration:Set ump max individual weight
    case 13097:  // Configuration:Set pvf checking enabled
    case 13098:  // Configuration:Set pvf voting ttl
    case 13099:  // Configuration:Set minimum validation upgrade delay
    case 13100:  // Configuration:Set bypass consistency check
    case 14592:  // Initializer:Force approve
    case 15104:  // Ump:Service overweight
    case 15364:  // Hrmp:Force process hrmp open
    case 15365:  // Hrmp:Force process hrmp close
    case 15872:  // ParasDisputes:Force unfreeze
    case 17925:  // Registrar:Reserve
    case 18432:  // Auctions:New auction
    case 18433:  // Auctions:Bid
    case 18434:  // Auctions:Cancel auction
    case 18688:  // Crowdloan:Create
    case 18689:  // Crowdloan:Contribute
    case 18690:  // Crowdloan:Withdraw
    case 18691:  // Crowdloan:Refund
    case 18692:  // Crowdloan:Dissolve
    case 18693:  // Crowdloan:Edit
    case 18694:  // Crowdloan:Add memo
    case 18695:  // Crowdloan:Poke
    case 18696:  // Crowdloan:Contribute all
      return false;
    default:
      return true;
  }
}
