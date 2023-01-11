#include <stddef.h>
#include <stdint.h>
#include "substrate_coin.h"
#include "substrate_dispatch_V18.h"

#include "../bignum.h"
#include "../common_defs.h"
#include "../parser_impl.h"
#include "util.h"

parser_error_t _readAbstractFungible_V18(parser_context_t* c,
                                         pd_AbstractFungible_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_substrate_readBytes(c, &v->id))
  CHECK_ERROR(_readCompactu128(c, &v->amount))
  return parser_ok;
}

parser_error_t _readAbstractNonFungible_V18(parser_context_t* c,
                                            pd_AbstractNonFungible_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_substrate_readBytes(c, &v->_class))
  CHECK_ERROR(_readAssetInstance_V18(c, &v->instance))
  return parser_ok;
}

parser_error_t _readAccountId32_V18(parser_context_t* c,
                                    pd_AccountId32_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readNetworkId_V18(c, &v->networkId))
  CHECK_ERROR(_readu8_array_32_V18(c, &v->key))
  return parser_ok;
}

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

parser_error_t
    _readAccountIndex64_V18(parser_context_t* c, pd_AccountIndex64_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readNetworkId_V18(c, &v->networkId))
  CHECK_ERROR(_readCompactu64(c, &v->index))
  return parser_ok;
}

parser_error_t _readAccountIndex_V18(parser_context_t* c,
                                     pd_AccountIndex_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readAccountKey20_V18(parser_context_t* c,
                                     pd_AccountKey20_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readNetworkId_V18(c, &v->networkId))
  CHECK_ERROR(_readu8_array_20(c, &v->key))
  return parser_ok;
}

parser_error_t _readAccountVoteSplit_V18(parser_context_t* c,
                                         pd_AccountVoteSplit_V18_t* v) {
  CHECK_ERROR(_readBalanceOf(c, &v->aye));
  CHECK_ERROR(_readBalanceOf(c, &v->nay));
  return parser_ok;
}

parser_error_t _readAccountVoteStandard_V18(parser_context_t* c,
                                            pd_AccountVoteStandard_V18_t* v) {
  CHECK_ERROR(_readVote_V18(c, &v->vote));
  CHECK_ERROR(_readBalanceOf(c, &v->balance));
  return parser_ok;
}

parser_error_t _readAccountVote_V18(parser_context_t* c,
                                    pd_AccountVote_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))

  switch (v->value) {
    case 0:
      CHECK_ERROR(_readAccountVoteStandard_V18(c, &v->voteStandard))
      break;
    case 1:
      CHECK_ERROR(_readAccountVoteSplit_V18(c, &v->voteSplit))
      break;
    default:
      break;
  }

  return parser_ok;
}

parser_error_t _readAssetInstance_V18(parser_context_t* c,
                                      pd_AssetInstance_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Undefined
      // Empty
      break;
    case 1:  // Index
      CHECK_ERROR(_readCompactu128(c, &v->index))
      break;
    case 2:  // array4
      GEN_DEF_READARRAY(4)
      break;
    case 3:  // array8
      GEN_DEF_READARRAY(8)
      break;
    case 4:  // array16
      GEN_DEF_READARRAY(16)
      break;
    case 5:  // array32
      GEN_DEF_READARRAY(32)
      break;
    case 6:  // blob
      CHECK_ERROR(_substrate_readBytes(c, &v->blob))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readBodyId_V18(parser_context_t* c, pd_BodyId_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Unit
      break;
    case 1:  // Named
      CHECK_ERROR(_substrate_readBytes(c, &v->named))
      break;
    case 2:  // Index
      CHECK_ERROR(_readCompactu32(c, &v->index))
      break;
    case 3:  // Executive
    case 4:  // Technical
    case 5:  // Legislative
    case 6:  // Judicial
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readBodyPart_V18(parser_context_t* c, pd_BodyPart_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Voice
      break;
    case 1:  // Members
      CHECK_ERROR(_readCompactu32(c, &v->count))
      break;
    case 2:  // Fraction
    case 3:  // AtleastProportion
    case 4:  // MoreThanProportion
      CHECK_ERROR(_readFraction_V18(c, &v->fraction))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readBondExtraBalanceOfT_V18(parser_context_t* c,
                                            pd_BondExtraBalanceOfT_V18_t* v) {
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

parser_error_t _readBoundedCallOfT_V18(parser_context_t* c,
                                       pd_BoundedCallOfT_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:
      CHECK_ERROR(_readH256(c, &v->legacy))
      break;
    case 1:
      CHECK_ERROR(_substrate_readBytes(c, &v->_inline))
      break;
    case 2:
      CHECK_ERROR(_readTupleH256u32_V18(c, &v->lookup))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readBoxVersionedMultiAssets_V18(
    parser_context_t* c, pd_BoxVersionedMultiAssets_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // V0
      CHECK_ERROR(_readVecMultiAssetV0_V18(c, &v->vecMultiassetV0))
      break;
    case 1:  // V1
      CHECK_ERROR(_readVecMultiAssetV1_V18(c, &v->vecMultiassetV1))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readBoxVersionedMultiLocation_V18(
    parser_context_t* c, pd_BoxVersionedMultiLocation_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // V0
      CHECK_ERROR(_readMultiLocationV0_V18(c, &v->multilocationV0))
      break;
    case 1:  // V1
      CHECK_ERROR(_readMultiLocationV1_V18(c, &v->multilocationV1))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readCompactAccountIndex_V18(parser_context_t* c,
                                            pd_CompactAccountIndex_V18_t* v) {
  return _readCompactInt(c, &v->value);
}

parser_error_t _readCompactPerBill_V18(parser_context_t* c,
                                       pd_CompactPerBill_V18_t* v) {
  return _readCompactInt(c, &v->value);
}

parser_error_t _readConcreteFungible_V18(parser_context_t* c,
                                         pd_ConcreteFungible_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readMultiLocationV0_V18(c, &v->id))
  CHECK_ERROR(_readCompactBalance(c, &v->amount))
  return parser_ok;
}

parser_error_t _readConcreteNonFungible_V18(parser_context_t* c,
                                            pd_ConcreteNonFungible_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readMultiLocationV0_V18(c, &v->_class))
  CHECK_ERROR(_readAssetInstance_V18(c, &v->instance))
  return parser_ok;
}

parser_error_t _readConfigOpAccountId_V18(parser_context_t* c,
                                          pd_ConfigOpAccountId_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Noop
    case 2:  // Remove
      break;
    case 1:
      CHECK_ERROR(_readAccountId_V18(c, &v->set))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readConfigOpBalanceOfT_V18(parser_context_t* c,
                                           pd_ConfigOpBalanceOfT_V18_t* v) {
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

parser_error_t _readConfigOpPerbill_V18(parser_context_t* c,
                                        pd_ConfigOpPerbill_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Noop
    case 2:  // Remove
      break;
    case 1:
      CHECK_ERROR(_readPerbill_V18(c, &v->set))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readConfigOpPercent_V18(parser_context_t* c,
                                        pd_ConfigOpPercent_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Noop
    case 2:  // Remove
      break;
    case 1:
      CHECK_ERROR(_readPercent_V18(c, &v->set))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readConfigOpu32_V18(parser_context_t* c,
                                    pd_ConfigOpu32_V18_t* v) {
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

parser_error_t _readConviction_V18(parser_context_t* c,
                                   pd_Conviction_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  if (v->value > 5) {
    return parser_value_out_of_range;
  }
  return parser_ok;
}

parser_error_t _readEcdsaPublic_V18(parser_context_t* c,
                                    pd_EcdsaPublic_V18_t* v){
    GEN_DEF_READARRAY(33)}

parser_error_t
    _readEcdsaSignature_V18(parser_context_t* c,
                            pd_EcdsaSignature_V18_t* v){GEN_DEF_READARRAY(65)}

parser_error_t
    _readEd25519Public_V18(parser_context_t* c,
                           pd_Ed25519Public_V18_t* v){GEN_DEF_READARRAY(32)}

parser_error_t _readEd25519Signature_V18(parser_context_t* c,
                                         pd_Ed25519Signature_V18_t* v){
    GEN_DEF_READARRAY(64)}

parser_error_t _readEraIndex_V18(parser_context_t* c, pd_EraIndex_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readEthereumAddress_V18(parser_context_t* c,
                                        pd_EthereumAddress_V18_t* v){
    GEN_DEF_READARRAY(20)}

parser_error_t _readFraction_V18(parser_context_t* c, pd_Fraction_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readCompactu32(c, &v->nom))
  CHECK_ERROR(_readCompactu32(c, &v->denom))
  return parser_ok;
}

parser_error_t _readFungibility_V18(parser_context_t* c,
                                    pd_Fungibility_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Undefined
      CHECK_ERROR(_readCompactu128(c, &v->fungible))
      break;
    case 1:  // Index
      CHECK_ERROR(_readAssetInstance_V18(c, &v->nonFungible))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readIdentityInfo_V18(parser_context_t* c,
                                     pd_IdentityInfo_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readVecTupleDataData(c, &v->additional));
  CHECK_ERROR(_readData(c, &v->display));
  CHECK_ERROR(_readData(c, &v->legal));
  CHECK_ERROR(_readData(c, &v->web));
  CHECK_ERROR(_readData(c, &v->riot));
  CHECK_ERROR(_readData(c, &v->email));
  CHECK_ERROR(_readOptionu8_array_20(c, &v->pgp_fingerprint));
  CHECK_ERROR(_readData(c, &v->image));
  CHECK_ERROR(_readData(c, &v->twitter));
  return parser_ok;
}

parser_error_t _readJudgementBalanceOfT_V18(parser_context_t* c,
                                            pd_JudgementBalanceOfT_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Unknown
    case 2:  // Reasonable
    case 3:  // KnownGood
    case 4:  // OutOfDate
    case 5:  // LowQuality
    case 6:  // Erroneous
      break;
    case 1:  // FeePaid
      CHECK_ERROR(_readBalance(c, &v->feePaid))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readJunctionV0X1_V18(parser_context_t* c,
                                     pd_JunctionV0X1_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction))
  return parser_ok;
}

parser_error_t _readJunctionV0X2_V18(parser_context_t* c,
                                     pd_JunctionV0X2_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  return parser_ok;
}

parser_error_t _readJunctionV0X3_V18(parser_context_t* c,
                                     pd_JunctionV0X3_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction2))
  return parser_ok;
}

parser_error_t _readJunctionV0X4_V18(parser_context_t* c,
                                     pd_JunctionV0X4_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction3))
  return parser_ok;
}

parser_error_t _readJunctionV0X5_V18(parser_context_t* c,
                                     pd_JunctionV0X5_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction4))
  return parser_ok;
}

parser_error_t _readJunctionV0X6_V18(parser_context_t* c,
                                     pd_JunctionV0X6_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction4))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction5))
  return parser_ok;
}

