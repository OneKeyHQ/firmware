#ifndef __POLKADOT_SUBSTRATE_FUNCTIONS_V18_H__
#define __POLKADOT_SUBSTRATE_FUNCTIONS_V18_H__

#include <stddef.h>
#include <stdint.h>
#include "../parser_common.h"
#include "substrate_methods_V18.h"
#include "substrate_types_V18.h"

// Read functions
parser_error_t _readAbstractFungible_V18(parser_context_t* c,
                                         pd_AbstractFungible_V18_t* v);
parser_error_t _readAbstractNonFungible_V18(parser_context_t* c,
                                            pd_AbstractNonFungible_V18_t* v);
parser_error_t _readAccountId32_V18(parser_context_t* c,
                                    pd_AccountId32_V18_t* v);
parser_error_t _readAccountIdLookupOfT_V18(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V18_t* v);
parser_error_t _readAccountId_V18(parser_context_t* c, pd_AccountId_V18_t* v);
parser_error_t _readAccountIndex64_V18(parser_context_t* c,
                                       pd_AccountIndex64_V18_t* v);
parser_error_t _readAccountIndex_V18(parser_context_t* c,
                                     pd_AccountIndex_V18_t* v);
parser_error_t _readAccountKey20_V18(parser_context_t* c,
                                     pd_AccountKey20_V18_t* v);
parser_error_t _readAccountVoteSplit_V18(parser_context_t* c,
                                         pd_AccountVoteSplit_V18_t* v);
parser_error_t _readAccountVoteStandard_V18(parser_context_t* c,
                                            pd_AccountVoteStandard_V18_t* v);
parser_error_t _readAccountVote_V18(parser_context_t* c,
                                    pd_AccountVote_V18_t* v);
parser_error_t _readAssetInstance_V18(parser_context_t* c,
                                      pd_AssetInstance_V18_t* v);
parser_error_t _readBodyId_V18(parser_context_t* c, pd_BodyId_V18_t* v);
parser_error_t _readBodyPart_V18(parser_context_t* c, pd_BodyPart_V18_t* v);
parser_error_t _readBondExtraBalanceOfT_V18(parser_context_t* c,
                                            pd_BondExtraBalanceOfT_V18_t* v);
parser_error_t _readBoundedCallOfT_V18(parser_context_t* c,
                                       pd_BoundedCallOfT_V18_t* v);
parser_error_t _readBoxVersionedMultiAssets_V18(
    parser_context_t* c, pd_BoxVersionedMultiAssets_V18_t* v);
parser_error_t _readBoxVersionedMultiLocation_V18(
    parser_context_t* c, pd_BoxVersionedMultiLocation_V18_t* v);
parser_error_t _readCompactAccountIndex_V18(parser_context_t* c,
                                            pd_CompactAccountIndex_V18_t* v);
parser_error_t _readCompactPerBill_V18(parser_context_t* c,
                                       pd_CompactPerBill_V18_t* v);
parser_error_t _readConcreteFungible_V18(parser_context_t* c,
                                         pd_ConcreteFungible_V18_t* v);
parser_error_t _readConcreteNonFungible_V18(parser_context_t* c,
                                            pd_ConcreteNonFungible_V18_t* v);
parser_error_t _readConfigOpAccountId_V18(parser_context_t* c,
                                          pd_ConfigOpAccountId_V18_t* v);
parser_error_t _readConfigOpBalanceOfT_V18(parser_context_t* c,
                                           pd_ConfigOpBalanceOfT_V18_t* v);
parser_error_t _readConfigOpPerbill_V18(parser_context_t* c,
                                        pd_ConfigOpPerbill_V18_t* v);
parser_error_t _readConfigOpPercent_V18(parser_context_t* c,
                                        pd_ConfigOpPercent_V18_t* v);
parser_error_t _readConfigOpu32_V18(parser_context_t* c,
                                    pd_ConfigOpu32_V18_t* v);
