#ifndef __POLKADOT_SUBSTRATE_METHODS_V18_H__
#define __POLKADOT_SUBSTRATE_METHODS_V18_H__

#include <stddef.h>
#include <stdint.h>
#include "substrate_types.h"
#include "substrate_types_V18.h"

#define PD_CALL_SYSTEM_V18 0
#define PD_CALL_PREIMAGE_V18 10
#define PD_CALL_TIMESTAMP_V18 3
#define PD_CALL_INDICES_V18 4
#define PD_CALL_BALANCES_V18 5
#define PD_CALL_STAKING_V18 7
#define PD_CALL_SESSION_V18 9
#define PD_CALL_DEMOCRACY_V18 14
#define PD_CALL_COUNCIL_V18 15
#define PD_CALL_TECHNICALCOMMITTEE_V18 16
#define PD_CALL_PHRAGMENELECTION_V18 17
#define PD_CALL_TECHNICALMEMBERSHIP_V18 18
#define PD_CALL_TREASURY_V18 19
#define PD_CALL_CLAIMS_V18 24
#define PD_CALL_VESTING_V18 25
#define PD_CALL_UTILITY_V18 26
#define PD_CALL_IDENTITY_V18 28
#define PD_CALL_PROXY_V18 29
#define PD_CALL_MULTISIG_V18 30
#define PD_CALL_BOUNTIES_V18 34
#define PD_CALL_CHILDBOUNTIES_V18 38
#define PD_CALL_TIPS_V18 35
#define PD_CALL_VOTERLIST_V18 37
#define PD_CALL_NOMINATIONPOOLS_V18 39
#define PD_CALL_FASTUNSTAKE_V18 40
#define PD_CALL_CONFIGURATION_V18 51
#define PD_CALL_INITIALIZER_V18 57
#define PD_CALL_UMP_V18 59
#define PD_CALL_PARASDISPUTES_V18 62
#define PD_CALL_AUCTIONS_V18 72
#define PD_CALL_CROWDLOAN_V18 73
#define PD_CALL_XCMPALLET_V18 99

#define PD_CALL_BALANCES_TRANSFER_ALL_V18 4
typedef struct {
  pd_AccountIdLookupOfT_V18_t dest;
  pd_bool_t keep_alive;
} pd_balances_transfer_all_V18_t;

#define PD_CALL_STAKING_BOND_EXTRA_V18 1
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_bond_extra_V18_t;

#define PD_CALL_STAKING_WITHDRAW_UNBONDED_V18 3
typedef struct {
  pd_u32_t num_slashing_spans;
} pd_staking_withdraw_unbonded_V18_t;

#define PD_CALL_STAKING_VALIDATE_V18 4
typedef struct {
  pd_ValidatorPrefs_V18_t prefs;
} pd_staking_validate_V18_t;

#define PD_CALL_STAKING_SET_PAYEE_V18 7
typedef struct {
  pd_RewardDestination_V18_t payee;
} pd_staking_set_payee_V18_t;

#define PD_CALL_STAKING_PAYOUT_STAKERS_V18 18
typedef struct {
  pd_AccountId_V18_t validator_stash;
  pd_EraIndex_V18_t era;
} pd_staking_payout_stakers_V18_t;

#define PD_CALL_STAKING_REBOND_V18 19
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_rebond_V18_t;

#define PD_CALL_SESSION_SET_KEYS_V18 0
typedef struct {
  pd_Keys_V18_t keys;
  pd_Bytes_t proof;
} pd_session_set_keys_V18_t;

#define PD_CALL_SESSION_PURGE_KEYS_V18 1
typedef struct {
} pd_session_purge_keys_V18_t;

#define PD_CALL_UTILITY_BATCH_V18 0
typedef struct {
  pd_VecCall_t calls;
} pd_utility_batch_V18_t;

#define PD_CALL_UTILITY_BATCH_ALL_V18 2
typedef struct {
  pd_VecCall_t calls;
} pd_utility_batch_all_V18_t;

#define PD_CALL_UTILITY_FORCE_BATCH_V18 4
typedef struct {
  pd_VecCall_t calls;
} pd_utility_force_batch_V18_t;

#define PD_CALL_CROWDLOAN_CREATE_V18 0
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t cap;
  pd_Compactu32_t first_period;
  pd_Compactu32_t last_period;
  pd_Compactu32_t end;
  pd_OptionMultiSigner_V18_t verifier;
} pd_crowdloan_create_V18_t;

#define PD_CALL_CROWDLOAN_CONTRIBUTE_V18 1
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t amount;
  pd_OptionMultiSignature_V18_t signature;
} pd_crowdloan_contribute_V18_t;

#define PD_CALL_CROWDLOAN_WITHDRAW_V18 2
typedef struct {
  pd_AccountId_V18_t who;
  pd_Compactu32_t index;
} pd_crowdloan_withdraw_V18_t;

#define PD_CALL_CROWDLOAN_REFUND_V18 3
typedef struct {
  pd_Compactu32_t index;
} pd_crowdloan_refund_V18_t;