parser_error_t _readJunctionV0X7_V18(parser_context_t* c,
                                     pd_JunctionV0X7_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction4))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction5))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction6))
  return parser_ok;
}

parser_error_t _readJunctionV0X8_V18(parser_context_t* c,
                                     pd_JunctionV0X8_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction4))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction5))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction6))
  CHECK_ERROR(_readJunctionV0_V18(c, &v->junction7))
  return parser_ok;
}

parser_error_t _readJunctionV0_V18(parser_context_t* c,
                                   pd_JunctionV0_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Parent
      break;
    case 1:  // Parachain
      CHECK_ERROR(_readCompactu32(c, &v->parachain))
      break;
    case 2:  // AccountId32
      CHECK_ERROR(_readAccountId32_V18(c, &v->accountId32))
      break;
    case 3:  // AccountIndex64
      CHECK_ERROR(_readAccountIndex64_V18(c, &v->accountIndex64))
      break;
    case 4:  // AccountKey20
      CHECK_ERROR(_readAccountKey20_V18(c, &v->accountKey20))
      break;
    case 5:  // PalletInstance
      CHECK_ERROR(_preadUInt8(c, &v->palletInstance))
      break;
    case 6:  // GeneralIndex
      CHECK_ERROR(_readCompactu128(c, &v->generalIndex))
      break;
    case 7:  // GeneralKey
      CHECK_ERROR(_substrate_readBytes(c, &v->generalKey))
      break;
    case 8:  // OnlyChild
      break;
    case 9:  // Plurality
      CHECK_ERROR(_readPlurality_V18(c, &v->plurality))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readJunctionV1X1_V18(parser_context_t* c,
                                     pd_JunctionV1X1_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction))
  return parser_ok;
}

parser_error_t _readJunctionV1X2_V18(parser_context_t* c,
                                     pd_JunctionV1X2_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  return parser_ok;
}

parser_error_t _readJunctionV1X3_V18(parser_context_t* c,
                                     pd_JunctionV1X3_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction2))
  return parser_ok;
}

parser_error_t _readJunctionV1X4_V18(parser_context_t* c,
                                     pd_JunctionV1X4_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction3))
  return parser_ok;
}

parser_error_t _readJunctionV1X5_V18(parser_context_t* c,
                                     pd_JunctionV1X5_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction4))
  return parser_ok;
}

parser_error_t _readJunctionV1X6_V18(parser_context_t* c,
                                     pd_JunctionV1X6_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction4))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction5))
  return parser_ok;
}

parser_error_t _readJunctionV1X7_V18(parser_context_t* c,
                                     pd_JunctionV1X7_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction4))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction5))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction6))
  return parser_ok;
}

parser_error_t _readJunctionV1X8_V18(parser_context_t* c,
                                     pd_JunctionV1X8_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction0))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction1))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction2))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction3))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction4))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction5))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction6))
  CHECK_ERROR(_readJunctionV1_V18(c, &v->junction7))
  return parser_ok;
}

