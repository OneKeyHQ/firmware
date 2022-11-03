#ifndef __ALGO_PARSER_TXDEF_H__
#define __ALGO_PARSER_TXDEF_H__

#include <stddef.h>
#include <stdint.h>

typedef enum tx_type_e {
  TX_UNKNOWN,
  TX_PAYMENT,
  TX_KEYREG,
  TX_ASSET_XFER,
  TX_ASSET_FREEZE,
  TX_ASSET_CONFIG,
  TX_APPLICATION,
} tx_type_e;

#define KEY_COMMON_TYPE "type"

#define KEY_TX_PAY "pay"
#define KEY_TX_KEYREG "keyreg"
#define KEY_TX_ASSET_XFER "axfer"
#define KEY_TX_ASSET_FREEZE "afrz"
#define KEY_TX_ASSET_CONFIG "acfg"
#define KEY_TX_APPLICATION "appl"

#define KEY_COMMON_SENDER "snd"
#define KEY_COMMON_REKEY "rekey"
#define KEY_COMMON_FEE "fee"
#define KEY_COMMON_FIRST_VALID "fv"
#define KEY_COMMON_LAST_VALID "lv"
#define KEY_COMMON_GEN_ID "gen"
#define KEY_COMMON_GEN_HASH "gh"
#define KEY_COMMON_GROUP_ID "grp"
#define KEY_COMMON_NOTE "note"

#define KEY_PAY_AMOUNT "amt"
#define KEY_PAY_RECEIVER "rcv"
#define KEY_PAY_CLOSE "close"

#define KEY_VRF_PK "selkey"
#define KEY_SPRF_PK "sprfkey"
#define KEY_VOTE_PK "votekey"
#define KEY_VOTE_FIRST "votefst"
#define KEY_VOTE_LAST "votelst"
#define KEY_VOTE_KEY_DILUTION "votekd"
#define KEY_VOTE_NON_PART_FLAG "nonpart"

#define KEY_XFER_AMOUNT "aamt"
#define KEY_XFER_CLOSE "aclose"
#define KEY_XFER_RECEIVER "arcv"
#define KEY_XFER_SENDER "asnd"
#define KEY_XFER_ID "xaid"

#define KEY_FREEZE_ID "faid"
#define KEY_FREEZE_ACCOUNT "fadd"
#define KEY_FREEZE_FLAG "afrz"

#define KEY_CONFIG_ID "caid"
#define KEY_CONFIG_PARAMS "apar"

#define KEY_APP_ID "apid"
#define KEY_APP_ARGS "apaa"
#define KEY_APP_APROG_LEN "apap"
#define KEY_APP_CPROG_LEN "apsu"
#define KEY_APP_ONCOMPLETION "apan"
#define KEY_APP_ACCOUNTS "apat"
#define KEY_APP_LOCAL_SCHEMA "apls"
#define KEY_APP_GLOBAL_SCHEMA "apgs"
#define KEY_APP_FOREIGN_APPS "apfa"
#define KEY_APP_FOREIGN_ASSETS "apas"

#define KEY_APARAMS_TOTAL "t"
#define KEY_APARAMS_DECIMALS "dc"
#define KEY_APARAMS_DEF_FROZEN "df"
#define KEY_APARAMS_UNIT_NAME "un"
#define KEY_APARAMS_ASSET_NAME "an"
#define KEY_APARAMS_URL "au"
#define KEY_APARAMS_METADATA_HASH "am"
#define KEY_APARAMS_MANAGER "m"
#define KEY_APARAMS_RESERVE "r"
#define KEY_APARAMS_FREEZE "f"
#define KEY_APARAMS_CLAWBACK "c"

#define KEY_SCHEMA_NUI "nui"
#define KEY_SCHEMA_NBS "nbs"

typedef enum oncompletion {
  NOOPOC = 0,
  OPTINOC = 1,
  CLOSEOUTOC = 2,
  CLEARSTATEOC = 3,
  UPDATEAPPOC = 4,
  DELETEAPPOC = 5,
} oncompletion_t;

typedef struct {
  uint64_t total;
  uint64_t decimals;
  uint8_t default_frozen;
  char unitname[9];
  char assetname[33];
  char url[97];
  uint8_t metadata_hash[32];
  uint8_t manager[32];
  uint8_t reserve[32];
  uint8_t freeze[32];
  uint8_t clawback[32];
} asset_params;

typedef struct {
  uint64_t num_uint;
  uint64_t num_byteslice;
} state_schema;

#define MAX_ACCT 4
#define ACCT_SIZE 32

#define ACCT_FOREIGN_LIMIT 8

#define MAX_ARG 16
#define MAX_ARGLEN 2048
#define MAX_FOREIGN_APPS 8
#define MAX_FOREIGN_ASSETS 8
#define MAX_APPROV_LEN 128
#define MAX_CLEAR_LEN 32

