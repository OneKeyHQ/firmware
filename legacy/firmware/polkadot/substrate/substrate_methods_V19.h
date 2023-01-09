#ifndef __POLKADOT_SUBSTRATE_METHODS_V19_H__
#define __POLKADOT_SUBSTRATE_METHODS_V19_H__

#include <stddef.h>
#include <stdint.h>
#include "substrate_types.h"
#include "substrate_types_V19.h"

#define PD_CALL_SYSTEM_V19 0
#define PD_CALL_PREIMAGE_V19 10
#define PD_CALL_TIMESTAMP_V19 3
#define PD_CALL_INDICES_V19 4
#define PD_CALL_BALANCES_V19 5
#define PD_CALL_STAKING_V19 7
#define PD_CALL_SESSION_V19 9
#define PD_CALL_DEMOCRACY_V19 14
#define PD_CALL_COUNCIL_V19 15
#define PD_CALL_TECHNICALCOMMITTEE_V19 16
#define PD_CALL_PHRAGMENELECTION_V19 17
#define PD_CALL_TECHNICALMEMBERSHIP_V19 18
#define PD_CALL_TREASURY_V19 19
#define PD_CALL_CLAIMS_V19 24
#define PD_CALL_VESTING_V19 25
#define PD_CALL_UTILITY_V19 26
#define PD_CALL_IDENTITY_V19 28
#define PD_CALL_PROXY_V19 29
#define PD_CALL_MULTISIG_V19 30
#define PD_CALL_BOUNTIES_V19 34
#define PD_CALL_CHILDBOUNTIES_V19 38
#define PD_CALL_TIPS_V19 35
#define PD_CALL_VOTERLIST_V19 37
#define PD_CALL_NOMINATIONPOOLS_V19 39
#define PD_CALL_FASTUNSTAKE_V19 40
#define PD_CALL_CONFIGURATION_V19 51
#define PD_CALL_INITIALIZER_V19 57
#define PD_CALL_UMP_V19 59
#define PD_CALL_PARASDISPUTES_V19 62
#define PD_CALL_AUCTIONS_V19 72
#define PD_CALL_CROWDLOAN_V19 73
#define PD_CALL_XCMPALLET_V19 99

#define PD_CALL_BALANCES_TRANSFER_ALL_V19 4
typedef struct {
  pd_AccountIdLookupOfT_V19_t dest;
  pd_bool_t keep_alive;
} pd_balances_transfer_all_V19_t;

#define PD_CALL_STAKING_BOND_EXTRA_V19 1
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_bond_extra_V19_t;

#define PD_CALL_STAKING_WITHDRAW_UNBONDED_V19 3
typedef struct {
  pd_u32_t num_slashing_spans;
} pd_staking_withdraw_unbonded_V19_t;

#define PD_CALL_STAKING_VALIDATE_V19 4
typedef struct {
  pd_ValidatorPrefs_V19_t prefs;
} pd_staking_validate_V19_t;

#define PD_CALL_STAKING_SET_PAYEE_V19 7
typedef struct {
  pd_RewardDestination_V19_t payee;
} pd_staking_set_payee_V19_t;

#define PD_CALL_STAKING_PAYOUT_STAKERS_V19 18
typedef struct {
  pd_AccountId_V19_t validator_stash;
  pd_EraIndex_V19_t era;
} pd_staking_payout_stakers_V19_t;

#define PD_CALL_STAKING_REBOND_V19 19
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_rebond_V19_t;

#define PD_CALL_SESSION_SET_KEYS_V19 0
typedef struct {
  pd_Keys_V19_t keys;
  pd_Bytes_t proof;
} pd_session_set_keys_V19_t;

#define PD_CALL_SESSION_PURGE_KEYS_V19 1
typedef struct {
} pd_session_purge_keys_V19_t;

#define PD_CALL_UTILITY_BATCH_V19 0
typedef struct {
  pd_VecCall_t calls;
} pd_utility_batch_V19_t;

#define PD_CALL_UTILITY_BATCH_ALL_V19 2
typedef struct {
  pd_VecCall_t calls;
} pd_utility_batch_all_V19_t;

#define PD_CALL_UTILITY_FORCE_BATCH_V19 4
typedef struct {
  pd_VecCall_t calls;
} pd_utility_force_batch_V19_t;

#define PD_CALL_CROWDLOAN_CREATE_V19 0
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t cap;
  pd_Compactu32_t first_period;
  pd_Compactu32_t last_period;
  pd_Compactu32_t end;
  pd_OptionMultiSigner_V19_t verifier;
} pd_crowdloan_create_V19_t;

#define PD_CALL_CROWDLOAN_CONTRIBUTE_V19 1
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t amount;
  pd_OptionMultiSignature_V19_t signature;
} pd_crowdloan_contribute_V19_t;

#define PD_CALL_CROWDLOAN_WITHDRAW_V19 2
typedef struct {
  pd_AccountId_V19_t who;
  pd_Compactu32_t index;
} pd_crowdloan_withdraw_V19_t;

#define PD_CALL_CROWDLOAN_REFUND_V19 3
typedef struct {
  pd_Compactu32_t index;
} pd_crowdloan_refund_V19_t;

