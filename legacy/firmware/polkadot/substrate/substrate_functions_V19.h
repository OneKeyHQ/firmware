#ifndef __POLKADOT_SUBSTRATE_FUNCTIONS_V19_H__
#define __POLKADOT_SUBSTRATE_FUNCTIONS_V19_H__

#include <stddef.h>
#include <stdint.h>
#include "../parser_common.h"
#include "substrate_methods_V19.h"
#include "substrate_types_V19.h"

// Read functions
parser_error_t _readAbstractFungible_V19(parser_context_t* c,
                                         pd_AbstractFungible_V19_t* v);
parser_error_t _readAbstractNonFungible_V19(parser_context_t* c,
                                            pd_AbstractNonFungible_V19_t* v);
parser_error_t _readAccountId32_V19(parser_context_t* c,
                                    pd_AccountId32_V19_t* v);
parser_error_t _readAccountIdLookupOfT_V19(parser_context_t* c,
                                           pd_AccountIdLookupOfT_V19_t* v);
parser_error_t _readAccountId_V19(parser_context_t* c, pd_AccountId_V19_t* v);
parser_error_t _readAccountIndex64_V19(parser_context_t* c,
                                       pd_AccountIndex64_V19_t* v);
parser_error_t _readAccountIndex_V19(parser_context_t* c,
                                     pd_AccountIndex_V19_t* v);
parser_error_t _readAccountKey20_V19(parser_context_t* c,
                                     pd_AccountKey20_V19_t* v);
parser_error_t _readAccountVoteSplit_V19(parser_context_t* c,
                                         pd_AccountVoteSplit_V19_t* v);
parser_error_t _readAccountVoteStandard_V19(parser_context_t* c,
                                            pd_AccountVoteStandard_V19_t* v);
parser_error_t _readAccountVote_V19(parser_context_t* c,
                                    pd_AccountVote_V19_t* v);
parser_error_t _readAssetInstance_V19(parser_context_t* c,
                                      pd_AssetInstance_V19_t* v);
parser_error_t _readBodyId_V19(parser_context_t* c, pd_BodyId_V19_t* v);
parser_error_t _readBodyPart_V19(parser_context_t* c, pd_BodyPart_V19_t* v);
parser_error_t _readBondExtraBalanceOfT_V19(parser_context_t* c,
                                            pd_BondExtraBalanceOfT_V19_t* v);
parser_error_t _readBoundedCallOfT_V19(parser_context_t* c,
                                       pd_BoundedCallOfT_V19_t* v);
parser_error_t _readBoxVersionedMultiAssets_V19(
    parser_context_t* c, pd_BoxVersionedMultiAssets_V19_t* v);
parser_error_t _readBoxVersionedMultiLocation_V19(
    parser_context_t* c, pd_BoxVersionedMultiLocation_V19_t* v);
parser_error_t _readCompactAccountIndex_V19(parser_context_t* c,
                                            pd_CompactAccountIndex_V19_t* v);
parser_error_t _readCompactPerBill_V19(parser_context_t* c,
                                       pd_CompactPerBill_V19_t* v);
parser_error_t _readConcreteFungible_V19(parser_context_t* c,
                                         pd_ConcreteFungible_V19_t* v);
parser_error_t _readConcreteNonFungible_V19(parser_context_t* c,
                                            pd_ConcreteNonFungible_V19_t* v);
parser_error_t _readConfigOpAccountId_V19(parser_context_t* c,
                                          pd_ConfigOpAccountId_V19_t* v);
parser_error_t _readConfigOpBalanceOfT_V19(parser_context_t* c,
                                           pd_ConfigOpBalanceOfT_V19_t* v);
parser_error_t _readConfigOpPerbill_V19(parser_context_t* c,
                                        pd_ConfigOpPerbill_V19_t* v);
parser_error_t _readConfigOpPercent_V19(parser_context_t* c,
                                        pd_ConfigOpPercent_V19_t* v);
parser_error_t _readConfigOpu32_V19(parser_context_t* c,
                                    pd_ConfigOpu32_V19_t* v);