parser_error_t _readConviction_V18(parser_context_t* c, pd_Conviction_V18_t* v);
parser_error_t _readEcdsaPublic_V18(parser_context_t* c,
                                    pd_EcdsaPublic_V18_t* v);
parser_error_t _readEcdsaSignature_V18(parser_context_t* c,
                                       pd_EcdsaSignature_V18_t* v);
parser_error_t _readEd25519Public_V18(parser_context_t* c,
                                      pd_Ed25519Public_V18_t* v);
parser_error_t _readEd25519Signature_V18(parser_context_t* c,
                                         pd_Ed25519Signature_V18_t* v);
parser_error_t _readEraIndex_V18(parser_context_t* c, pd_EraIndex_V18_t* v);
parser_error_t _readEthereumAddress_V18(parser_context_t* c,
                                        pd_EthereumAddress_V18_t* v);
parser_error_t _readFraction_V18(parser_context_t* c, pd_Fraction_V18_t* v);
parser_error_t _readFungibility_V18(parser_context_t* c,
                                    pd_Fungibility_V18_t* v);
parser_error_t _readIdentityInfo_V18(parser_context_t* c,
                                     pd_IdentityInfo_V18_t* v);
parser_error_t _readJudgementBalanceOfT_V18(parser_context_t* c,
                                            pd_JudgementBalanceOfT_V18_t* v);
parser_error_t _readJunctionV0X1_V18(parser_context_t* c,
                                     pd_JunctionV0X1_V18_t* v);
parser_error_t _readJunctionV0X2_V18(parser_context_t* c,
                                     pd_JunctionV0X2_V18_t* v);
parser_error_t _readJunctionV0X3_V18(parser_context_t* c,
                                     pd_JunctionV0X3_V18_t* v);
parser_error_t _readJunctionV0X4_V18(parser_context_t* c,
                                     pd_JunctionV0X4_V18_t* v);
parser_error_t _readJunctionV0X5_V18(parser_context_t* c,
                                     pd_JunctionV0X5_V18_t* v);
parser_error_t _readJunctionV0X6_V18(parser_context_t* c,
                                     pd_JunctionV0X6_V18_t* v);
parser_error_t _readJunctionV0X7_V18(parser_context_t* c,
                                     pd_JunctionV0X7_V18_t* v);
parser_error_t _readJunctionV0X8_V18(parser_context_t* c,
                                     pd_JunctionV0X8_V18_t* v);
parser_error_t _readJunctionV0_V18(parser_context_t* c, pd_JunctionV0_V18_t* v);
parser_error_t _readJunctionV1X1_V18(parser_context_t* c,
                                     pd_JunctionV1X1_V18_t* v);
parser_error_t _readJunctionV1X2_V18(parser_context_t* c,
                                     pd_JunctionV1X2_V18_t* v);
parser_error_t _readJunctionV1X3_V18(parser_context_t* c,
                                     pd_JunctionV1X3_V18_t* v);
parser_error_t _readJunctionV1X4_V18(parser_context_t* c,
                                     pd_JunctionV1X4_V18_t* v);
parser_error_t _readJunctionV1X5_V18(parser_context_t* c,
                                     pd_JunctionV1X5_V18_t* v);
parser_error_t _readJunctionV1X6_V18(parser_context_t* c,
                                     pd_JunctionV1X6_V18_t* v);
parser_error_t _readJunctionV1X7_V18(parser_context_t* c,
                                     pd_JunctionV1X7_V18_t* v);
parser_error_t _readJunctionV1X8_V18(parser_context_t* c,
                                     pd_JunctionV1X8_V18_t* v);
parser_error_t _readJunctionV1_V18(parser_context_t* c, pd_JunctionV1_V18_t* v);
parser_error_t _readJunctionsV0_V18(parser_context_t* c,
                                    pd_JunctionsV0_V18_t* v);
parser_error_t _readJunctionsV1_V18(parser_context_t* c,
                                    pd_JunctionsV1_V18_t* v);
