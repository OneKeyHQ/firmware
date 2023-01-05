#ifndef __POLKADOT_SUBSTRATE_FUNCTIONS_V15_H__
#define __POLKADOT_SUBSTRATE_FUNCTIONS_V15_H__

#include <stddef.h>
#include <stdint.h>
#include "../parser_common.h"
#include "substrate_methods_V15.h"
#include "substrate_types_V15.h"

// Read functions
parser_error_t _readAccountIdLookupOfT_V15(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V15_t* v);
parser_error_t _readAccountId_V15(parser_context_t* c, pd_AccountId_V15_t* v);
parser_error_t _readAccountIndex_V15(parser_context_t* c,
                                     pd_AccountIndex_V15_t* v);
parser_error_t _readAccountVoteSplit_V15(parser_context_t* c,
                                         pd_AccountVoteSplit_V15_t* v);
parser_error_t _readAccountVoteStandard_V15(parser_context_t* c,
                                            pd_AccountVoteStandard_V15_t* v);
parser_error_t _readAccountVote_V15(parser_context_t* c,
                                    pd_AccountVote_V15_t* v);
parser_error_t _readBondExtraBalanceOfT_V15(parser_context_t* c,
                                            pd_BondExtraBalanceOfT_V15_t* v);
parser_error_t _readCompactAccountIndex_V15(parser_context_t* c,
                                            pd_CompactAccountIndex_V15_t* v);
parser_error_t _readCompactPerBill_V15(parser_context_t* c,
                                       pd_CompactPerBill_V15_t* v);
parser_error_t _readConfigOpAccountId_V15(parser_context_t* c,
                                          pd_ConfigOpAccountId_V15_t* v);
parser_error_t _readConfigOpBalanceOfT_V15(parser_context_t* c,
                                           pd_ConfigOpBalanceOfT_V15_t* v);
parser_error_t _readConfigOpu32_V15(parser_context_t* c,
                                    pd_ConfigOpu32_V15_t* v);
parser_error_t _readConviction_V15(parser_context_t* c, pd_Conviction_V15_t* v);
parser_error_t _readEcdsaPublic_V15(parser_context_t* c,
                                    pd_EcdsaPublic_V15_t* v);
parser_error_t _readEcdsaSignature_V15(parser_context_t* c,
                                       pd_EcdsaSignature_V15_t* v);
parser_error_t _readEd25519Public_V15(parser_context_t* c,
                                      pd_Ed25519Public_V15_t* v);
parser_error_t _readEd25519Signature_V15(parser_context_t* c,
                                         pd_Ed25519Signature_V15_t* v);
parser_error_t _readEraIndex_V15(parser_context_t* c, pd_EraIndex_V15_t* v);
parser_error_t _readEthereumAddress_V15(parser_context_t* c,
                                        pd_EthereumAddress_V15_t* v);
parser_error_t _readKeys_V15(parser_context_t* c, pd_Keys_V15_t* v);
parser_error_t _readMemberCount_V15(parser_context_t* c,
                                    pd_MemberCount_V15_t* v);
parser_error_t _readMultiSignature_V15(parser_context_t* c,
                                       pd_MultiSignature_V15_t* v);
parser_error_t _readMultiSigner_V15(parser_context_t* c,
                                    pd_MultiSigner_V15_t* v);
parser_error_t _readOpaqueCall_V15(parser_context_t* c, pd_OpaqueCall_V15_t* v);
parser_error_t _readOptionAccountId_V15(parser_context_t* c,
                                        pd_OptionAccountId_V15_t* v);
parser_error_t _readOptionMultiSignature_V15(parser_context_t* c,
                                             pd_OptionMultiSignature_V15_t* v);
parser_error_t _readOptionMultiSigner_V15(parser_context_t* c,
                                          pd_OptionMultiSigner_V15_t* v);
parser_error_t _readOptionProxyType_V15(parser_context_t* c,
                                        pd_OptionProxyType_V15_t* v);
parser_error_t _readOptionTimepoint_V15(parser_context_t* c,
                                        pd_OptionTimepoint_V15_t* v);
parser_error_t _readOverweightIndex_V15(parser_context_t* c,
                                        pd_OverweightIndex_V15_t* v);
parser_error_t _readParaId_V15(parser_context_t* c, pd_ParaId_V15_t* v);
parser_error_t _readPerbill_V15(parser_context_t* c, pd_Perbill_V15_t* v);
parser_error_t _readPoolId_V15(parser_context_t* c, pd_PoolId_V15_t* v);
parser_error_t _readPoolState_V15(parser_context_t* c, pd_PoolState_V15_t* v);
parser_error_t _readProxyType_V15(parser_context_t* c, pd_ProxyType_V15_t* v);
parser_error_t _readReferendumIndex_V15(parser_context_t* c,
                                        pd_ReferendumIndex_V15_t* v);
