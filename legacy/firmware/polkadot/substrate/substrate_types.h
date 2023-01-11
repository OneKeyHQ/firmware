#ifndef __POLKADOT_SUBSTRATE_TYPES_H__
#define __POLKADOT_SUBSTRATE_TYPES_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// https://github.com/paritytech/substrate/blob/effe489951d1edab9d34846b1eefdfaf9511dab9/frame/identity/src/lib.rs
#define Data_e_NONE 0
#define Data_e_RAW_VECU8 33
#define Data_e_BLAKETWO256U8_32 34
#define Data_e_SHA256_U8_32 35
#define Data_e_KECCAK256_U8_32 36
#define Data_e_SHATHREE256_U8_32 37

typedef uint8_t pd_bool_t;
typedef uint8_t pd_u8_t;
typedef uint16_t pd_u16_t;
typedef uint32_t pd_u32_t;
typedef uint64_t pd_u64_t;
typedef uint32_t pd_BlockNumber_t;

#define CHECK_ERROR(FUNC_CALL)        \
  {                                   \
    parser_error_t err = FUNC_CALL;   \
    if (err != parser_ok) return err; \
  }

typedef struct {
  uint8_t moduleIdx;
  uint8_t idx;
} pd_CallIndex_t;

typedef enum { eEraImmortal = 0, eEraMortal = 1 } pd_ExtrinsicEra_e;

// This type has a non-standard serialization
// core/sr-primitives/src/generic/era.rs
typedef struct {
  pd_ExtrinsicEra_e type;
  uint64_t period;
  uint64_t phase;
} pd_ExtrinsicEra_t;

typedef struct {
  const uint8_t* ptr;
  uint8_t len;
} compactInt_t;

typedef struct {
  uint32_t _lenBuffer;
  const uint8_t* _ptr;      // Pointer to actual
  const uint8_t* _nextPtr;  // Pointer to next Call
  uint8_t slotIdx;          // Count of nested calls
  bool isTail;
} pd_NestCallIdx_t;

////////////////////////
// Common types
////////////////////////

typedef struct {
  const uint8_t* _ptr;
} pd_Balance_t;

typedef struct {
  pd_Balance_t value;
} pd_BalanceOf_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
} pd_Bytes_t;

typedef struct {
  pd_CallIndex_t callIndex;
  const uint32_t* _txVerPtr;
  pd_NestCallIdx_t nestCallIdx;
} pd_Call_t;

typedef struct {
  pd_Call_t call;
} pd_Proposal_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
  uint64_t _lenBuffer;
  uint32_t callTxVersion;
} pd_VecCall_t;

typedef compactInt_t pd_Compactu128_t;

typedef struct {
  const uint8_t* _ptr;
} pd_H256_t;

typedef struct {
  const uint8_t* _ptr;
} pd_Hash_t;

typedef struct {
  uint8_t some;
  pd_u32_t contained;
} pd_Optionu32_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
  uint64_t _lenBuffer;
} pd_Vecu32_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
  uint64_t _lenBuffer;
} pd_Vecu8_t;

typedef struct {
  const uint8_t* _ptr;
} pd_u8_array_20_t;

typedef struct {
  uint8_t value;
  const uint8_t* _ptr;
} pd_Data_t;

typedef struct {
  pd_Data_t data1;
  pd_Data_t data2;
} pd_TupleDataData_t;

typedef struct {
  uint8_t some;
  pd_u8_array_20_t contained;
} pd_Optionu8_array_20_t;

typedef struct {
  uint64_t _len;
  const uint8_t* _ptr;
  uint64_t _lenBuffer;
} pd_VecTupleDataData_t;

////////////////////////
// /Common types
////////////////////////

// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wextern-c-compat"
// #pragma clang diagnostic pop

////////////////////////
// Types that require out of order declaration
////////////////////////

typedef enum { eAddressIndex = 0, eAddressId = 1 } pd_Address_e;

typedef struct {
  pd_Address_e type;
  uint64_t idx;
  const uint8_t* idPtr;
} pd_Address_t;

typedef struct {
  compactInt_t index;
} pd_CompactIndex_t;

typedef struct {
  compactInt_t value;
} pd_CompactBalance_t;

////////////////////////
////////////////////////
////////////////////////
////////////////////////

typedef compactInt_t pd_Compactu32_t;  // u32
typedef compactInt_t pd_Compactu64_t;  // u64
typedef compactInt_t pd_CompactAssignments_t;
typedef compactInt_t pd_CompactBountyIndex_t;
typedef compactInt_t pd_CompactEraIndex_t;
typedef compactInt_t pd_CompactMemberCount_t;
typedef compactInt_t pd_CompactMoment_t;
typedef compactInt_t pd_CompactPropIndex_t;
typedef compactInt_t pd_CompactProposalIndex_t;
typedef compactInt_t pd_CompactReferendumIndex_t;
typedef compactInt_t pd_CompactRegistrarIndex_t;
typedef compactInt_t pd_CompactWeight_t;

#endif