parser_error_t _readKeys_V18(parser_context_t* c, pd_Keys_V18_t* v);
parser_error_t _readMemberCount_V18(parser_context_t* c,
                                    pd_MemberCount_V18_t* v);
parser_error_t _readMultiAssetId_V18(parser_context_t* c,
                                     pd_MultiAssetId_V18_t* v);
parser_error_t _readMultiAssetV0_V18(parser_context_t* c,
                                     pd_MultiAssetV0_V18_t* v);
parser_error_t _readMultiAssetV1_V18(parser_context_t* c,
                                     pd_MultiAssetV1_V18_t* v);
parser_error_t _readMultiLocationV0_V18(parser_context_t* c,
                                        pd_MultiLocationV0_V18_t* v);
parser_error_t _readMultiLocationV1_V18(parser_context_t* c,
                                        pd_MultiLocationV1_V18_t* v);
parser_error_t _readMultiSignature_V18(parser_context_t* c,
                                       pd_MultiSignature_V18_t* v);
parser_error_t _readMultiSigner_V18(parser_context_t* c,
                                    pd_MultiSigner_V18_t* v);
parser_error_t _readNetworkId_V18(parser_context_t* c, pd_NetworkId_V18_t* v);
parser_error_t _readOptionAccountId_V18(parser_context_t* c,
                                        pd_OptionAccountId_V18_t* v);
parser_error_t _readOptionMultiSignature_V18(parser_context_t* c,
                                             pd_OptionMultiSignature_V18_t* v);
parser_error_t _readOptionMultiSigner_V18(parser_context_t* c,
                                          pd_OptionMultiSigner_V18_t* v);
parser_error_t _readOptionProxyType_V18(parser_context_t* c,
                                        pd_OptionProxyType_V18_t* v);
parser_error_t _readOptionReferendumIndex_V18(
    parser_context_t* c, pd_OptionReferendumIndex_V18_t* v);
parser_error_t _readOptionTimepoint_V18(parser_context_t* c,
                                        pd_OptionTimepoint_V18_t* v);
parser_error_t _readOverweightIndex_V18(parser_context_t* c,
                                        pd_OverweightIndex_V18_t* v);
parser_error_t _readParaId_V18(parser_context_t* c, pd_ParaId_V18_t* v);
parser_error_t _readPerbill_V18(parser_context_t* c, pd_Perbill_V18_t* v);
parser_error_t _readPercent_V18(parser_context_t* c, pd_Percent_V18_t* v);
parser_error_t _readPlurality_V18(parser_context_t* c, pd_Plurality_V18_t* v);
parser_error_t _readPoolId_V18(parser_context_t* c, pd_PoolId_V18_t* v);
parser_error_t _readPoolState_V18(parser_context_t* c, pd_PoolState_V18_t* v);
parser_error_t _readProxyType_V18(parser_context_t* c, pd_ProxyType_V18_t* v);
parser_error_t _readReferendumIndex_V18(parser_context_t* c,
                                        pd_ReferendumIndex_V18_t* v);
parser_error_t _readRegistrarIndex_V18(parser_context_t* c,
                                       pd_RegistrarIndex_V18_t* v);
parser_error_t _readRewardDestination_V18(parser_context_t* c,
                                          pd_RewardDestination_V18_t* v);
parser_error_t _readSessionIndex_V18(parser_context_t* c,
                                     pd_SessionIndex_V18_t* v);
parser_error_t _readSr25519Public_V18(parser_context_t* c,
                                      pd_Sr25519Public_V18_t* v);
parser_error_t _readSr25519Signature_V18(parser_context_t* c,
                                         pd_Sr25519Signature_V18_t* v);
parser_error_t _readTimepoint_V18(parser_context_t* c, pd_Timepoint_V18_t* v);
parser_error_t _readTupleAccountIdData_V18(parser_context_t* c,
                                           pd_TupleAccountIdData_V18_t* v);
