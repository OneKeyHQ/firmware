#ifndef __COSMOS_COIN_H__
#define __COSMOS_COIN_H__

#define CLA 0x55

#define HDPATH_LEN_DEFAULT 5

#define HDPATH_0_DEFAULT (0x80000000u | 0x2cu)
#define HDPATH_1_DEFAULT (0x80000000u | 0x76u)
#define HDPATH_2_DEFAULT (0x80000000u | 0u)
#define HDPATH_3_DEFAULT (0u)

#define PK_LEN_SECP256K1 33u

typedef enum {
  addr_secp256k1 = 0,
} address_kind_e;

#define VIEW_ADDRESS_OFFSET_SECP256K1 PK_LEN_SECP256K1
#define VIEW_ADDRESS_LAST_PAGE_DEFAULT 0

#define MENU_MAIN_APP_LINE1 "Cosmos"
#define MENU_MAIN_APP_LINE2 "Ready"
#define APPVERSION_LINE1 "Version:"
#define APPVERSION_LINE2 ("v" APPVERSION)

#define COIN_DEFAULT_CHAINID "cosmoshub-4"

// In non-expert mode, the app will convert from uatom to ATOM
#define COIN_DEFAULT_DENOM_BASE "uatom"
#define COIN_DEFAULT_DENOM_REPR "ATOM"
#define COIN_DEFAULT_DENOM_FACTOR 6
#define COIN_DEFAULT_DENOM_TRIMMING 6

// Coin denoms may be up to 128 characters long
// https://github.com/cosmos/cosmos-sdk/blob/master/types/coin.go#L780
// https://github.com/cosmos/ibc-go/blob/main/docs/architecture/adr-001-coin-source-tracing.md
#define COIN_DENOM_MAXSIZE 129
#define COIN_AMOUNT_MAXSIZE 50

#define COIN_MAX_CHAINID_LEN 20
#define INDEXING_TMP_KEYSIZE 70
#define INDEXING_TMP_VALUESIZE 70
#define INDEXING_GROUPING_REF_TYPE_SIZE 70
#define INDEXING_GROUPING_REF_FROM_SIZE 70

#define MENU_MAIN_APP_LINE2_SECRET "?"
#define COIN_SECRET_REQUIRED_CLICKS 0

#endif  //__COSMOS_COIN_H__
