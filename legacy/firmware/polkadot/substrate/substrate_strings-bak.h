#ifndef __POLKADOT_SUBSTRATE_STRINGS_H__
#define __POLKADOT_SUBSTRATE_STRINGS_H__

// Modules names
static const char* STR_MO_SYSTEM = "System";
static const char* STR_MO_PREIMAGE = "Preimage";
static const char* STR_MO_TIMESTAMP = "Timestamp";
static const char* STR_MO_INDICES = "Indices";
static const char* STR_MO_BALANCES = "Balances";
static const char* STR_MO_STAKING = "Staking";
static const char* STR_MO_SESSION = "Session";
static const char* STR_MO_GRANDPA = "Grandpa";
static const char* STR_MO_DEMOCRACY = "Democracy";
static const char* STR_MO_COUNCIL = "Council";
static const char* STR_MO_TECHNICALCOMMITTEE = "Technicalcommittee";
static const char* STR_MO_PHRAGMENELECTION = "Phragmenelection";
static const char* STR_MO_TECHNICALMEMBERSHIP = "Technicalmembership";
static const char* STR_MO_TREASURY = "Treasury";
static const char* STR_MO_CLAIMS = "Claims";
static const char* STR_MO_VESTING = "Vesting";
static const char* STR_MO_UTILITY = "Utility";
static const char* STR_MO_IDENTITY = "Identity";
static const char* STR_MO_PROXY = "Proxy";
static const char* STR_MO_MULTISIG = "Multisig";
static const char* STR_MO_BOUNTIES = "Bounties";
static const char* STR_MO_CHILDBOUNTIES = "Childbounties";
static const char* STR_MO_TIPS = "Tips";
static const char* STR_MO_ELECTIONPROVIDERMULTIPHASE =
    "Electionprovidermultiphase";
static const char* STR_MO_VOTERLIST = "Voterlist";
static const char* STR_MO_NOMINATIONPOOLS = "Nominationpools";
static const char* STR_MO_FASTUNSTAKE = "Fastunstake";
static const char* STR_MO_CONFIGURATION = "Configuration";
static const char* STR_MO_INITIALIZER = "Initializer";
static const char* STR_MO_UMP = "Ump";
static const char* STR_MO_HRMP = "Hrmp";
static const char* STR_MO_PARASDISPUTES = "Parasdisputes";
static const char* STR_MO_REGISTRAR = "Registrar";
static const char* STR_MO_AUCTIONS = "Auctions";
static const char* STR_MO_CROWDLOAN = "Crowdloan";

// Methods names
static const char* STR_ME_FILL_BLOCK = "Fill block";
static const char* STR_ME_REMARK = "Remark";
static const char* STR_ME_SET_HEAP_PAGES = "Set heap pages";
static const char* STR_ME_SET_CODE = "Set code";
static const char* STR_ME_SET_CODE_WITHOUT_CHECKS = "Set code without checks";
static const char* STR_ME_REMARK_WITH_EVENT = "Remark with event";
static const char* STR_ME_NOTE_PREIMAGE = "Note preimage";
static const char* STR_ME_UNNOTE_PREIMAGE = "Unnote preimage";
static const char* STR_ME_REQUEST_PREIMAGE = "Request preimage";
static const char* STR_ME_UNREQUEST_PREIMAGE = "Unrequest preimage";
static const char* STR_ME_SET = "Set";
static const char* STR_ME_CLAIM = "Claim";
static const char* STR_ME_TRANSFER = "Transfer";
static const char* STR_ME_FREE = "Free";
static const char* STR_ME_FORCE_TRANSFER = "Force transfer";
static const char* STR_ME_FREEZE = "Freeze";
static const char* STR_ME_SET_BALANCE = "Set balance";
static const char* STR_ME_TRANSFER_KEEP_ALIVE = "Transfer keep alive";
static const char* STR_ME_TRANSFER_ALL = "Transfer all";
static const char* STR_ME_FORCE_UNRESERVE = "Force unreserve";
static const char* STR_ME_BOND = "Bond";
static const char* STR_ME_BOND_EXTRA = "Bond extra";
static const char* STR_ME_UNBOND = "Unbond";
static const char* STR_ME_WITHDRAW_UNBONDED = "Withdraw Unbonded";
static const char* STR_ME_VALIDATE = "Validate";
static const char* STR_ME_NOMINATE = "Nominate";
static const char* STR_ME_CHILL = "Chill";
static const char* STR_ME_SET_PAYEE = "Set payee";
static const char* STR_ME_SET_CONTROLLER = "Set controller";
static const char* STR_ME_SET_VALIDATOR_COUNT = "Set validator count";
static const char* STR_ME_INCREASE_VALIDATOR_COUNT = "Increase validator count";
static const char* STR_ME_FORCE_NO_ERAS = "Force no eras";
static const char* STR_ME_FORCE_NEW_ERA = "Force new era";
static const char* STR_ME_SET_INVULNERABLES = "Set invulnerables";
static const char* STR_ME_FORCE_UNSTAKE = "Force unstake";
static const char* STR_ME_FORCE_NEW_ERA_ALWAYS = "Force new era always";
static const char* STR_ME_CANCEL_DEFERRED_SLASH = "Cancel deferred slash";
static const char* STR_ME_PAYOUT_STAKERS = "Payout stakers";
static const char* STR_ME_REBOND = "Rebond";
static const char* STR_ME_REAP_STASH = "Reap stash";
static const char* STR_ME_KICK = "Kick";
static const char* STR_ME_CHILL_OTHER = "Chill other";
static const char* STR_ME_FORCE_APPLY_MIN_COMMISSION =
    "Force apply min commission";