#define PD_CALL_CROWDLOAN_DISSOLVE_V19 4
typedef struct {
  pd_Compactu32_t index;
} pd_crowdloan_dissolve_V19_t;

#define PD_CALL_CROWDLOAN_EDIT_V19 5
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t cap;
  pd_Compactu32_t first_period;
  pd_Compactu32_t last_period;
  pd_Compactu32_t end;
  pd_OptionMultiSigner_V19_t verifier;
} pd_crowdloan_edit_V19_t;

#define PD_CALL_CROWDLOAN_ADD_MEMO_V19 6
typedef struct {
  pd_ParaId_V19_t index;
  pd_Vecu8_t memo;
} pd_crowdloan_add_memo_V19_t;

#define PD_CALL_CROWDLOAN_POKE_V19 7
typedef struct {
  pd_ParaId_V19_t index;
} pd_crowdloan_poke_V19_t;

#define PD_CALL_CROWDLOAN_CONTRIBUTE_ALL_V19 8
typedef struct {
  pd_Compactu32_t index;
  pd_OptionMultiSignature_V19_t signature;
} pd_crowdloan_contribute_all_V19_t;

typedef union {
  pd_balances_transfer_all_V19_t balances_transfer_all_V19;
  pd_staking_bond_extra_V19_t staking_bond_extra_V19;
  pd_staking_withdraw_unbonded_V19_t staking_withdraw_unbonded_V19;
  pd_staking_validate_V19_t staking_validate_V19;
  pd_staking_set_payee_V19_t staking_set_payee_V19;
  pd_staking_payout_stakers_V19_t staking_payout_stakers_V19;
  pd_staking_rebond_V19_t staking_rebond_V19;
  pd_session_set_keys_V19_t session_set_keys_V19;
  pd_session_purge_keys_V19_t session_purge_keys_V19;
  pd_utility_batch_V19_t utility_batch_V19;
  pd_utility_batch_all_V19_t utility_batch_all_V19;
  pd_utility_force_batch_V19_t utility_force_batch_V19;
  pd_crowdloan_create_V19_t crowdloan_create_V19;
  pd_crowdloan_contribute_V19_t crowdloan_contribute_V19;
  pd_crowdloan_withdraw_V19_t crowdloan_withdraw_V19;
  pd_crowdloan_refund_V19_t crowdloan_refund_V19;
  pd_crowdloan_dissolve_V19_t crowdloan_dissolve_V19;
  pd_crowdloan_edit_V19_t crowdloan_edit_V19;
  pd_crowdloan_add_memo_V19_t crowdloan_add_memo_V19;
  pd_crowdloan_poke_V19_t crowdloan_poke_V19;
  pd_crowdloan_contribute_all_V19_t crowdloan_contribute_all_V19;
} pd_MethodBasic_V19_t;

#define PD_CALL_BALANCES_TRANSFER_V19 0
typedef struct {
  pd_AccountIdLookupOfT_V19_t dest;
  pd_CompactBalance_t amount;
} pd_balances_transfer_V19_t;

#define PD_CALL_BALANCES_FORCE_TRANSFER_V19 2
typedef struct {
  pd_AccountIdLookupOfT_V19_t source;
  pd_AccountIdLookupOfT_V19_t dest;
  pd_CompactBalance_t amount;
} pd_balances_force_transfer_V19_t;

#define PD_CALL_BALANCES_TRANSFER_KEEP_ALIVE_V19 3
typedef struct {
  pd_AccountIdLookupOfT_V19_t dest;
  pd_CompactBalance_t amount;
} pd_balances_transfer_keep_alive_V19_t;

#define PD_CALL_STAKING_BOND_V19 0
typedef struct {
  pd_AccountIdLookupOfT_V19_t controller;
  pd_CompactBalance_t amount;
  pd_RewardDestination_V19_t payee;
} pd_staking_bond_V19_t;

#define PD_CALL_STAKING_UNBOND_V19 2
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_unbond_V19_t;

#define PD_CALL_STAKING_NOMINATE_V19 5
typedef struct {
  pd_VecAccountIdLookupOfT_V19_t targets;
} pd_staking_nominate_V19_t;

#define PD_CALL_STAKING_CHILL_V19 6
typedef struct {
} pd_staking_chill_V19_t;

#define PD_CALL_STAKING_SET_CONTROLLER_V19 8
typedef struct {
  pd_AccountIdLookupOfT_V19_t controller;
} pd_staking_set_controller_V19_t;

typedef union {
  pd_balances_transfer_V19_t balances_transfer_V19;
  pd_balances_force_transfer_V19_t balances_force_transfer_V19;
  pd_balances_transfer_keep_alive_V19_t balances_transfer_keep_alive_V19;
  pd_staking_bond_V19_t staking_bond_V19;
  pd_staking_unbond_V19_t staking_unbond_V19;
  pd_staking_nominate_V19_t staking_nominate_V19;
  pd_staking_chill_V19_t staking_chill_V19;
  pd_staking_set_controller_V19_t staking_set_controller_V19;
} pd_MethodNested_V19_t;

typedef union {
  pd_MethodBasic_V19_t basic;
  pd_MethodNested_V19_t nested;
} pd_Method_V19_t;

#endif