parser_error_t _readConviction_V19(parser_context_t* c, pd_Conviction_V19_t* v);
parser_error_t _readEcdsaPublic_V19(parser_context_t* c,
                                    pd_EcdsaPublic_V19_t* v);
parser_error_t _readEcdsaSignature_V19(parser_context_t* c,
                                       pd_EcdsaSignature_V19_t* v);
parser_error_t _readEd25519Public_V19(parser_context_t* c,
                                      pd_Ed25519Public_V19_t* v);
parser_error_t _readEd25519Signature_V19(parser_context_t* c,
                                         pd_Ed25519Signature_V19_t* v);
parser_error_t _readEraIndex_V19(parser_context_t* c, pd_EraIndex_V19_t* v);
parser_error_t _readEthereumAddress_V19(parser_context_t* c,
                                        pd_EthereumAddress_V19_t* v);
parser_error_t _readFraction_V19(parser_context_t* c, pd_Fraction_V19_t* v);
parser_error_t _readFungibility_V19(parser_context_t* c,
                                    pd_Fungibility_V19_t* v);
parser_error_t _readIdentityInfo_V19(parser_context_t* c,
                                     pd_IdentityInfo_V19_t* v);
parser_error_t _readJudgementBalanceOfT_V19(parser_context_t* c,
                                            pd_JudgementBalanceOfT_V19_t* v);
parser_error_t _readJunctionV0X1_V19(parser_context_t* c,
                                     pd_JunctionV0X1_V19_t* v);
parser_error_t _readJunctionV0X2_V19(parser_context_t* c,
                                     pd_JunctionV0X2_V19_t* v);
parser_error_t _readJunctionV0X3_V19(parser_context_t* c,
                                     pd_JunctionV0X3_V19_t* v);
parser_error_t _readJunctionV0X4_V19(parser_context_t* c,
                                     pd_JunctionV0X4_V19_t* v);
parser_error_t _readJunctionV0X5_V19(parser_context_t* c,
                                     pd_JunctionV0X5_V19_t* v);
parser_error_t _readJunctionV0X6_V19(parser_context_t* c,
                                     pd_JunctionV0X6_V19_t* v);
parser_error_t _readJunctionV0X7_V19(parser_context_t* c,
                                     pd_JunctionV0X7_V19_t* v);
parser_error_t _readJunctionV0X8_V19(parser_context_t* c,
                                     pd_JunctionV0X8_V19_t* v);
parser_error_t _readJunctionV0_V19(parser_context_t* c, pd_JunctionV0_V19_t* v);
parser_error_t _readJunctionV1X1_V19(parser_context_t* c,
                                     pd_JunctionV1X1_V19_t* v);
parser_error_t _readJunctionV1X2_V19(parser_context_t* c,
                                     pd_JunctionV1X2_V19_t* v);
parser_error_t _readJunctionV1X3_V19(parser_context_t* c,
                                     pd_JunctionV1X3_V19_t* v);
parser_error_t _readJunctionV1X4_V19(parser_context_t* c,
                                     pd_JunctionV1X4_V19_t* v);
parser_error_t _readJunctionV1X5_V19(parser_context_t* c,
                                     pd_JunctionV1X5_V19_t* v);
parser_error_t _readJunctionV1X6_V19(parser_context_t* c,
                                     pd_JunctionV1X6_V19_t* v);
parser_error_t _readJunctionV1X7_V19(parser_context_t* c,
                                     pd_JunctionV1X7_V19_t* v);
parser_error_t _readJunctionV1X8_V19(parser_context_t* c,
                                     pd_JunctionV1X8_V19_t* v);
parser_error_t _readJunctionV1_V19(parser_context_t* c, pd_JunctionV1_V19_t* v);
parser_error_t _readJunctionsV0_V19(parser_context_t* c,
                                    pd_JunctionsV0_V19_t* v);
parser_error_t _readJunctionsV1_V19(parser_context_t* c,
                                    pd_JunctionsV1_V19_t* v);