static const char* STR_ME_SET_KEYS = "Set keys";
static const char* STR_ME_PURGE_KEYS = "Purge keys";
static const char* STR_ME_NOTE_STALLED = "Note stalled";
static const char* STR_ME_PROPOSE = "Propose";
static const char* STR_ME_SECOND = "Second";
static const char* STR_ME_VOTE = "Vote";
static const char* STR_ME_EMERGENCY_CANCEL = "Emergency cancel";
static const char* STR_ME_EXTERNAL_PROPOSE = "External propose";
static const char* STR_ME_EXTERNAL_PROPOSE_MAJORITY =
    "External propose majority";
static const char* STR_ME_EXTERNAL_PROPOSE_DEFAULT = "External propose default";
static const char* STR_ME_FAST_TRACK = "Fast track";
static const char* STR_ME_VETO_EXTERNAL = "Veto external";
static const char* STR_ME_CANCEL_REFERENDUM = "Cancel referendum";
static const char* STR_ME_CANCEL_QUEUED = "Cancel queued";
static const char* STR_ME_DELEGATE = "Delegate";
static const char* STR_ME_UNDELEGATE = "Undelegate";
static const char* STR_ME_CLEAR_PUBLIC_PROPOSALS = "Clear public proposals";
static const char* STR_ME_NOTE_PREIMAGE_OPERATIONAL =
    "Note preimage operational";
static const char* STR_ME_NOTE_IMMINENT_PREIMAGE = "Note imminent preimage";
static const char* STR_ME_NOTE_IMMINENT_PREIMAGE_OPERATIONAL =
    "Note imminent preimage operational";
