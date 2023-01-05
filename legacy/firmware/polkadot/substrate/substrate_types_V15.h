#ifndef __POLKADOT_SUBSTRATE_TYPES_V15_H__
#define __POLKADOT_SUBSTRATE_TYPES_V15_H__

#include <stddef.h>
#include <stdint.h>
#include "substrate_types.h"

// Based
// https://github.com/paritytech/substrate/blob/master/node/primitives/src/lib.rs

typedef struct {
  const uint8_t* _ptr;
} pd_AccountId_V15_t;

typedef struct {
  compactInt_t value;
} pd_CompactAccountIndex_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_EcdsaPublic_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_EcdsaSignature_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_Ed25519Public_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_Ed25519Signature_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_Sr25519Public_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_Sr25519Signature_V15_t;

typedef struct {
  uint8_t value;
} pd_Vote_V15_t;

typedef struct {
  uint8_t value;
  union {
    pd_AccountId_V15_t id;
    pd_CompactAccountIndex_V15_t index;
    pd_Bytes_t raw;
    const uint8_t* _ptr;
  };
} pd_AccountIdLookupOfT_V15_t;

typedef struct {
  pd_BalanceOf_t aye;
  pd_BalanceOf_t nay;
} pd_AccountVoteSplit_V15_t;

typedef struct {
  pd_Vote_V15_t vote;
  pd_BalanceOf_t balance;
} pd_AccountVoteStandard_V15_t;

typedef struct {
  compactInt_t value;
} pd_CompactPerBill_V15_t;

typedef struct {
  uint8_t value;
  union {
    pd_Ed25519Signature_V15_t ed25519;
    pd_Sr25519Signature_V15_t sr25519;
    pd_EcdsaSignature_V15_t ecdsa;
  };
} pd_MultiSignature_V15_t;

typedef struct {
  uint8_t value;
  union {
    pd_Ed25519Public_V15_t ed25519;
    pd_Sr25519Public_V15_t sr25519;
    pd_EcdsaPublic_V15_t ecdsa;
  };
} pd_MultiSigner_V15_t;

typedef struct {
  uint8_t value;
} pd_ProxyType_V15_t;

typedef struct {
  pd_BlockNumber_t height;
  uint32_t index;
} pd_Timepoint_V15_t;

typedef struct {
  uint8_t value;
  union {
    pd_AccountVoteStandard_V15_t voteStandard;
    pd_AccountVoteSplit_V15_t voteSplit;
  };
} pd_AccountVote_V15_t;

typedef struct {
  uint8_t value;
  pd_Balance_t freeBalance;
} pd_BondExtraBalanceOfT_V15_t;

typedef struct {
  uint8_t value;
  pd_AccountId_V15_t set;
} pd_ConfigOpAccountId_V15_t;

typedef struct {
  uint8_t value;
  pd_Balance_t set;
} pd_ConfigOpBalanceOfT_V15_t;

typedef struct {
  pd_Call_t call;
} pd_OpaqueCall_V15_t;

typedef struct {
  uint8_t some;
  pd_MultiSignature_V15_t contained;
} pd_OptionMultiSignature_V15_t;

typedef struct {
  uint8_t some;
  pd_MultiSigner_V15_t contained;
} pd_OptionMultiSigner_V15_t;

typedef struct {
  uint8_t some;
  pd_Timepoint_V15_t contained;
} pd_OptionTimepoint_V15_t;

typedef struct {
  uint8_t value;
  pd_AccountId_V15_t accountId;
} pd_RewardDestination_V15_t;

typedef struct {
  pd_CompactPerBill_V15_t commission;
  pd_bool_t blocked;
} pd_ValidatorPrefs_V15_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
  uint64_t _lenBuffer;
} pd_VecAccountIdLookupOfT_V15_t;

typedef struct {
  pd_BalanceOf_t locked;
  pd_BalanceOf_t per_block;
  pd_BlockNumber_t starting_block;
} pd_VestingInfo_V15_t;

typedef struct {
  uint32_t value;
} pd_AccountIndex_V15_t;

typedef struct {
  uint8_t value;
  uint32_t set;
} pd_ConfigOpu32_V15_t;

typedef struct {
  uint8_t value;
} pd_Conviction_V15_t;

typedef struct {
  uint32_t value;
} pd_EraIndex_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_EthereumAddress_V15_t;

typedef struct {
  const uint8_t* _ptr;
} pd_Keys_V15_t;

typedef struct {
  uint32_t value;
} pd_MemberCount_V15_t;

typedef struct {
  uint8_t some;
  pd_AccountId_V15_t contained;
} pd_OptionAccountId_V15_t;

typedef struct {
  uint8_t some;
  pd_ProxyType_V15_t contained;
} pd_OptionProxyType_V15_t;

typedef struct {
  uint64_t value;
} pd_OverweightIndex_V15_t;

typedef struct {
  uint32_t value;
} pd_ParaId_V15_t;

typedef struct {
  uint32_t value;
} pd_Perbill_V15_t;

typedef struct {
  uint32_t value;
} pd_PoolId_V15_t;

typedef struct {
  uint8_t value;
} pd_PoolState_V15_t;

typedef struct {
  uint32_t value;
} pd_ReferendumIndex_V15_t;

typedef struct {
  uint32_t value;
} pd_RegistrarIndex_V15_t;

typedef struct {
  uint32_t value;
} pd_SessionIndex_V15_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
  uint64_t _lenBuffer;
} pd_VecAccountId_V15_t;

typedef struct {
  uint64_t value;
} pd_Weight_V15_t;

#endif