parser_error_t _readTupleH256u32_V18(parser_context_t* c,
                                     pd_TupleH256u32_V18_t* v);
parser_error_t _readValidatorPrefs_V18(parser_context_t* c,
                                       pd_ValidatorPrefs_V18_t* v);
parser_error_t _readVecAccountIdLookupOfT_V18(
    parser_context_t* c, pd_VecAccountIdLookupOfT_V18_t* v);
parser_error_t _readVecAccountId_V18(parser_context_t* c,
                                     pd_VecAccountId_V18_t* v);
parser_error_t _readVecMultiAssetV0_V18(parser_context_t* c,
                                        pd_VecMultiAssetV0_V18_t* v);
parser_error_t _readVecMultiAssetV1_V18(parser_context_t* c,
                                        pd_VecMultiAssetV1_V18_t* v);
parser_error_t _readVecTupleAccountIdData_V18(
    parser_context_t* c, pd_VecTupleAccountIdData_V18_t* v);
parser_error_t _readVestingInfo_V18(parser_context_t* c,
                                    pd_VestingInfo_V18_t* v);
parser_error_t _readVote_V18(parser_context_t* c, pd_Vote_V18_t* v);
parser_error_t _readWeightLimit_V18(parser_context_t* c,
                                    pd_WeightLimit_V18_t* v);
parser_error_t _readWeight_V18(parser_context_t* c, pd_Weight_V18_t* v);
parser_error_t _readu8_array_32_V18(parser_context_t* c,
                                    pd_u8_array_32_V18_t* v);