static const char* STR_ME_REAP_PREIMAGE = "Reap preimage";
static const char* STR_ME_UNLOCK = "Unlock";
static const char* STR_ME_REMOVE_VOTE = "Remove vote";
static const char* STR_ME_REMOVE_OTHER_VOTE = "Remove other vote";
static const char* STR_ME_ENACT_PROPOSAL = "Enact proposal";
static const char* STR_ME_CANCEL_PROPOSAL = "Cancel proposal";
static const char* STR_ME_SET_MEMBERS = "Set members";
static const char* STR_ME_EXECUTE = "Execute";
static const char* STR_ME_CLOSE = "Close";
static const char* STR_ME_DISAPPROVE_PROPOSAL = "Disapprove proposal";
static const char* STR_ME_REMOVE_VOTER = "Remove voter";
static const char* STR_ME_SUBMIT_CANDIDACY = "Submit candidacy";
static const char* STR_ME_REMOVE_MEMBER = "Remove member";
static const char* STR_ME_CLEAN_DEFUNCT_VOTERS = "Clean defunct voters";
static const char* STR_ME_ADD_MEMBER = "Add member";
static const char* STR_ME_SWAP_MEMBER = "Swap member";
static const char* STR_ME_RESET_MEMBERS = "Reset members";
static const char* STR_ME_CHANGE_KEY = "Change key";
static const char* STR_ME_SET_PRIME = "Set prime";
static const char* STR_ME_CLEAR_PRIME = "Clear prime";
static const char* STR_ME_PROPOSE_SPEND = "Propose spend";
static const char* STR_ME_REJECT_PROPOSAL = "Reject proposal";
static const char* STR_ME_APPROVE_PROPOSAL = "Approve proposal";
static const char* STR_ME_SPEND = "Spend";
static const char* STR_ME_REMOVE_APPROVAL = "Remove approval";
static const char* STR_ME_CLAIM_ATTEST = "Claim attest";
static const char* STR_ME_ATTEST = "Attest";
static const char* STR_ME_MOVE_CLAIM = "Move claim";
static const char* STR_ME_VEST = "Vest";
static const char* STR_ME_VEST_OTHER = "Vest other";
static const char* STR_ME_VESTED_TRANSFER = "Vested transfer";
static const char* STR_ME_FORCE_VESTED_TRANSFER = "Force vested transfer";
static const char* STR_ME_MERGE_SCHEDULES = "Merge schedules";
static const char* STR_ME_BATCH = "Batch";
static const char* STR_ME_BATCH_ALL = "Batch all";
static const char* STR_ME_FORCE_BATCH = "Force batch";
static const char* STR_ME_ADD_REGISTRAR = "Add registrar";
static const char* STR_ME_CLEAR_IDENTITY = "Clear identity";
static const char* STR_ME_REQUEST_JUDGEMENT = "Request judgement";
static const char* STR_ME_CANCEL_REQUEST = "Cancel request";
static const char* STR_ME_SET_FEE = "Set fee";
static const char* STR_ME_SET_ACCOUNT_ID = "Set account id";
static const char* STR_ME_KILL_IDENTITY = "Kill identity";
static const char* STR_ME_REMOVE_SUB = "Remove sub";
static const char* STR_ME_QUIT_SUB = "Quit sub";
static const char* STR_ME_PROXY = "Proxy";
static const char* STR_ME_ADD_PROXY = "Add proxy";
static const char* STR_ME_REMOVE_PROXY = "Remove proxy";
static const char* STR_ME_REMOVE_PROXIES = "Remove proxies";
static const char* STR_ME_CREATE_PURE = "Create pure";
static const char* STR_ME_KILL_PURE = "Kill pure";
static const char* STR_ME_PROXY_ANNOUNCED = "Proxy announced";
static const char* STR_ME_AS_MULTI_THRESHOLD_1 = "As multi threshold 1";
static const char* STR_ME_AS_MULTI = "As multi";
static const char* STR_ME_APPROVE_AS_MULTI = "Approve as multi";
static const char* STR_ME_CANCEL_AS_MULTI = "Cancel as multi";
static const char* STR_ME_PROPOSE_BOUNTY = "Propose bounty";
static const char* STR_ME_APPROVE_BOUNTY = "Approve bounty";
static const char* STR_ME_PROPOSE_CURATOR = "Propose curator";
static const char* STR_ME_UNASSIGN_CURATOR = "Unassign curator";
static const char* STR_ME_ACCEPT_CURATOR = "Accept curator";
static const char* STR_ME_AWARD_BOUNTY = "Award bounty";
static const char* STR_ME_CLAIM_BOUNTY = "Claim bounty";
static const char* STR_ME_CLOSE_BOUNTY = "Close bounty";
static const char* STR_ME_EXTEND_BOUNTY_EXPIRY = "Extend bounty expiry";
static const char* STR_ME_ADD_CHILD_BOUNTY = "Add child bounty";
static const char* STR_ME_AWARD_CHILD_BOUNTY = "Award child bounty";
static const char* STR_ME_CLAIM_CHILD_BOUNTY = "Claim child bounty";
static const char* STR_ME_CLOSE_CHILD_BOUNTY = "Close child bounty";
static const char* STR_ME_REPORT_AWESOME = "Report awesome";
static const char* STR_ME_RETRACT_TIP = "Retract tip";
static const char* STR_ME_TIP_NEW = "Tip new";
static const char* STR_ME_TIP = "Tip";
static const char* STR_ME_CLOSE_TIP = "Close tip";
static const char* STR_ME_SLASH_TIP = "Slash tip";
static const char* STR_ME_GOVERNANCE_FALLBACK = "Governance fallback";
static const char* STR_ME_REBAG = "Rebag";
static const char* STR_ME_PUT_IN_FRONT_OF = "Put in front of";
static const char* STR_ME_JOIN = "Join";
static const char* STR_ME_CLAIM_PAYOUT = "Claim payout";
static const char* STR_ME_POOL_WITHDRAW_UNBONDED = "Pool withdraw unbonded";
static const char* STR_ME_CREATE = "Create";
static const char* STR_ME_SET_STATE = "Set state";
static const char* STR_ME_SET_METADATA = "Set metadata";
static const char* STR_ME_SET_CONFIGS = "Set configs";
static const char* STR_ME_UPDATE_ROLES = "Update roles";
static const char* STR_ME_REGISTER_FAST_UNSTAKE = "Register fast unstake";
static const char* STR_ME_DEREGISTER = "Deregister";
static const char* STR_ME_CONTROL = "Control";
static const char* STR_ME_SET_VALIDATION_UPGRADE_COOLDOWN =
    "Set validation upgrade cooldown";
