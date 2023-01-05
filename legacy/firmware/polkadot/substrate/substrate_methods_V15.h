#ifndef __POLKADOT_SUBSTRATE_METHODS_V15_H__
#define __POLKADOT_SUBSTRATE_METHODS_V15_H__

#include <stddef.h>
#include <stdint.h>
#include "substrate_types.h"
#include "substrate_types_V15.h"

#define PD_CALL_SYSTEM_V15 0
#define PD_CALL_PREIMAGE_V15 10
#define PD_CALL_TIMESTAMP_V15 3
#define PD_CALL_INDICES_V15 4
#define PD_CALL_BALANCES_V15 5
#define PD_CALL_STAKING_V15 7
#define PD_CALL_SESSION_V15 9
#define PD_CALL_GRANDPA_V15 11
#define PD_CALL_DEMOCRACY_V15 14
#define PD_CALL_COUNCIL_V15 15
#define PD_CALL_TECHNICALCOMMITTEE_V15 16
#define PD_CALL_PHRAGMENELECTION_V15 17
#define PD_CALL_TECHNICALMEMBERSHIP_V15 18
#define PD_CALL_TREASURY_V15 19
#define PD_CALL_CLAIMS_V15 24
#define PD_CALL_VESTING_V15 25
#define PD_CALL_UTILITY_V15 26
#define PD_CALL_IDENTITY_V15 28
#define PD_CALL_PROXY_V15 29
#define PD_CALL_MULTISIG_V15 30
#define PD_CALL_BOUNTIES_V15 34
#define PD_CALL_CHILDBOUNTIES_V15 38
#define PD_CALL_TIPS_V15 35
#define PD_CALL_ELECTIONPROVIDERMULTIPHASE_V15 36
#define PD_CALL_VOTERLIST_V15 37
#define PD_CALL_NOMINATIONPOOLS_V15 39
#define PD_CALL_FASTUNSTAKE_V15 40
#define PD_CALL_CONFIGURATION_V15 51
#define PD_CALL_INITIALIZER_V15 57
#define PD_CALL_UMP_V15 59
#define PD_CALL_HRMP_V15 60
#define PD_CALL_PARASDISPUTES_V15 62
#define PD_CALL_REGISTRAR_V15 70
#define PD_CALL_AUCTIONS_V15 72
#define PD_CALL_CROWDLOAN_V15 73

#define PD_CALL_BALANCES_TRANSFER_ALL_V15 4
typedef struct {
  pd_AccountIdLookupOfT_V15_t dest;
  pd_bool_t keep_alive;
} pd_balances_transfer_all_V15_t;

#define PD_CALL_STAKING_BOND_V15 0
typedef struct {
  pd_AccountIdLookupOfT_V15_t controller;
  pd_CompactBalance_t amount;
  pd_RewardDestination_V15_t payee;
} pd_staking_bond_V15_t;

#define PD_CALL_STAKING_BOND_EXTRA_V15 1
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_bond_extra_V15_t;

#define PD_CALL_STAKING_UNBOND_V15 2
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_unbond_V15_t;

#define PD_CALL_STAKING_WITHDRAW_UNBONDED_V15 3
typedef struct {
  pd_u32_t num_slashing_spans;
} pd_staking_withdraw_unbonded_V15_t;

#define PD_CALL_STAKING_VALIDATE_V15 4
typedef struct {
  pd_ValidatorPrefs_V15_t prefs;
} pd_staking_validate_V15_t;

#define PD_CALL_STAKING_NOMINATE_V15 5
typedef struct {
  pd_VecAccountIdLookupOfT_V15_t targets;
} pd_staking_nominate_V15_t;

#define PD_CALL_STAKING_CHILL_V15 6
typedef struct {
} pd_staking_chill_V15_t;

#define PD_CALL_STAKING_SET_PAYEE_V15 7
typedef struct {
  pd_RewardDestination_V15_t payee;
} pd_staking_set_payee_V15_t;

#define PD_CALL_STAKING_SET_CONTROLLER_V15 8
typedef struct {
  pd_AccountIdLookupOfT_V15_t controller;
} pd_staking_set_controller_V15_t;

#define PD_CALL_STAKING_PAYOUT_STAKERS_V15 18
typedef struct {
  pd_AccountId_V15_t validator_stash;
  pd_EraIndex_V15_t era;
} pd_staking_payout_stakers_V15_t;

#define PD_CALL_STAKING_REBOND_V15 19
typedef struct {
  pd_CompactBalance_t amount;
} pd_staking_rebond_V15_t;

#define PD_CALL_SESSION_SET_KEYS_V15 0
typedef struct {
  pd_Keys_V15_t keys;
  pd_Bytes_t proof;
} pd_session_set_keys_V15_t;

#define PD_CALL_SESSION_PURGE_KEYS_V15 1
typedef struct {
} pd_session_purge_keys_V15_t;

#define PD_CALL_UTILITY_BATCH_V15 0
typedef struct {
  pd_VecCall_t calls;
} pd_utility_batch_V15_t;

#define PD_CALL_UTILITY_BATCH_ALL_V15 2
typedef struct {
  pd_VecCall_t calls;
} pd_utility_batch_all_V15_t;

#define PD_CALL_UTILITY_FORCE_BATCH_V15 4
typedef struct {
  pd_VecCall_t calls;
} pd_utility_force_batch_V15_t;