// toString functions
parser_error_t _toStringAbstractFungible_V18(const pd_AbstractFungible_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringAbstractNonFungible_V18(
    const pd_AbstractNonFungible_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountId32_V18(const pd_AccountId32_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIdLookupOfT_V18(
    const pd_AccountIdLookupOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountId_V18(const pd_AccountId_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIndex64_V18(const pd_AccountIndex64_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIndex_V18(const pd_AccountIndex_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountKey20_V18(const pd_AccountKey20_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountVoteSplit_V18(const pd_AccountVoteSplit_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringAccountVoteStandard_V18(
    const pd_AccountVoteStandard_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountVote_V18(const pd_AccountVote_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAssetInstance_V18(const pd_AssetInstance_V18_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBodyId_V18(const pd_BodyId_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringBodyPart_V18(const pd_BodyPart_V18_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringBondExtraBalanceOfT_V18(
    const pd_BondExtraBalanceOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBoundedCallOfT_V18(const pd_BoundedCallOfT_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBoxVersionedMultiAssets_V18(
    const pd_BoxVersionedMultiAssets_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBoxVersionedMultiLocation_V18(
    const pd_BoxVersionedMultiLocation_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringCompactAccountIndex_V18(
    const pd_CompactAccountIndex_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringCompactPerBill_V18(const pd_CompactPerBill_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConcreteFungible_V18(const pd_ConcreteFungible_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringConcreteNonFungible_V18(
    const pd_ConcreteNonFungible_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpAccountId_V18(
    const pd_ConfigOpAccountId_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpBalanceOfT_V18(
    const pd_ConfigOpBalanceOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpPerbill_V18(const pd_ConfigOpPerbill_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringConfigOpPercent_V18(const pd_ConfigOpPercent_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringConfigOpu32_V18(const pd_ConfigOpu32_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEcdsaPublic_V18(const pd_EcdsaPublic_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEcdsaSignature_V18(const pd_EcdsaSignature_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEd25519Public_V18(const pd_Ed25519Public_V18_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEd25519Signature_V18(const pd_Ed25519Signature_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringEraIndex_V18(const pd_EraIndex_V18_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringEthereumAddress_V18(const pd_EthereumAddress_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringFraction_V18(const pd_Fraction_V18_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringFungibility_V18(const pd_Fungibility_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringIdentityInfo_V18(const pd_IdentityInfo_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJudgementBalanceOfT_V18(
    const pd_JudgementBalanceOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X1_V18(const pd_JunctionV0X1_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X2_V18(const pd_JunctionV0X2_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X3_V18(const pd_JunctionV0X3_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X4_V18(const pd_JunctionV0X4_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X5_V18(const pd_JunctionV0X5_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X6_V18(const pd_JunctionV0X6_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X7_V18(const pd_JunctionV0X7_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X8_V18(const pd_JunctionV0X8_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0_V18(const pd_JunctionV0_V18_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X1_V18(const pd_JunctionV1X1_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X2_V18(const pd_JunctionV1X2_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X3_V18(const pd_JunctionV1X3_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X4_V18(const pd_JunctionV1X4_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X5_V18(const pd_JunctionV1X5_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X6_V18(const pd_JunctionV1X6_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X7_V18(const pd_JunctionV1X7_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X8_V18(const pd_JunctionV1X8_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1_V18(const pd_JunctionV1_V18_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionsV0_V18(const pd_JunctionsV0_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionsV1_V18(const pd_JunctionsV1_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringKeys_V18(const pd_Keys_V18_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t pageIdx,
                                 uint8_t* pageCount);

parser_error_t _toStringMemberCount_V18(const pd_MemberCount_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiAssetId_V18(const pd_MultiAssetId_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiAssetV0_V18(const pd_MultiAssetV0_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiAssetV1_V18(const pd_MultiAssetV1_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiLocationV0_V18(const pd_MultiLocationV0_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringMultiLocationV1_V18(const pd_MultiLocationV1_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringMultiSignature_V18(const pd_MultiSignature_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiSigner_V18(const pd_MultiSigner_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringNetworkId_V18(const pd_NetworkId_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionAccountId_V18(const pd_OptionAccountId_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringOptionMultiSignature_V18(
    const pd_OptionMultiSignature_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionMultiSigner_V18(
    const pd_OptionMultiSigner_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionReferendumIndex_V18(
    const pd_OptionReferendumIndex_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionTimepoint_V18(const pd_OptionTimepoint_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringOverweightIndex_V18(const pd_OverweightIndex_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringParaId_V18(const pd_ParaId_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringPerbill_V18(const pd_Perbill_V18_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringPercent_V18(const pd_Percent_V18_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringPlurality_V18(const pd_Plurality_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringPoolId_V18(const pd_PoolId_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringProxyType_V18(const pd_ProxyType_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t* pageCount);

parser_error_t _toStringReferendumIndex_V18(const pd_ReferendumIndex_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringRegistrarIndex_V18(const pd_RegistrarIndex_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringRewardDestination_V18(
    const pd_RewardDestination_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSessionIndex_V18(const pd_SessionIndex_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSr25519Public_V18(const pd_Sr25519Public_V18_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSr25519Signature_V18(const pd_Sr25519Signature_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringTimepoint_V18(const pd_Timepoint_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringTupleAccountIdData_V18(
    const pd_TupleAccountIdData_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringTupleH256u32_V18(const pd_TupleH256u32_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringValidatorPrefs_V18(const pd_ValidatorPrefs_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecAccountIdLookupOfT_V18(
    const pd_VecAccountIdLookupOfT_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecAccountId_V18(const pd_VecAccountId_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecMultiAssetV0_V18(const pd_VecMultiAssetV0_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringVecMultiAssetV1_V18(const pd_VecMultiAssetV1_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringVecTupleAccountIdData_V18(
    const pd_VecTupleAccountIdData_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVestingInfo_V18(const pd_VestingInfo_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVote_V18(const pd_Vote_V18_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t* pageCount);

parser_error_t _toStringWeightLimit_V18(const pd_WeightLimit_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringWeight_V18(const pd_Weight_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringu8_array_32_V18(const pd_u8_array_32_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

#endif