static const char* STR_ME_SET_VALIDATION_UPGRADE_DELAY =
    "Set validation upgrade delay";
static const char* STR_ME_SET_CODE_RETENTION_PERIOD =
    "Set code retention period";
static const char* STR_ME_SET_MAX_CODE_SIZE = "Set max code size";
static const char* STR_ME_SET_MAX_POV_SIZE = "Set max pov size";
static const char* STR_ME_SET_MAX_HEAD_DATA_SIZE = "Set max head data size";
static const char* STR_ME_SET_PARATHREAD_CORES = "Set parathread cores";
static const char* STR_ME_SET_PARATHREAD_RETRIES = "Set parathread retries";
static const char* STR_ME_SET_GROUP_ROTATION_FREQUENCY =
    "Set group rotation frequency";
static const char* STR_ME_SET_CHAIN_AVAILABILITY_PERIOD =
    "Set chain availability period";
static const char* STR_ME_SET_THREAD_AVAILABILITY_PERIOD =
    "Set thread availability period";
static const char* STR_ME_SET_SCHEDULING_LOOKAHEAD = "Set scheduling lookahead";
static const char* STR_ME_SET_MAX_VALIDATORS_PER_CORE =
    "Set max validators per core";
static const char* STR_ME_SET_MAX_VALIDATORS = "Set max validators";
static const char* STR_ME_SET_DISPUTE_POST_CONCLUSION_ACCEPTANCE_PERIOD =
    "Set dispute post conclusion acceptance period";
static const char* STR_ME_SET_DISPUTE_MAX_SPAM_SLOTS =
    "Set dispute max spam slots";
static const char* STR_ME_SET_DISPUTE_CONCLUSION_BY_TIME_OUT_PERIOD =
    "Set dispute conclusion by time out period";
static const char* STR_ME_SET_NO_SHOW_SLOTS = "Set no show slots";
static const char* STR_ME_SET_N_DELAY_TRANCHES = "Set n delay tranches";
static const char* STR_ME_SET_ZEROTH_DELAY_TRANCHE_WIDTH =
    "Set zeroth delay tranche width";
static const char* STR_ME_SET_NEEDED_APPROVALS = "Set needed approvals";
static const char* STR_ME_SET_RELAY_VRF_MODULO_SAMPLES =
    "Set relay vrf modulo samples";
static const char* STR_ME_SET_MAX_UPWARD_QUEUE_COUNT =
    "Set max upward queue count";
static const char* STR_ME_SET_MAX_UPWARD_QUEUE_SIZE =
    "Set max upward queue size";
static const char* STR_ME_SET_MAX_DOWNWARD_MESSAGE_SIZE =
    "Set max downward message size";
static const char* STR_ME_SET_UMP_SERVICE_TOTAL_WEIGHT =
    "Set ump service total weight";
static const char* STR_ME_SET_MAX_UPWARD_MESSAGE_SIZE =
    "Set max upward message size";
static const char* STR_ME_SET_MAX_UPWARD_MESSAGE_NUM_PER_CANDIDATE =
    "Set max upward message num per candidate";
static const char* STR_ME_SET_HRMP_OPEN_REQUEST_TTL =
    "Set hrmp open request ttl";