#define PD_CALL_CROWDLOAN_CREATE_V15 0
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t cap;
  pd_Compactu32_t first_period;
  pd_Compactu32_t last_period;
  pd_Compactu32_t end;
  pd_OptionMultiSigner_V15_t verifier;
} pd_crowdloan_create_V15_t;

#define PD_CALL_CROWDLOAN_CONTRIBUTE_V15 1
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t amount;
  pd_OptionMultiSignature_V15_t signature;
} pd_crowdloan_contribute_V15_t;

#define PD_CALL_CROWDLOAN_WITHDRAW_V15 2
typedef struct {
  pd_AccountId_V15_t who;
  pd_Compactu32_t index;
} pd_crowdloan_withdraw_V15_t;

#define PD_CALL_CROWDLOAN_REFUND_V15 3
typedef struct {
  pd_Compactu32_t index;
} pd_crowdloan_refund_V15_t;

#define PD_CALL_CROWDLOAN_DISSOLVE_V15 4
typedef struct {
  pd_Compactu32_t index;
} pd_crowdloan_dissolve_V15_t;

#define PD_CALL_CROWDLOAN_EDIT_V15 5
typedef struct {
  pd_Compactu32_t index;
  pd_Compactu128_t cap;
  pd_Compactu32_t first_period;
  pd_Compactu32_t last_period;
  pd_Compactu32_t end;
  pd_OptionMultiSigner_V15_t verifier;
} pd_crowdloan_edit_V15_t;

#define PD_CALL_CROWDLOAN_ADD_MEMO_V15 6
typedef struct {
  pd_ParaId_V15_t index;
  pd_Vecu8_t memo;
} pd_crowdloan_add_memo_V15_t;

#define PD_CALL_CROWDLOAN_POKE_V15 7
typedef struct {
  pd_ParaId_V15_t index;
} pd_crowdloan_poke_V15_t;

#define PD_CALL_CROWDLOAN_CONTRIBUTE_ALL_V15 8
typedef struct {
  pd_Compactu32_t index;
  pd_OptionMultiSignature_V15_t signature;
} pd_crowdloan_contribute_all_V15_t;

typedef union {
  pd_balances_transfer_all_V15_t balances_transfer_all_V15;
  pd_staking_bond_V15_t staking_bond_V15;
  pd_staking_bond_extra_V15_t staking_bond_extra_V15;
  pd_staking_unbond_V15_t staking_unbond_V15;
  pd_staking_withdraw_unbonded_V15_t staking_withdraw_unbonded_V15;
  pd_staking_validate_V15_t staking_validate_V15;
  pd_staking_nominate_V15_t staking_nominate_V15;
  pd_staking_chill_V15_t staking_chill_V15;
  pd_staking_set_payee_V15_t staking_set_payee_V15;
  pd_staking_set_controller_V15_t staking_set_controller_V15;
  pd_staking_payout_stakers_V15_t staking_payout_stakers_V15;
  pd_staking_rebond_V15_t staking_rebond_V15;
  pd_session_set_keys_V15_t session_set_keys_V15;
  pd_session_purge_keys_V15_t session_purge_keys_V15;
  pd_utility_batch_V15_t utility_batch_V15;
  pd_utility_batch_all_V15_t utility_batch_all_V15;
  pd_utility_force_batch_V15_t utility_force_batch_V15;
  pd_crowdloan_create_V15_t crowdloan_create_V15;
  pd_crowdloan_contribute_V15_t crowdloan_contribute_V15;
  pd_crowdloan_withdraw_V15_t crowdloan_withdraw_V15;
  pd_crowdloan_refund_V15_t crowdloan_refund_V15;
  pd_crowdloan_dissolve_V15_t crowdloan_dissolve_V15;
  pd_crowdloan_edit_V15_t crowdloan_edit_V15;
  pd_crowdloan_add_memo_V15_t crowdloan_add_memo_V15;
  pd_crowdloan_poke_V15_t crowdloan_poke_V15;
  pd_crowdloan_contribute_all_V15_t crowdloan_contribute_all_V15;
} pd_MethodBasic_V15_t;

#define PD_CALL_BALANCES_TRANSFER_V15 0
typedef struct {
  pd_AccountIdLookupOfT_V15_t dest;
  pd_CompactBalance_t amount;
} pd_balances_transfer_V15_t;

#define PD_CALL_BALANCES_FORCE_TRANSFER_V15 2
typedef struct {
  pd_AccountIdLookupOfT_V15_t source;
  pd_AccountIdLookupOfT_V15_t dest;
  pd_CompactBalance_t amount;
} pd_balances_force_transfer_V15_t;

#define PD_CALL_BALANCES_TRANSFER_KEEP_ALIVE_V15 3
typedef struct {
  pd_AccountIdLookupOfT_V15_t dest;
  pd_CompactBalance_t amount;
} pd_balances_transfer_keep_alive_V15_t;

typedef union {
  pd_balances_transfer_V15_t balances_transfer_V15;
  pd_balances_force_transfer_V15_t balances_force_transfer_V15;
  pd_balances_transfer_keep_alive_V15_t balances_transfer_keep_alive_V15;
} pd_MethodNested_V15_t;

typedef union {
  pd_MethodBasic_V15_t basic;
  pd_MethodNested_V15_t nested;
} pd_Method_V15_t;

#endif