parser_error_t _readKeys_V19(parser_context_t* c, pd_Keys_V19_t* v);
parser_error_t _readMemberCount_V19(parser_context_t* c,
                                    pd_MemberCount_V19_t* v);
parser_error_t _readMultiAssetId_V19(parser_context_t* c,
                                     pd_MultiAssetId_V19_t* v);
parser_error_t _readMultiAssetV0_V19(parser_context_t* c,
                                     pd_MultiAssetV0_V19_t* v);
parser_error_t _readMultiAssetV1_V19(parser_context_t* c,
                                     pd_MultiAssetV1_V19_t* v);
parser_error_t _readMultiLocationV0_V19(parser_context_t* c,
                                        pd_MultiLocationV0_V19_t* v);
parser_error_t _readMultiLocationV1_V19(parser_context_t* c,
                                        pd_MultiLocationV1_V19_t* v);
parser_error_t _readMultiSignature_V19(parser_context_t* c,
                                       pd_MultiSignature_V19_t* v);
parser_error_t _readMultiSigner_V19(parser_context_t* c,
                                    pd_MultiSigner_V19_t* v);
parser_error_t _readNetworkId_V19(parser_context_t* c, pd_NetworkId_V19_t* v);
parser_error_t _readOptionAccountId_V19(parser_context_t* c,
                                        pd_OptionAccountId_V19_t* v);
parser_error_t _readOptionMultiSignature_V19(parser_context_t* c,
                                             pd_OptionMultiSignature_V19_t* v);
parser_error_t _readOptionMultiSigner_V19(parser_context_t* c,
                                          pd_OptionMultiSigner_V19_t* v);
parser_error_t _readOptionProxyType_V19(parser_context_t* c,
                                        pd_OptionProxyType_V19_t* v);
parser_error_t _readOptionReferendumIndex_V19(
    parser_context_t* c, pd_OptionReferendumIndex_V19_t* v);
parser_error_t _readOptionTimepoint_V19(parser_context_t* c,
                                        pd_OptionTimepoint_V19_t* v);
parser_error_t _readOverweightIndex_V19(parser_context_t* c,
                                        pd_OverweightIndex_V19_t* v);
parser_error_t _readParaId_V19(parser_context_t* c, pd_ParaId_V19_t* v);
parser_error_t _readPerbill_V19(parser_context_t* c, pd_Perbill_V19_t* v);
parser_error_t _readPercent_V19(parser_context_t* c, pd_Percent_V19_t* v);
parser_error_t _readPlurality_V19(parser_context_t* c, pd_Plurality_V19_t* v);
parser_error_t _readPoolId_V19(parser_context_t* c, pd_PoolId_V19_t* v);
parser_error_t _readPoolState_V19(parser_context_t* c, pd_PoolState_V19_t* v);
parser_error_t _readProxyType_V19(parser_context_t* c, pd_ProxyType_V19_t* v);
parser_error_t _readReferendumIndex_V19(parser_context_t* c,
                                        pd_ReferendumIndex_V19_t* v);
parser_error_t _readRegistrarIndex_V19(parser_context_t* c,
                                       pd_RegistrarIndex_V19_t* v);
parser_error_t _readRewardDestination_V19(parser_context_t* c,
                                          pd_RewardDestination_V19_t* v);
parser_error_t _readSessionIndex_V19(parser_context_t* c,
                                     pd_SessionIndex_V19_t* v);
parser_error_t _readSr25519Public_V19(parser_context_t* c,
                                      pd_Sr25519Public_V19_t* v);
parser_error_t _readSr25519Signature_V19(parser_context_t* c,
                                         pd_Sr25519Signature_V19_t* v);
parser_error_t _readTimepoint_V19(parser_context_t* c, pd_Timepoint_V19_t* v);
parser_error_t _readTupleAccountIdData_V19(parser_context_t* c,
                                           pd_TupleAccountIdData_V19_t* v);
parser_error_t _readTupleH256u32_V19(parser_context_t* c,
                                     pd_TupleH256u32_V19_t* v);