static const char* STR_ME_SET_HRMP_SENDER_DEPOSIT = "Set hrmp sender deposit";
static const char* STR_ME_SET_HRMP_RECIPIENT_DEPOSIT =
    "Set hrmp recipient deposit";
static const char* STR_ME_SET_HRMP_CHANNEL_MAX_CAPACITY =
    "Set hrmp channel max capacity";
static const char* STR_ME_SET_HRMP_CHANNEL_MAX_TOTAL_SIZE =
    "Set hrmp channel max total size";
static const char* STR_ME_SET_HRMP_MAX_PARACHAIN_INBOUND_CHANNELS =
    "Set hrmp max parachain inbound channels";
static const char* STR_ME_SET_HRMP_MAX_PARATHREAD_INBOUND_CHANNELS =
    "Set hrmp max parathread inbound channels";
static const char* STR_ME_SET_HRMP_CHANNEL_MAX_MESSAGE_SIZE =
    "Set hrmp channel max message size";
static const char* STR_ME_SET_HRMP_MAX_PARACHAIN_OUTBOUND_CHANNELS =
    "Set hrmp max parachain outbound channels";
static const char* STR_ME_SET_HRMP_MAX_PARATHREAD_OUTBOUND_CHANNELS =
    "Set hrmp max parathread outbound channels";
static const char* STR_ME_SET_HRMP_MAX_MESSAGE_NUM_PER_CANDIDATE =
    "Set hrmp max message num per candidate";
static const char* STR_ME_SET_UMP_MAX_INDIVIDUAL_WEIGHT =
    "Set ump max individual weight";
static const char* STR_ME_SET_PVF_CHECKING_ENABLED = "Set pvf checking enabled";
static const char* STR_ME_SET_PVF_VOTING_TTL = "Set pvf voting ttl";
static const char* STR_ME_SET_MINIMUM_VALIDATION_UPGRADE_DELAY =
    "Set minimum validation upgrade delay";
static const char* STR_ME_SET_BYPASS_CONSISTENCY_CHECK =
    "Set bypass consistency check";
static const char* STR_ME_FORCE_APPROVE = "Force approve";
static const char* STR_ME_SERVICE_OVERWEIGHT = "Service overweight";
static const char* STR_ME_FORCE_PROCESS_HRMP_OPEN = "Force process hrmp open";
static const char* STR_ME_FORCE_PROCESS_HRMP_CLOSE = "Force process hrmp close";
static const char* STR_ME_FORCE_UNFREEZE = "Force unfreeze";
static const char* STR_ME_RESERVE = "Reserve";
static const char* STR_ME_NEW_AUCTION = "New auction";
static const char* STR_ME_BID = "Bid";
static const char* STR_ME_CANCEL_AUCTION = "Cancel auction";
static const char* STR_ME_CONTRIBUTE = "Contribute";
static const char* STR_ME_WITHDRAW = "Withdraw";
static const char* STR_ME_REFUND = "Refund";
static const char* STR_ME_DISSOLVE = "Dissolve";
static const char* STR_ME_EDIT = "Edit";
static const char* STR_ME_ADD_MEMO = "Add memo";
static const char* STR_ME_POKE = "Poke";
static const char* STR_ME_CONTRIBUTE_ALL = "Contribute all";

