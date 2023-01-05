#include <stddef.h>
#include <stdint.h>
#include "substrate_coin.h"
#include "substrate_dispatch_V15.h"

#include "../bignum.h"
#include "../common_defs.h"
#include "../parser_impl.h"
#include "util.h"

parser_error_t _readAccountIdLookupOfT_V15(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))

  switch (v->value) {
    case 0:  // Id
      CHECK_ERROR(_readAccountId_V15(c, &v->id))
      break;
    case 1:  // Index
      CHECK_ERROR(_readCompactAccountIndex_V15(c, &v->index))
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

parser_error_t _readAccountId_V15(parser_context_t* c,
                                  pd_AccountId_V15_t* v){GEN_DEF_READARRAY(32)}

parser_error_t
    _readAccountIndex_V15(parser_context_t* c, pd_AccountIndex_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readAccountVoteSplit_V15(parser_context_t* c,
                                         pd_AccountVoteSplit_V15_t* v) {
  CHECK_ERROR(_readBalanceOf(c, &v->aye));
  CHECK_ERROR(_readBalanceOf(c, &v->nay));
  return parser_ok;
}

parser_error_t _readAccountVoteStandard_V15(parser_context_t* c,
                                            pd_AccountVoteStandard_V15_t* v) {
  CHECK_ERROR(_readVote_V15(c, &v->vote));
  CHECK_ERROR(_readBalanceOf(c, &v->balance));
  return parser_ok;
}

parser_error_t _readAccountVote_V15(parser_context_t* c,
                                    pd_AccountVote_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))

  switch (v->value) {
    case 0:
      CHECK_ERROR(_readAccountVoteStandard_V15(c, &v->voteStandard))
      break;
    case 1:
      CHECK_ERROR(_readAccountVoteSplit_V15(c, &v->voteSplit))
      break;
    default:
      break;
  }

  return parser_ok;
}