parser_error_t _readValidatorPrefs_V19(parser_context_t* c,
                                       pd_ValidatorPrefs_V19_t* v);
parser_error_t _readVecAccountIdLookupOfT_V19(
    parser_context_t* c, pd_VecAccountIdLookupOfT_V19_t* v);
parser_error_t _readVecAccountId_V19(parser_context_t* c,
                                     pd_VecAccountId_V19_t* v);
parser_error_t _readVecMultiAssetV0_V19(parser_context_t* c,
                                        pd_VecMultiAssetV0_V19_t* v);
parser_error_t _readVecMultiAssetV1_V19(parser_context_t* c,
                                        pd_VecMultiAssetV1_V19_t* v);
parser_error_t _readVecTupleAccountIdData_V19(
    parser_context_t* c, pd_VecTupleAccountIdData_V19_t* v);
parser_error_t _readVestingInfo_V19(parser_context_t* c,
                                    pd_VestingInfo_V19_t* v);
parser_error_t _readVote_V19(parser_context_t* c, pd_Vote_V19_t* v);
parser_error_t _readWeightLimit_V19(parser_context_t* c,
                                    pd_WeightLimit_V19_t* v);
parser_error_t _readWeight_V19(parser_context_t* c, pd_Weight_V19_t* v);
parser_error_t _readu8_array_32_V19(parser_context_t* c,
                                    pd_u8_array_32_V19_t* v);