#define PD_CALL_CROWDLOAN_DISSOLVE_V18 4
typedef struct {
  pd_Compactu32_t index;
} pd_crowdloan_dissolve_V18_t;

#define PD_CALL_CROWDLOAN_EDIT_V18 5
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t cap;
  pd_Compactu32_t first_period;
  pd_Compactu32_t last_period;
  pd_Compactu32_t end;
  pd_OptionMultiSigner_V18_t verifier;
} pd_crowdloan_edit_V18_t;

#define PD_CALL_CROWDLOAN_ADD_MEMO_V18 6
typedef struct {
  pd_ParaId_V18_t index;
  pd_Vecu8_t memo;
} pd_crowdloan_add_memo_V18_t;

#define PD_CALL_CROWDLOAN_POKE_V18 7
typedef struct {
  pd_ParaId_V18_t index;
} pd_crowdloan_poke_V18_t;

#define PD_CALL_CROWDLOAN_CONTRIBUTE_ALL_V18 8
typedef struct {
  pd_Compactu32_t index;
  pd_OptionMultiSignature_V18_t signature;
} pd_crowdloan_contribute_all_V18_t;

typedef union {
  pd_balances_transfer_all_V18_t balances_transfer_all_V18;
  pd_staking_bond_extra_V18_t staking_bond_extra_V18;
  pd_staking_withdraw_unbonded_V18_t staking_withdraw_unbonded_V18;
  pd_staking_validate_V18_t staking_validate_V18;
  pd_staking_set_payee_V18_t staking_set_payee_V18;
  pd_staking_payout_stakers_V18_t staking_payout_stakers_V18;
  pd_staking_rebond_V18_t staking_rebond_V18;
  pd_session_set_keys_V18_t session_set_keys_V18;
  pd_session_purge_keys_V18_t session_purge_keys_V18;
  pd_utility_batch_V18_t utility_batch_V18;
  pd_utility_batch_all_V18_t utility_batch_all_V18;
  pd_utility_force_batch_V18_t utility_force_batch_V18;
  pd_crowdloan_create_V18_t crowdloan_create_V18;
  pd_crowdloan_contribute_V18_t crowdloan_contribute_V18;
  pd_crowdloan_withdraw_V18_t crowdloan_withdraw_V18;
  pd_crowdloan_refund_V18_t crowdloan_refund_V18;
  pd_crowdloan_dissolve_V18_t crowdloan_dissolve_V18;
  pd_crowdloan_edit_V18_t crowdloan_edit_V18;
  pd_crowdloan_add_memo_V18_t crowdloan_add_memo_V18;
  pd_crowdloan_poke_V18_t crowdloan_poke_V18;
  pd_crowdloan_contribute_all_V18_t crowdloan_contribute_all_V18;
} pd_MethodBasic_V18_t;

#define PD_CALL_BALANCES_TRANSFER_V18 0
typedef struct {
  pd_AccountIdLookupOfT_V18_t dest;
  pd_CompactBalance_t amount;
} pd_balances_transfer_V18_t;

#define PD_CALL_BALANCES_FORCE_TRANSFER_V18 2
typedef struct {
  pd_AccountIdLookupOfT_V18_t source;
  pd_AccountIdLookupOfT_V18_t dest;
  pd_CompactBalance_t amount;
} pd_balances_force_transfer_V18_t;

#define PD_CALL_BALANCES_TRANSFER_KEEP_ALIVE_V18 3
typedef struct {
  pd_AccountIdLookupOfT_V18_t dest;
  pd_CompactBalance_t amount;
} pd_balances_transfer_keep_alive_V18_t;

#define PD_CALL_STAKING_BOND_V18 0
typedef struct {
  pd_AccountIdLookupOfT_V18_t controller;
  pd_CompactBalance_t amount;
  pd_RewardDestination_V18_t payee;
} pd_staking_bond_V18_t;

#define PD_CALL_STAKING_UNBOND_V18 2
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_unbond_V18_t;

#define PD_CALL_STAKING_NOMINATE_V18 5
typedef struct {
  pd_VecAccountIdLookupOfT_V18_t targets;
} pd_staking_nominate_V18_t;

#define PD_CALL_STAKING_CHILL_V18 6
typedef struct {
} pd_staking_chill_V18_t;

#define PD_CALL_STAKING_SET_CONTROLLER_V18 8
typedef struct {
  pd_AccountIdLookupOfT_V18_t controller;
} pd_staking_set_controller_V18_t;

typedef union {
  pd_balances_transfer_V18_t balances_transfer_V18;
  pd_balances_force_transfer_V18_t balances_force_transfer_V18;
  pd_balances_transfer_keep_alive_V18_t balances_transfer_keep_alive_V18;
  pd_staking_bond_V18_t staking_bond_V18;
  pd_staking_unbond_V18_t staking_unbond_V18;
  pd_staking_nominate_V18_t staking_nominate_V18;
  pd_staking_chill_V18_t staking_chill_V18;
  pd_staking_set_controller_V18_t staking_set_controller_V18;
} pd_MethodNested_V18_t;

typedef union {
  pd_MethodBasic_V18_t basic;
  pd_MethodNested_V18_t nested;
} pd_Method_V18_t;

#endif