parser_error_t _readRegistrarIndex_V15(parser_context_t* c,
                                       pd_RegistrarIndex_V15_t* v);
parser_error_t _readRewardDestination_V15(parser_context_t* c,
                                          pd_RewardDestination_V15_t* v);
parser_error_t _readSessionIndex_V15(parser_context_t* c,
                                     pd_SessionIndex_V15_t* v);
parser_error_t _readSr25519Public_V15(parser_context_t* c,
                                      pd_Sr25519Public_V15_t* v);
parser_error_t _readSr25519Signature_V15(parser_context_t* c,
                                         pd_Sr25519Signature_V15_t* v);
parser_error_t _readTimepoint_V15(parser_context_t* c, pd_Timepoint_V15_t* v);
parser_error_t _readValidatorPrefs_V15(parser_context_t* c,
                                       pd_ValidatorPrefs_V15_t* v);
parser_error_t _readVecAccountIdLookupOfT_V15(
    parser_context_t* c, pd_VecAccountIdLookupOfT_V15_t* v);
parser_error_t _readVecAccountId_V15(parser_context_t* c,
                                     pd_VecAccountId_V15_t* v);
parser_error_t _readVestingInfo_V15(parser_context_t* c,
                                    pd_VestingInfo_V15_t* v);
parser_error_t _readVote_V15(parser_context_t* c, pd_Vote_V15_t* v);
parser_error_t _readWeight_V15(parser_context_t* c, pd_Weight_V15_t* v);

// toString functions
parser_error_t _toStringAccountIdLookupOfT_V15(
    const pd_AccountIdLookupOfT_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountId_V15(const pd_AccountId_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIndex_V15(const pd_AccountIndex_V15_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountVoteSplit_V15(const pd_AccountVoteSplit_V15_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringAccountVoteStandard_V15(
    const pd_AccountVoteStandard_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountVote_V15(const pd_AccountVote_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBondExtraBalanceOfT_V15(
    const pd_BondExtraBalanceOfT_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringCompactAccountIndex_V15(
    const pd_CompactAccountIndex_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringCompactPerBill_V15(const pd_CompactPerBill_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpAccountId_V15(
    const pd_ConfigOpAccountId_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpBalanceOfT_V15(
    const pd_ConfigOpBalanceOfT_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpu32_V15(const pd_ConfigOpu32_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConviction_V15(const pd_Conviction_V15_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t* pageCount);

parser_error_t _toStringEcdsaPublic_V15(const pd_EcdsaPublic_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEcdsaSignature_V15(const pd_EcdsaSignature_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEd25519Public_V15(const pd_Ed25519Public_V15_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEd25519Signature_V15(const pd_Ed25519Signature_V15_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringEraIndex_V15(const pd_EraIndex_V15_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringEthereumAddress_V15(const pd_EthereumAddress_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringKeys_V15(const pd_Keys_V15_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t pageIdx,
                                 uint8_t* pageCount);

parser_error_t _toStringMemberCount_V15(const pd_MemberCount_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiSignature_V15(const pd_MultiSignature_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiSigner_V15(const pd_MultiSigner_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOpaqueCall_V15(const pd_OpaqueCall_V15_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionAccountId_V15(const pd_OptionAccountId_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringOptionMultiSignature_V15(
    const pd_OptionMultiSignature_V15_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionMultiSigner_V15(
    const pd_OptionMultiSigner_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionProxyType_V15(const pd_OptionProxyType_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t* pageCount);

parser_error_t _toStringOptionTimepoint_V15(const pd_OptionTimepoint_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringOverweightIndex_V15(const pd_OverweightIndex_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringParaId_V15(const pd_ParaId_V15_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringPerbill_V15(const pd_Perbill_V15_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringPoolId_V15(const pd_PoolId_V15_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringPoolState_V15(const pd_PoolState_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t* pageCount);

parser_error_t _toStringProxyType_V15(const pd_ProxyType_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t* pageCount);

parser_error_t _toStringReferendumIndex_V15(const pd_ReferendumIndex_V15_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringRegistrarIndex_V15(const pd_RegistrarIndex_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringRewardDestination_V15(
    const pd_RewardDestination_V15_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSessionIndex_V15(const pd_SessionIndex_V15_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSr25519Public_V15(const pd_Sr25519Public_V15_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSr25519Signature_V15(const pd_Sr25519Signature_V15_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringTimepoint_V15(const pd_Timepoint_V15_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringValidatorPrefs_V15(const pd_ValidatorPrefs_V15_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecAccountIdLookupOfT_V15(
    const pd_VecAccountIdLookupOfT_V15_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecAccountId_V15(const pd_VecAccountId_V15_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVestingInfo_V15(const pd_VestingInfo_V15_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVote_V15(const pd_Vote_V15_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t* pageCount);

parser_error_t _toStringWeight_V15(const pd_Weight_V15_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

#endif