// toString functions
parser_error_t _toStringAbstractFungible_V19(const pd_AbstractFungible_V19_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringAbstractNonFungible_V19(
    const pd_AbstractNonFungible_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountId32_V19(const pd_AccountId32_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIdLookupOfT_V19(
    const pd_AccountIdLookupOfT_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountId_V19(const pd_AccountId_V19_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIndex64_V19(const pd_AccountIndex64_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountIndex_V19(const pd_AccountIndex_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountKey20_V19(const pd_AccountKey20_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountVoteSplit_V19(const pd_AccountVoteSplit_V19_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringAccountVoteStandard_V19(
    const pd_AccountVoteStandard_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAccountVote_V19(const pd_AccountVote_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringAssetInstance_V19(const pd_AssetInstance_V19_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBodyId_V19(const pd_BodyId_V19_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringBodyPart_V19(const pd_BodyPart_V19_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringBondExtraBalanceOfT_V19(
    const pd_BondExtraBalanceOfT_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBoundedCallOfT_V19(const pd_BoundedCallOfT_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBoxVersionedMultiAssets_V19(
    const pd_BoxVersionedMultiAssets_V19_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringBoxVersionedMultiLocation_V19(
    const pd_BoxVersionedMultiLocation_V19_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringCompactAccountIndex_V19(
    const pd_CompactAccountIndex_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringCompactPerBill_V19(const pd_CompactPerBill_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConcreteFungible_V19(const pd_ConcreteFungible_V19_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringConcreteNonFungible_V19(
    const pd_ConcreteNonFungible_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpAccountId_V19(
    const pd_ConfigOpAccountId_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpBalanceOfT_V19(
    const pd_ConfigOpBalanceOfT_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringConfigOpPerbill_V19(const pd_ConfigOpPerbill_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringConfigOpPercent_V19(const pd_ConfigOpPercent_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringConfigOpu32_V19(const pd_ConfigOpu32_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEcdsaPublic_V19(const pd_EcdsaPublic_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEcdsaSignature_V19(const pd_EcdsaSignature_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEd25519Public_V19(const pd_Ed25519Public_V19_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringEd25519Signature_V19(const pd_Ed25519Signature_V19_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringEraIndex_V19(const pd_EraIndex_V19_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringEthereumAddress_V19(const pd_EthereumAddress_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringFraction_V19(const pd_Fraction_V19_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount);

parser_error_t _toStringFungibility_V19(const pd_Fungibility_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringIdentityInfo_V19(const pd_IdentityInfo_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJudgementBalanceOfT_V19(
    const pd_JudgementBalanceOfT_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X1_V19(const pd_JunctionV0X1_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X2_V19(const pd_JunctionV0X2_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X3_V19(const pd_JunctionV0X3_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X4_V19(const pd_JunctionV0X4_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X5_V19(const pd_JunctionV0X5_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X6_V19(const pd_JunctionV0X6_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X7_V19(const pd_JunctionV0X7_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0X8_V19(const pd_JunctionV0X8_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV0_V19(const pd_JunctionV0_V19_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X1_V19(const pd_JunctionV1X1_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X2_V19(const pd_JunctionV1X2_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X3_V19(const pd_JunctionV1X3_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X4_V19(const pd_JunctionV1X4_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X5_V19(const pd_JunctionV1X5_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X6_V19(const pd_JunctionV1X6_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X7_V19(const pd_JunctionV1X7_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1X8_V19(const pd_JunctionV1X8_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionV1_V19(const pd_JunctionV1_V19_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionsV0_V19(const pd_JunctionsV0_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringJunctionsV1_V19(const pd_JunctionsV1_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringKeys_V19(const pd_Keys_V19_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t pageIdx,
                                 uint8_t* pageCount);

parser_error_t _toStringMemberCount_V19(const pd_MemberCount_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiAssetId_V19(const pd_MultiAssetId_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiAssetV0_V19(const pd_MultiAssetV0_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiAssetV1_V19(const pd_MultiAssetV1_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiLocationV0_V19(const pd_MultiLocationV0_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringMultiLocationV1_V19(const pd_MultiLocationV1_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringMultiSignature_V19(const pd_MultiSignature_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringMultiSigner_V19(const pd_MultiSigner_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringNetworkId_V19(const pd_NetworkId_V19_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionAccountId_V19(const pd_OptionAccountId_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringOptionMultiSignature_V19(
    const pd_OptionMultiSignature_V19_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionMultiSigner_V19(
    const pd_OptionMultiSigner_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionReferendumIndex_V19(
    const pd_OptionReferendumIndex_V19_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringOptionTimepoint_V19(const pd_OptionTimepoint_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringOverweightIndex_V19(const pd_OverweightIndex_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringParaId_V19(const pd_ParaId_V19_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringPerbill_V19(const pd_Perbill_V19_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringPercent_V19(const pd_Percent_V19_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringPlurality_V19(const pd_Plurality_V19_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringPoolId_V19(const pd_PoolId_V19_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringProxyType_V19(const pd_ProxyType_V19_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t* pageCount);

parser_error_t _toStringReferendumIndex_V19(const pd_ReferendumIndex_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringRegistrarIndex_V19(const pd_RegistrarIndex_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringRewardDestination_V19(
    const pd_RewardDestination_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSessionIndex_V19(const pd_SessionIndex_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSr25519Public_V19(const pd_Sr25519Public_V19_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringSr25519Signature_V19(const pd_Sr25519Signature_V19_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount);

parser_error_t _toStringTimepoint_V19(const pd_Timepoint_V19_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringTupleAccountIdData_V19(
    const pd_TupleAccountIdData_V19_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringTupleH256u32_V19(const pd_TupleH256u32_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringValidatorPrefs_V19(const pd_ValidatorPrefs_V19_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecAccountIdLookupOfT_V19(
    const pd_VecAccountIdLookupOfT_V19_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecAccountId_V19(const pd_VecAccountId_V19_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVecMultiAssetV0_V19(const pd_VecMultiAssetV0_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringVecMultiAssetV1_V19(const pd_VecMultiAssetV1_V19_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringVecTupleAccountIdData_V19(
    const pd_VecTupleAccountIdData_V19_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVestingInfo_V19(const pd_VestingInfo_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringVote_V19(const pd_Vote_V19_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t* pageCount);

parser_error_t _toStringWeightLimit_V19(const pd_WeightLimit_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

parser_error_t _toStringWeight_V19(const pd_Weight_V19_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringu8_array_32_V19(const pd_u8_array_32_V19_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

#endif