parser_error_t _readJunctionV1_V18(parser_context_t* c,
                                   pd_JunctionV1_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Parachain
      CHECK_ERROR(_readCompactu32(c, &v->parachain))
      break;
    case 1:  // AccountId32
      CHECK_ERROR(_readAccountId32_V18(c, &v->accountId32))
      break;
    case 2:  // AccountIndex64
      CHECK_ERROR(_readAccountIndex64_V18(c, &v->accountIndex64))
      break;
    case 3:  // AccountKey20
      CHECK_ERROR(_readAccountKey20_V18(c, &v->accountKey20))
      break;
    case 4:  // PalletInstance
      CHECK_ERROR(_preadUInt8(c, &v->palletInstance))
      break;
    case 5:  // GeneralIndex
      CHECK_ERROR(_readCompactu128(c, &v->generalIndex))
      break;
    case 6:  // GeneralKey
      CHECK_ERROR(_substrate_readBytes(c, &v->generalKey))
      break;
    case 7:  // OnlyChild
      break;
    case 8:  // Plurality
      CHECK_ERROR(_readPlurality_V18(c, &v->plurality))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readJunctionsV0_V18(parser_context_t* c,
                                    pd_JunctionsV0_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Null
      break;
    case 1:  // X1
      CHECK_ERROR(_readJunctionV0X1_V18(c, &v->x1))
      break;
    case 2:  // X2
      CHECK_ERROR(_readJunctionV0X2_V18(c, &v->x2))
      break;
    case 3:  // X3
      CHECK_ERROR(_readJunctionV0X3_V18(c, &v->x3))
      break;
    case 4:  // X4
      CHECK_ERROR(_readJunctionV0X4_V18(c, &v->x4))
      break;
    case 5:  // X5
      CHECK_ERROR(_readJunctionV0X5_V18(c, &v->x5))
      break;
    case 6:  // X6
      CHECK_ERROR(_readJunctionV0X6_V18(c, &v->x6))
      break;
    case 7:  // X7
      CHECK_ERROR(_readJunctionV0X7_V18(c, &v->x7))
      break;
    case 8:  // X8
      CHECK_ERROR(_readJunctionV0X8_V18(c, &v->x8))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readJunctionsV1_V18(parser_context_t* c,
                                    pd_JunctionsV1_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Here
      break;
    case 1:  // X1
      CHECK_ERROR(_readJunctionV1X1_V18(c, &v->x1))
      break;
    case 2:  // X2
      CHECK_ERROR(_readJunctionV1X2_V18(c, &v->x2))
      break;
    case 3:  // X3
      CHECK_ERROR(_readJunctionV1X3_V18(c, &v->x3))
      break;
    case 4:  // X4
      CHECK_ERROR(_readJunctionV1X4_V18(c, &v->x4))
      break;
    case 5:  // X5
      CHECK_ERROR(_readJunctionV1X5_V18(c, &v->x5))
      break;
    case 6:  // X6
      CHECK_ERROR(_readJunctionV1X6_V18(c, &v->x6))
      break;
    case 7:  // X7
      CHECK_ERROR(_readJunctionV1X7_V18(c, &v->x7))
      break;
    case 8:  // X8
      CHECK_ERROR(_readJunctionV1X8_V18(c, &v->x8))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readKeys_V18(parser_context_t* c,
                             pd_Keys_V18_t* v){GEN_DEF_READARRAY(6 * 32)}

parser_error_t
    _readMemberCount_V18(parser_context_t* c, pd_MemberCount_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readMultiAssetId_V18(parser_context_t* c,
                                     pd_MultiAssetId_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Concrete
      CHECK_ERROR(_readMultiLocationV1_V18(c, &v->concrete))
      break;
    case 1:  // Abstract
      CHECK_ERROR(_substrate_readBytes(c, &v->abstract))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readMultiAssetV0_V18(parser_context_t* c,
                                     pd_MultiAssetV0_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // None
    case 1:  // All
    case 2:  // AllFungible
    case 3:  // AllNonFungible
      break;
    case 4:  // AllAbstractFungible
    case 5:  // AllAbstractNonFungible
      CHECK_ERROR(_substrate_readBytes(c, &v->abstract))
      break;
    case 6:  // AllConcreteFungible
    case 7:  // AllConcreteNonFungible
      CHECK_ERROR(_readMultiLocationV0_V18(c, &v->concrete))
      break;
    case 8:  // AbstractFungible
      CHECK_ERROR(_readAbstractFungible_V18(c, &v->abstractFungible))
      break;
    case 9:  // AbstractNonFungible
      CHECK_ERROR(_readAbstractNonFungible_V18(c, &v->abstractNonFungible))
      break;
    case 10:  // ConcreteFungible
      CHECK_ERROR(_readConcreteFungible_V18(c, &v->concreteFungible))
      break;
    case 11:  // ConcreteNonFungible
      CHECK_ERROR(_readConcreteNonFungible_V18(c, &v->concreteNonFungible))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readMultiAssetV1_V18(parser_context_t* c,
                                     pd_MultiAssetV1_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readMultiAssetId_V18(c, &v->assetId))
  CHECK_ERROR(_readFungibility_V18(c, &v->fungibility))
  return parser_ok;
}

parser_error_t _readMultiLocationV0_V18(parser_context_t* c,
                                        pd_MultiLocationV0_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readJunctionsV0_V18(c, &v->junctions))
  return parser_ok;
}

parser_error_t _readMultiLocationV1_V18(parser_context_t* c,
                                        pd_MultiLocationV1_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->parents))
  CHECK_ERROR(_readJunctionsV1_V18(c, &v->interior))
  return parser_ok;
}

parser_error_t _readMultiSignature_V18(parser_context_t* c,
                                       pd_MultiSignature_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_readEd25519Signature_V18(c, &v->ed25519))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_readSr25519Signature_V18(c, &v->sr25519))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_readEcdsaSignature_V18(c, &v->ecdsa))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readMultiSigner_V18(parser_context_t* c,
                                    pd_MultiSigner_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_readEd25519Public_V18(c, &v->ed25519))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_readSr25519Public_V18(c, &v->sr25519))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_readEcdsaPublic_V18(c, &v->ecdsa))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readNetworkId_V18(parser_context_t* c, pd_NetworkId_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Any
    case 2:  // Polkadot
    case 3:  // Kusama
      break;
    case 1:  // Index
      CHECK_ERROR(_substrate_readBytes(c, &v->named))
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _readOverweightIndex_V18(parser_context_t* c,
                                        pd_OverweightIndex_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt64(c, &v->value))
  return parser_ok;
}

parser_error_t _readParaId_V18(parser_context_t* c, pd_ParaId_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readPerbill_V18(parser_context_t* c, pd_Perbill_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readPercent_V18(parser_context_t* c, pd_Percent_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  return parser_ok;
}

parser_error_t _readPlurality_V18(parser_context_t* c, pd_Plurality_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readBodyId_V18(c, &v->id))
  CHECK_ERROR(_readBodyPart_V18(c, &v->part))
  return parser_ok;
}

parser_error_t _readPoolId_V18(parser_context_t* c, pd_PoolId_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readPoolState_V18(parser_context_t* c, pd_PoolState_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  return parser_ok;
}

parser_error_t _readProxyType_V18(parser_context_t* c, pd_ProxyType_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->value))
  if (v->value == 4 || v->value > 7) {
    return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readReferendumIndex_V18(parser_context_t* c,
                                        pd_ReferendumIndex_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readRegistrarIndex_V18(parser_context_t* c,
                                       pd_RegistrarIndex_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt32(c, &v->value))
  return parser_ok;
}

parser_error_t _readRewardDestination_V18(parser_context_t* c,
                                          pd_RewardDestination_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))

  if (v->value == 3) {
    CHECK_ERROR(_readAccountId_V18(c, &v->accountId))
  } else if (v->value > 4) {
    return parser_value_out_of_range;
  }
  return parser_ok;
}

parser_error_t _readSessionIndex_V18(parser_context_t* c,
                                     pd_SessionIndex_V18_t* v) {
  return _preadUInt32(c, &v->value);
}

parser_error_t _readSr25519Public_V18(parser_context_t* c,
                                      pd_Sr25519Public_V18_t* v){
    GEN_DEF_READARRAY(32)}

parser_error_t _readSr25519Signature_V18(parser_context_t* c,
                                         pd_Sr25519Signature_V18_t* v){
    GEN_DEF_READARRAY(64)}

parser_error_t _readTimepoint_V18(parser_context_t* c, pd_Timepoint_V18_t* v) {
  CHECK_ERROR(_readBlockNumber(c, &v->height))
  CHECK_ERROR(_readu32(c, &v->index))
  return parser_ok;
}

parser_error_t _readTupleAccountIdData_V18(parser_context_t* c,
                                           pd_TupleAccountIdData_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readAccountId_V18(c, &v->id));
  CHECK_ERROR(_readData(c, &v->data));
  return parser_ok;
}

parser_error_t _readTupleH256u32_V18(parser_context_t* c,
                                     pd_TupleH256u32_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readH256(c, &v->h256))
  CHECK_ERROR(_preadUInt32(c, &v->u32))
  return parser_ok;
}

parser_error_t _readValidatorPrefs_V18(parser_context_t* c,
                                       pd_ValidatorPrefs_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readCompactPerBill_V18(c, &v->commission));
  CHECK_ERROR(_readbool(c, &v->blocked))
  return parser_ok;
}

parser_error_t _readVestingInfo_V18(parser_context_t* c,
                                    pd_VestingInfo_V18_t* v) {
  CHECK_ERROR(_readBalanceOf(c, &v->locked))
  CHECK_ERROR(_readBalanceOf(c, &v->per_block))
  CHECK_ERROR(_readBlockNumber(c, &v->starting_block))
  return parser_ok;
}