// Items names
static const char* STR_IT_ratio = "Ratio";
static const char* STR_IT_remark = "Remark";
static const char* STR_IT_pages = "Pages";
static const char* STR_IT_code = "Code";
// static const char* STR_IT_items = "Items";
static const char* STR_IT_keys = "Keys";
// static const char* STR_IT_prefix = "Prefix";
// static const char* STR_IT_subkeys = "Subkeys";
static const char* STR_IT_bytes = "Bytes";
static const char* STR_IT_hash = "Hash";
static const char* STR_IT_now = "Now";
static const char* STR_IT_index = "Index";
static const char* STR_IT_new_ = "New";
static const char* STR_IT_freeze = "Freeze";
static const char* STR_IT_dest = "Dest";
static const char* STR_IT_amount = "Amount";
static const char* STR_IT_who = "Who";
static const char* STR_IT_new_free = "New free";
static const char* STR_IT_new_reserved = "New reserved";
static const char* STR_IT_source = "Source";
static const char* STR_IT_keep_alive = "Keep alive";
static const char* STR_IT_controller = "Controller";
static const char* STR_IT_payee = "Payee";
static const char* STR_IT_num_slashing_spans = "Num slashing spans";
static const char* STR_IT_prefs = "Prefs";
static const char* STR_IT_targets = "Targets";
static const char* STR_IT_additional = "Additional";
// static const char* STR_IT_factor = "Factor";
static const char* STR_IT_invulnerables = "Invulnerables";
static const char* STR_IT_stash = "Stash";
static const char* STR_IT_era = "Era";
static const char* STR_IT_slash_indices = "Slash indices";
static const char* STR_IT_validator_stash = "Validator stash";
// static const char* STR_IT_min_nominator_bond = "Min nominator bond";
// static const char* STR_IT_min_validator_bond = "Min validator bond";
// static const char* STR_IT_max_nominator_count = "Max nominator count";
// static const char* STR_IT_max_validator_count = "Max validator count";
// static const char* STR_IT_chill_threshold = "Chill threshold";
// static const char* STR_IT_min_commission = "Min commission";
static const char* STR_IT_proof = "Proof";
// static const char* STR_IT_equivocation_proof = "Equivocation proof";
// static const char* STR_IT_key_owner_proof = "Key owner proof";
static const char* STR_IT_delay = "Delay";
static const char* STR_IT_best_finalized_block_number =
    "Best finalized block number";