// TXs structs
typedef struct {
  uint8_t receiver[32];
  uint64_t amount;
  uint8_t close[32];
} txn_payment;

typedef struct {
  uint8_t votepk[32];
  uint8_t vrfpk[32];
  uint8_t sprfkey[64];
  uint64_t voteFirst;
  uint64_t voteLast;
  uint64_t keyDilution;
  uint8_t nonpartFlag;
} txn_keyreg;

typedef struct {
  uint64_t id;
  uint64_t amount;
  uint8_t sender[32];
  uint8_t receiver[32];
  uint8_t close[32];
} txn_asset_xfer;

typedef struct {
  uint64_t id;
  uint8_t account[32];
  uint8_t flag;
} txn_asset_freeze;

typedef struct {
  uint64_t id;
  asset_params params;
} txn_asset_config;

typedef struct {
  uint8_t num_accounts;
  uint8_t num_foreign_apps;
  uint8_t num_foreign_assets;
  uint8_t num_app_args;
  uint16_t aprog_len;
  uint16_t cprog_len;
  uint64_t id;
  uint64_t oncompletion;
  state_schema local_schema;
  state_schema global_schema;

  uint8_t aprog[MAX_APPROV_LEN];
  uint8_t cprog[MAX_CLEAR_LEN];
  uint16_t app_args_len[MAX_ARG];

  uint64_t foreign_apps[MAX_FOREIGN_APPS];
  uint64_t foreign_assets[MAX_FOREIGN_ASSETS];

} txn_application;

typedef struct {
  union {
    txn_payment payment;
    txn_keyreg keyreg;
    txn_asset_xfer asset_xfer;
    txn_asset_freeze asset_freeze;
    txn_asset_config asset_config;
    txn_application application;
  };

  tx_type_e type;
  uint32_t accountId;

  uint8_t sender[32];
  uint8_t rekey[32];
  uint64_t fee;
  uint64_t firstValid;
  uint64_t lastValid;
  char genesisID[32];
  uint8_t genesisHash[32];
  uint8_t groupID[32];

  uint16_t note_len;
} parser_tx_t;

typedef parser_tx_t txn_t;

typedef enum {
  IDX_COMMON_SENDER = 0,
  IDX_COMMON_FEE,
  IDX_COMMON_GEN_HASH,
  IDX_COMMON_GEN_ID,
  IDX_COMMON_NOTE,
  IDX_COMMON_GROUP_ID,
  IDX_COMMON_REKEY_TO,
  IDX_COMMON_FIRST_VALID,
  IDX_COMMON_LAST_VALID,
} txn_common_index_e;

typedef enum {
  IDX_PAYMENT_RECEIVER = 0,
  IDX_PAYMENT_AMOUNT,
  IDX_PAYMENT_CLOSE_TO,
} txn_payment_index_e;

typedef enum {
  IDX_KEYREG_VOTE_PK = 0,
  IDX_KEYREG_VRF_PK,
  IDX_KEYREG_SPRF_PK,
  IDX_KEYREG_VOTE_FIRST,
  IDX_KEYREG_VOTE_LAST,
  IDX_KEYREG_KEY_DILUTION,
  IDX_KEYREG_PARTICIPATION,
} txn_keyreg_index_e;

typedef enum {
  IDX_XFER_ASSET_ID = 0,
  IDX_XFER_AMOUNT,
  IDX_XFER_DESTINATION,
  IDX_XFER_SOURCE,
  IDX_XFER_CLOSE,
} txn_asset_transfer_index_e;

typedef enum {
  IDX_FREEZE_ASSET_ID = 0,
  IDX_FREEZE_ACCOUNT,
  IDX_FREEZE_FLAG,
} txn_asset_freeze_index_e;

typedef enum {
  IDX_CONFIG_ASSET_ID = 0,
  IDX_CONFIG_TOTAL_UNITS,
  IDX_CONFIG_FROZEN,
  IDX_CONFIG_UNIT_NAME,
  IDX_CONFIG_DECIMALS,
  IDX_CONFIG_ASSET_NAME,
  IDX_CONFIG_URL,
  IDX_CONFIG_METADATA_HASH,
  IDX_CONFIG_MANAGER,
  IDX_CONFIG_RESERVE,
  IDX_CONFIG_FREEZER,
  IDX_CONFIG_CLAWBACK,
} txn_asset_config_index_e;

typedef enum {
  IDX_APP_ID = 0,
  IDX_ON_COMPLETION,
  IDX_FOREIGN_APP,
  IDX_FOREIGN_ASSET,
  IDX_ACCOUNTS,
  IDX_APP_ARGS,
  IDX_GLOBAL_SCHEMA,
  IDX_LOCAL_SCHEMA,
  IDX_APPROVE,
  IDX_CLEAR,
} txn_application_index_e;

#define MAX_NOTE_LEN 1024

#endif  // __ALGO_PARSER_TXDEF_H__