parser_error_t _readVote_V18(parser_context_t* c, pd_Vote_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  const uint8_t aye = v->value & 0xF0;
  const uint8_t conviction = v->value & 0x0F;
  if ((aye == 0x80 || aye == 0x00) && conviction <= 0x06) {
    return parser_ok;
  }
  return parser_value_out_of_range;
}

parser_error_t _readWeightLimit_V18(parser_context_t* c,
                                    pd_WeightLimit_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_preadUInt8(c, &v->value))
  switch (v->value) {
    case 0:  // Unlimited
      break;
    case 1:  // Limited
      CHECK_ERROR(_readCompactu64(c, &v->limited))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _readWeight_V18(parser_context_t* c, pd_Weight_V18_t* v) {
  CHECK_INPUT()
  CHECK_ERROR(_readCompactu64(c, &v->refTime))
  CHECK_ERROR(_readCompactu64(c, &v->proofSize))
  return parser_ok;
}

parser_error_t _readu8_array_32_V18(parser_context_t* c,
                                    pd_u8_array_32_V18_t* v){
    GEN_DEF_READARRAY(32)}

parser_error_t
    _readVecAccountIdLookupOfT_V18(parser_context_t* c,
                                   pd_VecAccountIdLookupOfT_V18_t* v){
        GEN_DEF_READVECTOR(AccountIdLookupOfT_V18)}

parser_error_t
    _readVecAccountId_V18(parser_context_t* c, pd_VecAccountId_V18_t* v){
        GEN_DEF_READVECTOR(AccountId_V18)}

parser_error_t
    _readVecMultiAssetV0_V18(parser_context_t* c, pd_VecMultiAssetV0_V18_t* v){
        GEN_DEF_READVECTOR(MultiAssetV0_V18)}

parser_error_t
    _readVecMultiAssetV1_V18(parser_context_t* c, pd_VecMultiAssetV1_V18_t* v){
        GEN_DEF_READVECTOR(MultiAssetV1_V18)}

parser_error_t
    _readVecTupleAccountIdData_V18(parser_context_t* c,
                                   pd_VecTupleAccountIdData_V18_t* v){
        GEN_DEF_READVECTOR(TupleAccountIdData_V18)}

parser_error_t
    _readOptionAccountId_V18(parser_context_t* c, pd_OptionAccountId_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readAccountId_V18(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionMultiSignature_V18(parser_context_t* c,
                                             pd_OptionMultiSignature_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readMultiSignature_V18(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionMultiSigner_V18(parser_context_t* c,
                                          pd_OptionMultiSigner_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readMultiSigner_V18(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionProxyType_V18(parser_context_t* c,
                                        pd_OptionProxyType_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readProxyType_V18(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionReferendumIndex_V18(
    parser_context_t* c, pd_OptionReferendumIndex_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readReferendumIndex_V18(c, &v->contained))
  }
  return parser_ok;
}

parser_error_t _readOptionTimepoint_V18(parser_context_t* c,
                                        pd_OptionTimepoint_V18_t* v) {
  CHECK_ERROR(_preadUInt8(c, &v->some))
  if (v->some > 0) {
    CHECK_ERROR(_readTimepoint_V18(c, &v->contained))
  }
  return parser_ok;
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

parser_error_t _toStringAbstractFungible_V18(const pd_AbstractFungible_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringBytes(&v->id, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(
      _toStringCompactu128(&v->amount, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringBytes(&v->id, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringCompactu128(&v->amount, outValue, outValueLen, pageIdx,
                                     &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringAbstractNonFungible_V18(
    const pd_AbstractNonFungible_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringBytes(&v->_class, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(_toStringAssetInstance_V18(&v->instance, outValue, outValueLen, 0,
                                         &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringBytes(&v->_class, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringAssetInstance_V18(&v->instance, outValue, outValueLen,
                                           pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringAccountId32_V18(const pd_AccountId32_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringNetworkId_V18(&v->networkId, outValue, outValueLen, 0,
                                     &pages[0]))
  CHECK_ERROR(
      _toStringu8_array_32_V18(&v->key, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringNetworkId_V18(&v->networkId, outValue, outValueLen,
                                       pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringu8_array_32_V18(&v->key, outValue, outValueLen,
                                         pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

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

parser_error_t _toStringAccountIndex64_V18(const pd_AccountIndex64_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringNetworkId_V18(&v->networkId, outValue, outValueLen, 0,
                                     &pages[0]))
  CHECK_ERROR(
      _toStringCompactu64(&v->index, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringNetworkId_V18(&v->networkId, outValue, outValueLen,
                                       pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringCompactu64(&v->index, outValue, outValueLen, pageIdx,
                                    &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringAccountIndex_V18(const pd_AccountIndex_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringAccountKey20_V18(const pd_AccountKey20_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringNetworkId_V18(&v->networkId, outValue, outValueLen, 0,
                                     &pages[0]))
  CHECK_ERROR(
      _toStringu8_array_20(&v->key, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringNetworkId_V18(&v->networkId, outValue, outValueLen,
                                       pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringu8_array_20(&v->key, outValue, outValueLen, pageIdx,
                                     &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringAccountVoteSplit_V18(const pd_AccountVoteSplit_V18_t* v,
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

parser_error_t _toStringAccountVoteStandard_V18(
    const pd_AccountVoteStandard_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  // First measure number of pages
  uint8_t pages[3];

  pages[0] = 1;
  CHECK_ERROR(_toStringVote_V18(&v->vote, outValue, outValueLen, &pages[1]))
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
    CHECK_ERROR(_toStringVote_V18(&v->vote, outValue, outValueLen, &pages[1]));
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

parser_error_t _toStringAccountVote_V18(const pd_AccountVote_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:
      CHECK_ERROR(_toStringAccountVoteStandard_V18(
          &v->voteStandard, outValue, outValueLen, pageIdx, pageCount));
      break;
    case 1:
      CHECK_ERROR(_toStringAccountVoteSplit_V18(
          &v->voteSplit, outValue, outValueLen, pageIdx, pageCount));
      break;
    default:
      return parser_unexpected_value;
  }

  return parser_ok;
}

parser_error_t _toStringAssetInstance_V18(const pd_AssetInstance_V18_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Undefined
      *pageCount = 1;
      snprintf(outValue, outValueLen, "Undefined");
      break;
    case 1:  // Index
      CHECK_ERROR(_toStringCompactu128(&v->index, outValue, outValueLen,
                                       pageIdx, pageCount))
      break;
    case 2:  // Array4
    {
      GEN_DEF_TOSTRING_ARRAY(4)
    }
    case 3:  // Array8
    {
      GEN_DEF_TOSTRING_ARRAY(8)
    }
    case 4:  // Array16
    {
      GEN_DEF_TOSTRING_ARRAY(16)
    }
    case 5:  // Array32
    {
      GEN_DEF_TOSTRING_ARRAY(32)
    }
    case 6:  // Blob
    {
      CHECK_ERROR(
          _toStringBytes(&v->blob, outValue, outValueLen, pageIdx, pageCount))
      break;
    }
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringBodyId_V18(const pd_BodyId_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // Unit
      snprintf(outValue, outValueLen, "Unit");
      break;
    case 1:  // Named
      CHECK_ERROR(
          _toStringBytes(&v->named, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 2:  // Index
      CHECK_ERROR(_toStringCompactu32(&v->index, outValue, outValueLen, pageIdx,
                                      pageCount))
      break;
    case 3:  // Executive
      snprintf(outValue, outValueLen, "Executive");
      break;
    case 4:  // Technical
      snprintf(outValue, outValueLen, "Technical");
      break;
    case 5:  // Legislative
      snprintf(outValue, outValueLen, "Legislative");
      break;
    case 6:  // Judicial
      snprintf(outValue, outValueLen, "Judicial");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringBodyPart_V18(const pd_BodyPart_V18_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // Voice
      snprintf(outValue, outValueLen, "Voice");
      break;
    case 1:  // Members
      CHECK_ERROR(_toStringCompactu32(&v->count, outValue, outValueLen, pageIdx,
                                      pageCount))
      break;
    case 2:  // Fraction
      CHECK_ERROR(_toStringFraction_V18(&v->fraction, outValue, outValueLen,
                                        pageIdx, pageCount))
      break;
    case 3:  // AtLeastProportion
      CHECK_ERROR(_toStringFraction_V18(&v->fraction, outValue, outValueLen,
                                        pageIdx, pageCount))
      break;
    case 4:  // MoreThanProportion
      CHECK_ERROR(_toStringFraction_V18(&v->fraction, outValue, outValueLen,
                                        pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringBondExtraBalanceOfT_V18(
    const pd_BondExtraBalanceOfT_V18_t* v, char* outValue, uint16_t outValueLen,
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

parser_error_t _toStringBoundedCallOfT_V18(const pd_BoundedCallOfT_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:
      CHECK_ERROR(
          _toStringH256(&v->legacy, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 1:
      CHECK_ERROR(_toStringBytes(&v->_inline, outValue, outValueLen, pageIdx,
                                 pageCount))
      break;
    case 2:
      CHECK_ERROR(_toStringTupleH256u32_V18(&v->lookup, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringBoxVersionedMultiAssets_V18(
    const pd_BoxVersionedMultiAssets_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // V0
      CHECK_ERROR(_toStringVecMultiAssetV0_V18(&v->vecMultiassetV0, outValue,
                                               outValueLen, pageIdx, pageCount))
      break;
    case 1:  // V1
      CHECK_ERROR(_toStringVecMultiAssetV1_V18(&v->vecMultiassetV1, outValue,
                                               outValueLen, pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringBoxVersionedMultiLocation_V18(
    const pd_BoxVersionedMultiLocation_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // V0
      CHECK_ERROR(_toStringMultiLocationV0_V18(&v->multilocationV0, outValue,
                                               outValueLen, pageIdx, pageCount))
      break;
    case 1:  // V1
      CHECK_ERROR(_toStringMultiLocationV1_V18(&v->multilocationV1, outValue,
                                               outValueLen, pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringCompactAccountIndex_V18(
    const pd_CompactAccountIndex_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringCompactInt(&v->value, 0, false, "", "", outValue, outValueLen,
                             pageIdx, pageCount);
}

parser_error_t _toStringCompactPerBill_V18(const pd_CompactPerBill_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  // 9 but shift 2 to show as percentage
  return _toStringCompactInt(&v->value, 7, false, "%", "", outValue,
                             outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringConcreteFungible_V18(const pd_ConcreteFungible_V18_t* v,
                                             char* outValue,
                                             uint16_t outValueLen,
                                             uint8_t pageIdx,
                                             uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(
      _toStringMultiLocationV0_V18(&v->id, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(
      _toStringCompactBalance(&v->amount, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringMultiLocationV0_V18(&v->id, outValue, outValueLen,
                                             pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringCompactBalance(&v->amount, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringConcreteNonFungible_V18(
    const pd_ConcreteNonFungible_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringMultiLocationV0_V18(&v->_class, outValue, outValueLen, 0,
                                           &pages[0]))
  CHECK_ERROR(_toStringAssetInstance_V18(&v->instance, outValue, outValueLen, 0,
                                         &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringMultiLocationV0_V18(&v->_class, outValue, outValueLen,
                                             pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringAssetInstance_V18(&v->instance, outValue, outValueLen,
                                           pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringConfigOpAccountId_V18(
    const pd_ConfigOpAccountId_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Noop");
      break;
    case 1:
      CHECK_ERROR(_toStringAccountId_V18(&v->set, outValue, outValueLen,
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

parser_error_t _toStringConfigOpBalanceOfT_V18(
    const pd_ConfigOpBalanceOfT_V18_t* v, char* outValue, uint16_t outValueLen,
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

parser_error_t _toStringConfigOpPerbill_V18(const pd_ConfigOpPerbill_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Noop");
      break;
    case 1:
      CHECK_ERROR(_toStringPerbill_V18(&v->set, outValue, outValueLen, pageIdx,
                                       pageCount))
      break;
    case 2:
      snprintf(outValue, outValueLen, "Remove");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringConfigOpPercent_V18(const pd_ConfigOpPercent_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:
      snprintf(outValue, outValueLen, "Noop");
      break;
    case 1:
      CHECK_ERROR(_toStringPercent_V18(&v->set, outValue, outValueLen, pageIdx,
                                       pageCount))
      break;
    case 2:
      snprintf(outValue, outValueLen, "Remove");
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringConfigOpu32_V18(const pd_ConfigOpu32_V18_t* v,
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

parser_error_t _toStringEcdsaPublic_V18(const pd_EcdsaPublic_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(33)}

parser_error_t
    _toStringEcdsaSignature_V18(const pd_EcdsaSignature_V18_t* v,
                                char* outValue, uint16_t outValueLen,
                                uint8_t pageIdx,
                                uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(65)}

parser_error_t
    _toStringEd25519Public_V18(const pd_Ed25519Public_V18_t* v, char* outValue,
                               uint16_t outValueLen, uint8_t pageIdx,
                               uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(32)}

parser_error_t
    _toStringEd25519Signature_V18(const pd_Ed25519Signature_V18_t* v,
                                  char* outValue, uint16_t outValueLen,
                                  uint8_t pageIdx, uint8_t* pageCount){
        GEN_DEF_TOSTRING_ARRAY(64)}

parser_error_t _toStringEraIndex_V18(const pd_EraIndex_V18_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringEthereumAddress_V18(
    const pd_EthereumAddress_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount){GEN_DEF_TOSTRING_ARRAY(20)}

parser_error_t _toStringFraction_V18(const pd_Fraction_V18_t* v, char* outValue,
                                     uint16_t outValueLen, uint8_t pageIdx,
                                     uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringCompactu32(&v->nom, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(
      _toStringCompactu32(&v->denom, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringCompactu32(&v->nom, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringCompactu32(&v->denom, outValue, outValueLen, pageIdx,
                                    &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringFungibility_V18(const pd_Fungibility_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Undefined
      CHECK_ERROR(_toStringCompactu128(&v->fungible, outValue, outValueLen,
                                       pageIdx, pageCount))
      break;
    case 1:  // Index
      CHECK_ERROR(_toStringAssetInstance_V18(&v->nonFungible, outValue,
                                             outValueLen, pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringIdentityInfo_V18(const pd_IdentityInfo_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[9] = {0};
  CHECK_ERROR(_toStringVecTupleDataData(&v->additional, outValue, outValueLen,
                                        0, &pages[0]))
  CHECK_ERROR(_toStringData(&v->display, outValue, outValueLen, 0, &pages[1]))
  CHECK_ERROR(_toStringData(&v->legal, outValue, outValueLen, 0, &pages[2]))
  CHECK_ERROR(_toStringData(&v->web, outValue, outValueLen, 0, &pages[3]))
  CHECK_ERROR(_toStringData(&v->riot, outValue, outValueLen, 0, &pages[4]))
  CHECK_ERROR(_toStringData(&v->email, outValue, outValueLen, 0, &pages[5]))
  CHECK_ERROR(_toStringOptionu8_array_20(&v->pgp_fingerprint, outValue,
                                         outValueLen, 0, &pages[6]))
  CHECK_ERROR(_toStringData(&v->image, outValue, outValueLen, 0, &pages[7]))
  CHECK_ERROR(_toStringData(&v->twitter, outValue, outValueLen, 0, &pages[8]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringVecTupleDataData(&v->additional, outValue, outValueLen,
                                          pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(
        _toStringData(&v->display, outValue, outValueLen, pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(
        _toStringData(&v->legal, outValue, outValueLen, pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(
        _toStringData(&v->web, outValue, outValueLen, pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(
        _toStringData(&v->riot, outValue, outValueLen, pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(
        _toStringData(&v->email, outValue, outValueLen, pageIdx, &pages[5]))
    return parser_ok;
  }
  pageIdx -= pages[5];

  if (pageIdx < pages[6]) {
    CHECK_ERROR(_toStringOptionu8_array_20(&v->pgp_fingerprint, outValue,
                                           outValueLen, pageIdx, &pages[6]))
    return parser_ok;
  }
  pageIdx -= pages[6];

  if (pageIdx < pages[7]) {
    CHECK_ERROR(
        _toStringData(&v->image, outValue, outValueLen, pageIdx, &pages[7]))
    return parser_ok;
  }
  pageIdx -= pages[7];

  if (pageIdx < pages[8]) {
    CHECK_ERROR(
        _toStringData(&v->twitter, outValue, outValueLen, pageIdx, &pages[8]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJudgementBalanceOfT_V18(
    const pd_JudgementBalanceOfT_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // Unknown
      snprintf(outValue, outValueLen, "Unknown");
      break;
    case 2:  // Reasonable
      snprintf(outValue, outValueLen, "Reasonable");
      break;
    case 3:  // KnownGood
      snprintf(outValue, outValueLen, "KnownGood");
      break;
    case 4:  // OutOfDate
      snprintf(outValue, outValueLen, "OutOfDate");
      break;
    case 5:  // LowQuality
      snprintf(outValue, outValueLen, "LowQuality");
      break;
    case 6:  // Erroneous
      snprintf(outValue, outValueLen, "Erroneous");
      break;
    case 1:  // FeePaid
      CHECK_ERROR(_substrate_toStringBalance(&v->feePaid, outValue, outValueLen,
                                             pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringJunctionV0X1_V18(const pd_JunctionV0X1_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[1] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction, outValue, outValueLen, 0,
                                      &pages[0]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X2_V18(const pd_JunctionV0X2_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X3_V18(const pd_JunctionV0X3_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[3] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X4_V18(const pd_JunctionV0X4_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[4] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X5_V18(const pd_JunctionV0X5_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[5] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X6_V18(const pd_JunctionV0X6_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[6] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction5, outValue, outValueLen, 0,
                                      &pages[5]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction5, outValue, outValueLen,
                                        pageIdx, &pages[5]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X7_V18(const pd_JunctionV0X7_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[7] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction5, outValue, outValueLen, 0,
                                      &pages[5]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction6, outValue, outValueLen, 0,
                                      &pages[6]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction5, outValue, outValueLen,
                                        pageIdx, &pages[5]))
    return parser_ok;
  }
  pageIdx -= pages[5];

  if (pageIdx < pages[6]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction6, outValue, outValueLen,
                                        pageIdx, &pages[6]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0X8_V18(const pd_JunctionV0X8_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[8] = {0};
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction5, outValue, outValueLen, 0,
                                      &pages[5]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction6, outValue, outValueLen, 0,
                                      &pages[6]))
  CHECK_ERROR(_toStringJunctionV0_V18(&v->junction7, outValue, outValueLen, 0,
                                      &pages[7]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction5, outValue, outValueLen,
                                        pageIdx, &pages[5]))
    return parser_ok;
  }
  pageIdx -= pages[5];

  if (pageIdx < pages[6]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction6, outValue, outValueLen,
                                        pageIdx, &pages[6]))
    return parser_ok;
  }
  pageIdx -= pages[6];

  if (pageIdx < pages[7]) {
    CHECK_ERROR(_toStringJunctionV0_V18(&v->junction7, outValue, outValueLen,
                                        pageIdx, &pages[7]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV0_V18(const pd_JunctionV0_V18_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // Parent
      snprintf(outValue, outValueLen, "Parent");
      break;
    case 1:  // Parachain
      CHECK_ERROR(_toStringCompactu32(&v->parachain, outValue, outValueLen,
                                      pageIdx, pageCount))
      break;
    case 2:  // AccountId32
      CHECK_ERROR(_toStringAccountId32_V18(&v->accountId32, outValue,
                                           outValueLen, pageIdx, pageCount))
      break;
    case 3:  // AccountIndex64
      CHECK_ERROR(_toStringAccountIndex64_V18(&v->accountIndex64, outValue,
                                              outValueLen, pageIdx, pageCount))
      break;
    case 4:  // AccountKey20
      CHECK_ERROR(_toStringAccountKey20_V18(&v->accountKey20, outValue,
                                            outValueLen, pageIdx, pageCount))
      break;
    case 5:  // PalletInstance
      CHECK_ERROR(_toStringu8(&v->palletInstance, outValue, outValueLen,
                              pageIdx, pageCount))
      break;
    case 6:  // GeneralIndex
      CHECK_ERROR(_toStringCompactu128(&v->generalIndex, outValue, outValueLen,
                                       pageIdx, pageCount))
      break;
    case 7:  // GeneralKey
      CHECK_ERROR(_toStringBytes(&v->generalKey, outValue, outValueLen, pageIdx,
                                 pageCount))
      break;
    case 8:  // OnlyChild
      snprintf(outValue, outValueLen, "OnlyChild");
      break;
    case 9:  // Plurality
      CHECK_ERROR(_toStringPlurality_V18(&v->plurality, outValue, outValueLen,
                                         pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringJunctionV1X1_V18(const pd_JunctionV1X1_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[1] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction, outValue, outValueLen, 0,
                                      &pages[0]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X2_V18(const pd_JunctionV1X2_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X3_V18(const pd_JunctionV1X3_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[3] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X4_V18(const pd_JunctionV1X4_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[4] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X5_V18(const pd_JunctionV1X5_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[5] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X6_V18(const pd_JunctionV1X6_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[6] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction5, outValue, outValueLen, 0,
                                      &pages[5]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction5, outValue, outValueLen,
                                        pageIdx, &pages[5]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X7_V18(const pd_JunctionV1X7_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[7] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction5, outValue, outValueLen, 0,
                                      &pages[5]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction6, outValue, outValueLen, 0,
                                      &pages[6]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction5, outValue, outValueLen,
                                        pageIdx, &pages[5]))
    return parser_ok;
  }
  pageIdx -= pages[5];

  if (pageIdx < pages[6]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction6, outValue, outValueLen,
                                        pageIdx, &pages[6]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1X8_V18(const pd_JunctionV1X8_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[8] = {0};
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen, 0,
                                      &pages[0]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen, 0,
                                      &pages[1]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen, 0,
                                      &pages[2]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen, 0,
                                      &pages[3]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen, 0,
                                      &pages[4]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction5, outValue, outValueLen, 0,
                                      &pages[5]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction6, outValue, outValueLen, 0,
                                      &pages[6]))
  CHECK_ERROR(_toStringJunctionV1_V18(&v->junction7, outValue, outValueLen, 0,
                                      &pages[7]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction0, outValue, outValueLen,
                                        pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction1, outValue, outValueLen,
                                        pageIdx, &pages[1]))
    return parser_ok;
  }
  pageIdx -= pages[1];

  if (pageIdx < pages[2]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction2, outValue, outValueLen,
                                        pageIdx, &pages[2]))
    return parser_ok;
  }
  pageIdx -= pages[2];

  if (pageIdx < pages[3]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction3, outValue, outValueLen,
                                        pageIdx, &pages[3]))
    return parser_ok;
  }
  pageIdx -= pages[3];

  if (pageIdx < pages[4]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction4, outValue, outValueLen,
                                        pageIdx, &pages[4]))
    return parser_ok;
  }
  pageIdx -= pages[4];

  if (pageIdx < pages[5]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction5, outValue, outValueLen,
                                        pageIdx, &pages[5]))
    return parser_ok;
  }
  pageIdx -= pages[5];

  if (pageIdx < pages[6]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction6, outValue, outValueLen,
                                        pageIdx, &pages[6]))
    return parser_ok;
  }
  pageIdx -= pages[6];

  if (pageIdx < pages[7]) {
    CHECK_ERROR(_toStringJunctionV1_V18(&v->junction7, outValue, outValueLen,
                                        pageIdx, &pages[7]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringJunctionV1_V18(const pd_JunctionV1_V18_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // Parachain
      CHECK_ERROR(_toStringCompactu32(&v->parachain, outValue, outValueLen,
                                      pageIdx, pageCount))
      break;
    case 1:  // AccountId32
      CHECK_ERROR(_toStringAccountId32_V18(&v->accountId32, outValue,
                                           outValueLen, pageIdx, pageCount))
      break;
    case 2:  // AccountIndex64
      CHECK_ERROR(_toStringAccountIndex64_V18(&v->accountIndex64, outValue,
                                              outValueLen, pageIdx, pageCount))
      break;
    case 3:  // AccountKey20
      CHECK_ERROR(_toStringAccountKey20_V18(&v->accountKey20, outValue,
                                            outValueLen, pageIdx, pageCount))
      break;
    case 4:  // PalletInstance
      CHECK_ERROR(_toStringu8(&v->palletInstance, outValue, outValueLen,
                              pageIdx, pageCount))
      break;
    case 5:  // GeneralIndex
      CHECK_ERROR(_toStringCompactu128(&v->generalIndex, outValue, outValueLen,
                                       pageIdx, pageCount))
      break;
    case 6:  // GeneralKey
      CHECK_ERROR(_toStringBytes(&v->generalKey, outValue, outValueLen, pageIdx,
                                 pageCount))
      break;
    case 7:  // OnlyChild
      snprintf(outValue, outValueLen, "OnlyChild");
      break;
    case 8:  // Plurality
      CHECK_ERROR(_toStringPlurality_V18(&v->plurality, outValue, outValueLen,
                                         pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringJunctionsV0_V18(const pd_JunctionsV0_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Null
      *pageCount = 1;
      snprintf(outValue, outValueLen, "Null");
      break;
    case 1:  // X1
      CHECK_ERROR(_toStringJunctionV0X1_V18(&v->x1, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 2:  // X2
      CHECK_ERROR(_toStringJunctionV0X2_V18(&v->x2, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 3:  // X3
      CHECK_ERROR(_toStringJunctionV0X3_V18(&v->x3, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 4:  // X4
      CHECK_ERROR(_toStringJunctionV0X4_V18(&v->x4, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 5:  // X5
      CHECK_ERROR(_toStringJunctionV0X5_V18(&v->x5, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 6:  // X6
      CHECK_ERROR(_toStringJunctionV0X6_V18(&v->x6, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 7:  // X7
      CHECK_ERROR(_toStringJunctionV0X7_V18(&v->x7, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 8:  // X8
      CHECK_ERROR(_toStringJunctionV0X8_V18(&v->x8, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringJunctionsV1_V18(const pd_JunctionsV1_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Here
      *pageCount = 1;
      snprintf(outValue, outValueLen, "Here");
      break;
    case 1:  // X1
      CHECK_ERROR(_toStringJunctionV1X1_V18(&v->x1, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 2:  // X2
      CHECK_ERROR(_toStringJunctionV1X2_V18(&v->x2, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 3:  // X3
      CHECK_ERROR(_toStringJunctionV1X3_V18(&v->x3, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 4:  // X4
      CHECK_ERROR(_toStringJunctionV1X4_V18(&v->x4, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 5:  // X5
      CHECK_ERROR(_toStringJunctionV1X5_V18(&v->x5, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 6:  // X6
      CHECK_ERROR(_toStringJunctionV1X6_V18(&v->x6, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 7:  // X7
      CHECK_ERROR(_toStringJunctionV1X7_V18(&v->x7, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    case 8:  // X8
      CHECK_ERROR(_toStringJunctionV1X8_V18(&v->x8, outValue, outValueLen,
                                            pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringKeys_V18(const pd_Keys_V18_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t pageIdx,
                                 uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(6 * 32)}

parser_error_t _toStringMemberCount_V18(const pd_MemberCount_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringMultiAssetId_V18(const pd_MultiAssetId_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Concrete
      CHECK_ERROR(_toStringMultiLocationV1_V18(&v->concrete, outValue,
                                               outValueLen, pageIdx, pageCount))
      break;
    case 1:  // Abstract
      CHECK_ERROR(_toStringBytes(&v->abstract, outValue, outValueLen, pageIdx,
                                 pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringMultiAssetV0_V18(const pd_MultiAssetV0_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  *pageCount = 1;
  switch (v->value) {
    case 0:  // None
      snprintf(outValue, outValueLen, "None");
      break;
    case 1:  // All
      snprintf(outValue, outValueLen, "All");
      break;
    case 2:  // AllFungible
      snprintf(outValue, outValueLen, "AllFungible");
      break;
    case 3:  // AllNonFungible
      snprintf(outValue, outValueLen, "AllNonFungible");
      break;
    case 4:  // AllAbstractFungible
    case 5:  // AllAbstractNonFungible
      CHECK_ERROR(_toStringBytes(&v->abstract, outValue, outValueLen, pageIdx,
                                 pageCount))
      break;
    case 6:  // AllConcreteFungible
    case 7:  // AllConcreteNonFungible
      CHECK_ERROR(_toStringMultiLocationV0_V18(&v->concrete, outValue,
                                               outValueLen, pageIdx, pageCount))
      break;
    case 8:  // AbstractFungible
      CHECK_ERROR(_toStringAbstractFungible_V18(
          &v->abstractFungible, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 9:  // AbstractNonFungible
      CHECK_ERROR(_toStringAbstractNonFungible_V18(
          &v->abstractNonFungible, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 10:  // ConcreteFungible
      CHECK_ERROR(_toStringConcreteFungible_V18(
          &v->concreteFungible, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 11:  // ConcreteNonFungible
      CHECK_ERROR(_toStringConcreteNonFungible_V18(
          &v->concreteNonFungible, outValue, outValueLen, pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringMultiAssetV1_V18(const pd_MultiAssetV1_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringMultiAssetId_V18(&v->assetId, outValue, outValueLen, 0,
                                        &pages[0]))
  CHECK_ERROR(_toStringFungibility_V18(&v->fungibility, outValue, outValueLen,
                                       0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringMultiAssetId_V18(&v->assetId, outValue, outValueLen,
                                          pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringFungibility_V18(&v->fungibility, outValue, outValueLen,
                                         pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringMultiLocationV0_V18(const pd_MultiLocationV0_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  CHECK_ERROR(_toStringJunctionsV0_V18(&v->junctions, outValue, outValueLen,
                                       pageIdx, pageCount))
  return parser_ok;
}

parser_error_t _toStringMultiLocationV1_V18(const pd_MultiLocationV1_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringu8(&v->parents, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(_toStringJunctionsV1_V18(&v->interior, outValue, outValueLen, 0,
                                       &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringu8(&v->parents, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringJunctionsV1_V18(&v->interior, outValue, outValueLen,
                                         pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringMultiSignature_V18(const pd_MultiSignature_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_toStringEd25519Signature_V18(
          &v->ed25519, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_toStringSr25519Signature_V18(
          &v->sr25519, outValue, outValueLen, pageIdx, pageCount))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_toStringEcdsaSignature_V18(&v->ecdsa, outValue, outValueLen,
                                              pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
}

parser_error_t _toStringMultiSigner_V18(const pd_MultiSigner_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Ed25519
      CHECK_ERROR(_toStringEd25519Public_V18(&v->ed25519, outValue, outValueLen,
                                             pageIdx, pageCount))
      break;
    case 1:  // Sr25519
      CHECK_ERROR(_toStringSr25519Public_V18(&v->sr25519, outValue, outValueLen,
                                             pageIdx, pageCount))
      break;
    case 2:  // Ecdsa
      CHECK_ERROR(_toStringEcdsaPublic_V18(&v->ecdsa, outValue, outValueLen,
                                           pageIdx, pageCount))
      break;
    default:
      return parser_not_supported;
  }

  return parser_ok;
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

parser_error_t _toStringOverweightIndex_V18(const pd_OverweightIndex_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  return _toStringu64(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringParaId_V18(const pd_ParaId_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringPerbill_V18(const pd_Perbill_V18_t* v, char* outValue,
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

parser_error_t _toStringPercent_V18(const pd_Percent_V18_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  char bufferUI[51];
  char bufferRatio[50];

  uint64_to_str(bufferRatio, sizeof(bufferRatio), v->value);

  snprintf(bufferUI, sizeof(bufferUI), "%s%%", bufferRatio);
  pageString(outValue, outValueLen, bufferUI, pageIdx, pageCount);
  return parser_ok;
}

parser_error_t _toStringPlurality_V18(const pd_Plurality_V18_t* v,
                                      char* outValue, uint16_t outValueLen,
                                      uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringBodyId_V18(&v->id, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(
      _toStringBodyPart_V18(&v->part, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringBodyId_V18(&v->id, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringBodyPart_V18(&v->part, outValue, outValueLen, pageIdx,
                                      &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringPoolId_V18(const pd_PoolId_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringProxyType_V18(const pd_ProxyType_V18_t* v,
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

parser_error_t _toStringReferendumIndex_V18(const pd_ReferendumIndex_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringRegistrarIndex_V18(const pd_RegistrarIndex_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringRewardDestination_V18(
    const pd_RewardDestination_V18_t* v, char* outValue, uint16_t outValueLen,
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
      CHECK_ERROR(_toStringAccountId_V18(&v->accountId, outValue, outValueLen,
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

parser_error_t _toStringSessionIndex_V18(const pd_SessionIndex_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  return _toStringu32(&v->value, outValue, outValueLen, pageIdx, pageCount);
}

parser_error_t _toStringSr25519Public_V18(const pd_Sr25519Public_V18_t* v,
                                          char* outValue, uint16_t outValueLen,
                                          uint8_t pageIdx, uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(32)}

parser_error_t
    _toStringSr25519Signature_V18(const pd_Sr25519Signature_V18_t* v,
                                  char* outValue, uint16_t outValueLen,
                                  uint8_t pageIdx, uint8_t* pageCount){
        GEN_DEF_TOSTRING_ARRAY(64)}

parser_error_t _toStringTimepoint_V18(const pd_Timepoint_V18_t* v,
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

parser_error_t _toStringTupleAccountIdData_V18(
    const pd_TupleAccountIdData_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(
      _toStringAccountId_V18(&v->id, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(_toStringData(&v->data, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringAccountId_V18(&v->id, outValue, outValueLen, pageIdx,
                                       &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(
        _toStringData(&v->data, outValue, outValueLen, pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringTupleH256u32_V18(const pd_TupleH256u32_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringH256(&v->h256, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(_toStringu32(&v->u32, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(
        _toStringH256(&v->h256, outValue, outValueLen, pageIdx, &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(
        _toStringu32(&v->u32, outValue, outValueLen, pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringValidatorPrefs_V18(const pd_ValidatorPrefs_V18_t* v,
                                           char* outValue, uint16_t outValueLen,
                                           uint8_t pageIdx,
                                           uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(_toStringCompactPerBill_V18(&v->commission, outValue, outValueLen,
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
    CHECK_ERROR(_toStringCompactPerBill_V18(&v->commission, outValue,
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

parser_error_t _toStringVestingInfo_V18(const pd_VestingInfo_V18_t* v,
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

parser_error_t _toStringVote_V18(const pd_Vote_V18_t* v, char* outValue,
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

parser_error_t _toStringWeightLimit_V18(const pd_WeightLimit_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()
  switch (v->value) {
    case 0:  // Unlimited
      *pageCount = 1;
      snprintf(outValue, outValueLen, "Unlimited");
      break;
    case 1:  // Limited
      CHECK_ERROR(_toStringCompactu64(&v->limited, outValue, outValueLen,
                                      pageIdx, pageCount))
      break;
    default:
      return parser_unexpected_value;
  }
  return parser_ok;
}

parser_error_t _toStringWeight_V18(const pd_Weight_V18_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  // First measure number of pages
  uint8_t pages[2] = {0};
  CHECK_ERROR(
      _toStringCompactu64(&v->refTime, outValue, outValueLen, 0, &pages[0]))
  CHECK_ERROR(
      _toStringCompactu64(&v->proofSize, outValue, outValueLen, 0, &pages[1]))

  *pageCount = 0;
  for (uint8_t i = 0; i < (uint8_t)sizeof(pages); i++) {
    *pageCount += pages[i];
  }

  if (pageIdx > *pageCount) {
    return parser_display_idx_out_of_range;
  }

  if (pageIdx < pages[0]) {
    CHECK_ERROR(_toStringCompactu64(&v->refTime, outValue, outValueLen, pageIdx,
                                    &pages[0]))
    return parser_ok;
  }
  pageIdx -= pages[0];

  if (pageIdx < pages[1]) {
    CHECK_ERROR(_toStringCompactu64(&v->proofSize, outValue, outValueLen,
                                    pageIdx, &pages[1]))
    return parser_ok;
  }

  return parser_display_idx_out_of_range;
}

parser_error_t _toStringu8_array_32_V18(const pd_u8_array_32_V18_t* v,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount){
    GEN_DEF_TOSTRING_ARRAY(32)}

parser_error_t
    _toStringVecAccountIdLookupOfT_V18(const pd_VecAccountIdLookupOfT_V18_t* v,
                                       char* outValue, uint16_t outValueLen,
                                       uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(AccountIdLookupOfT_V18);
}

parser_error_t _toStringVecAccountId_V18(const pd_VecAccountId_V18_t* v,
                                         char* outValue, uint16_t outValueLen,
                                         uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(AccountId_V18);
}

parser_error_t _toStringVecMultiAssetV0_V18(const pd_VecMultiAssetV0_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(MultiAssetV0_V18);
}

parser_error_t _toStringVecMultiAssetV1_V18(const pd_VecMultiAssetV1_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(MultiAssetV1_V18);
}

parser_error_t _toStringVecTupleAccountIdData_V18(
    const pd_VecTupleAccountIdData_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  GEN_DEF_TOSTRING_VECTOR(TupleAccountIdData_V18);
}

parser_error_t _toStringOptionAccountId_V18(const pd_OptionAccountId_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringAccountId_V18(&v->contained, outValue, outValueLen,
                                       pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionMultiSignature_V18(
    const pd_OptionMultiSignature_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringMultiSignature_V18(&v->contained, outValue,
                                            outValueLen, pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionMultiSigner_V18(
    const pd_OptionMultiSigner_V18_t* v, char* outValue, uint16_t outValueLen,
    uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringMultiSigner_V18(&v->contained, outValue, outValueLen,
                                         pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionReferendumIndex_V18(
    const pd_OptionReferendumIndex_V18_t* v, char* outValue,
    uint16_t outValueLen, uint8_t pageIdx, uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringReferendumIndex_V18(&v->contained, outValue,
                                             outValueLen, pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}

parser_error_t _toStringOptionTimepoint_V18(const pd_OptionTimepoint_V18_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount) {
  CLEAN_AND_CHECK()

  *pageCount = 1;
  if (v->some > 0) {
    CHECK_ERROR(_toStringTimepoint_V18(&v->contained, outValue, outValueLen,
                                       pageIdx, pageCount));
  } else {
    snprintf(outValue, outValueLen, "None");
  }
  return parser_ok;
}