parser_error_t _readBondExtraBalanceOfT_V15(parser_context_t* c,
                                            pd_BondExtraBalanceOfT_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:
      CHECK_ERROR(_readBalance(c, &v->freeBalance))
      break;
    case 1:  // Rewards
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readCompactAccountIndex_V15(parser_context_t* c,
                                            pd_CompactAccountIndex_V15_t* v) {
  return _readCompactInt(c, &v->value);
}

parser_error_t _readCompactPerBill_V15(parser_context_t* c,
                                       pd_CompactPerBill_V15_t* v) {
  return _readCompactInt(c, &v->value);
}

parser_error_t _readConfigOpAccountId_V15(parser_context_t* c,
                                          pd_ConfigOpAccountId_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Noop
    case 2:  // Remove
      break;
    case 1:
      CHECK_ERROR(_readAccountId_V15(c, &v->set))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readConfigOpBalanceOfT_V15(parser_context_t* c,
                                           pd_ConfigOpBalanceOfT_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Noop
    case 2:  // Remove
      break;
    case 1:
      CHECK_ERROR(_readBalance(c, &v->set))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readConfigOpu32_V15(parser_context_t* c,
                                    pd_ConfigOpu32_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Noop
    case 2:  // Remove
      break;
    case 1:
      CHECK_ERROR(_preadUInt32(c, &v->set))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readConviction_V15(parser_context_t* c,
                                   pd_Conviction_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  if (v->value > 5) {
    return parser_value_out_of_range;
  }
  return parser_ok;
}

parser_error_t _readEcdsaPublic_V15(parser_context_t* c,
                                    pd_EcdsaPublic_V15_t* v){
    GEN_DEF_READARRAY(33)}

parser_error_t
    _readEcdsaSignature_V15(parser_context_t* c,
                            pd_EcdsaSignature_V15_t* v){GEN_DEF_READARRAY(65)}

parser_error_t
    _readEd25519Public_V15(parser_context_t* c,
                           pd_Ed25519Public_V15_t* v){GEN_DEF_READARRAY(32)}

parser_error_t _readEd25519Signature_V15(parser_context_t* c,
                                         pd_Ed25519Signature_V15_t* v){
    GEN_DEF_READARRAY(64)}

parser_error_t _readEraIndex_V15(parser_context_t* c, pd_EraIndex_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readEthereumAddress_V15(parser_context_t* c,
                                        pd_EthereumAddress_V15_t* v){
    GEN_DEF_READARRAY(20)}

parser_error_t _readKeys_V15(parser_context_t* c,
                             pd_Keys_V15_t* v){GEN_DEF_READARRAY(6 * 32)}

parser_error_t
    _readMemberCount_V15(parser_context_t* c, pd_MemberCount_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readMultiSignature_V15(parser_context_t* c,
                                       pd_MultiSignature_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_readEd25519Signature_V15(c, &v->ed25519))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_readSr25519Signature_V15(c, &v->sr25519))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_readEcdsaSignature_V15(c, &v->ecdsa))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readMultiSigner_V15(parser_context_t* c,
                                    pd_MultiSigner_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_readEd25519Public_V15(c, &v->ed25519))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_readSr25519Public_V15(c, &v->sr25519))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_readEcdsaPublic_V15(c, &v->ecdsa))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readOpaqueCall_V15(parser_context_t* c,
                                   pd_OpaqueCall_V15_t* v) {
  // Encoded as Byte[], array size comes first
  uint8_t size;
  CHECK_ERROR(_preadUInt8(c, &size))
  return _readCall(c, &v->call);
}

parser_error_t _readOverweightIndex_V15(parser_context_t* c,
                                        pd_OverweightIndex_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt64(c, &v->value))
  return parser_ok;
}

parser_error_t _readParaId_V15(parser_context_t* c, pd_ParaId_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readPerbill_V15(parser_context_t* c, pd_Perbill_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readPoolId_V15(parser_context_t* c, pd_PoolId_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readPoolState_V15(parser_context_t* c, pd_PoolState_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  return parser_ok;
}

parser_error_t _readProxyType_V15(parser_context_t* c, pd_ProxyType_V15_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->value))
  if (v->value == 4 || v->value > 7) {
    return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readReferendumIndex_V15(parser_context_t* c,
                                        pd_ReferendumIndex_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readRegistrarIndex_V15(parser_context_t* c,
                                       pd_RegistrarIndex_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readRewardDestination_V15(parser_context_t* c,
                                          pd_RewardDestination_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))

  if (v->value == 3) {
    CHECK_ERROR(_readAccountId_V15(c, &v->accountId))
  } else if (v->value > 4) {
    return parser_value_out_of_range;
  }
  return parser_ok;
}

parser_error_t _readSessionIndex_V15(parser_context_t* c,
                                     pd_SessionIndex_V15_t* v) {
  return _preadUInt32(c, &v->value);
}

parser_error_t _readSr25519Public_V15(parser_context_t* c,
                                      pd_Sr25519Public_V15_t* v){
    GEN_DEF_READARRAY(32)}

parser_error_t _readSr25519Signature_V15(parser_context_t* c,
                                         pd_Sr25519Signature_V15_t* v){
    GEN_DEF_READARRAY(64)}

parser_error_t _readTimepoint_V15(parser_context_t* c, pd_Timepoint_V15_t* v) {
  CHECK_ERROR(_readBlockNumber(c, &v->height))
  CHECK_ERROR(_readu32(c, &v->index))
  return parser_ok;
}

parser_error_t _readValidatorPrefs_V15(parser_context_t* c,
                                       pd_ValidatorPrefs_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readCompactPerBill_V15(c, &v->commission));
  CHECK_ERROR(_readbool(c, &v->blocked))
  return parser_ok;
}

parser_error_t _readVestingInfo_V15(parser_context_t* c,
                                    pd_VestingInfo_V15_t* v) {
  CHECK_ERROR(_readBalanceOf(c, &v->locked))
  CHECK_ERROR(_readBalanceOf(c, &v->per_block))
  CHECK_ERROR(_readBlockNumber(c, &v->starting_block))
  return parser_ok;
}

parser_error_t _readVote_V15(parser_context_t* c, pd_Vote_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  const uint8_t aye = v->value & 0xF0;
  const uint8_t conviction = v->value & 0x0F;
  if ((aye == 0x80 || aye == 0x00) && conviction <= 0x06) {
    return parser_ok;
  }
  return parser_value_out_of_range;
}

parser_error_t _readWeight_V15(parser_context_t* c, pd_Weight_V15_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt64(c, &v->value))
  return parser_ok;
}

parser_error_t _readVecAccountIdLookupOfT_V15(
    parser_context_t* c, pd_VecAccountIdLookupOfT_V15_t* v){
    GEN_DEF_READVECTOR(AccountIdLookupOfT_V15)}

parser_error_t
    _readVecAccountId_V15(parser_context_t* c, pd_VecAccountId_V15_t* v){
        GEN_DEF_READVECTOR(AccountId_V15)}

parser_error_t
    _readOptionAccountId_V15(parser_context_t* c, pd_OptionAccountId_V15_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readAccountId_V15(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionMultiSignature_V15(parser_context_t* c,
                                             pd_OptionMultiSignature_V15_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readMultiSignature_V15(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionMultiSigner_V15(parser_context_t* c,
                                          pd_OptionMultiSigner_V15_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readMultiSigner_V15(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionProxyType_V15(parser_context_t* c,
                                        pd_OptionProxyType_V15_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readProxyType_V15(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionTimepoint_V15(parser_context_t* c,
                                        pd_OptionTimepoint_V15_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readTimepoint_V15(c, &v->contained))
  }
  return parser_ok;
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

parser_error_t _toStringAccountIdLookupOfT_V15(
    const pd_AccountIdLookupOfT_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Id
      CHECK_ERROR(_toStringAccountId_V15(&v->id, outValue, outValueLen, pageIdx,
                                         pageCount))
      break;
    case 1:  // Index
      CHECK_ERROR(_toStringCompactAccountIndex_V15(
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

parser_error_t _toStringAccountId_V15(const pd_AccountId_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringPubkeyAsAddress(v->_ptr, outValue, outValueLen, pageIdx,
                                  pageCount);
}

parser_error_t _toStringAccountIndex_V15(const pd_AccountIndex_V15_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringAccountVoteSplit_V15(const pd_AccountVoteSplit_V15_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  // First measure number of pages
  uint8_t pages[3];

  pages[0] = 1;
  CHECK_ERROR(_substrate_toStringBalanceOf(&v->aye, outValue, outValueLen, 0,
                                           &pages[1]))
  CHECK_ERROR(_substrate_toStringBalanceOf(&v->nay, outValue, outValueLen, 0,
                                           &pages[2]));

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx < pages[0]) {
    snprintf(outValue, outValueLen, "Split");
    return parser_ok;
  }
  pageIdx -= pages[0];

  /////////
  /////////

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_substrate_toStringBalanceOf(&v->aye, outValue, outValueLen,
                                             pageIdx, &pages[1]));
    return parser_ok;
  }
  pageIdx -= pages[1];

  /////////
  /////////

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_substrate_toStringBalanceOf(&v->nay, outValue, outValueLen,
                                             pageIdx, &pages[2]));
    return parser_ok;
  }

  /////////
  /////////

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringAccountVoteStandard_V15(
    const pd_AccountVoteStandard_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  // First measure number of pages
  uint8_t pages[3];

  pages[0] = 1;
  CHECK_ERROR(_toStringVote_V15(&v->vote, outValue, outValueLen, &pages[1]))
  CHECK_ERROR(_substrate_toStringBalanceOf(&v->balance, outValue, outValueLen,
                                           0, &pages[2]));

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    snprintf(outValue, outValueLen, "Standard");
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringVote_V15(&v->vote, outValue, outValueLen, &pages[1]));
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_substrate_toStringBalanceOf(&v->balance, outValue, outValueLen,
                                             pageIdx, &pages[2]));
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringAccountVote_V15(const pd_AccountVote_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:
      CHECK_ERROR(_toStringAccountVoteStandard_V15(
          &v->voteStandard, outValue, outValueLen, pageIdx, pageCount));
      break;
    case 1:
      CHECK_ERROR(_toStringAccountVoteSplit_V15(
          &v->voteSplit, outValue, outValueLen, pageIdx, pageCount));
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _toStringBondExtraBalanceOfT_V15(
    const pd_BondExtraBalanceOfT_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      CHECK_ERROR(_substrate_toStringBalance(&v->freeBalance, outValue,
                                             outValueLen, pageIdx, pageCount))
      break;
    case 1:
      snprintf(outValue, outValueLen, "Rewards");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringCompactAccountIndex_V15(
    const pd_CompactAccountIndex_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringCompactInt(&v->value, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _toStringCompactPerBill_V15(const pd_CompactPerBill_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  // 9 but shift 2 to show as percentage
  return _toStringCompactInt(&v->value, 7, false, "%", "", outValue,
                             outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringConfigOpAccountId_V15(
    const pd_ConfigOpAccountId_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Noop");
      break;
    case 1:
      CHECK_ERROR(_toStringAccountId_V15(&v->set, outValue, outValueLen,
                                         pageIdx, pageCount))
      break;
    case 2:
      snprintf(outValue, outValueLen, "Remove");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringConfigOpBalanceOfT_V15(
    const pd_ConfigOpBalanceOfT_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Noop");
      break;
    case 1:
      CHECK_ERROR(_substrate_toStringBalance(&v->set, outValue, outValueLen,
                                             pageIdx, pageCount))
      break;
    case 2:
      snprintf(outValue, outValueLen, "Remove");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringConfigOpu32_V15(const pd_ConfigOpu32_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Noop");
      break;
    case 1:
      CHECK_ERROR(
          _toStringu32(&v->set, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 2:
      snprintf(outValue, outValueLen, "Remove");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringConviction_V15(const pd_Conviction_V15_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "None");
      break;
    case 1:
      snprintf(outValue, outValueLen, "Locked1x");
      break;
    case 2:
      snprintf(outValue, outValueLen, "Locked2x");
      break;
    case 3:
      snprintf(outValue, outValueLen, "Locked3x");
      break;
    case 4:
      snprintf(outValue, outValueLen, "Locked4x");
      break;
    case 5:
      snprintf(outValue, outValueLen, "Locked5x");
      break;
    default:
      return parser_print_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringEcdsaPublic_V15(const pd_EcdsaPublic_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(33)}

parser_error_t
    _toStringEcdsaSignature_V15(const pd_EcdsaSignature_V15_t* v,
                                char* outValue, uint16_t outValueLen,
                                uint8_t pageIdx,
                                uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(65)}

parser_error_t
    _toStringEd25519Public_V15(const pd_Ed25519Public_V15_t* v, char* outValue,
                               uint16_t outValueLen, uint8_t pageIdx,
                               uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(32)}

parser_error_t
    _toStringEd25519Signature_V15(const pd_Ed25519Signature_V15_t* v,
                                  char* outValue, uint16_t outValueLen,
                                  uint8_t pageIdx, uint8_t* pageCount){
        GEN_DEF_TOSTRING_ARRAY(64)}

parser_error_t _toStringEraIndex_V15(const pd_EraIndex_V15_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringEthereumAddress_V15(
    const pd_EthereumAddress_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(20)}

parser_error_t
    _toStringKeys_V15(const pd_Keys_V15_t* v, char* outValue,
                      uint16_t outValueLen, uint8_t pageIdx,
                      uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(6 * 32)}

parser_error_t _toStringMemberCount_V15(const pd_MemberCount_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringMultiSignature_V15(const pd_MultiSignature_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_toStringEd25519Signature_V15(
          &v->ed25519, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_toStringSr25519Signature_V15(
          &v->sr25519, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_toStringEcdsaSignature_V15(&v->ecdsa, outValue, outValueLen,
                                              pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringMultiSigner_V15(const pd_MultiSigner_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_toStringEd25519Public_V15(&v->ed25519, outValue, outValueLen,
                                             pageIdx, pageCount))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_toStringSr25519Public_V15(&v->sr25519, outValue, outValueLen,
                                             pageIdx, pageCount))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_toStringEcdsaPublic_V15(&v->ecdsa, outValue, outValueLen,
                                           pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringOpaqueCall_V15(const pd_OpaqueCall_V15_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringCall(&v->call, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringOverweightIndex_V15(const pd_OverweightIndex_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  return _toStringu64(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringParaId_V15(const pd_ParaId_V15_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringPerbill_V15(const pd_Perbill_V15_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  char bufferUI[100];
  char ratioBuffer[80];
  memset(outValue, 0, outValueLen);
  memset(ratioBuffer, 0, sizeof(ratioBuffer));
  memset(bufferUI, 0, sizeof(bufferUI));
  *pageCount = 1;

  if (fpuint64_to_str(ratioBuffer, sizeof(ratioBuffer), v->value, 7) == 0) {
    return parser_unexpected_value;
  }

  snprintf(bufferUI, sizeof(bufferUI), "%s%%", ratioBuffer);
  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _toStringPoolId_V15(const pd_PoolId_V15_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringPoolState_V15(const pd_PoolState_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Open");
      break;
    case 1:
      snprintf(outValue, outValueLen, "Blocked");
      break;
    case 2:
      snprintf(outValue, outValueLen, "Destroying");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringProxyType_V15(const pd_ProxyType_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Any");
      break;
    case 1:
      snprintf(outValue, outValueLen, "NonTransfer");
      break;
    case 2:
      snprintf(outValue, outValueLen, "Governance");
      break;
    case 3:
      snprintf(outValue, outValueLen, "Staking");
      break;
    case 5:
      snprintf(outValue, outValueLen, "IdentityJudgement");
      break;
    case 6:
      snprintf(outValue, outValueLen, "CancelProxy");
      break;
    case 7:
      snprintf(outValue, outValueLen, "Auction");
      break;
    default:
      return parser_print_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringReferendumIndex_V15(const pd_ReferendumIndex_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringRegistrarIndex_V15(const pd_RegistrarIndex_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringRewardDestination_V15(
    const pd_RewardDestination_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Staked");
      break;
    case 1:
      snprintf(outValue, outValueLen, "Stash");
      break;
    case 2:
      snprintf(outValue, outValueLen, "Controller");
      break;
    case 3:
      CHECK_ERROR(_toStringAccountId_V15(&v->accountId, outValue, outValueLen,
                                         pageIdx, pageCount));
      break;
    case 4:
      snprintf(outValue, outValueLen, "None");
      break;
    default:
      return parser_print_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringSessionIndex_V15(const pd_SessionIndex_V15_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringSr25519Public_V15(const pd_Sr25519Public_V15_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(32)}

parser_error_t
    _toStringSr25519Signature_V15(const pd_Sr25519Signature_V15_t* v,
                                  char* outValue, uint16_t outValueLen,
                                  uint8_t pageIdx, uint8_t* pageCount){
        GEN_DEF_TOSTRING_ARRAY(64)}

parser_error_t _toStringTimepoint_V15(const pd_Timepoint_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(
      _toStringBlockNumber(&v->height, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(_toStringu32(&v->index, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringBlockNumber(&v->height, outValue, outValueLen, pageIdx,
                                     &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(
        _toStringu32(&v->index, outValue, outValueLen, pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringValidatorPrefs_V15(const pd_ValidatorPrefs_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringCompactPerBill_V15(&v->commission, outValue, outValueLen,
                                          0, &pages[0]))
  CHECK_ERROR(_toStringbool(&v->blocked, outValue, outValueLen, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringCompactPerBill_V15(&v->commission, outValue,
                                            outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringbool(&v->blocked, outValue, outValueLen, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringVestingInfo_V15(const pd_VestingInfo_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[3] = {0};
  CHECK_ERROR(_substrate_toStringBalanceOf(&v->locked, outValue, outValueLen, 0,
                                           &pages[0]))
  CHECK_ERROR(_substrate_toStringBalanceOf(&v->per_block, outValue, outValueLen,
                                           0, &pages[1]))
  CHECK_ERROR(_toStringBlockNumber(&v->starting_block, outValue, outValueLen, 0,
                                   &pages[2]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_substrate_toStringBalanceOf(&v->locked, outValue, outValueLen,
                                             pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_substrate_toStringBalanceOf(&v->per_block, outValue,
                                             outValueLen, pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringBlockNumber(&v->starting_block, outValue, outValueLen,
                                     pageIdx, &pages[2]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringVote_V15(const pd_Vote_V15_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  const uint8_t conviction = v->value & 0x0F;

  switch (v->value & 0xF0) {
    case 0x80:
      snprintf(outValue, outValueLen, "Aye - ");
      break;
    case 0x00:
      snprintf(outValue, outValueLen, "Nay - ");
      break;
    default:
      return parser_unexpected_value;
  }

  switch (conviction) {
    case 0:
      snprintf(outValue + 6, outValueLen - 6, "None");
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      snprintf(outValue + 6, outValueLen - 6, "Locked%dx", conviction);
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringWeight_V15(const pd_Weight_V15_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringu64(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringVecAccountIdLookupOfT_V15(
    const pd_VecAccountIdLookupOfT_V15_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(AccountIdLookupOfT_V15);
}

parser_error_t _toStringVecAccountId_V15(const pd_VecAccountId_V15_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(AccountId_V15);
}

parser_error_t _toStringOptionAccountId_V15(const pd_OptionAccountId_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringAccountId_V15(&v->contained, outValue, outValueLen,
                                       pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionMultiSignature_V15(
    const pd_OptionMultiSignature_V15_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringMultiSignature_V15(&v->contained, outValue,
                                            outValueLen, pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionMultiSigner_V15(
    const pd_OptionMultiSigner_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringMultiSigner_V15(&v->contained, outValue, outValueLen,
                                         pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionProxyType_V15(const pd_OptionProxyType_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringProxyType_V15(&v->contained, outValue, outValueLen,
                                       pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionTimepoint_V15(const pd_OptionTimepoint_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringTimepoint_V15(&v->contained, outValue, outValueLen,
                                       pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}