static const char* STR_IT_proposal_hash = "Proposal hash";
static const char* STR_IT_proposal = "Proposal";
static const char* STR_IT_seconds_upper_bound = "Seconds upper bound";
static const char* STR_IT_ref_index = "Ref index";
static const char* STR_IT_vote = "Vote";
static const char* STR_IT_voting_period = "Voting period";
static const char* STR_IT_which = "Which";
static const char* STR_IT_to = "To";
static const char* STR_IT_conviction = "Conviction";
static const char* STR_IT_balance = "Balance";
static const char* STR_IT_encoded_proposal = "Encoded proposal";
static const char* STR_IT_proposal_len_upper_bound = "Proposal len upper bound";
static const char* STR_IT_target = "Target";
// static const char* STR_IT_maybe_ref_index = "Maybe ref index";
static const char* STR_IT_prop_index = "Prop index";
static const char* STR_IT_new_members = "New members";
static const char* STR_IT_prime = "Prime";
static const char* STR_IT_old_count = "Old count";
static const char* STR_IT_length_bound = "Length bound";
static const char* STR_IT_threshold = "Threshold";
static const char* STR_IT_approve = "Approve";
static const char* STR_IT_proposal_weight_bound = "Proposal weight bound";
static const char* STR_IT_votes = "Votes";
static const char* STR_IT_candidate_count = "Candidate count";
// static const char* STR_IT_renouncing = "Renouncing";
static const char* STR_IT_slash_bond = "Slash bond";
static const char* STR_IT_rerun_election = "Rerun election";
static const char* STR_IT_num_voters = "Num voters";
static const char* STR_IT_num_defunct = "Num defunct";
static const char* STR_IT_remove = "Remove";
static const char* STR_IT_add = "Add";
static const char* STR_IT_members = "Members";
static const char* STR_IT_beneficiary = "Beneficiary";
static const char* STR_IT_proposal_id = "Proposal id";
static const char* STR_IT_ethereum_signature = "Ethereum signature";
// static const char* STR_IT_vesting_schedule = "Vesting schedule";
static const char* STR_IT_statement = "Statement";
static const char* STR_IT_old = "Old";
static const char* STR_IT_maybe_preclaim = "Maybe preclaim";
static const char* STR_IT_schedule = "Schedule";
static const char* STR_IT_schedule1_index = "Schedule1 index";
static const char* STR_IT_schedule2_index = "Schedule2 index";
static const char* STR_IT_calls = "Calls";
static const char* STR_IT_call = "Call";
// static const char* STR_IT_as_origin = "As origin";
static const char* STR_IT_account = "Account";
// static const char* STR_IT_info = "Info";
// static const char* STR_IT_subs = "Subs";
static const char* STR_IT_reg_index = "Reg index";
static const char* STR_IT_max_fee = "Max fee";
static const char* STR_IT_fee = "Fee";
// static const char* STR_IT_fields = "Fields";
// static const char* STR_IT_judgement = "Judgement";
// static const char* STR_IT_identity = "Identity";
static const char* STR_IT_sub = "Sub";
// static const char* STR_IT_data = "Data";
static const char* STR_IT_real = "Real";
static const char* STR_IT_force_proxy_type = "Force proxy type";
static const char* STR_IT_delegate = "Delegate";
static const char* STR_IT_proxy_type = "Proxy type";
static const char* STR_IT_spawner = "Spawner";
static const char* STR_IT_height = "Height";
static const char* STR_IT_ext_index = "Ext index";
static const char* STR_IT_call_hash = "Call hash";
static const char* STR_IT_other_signatories = "Other signatories";
static const char* STR_IT_maybe_timepoint = "Maybe timepoint";
static const char* STR_IT_store_call = "Store call";
static const char* STR_IT_max_weight = "Max weight";
static const char* STR_IT_timepoint = "Timepoint";
static const char* STR_IT_description = "Description";
static const char* STR_IT_bounty_id = "Bounty id";
static const char* STR_IT_curator = "Curator";
static const char* STR_IT_parent_bounty_id = "Parent bounty id";
static const char* STR_IT_child_bounty_id = "Child bounty id";
static const char* STR_IT_reason = "Reason";
static const char* STR_IT_tip_value = "Tip value";
// static const char* STR_IT_raw_solution = "Raw solution";
// static const char* STR_IT_witness = "Witness";
// static const char* STR_IT_maybe_next_score = "Maybe next score";
// static const char* STR_IT_supports = "Supports";
static const char* STR_IT_maybe_max_voters = "Maybe max voters";
static const char* STR_IT_maybe_max_targets = "Maybe max targets";
static const char* STR_IT_dislocated = "Dislocated";
static const char* STR_IT_lighter = "Lighter";
static const char* STR_IT_pool_id = "Pool id";
static const char* STR_IT_extra = "Extra";
static const char* STR_IT_member_account = "Member account";
static const char* STR_IT_unbonding_points = "Unbonding points";
static const char* STR_IT_root = "Root";
static const char* STR_IT_nominator = "Nominator";
static const char* STR_IT_state_toggler = "State toggler";
static const char* STR_IT_validators = "Validators";
static const char* STR_IT_state = "State";
static const char* STR_IT_metadata = "Metadata";
static const char* STR_IT_min_join_bond = "Min join bond";
static const char* STR_IT_min_create_bond = "Min create bond";
static const char* STR_IT_max_pools = "Max pools";
static const char* STR_IT_max_members = "Max members";
static const char* STR_IT_max_members_per_pool = "Max members per pool";
static const char* STR_IT_new_root = "New root";
static const char* STR_IT_new_nominator = "New nominator";
static const char* STR_IT_new_state_toggler = "New state toggler";
static const char* STR_IT_unchecked_eras_to_check = "Unchecked eras to check";
static const char* STR_IT_up_to = "Up to";
static const char* STR_IT_weight_limit = "Weight limit";
// static const char* STR_IT_recipient = "Recipient";
// static const char* STR_IT_proposed_max_capacity = "Proposed max capacity";
// static const char* STR_IT_proposed_max_message_size = "Proposed max message
// size"; static const char* STR_IT_sender = "Sender"; static const char*
// STR_IT_channel_id = "Channel id";
static const char* STR_IT_para = "Para";
// static const char* STR_IT_inbound = "Inbound";
// static const char* STR_IT_outbound = "Outbound";
static const char* STR_IT_channels = "Channels";
// static const char* STR_IT_open_requests = "Open requests";
// static const char* STR_IT_id = "Id";
// static const char* STR_IT_genesis_head = "Genesis head";
// static const char* STR_IT_validation_code = "Validation code";
// static const char* STR_IT_deposit = "Deposit";
// static const char* STR_IT_other = "Other";
static const char* STR_IT_duration = "Duration";
static const char* STR_IT_lease_period_index = "Lease period index";
static const char* STR_IT_auction_index = "Auction index";
static const char* STR_IT_first_slot = "First slot";
static const char* STR_IT_last_slot = "Last slot";
static const char* STR_IT_cap = "Cap";
static const char* STR_IT_first_period = "First period";
static const char* STR_IT_last_period = "Last period";
static const char* STR_IT_end = "End";
static const char* STR_IT_verifier = "Verifier";
static const char* STR_IT_signature = "Signature";
static const char* STR_IT_memo = "Memo";

#endif