#ifndef __POLKADOT_SUBSTRATE_DISPATCH_H__
#define __POLKADOT_SUBSTRATE_DISPATCH_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../parser_common.h"
#include "substrate_dispatch_V15.h"

#define GEN_GETCALL(CALL) _getpdCall_##CALL(ctx->tx_obj->transactionVersion)
#define GEN_DEC_GETCALL(CALL) uint32_t _getpdCall_##CALL(uint32_t txVersion)
#define GEN_DEF_GETCALL(CALL)                      \
  uint32_t _getpdCall_##CALL(uint32_t txVersion) { \
    switch (txVersion) {                           \
      case 15:                                     \
        return PD_CALL_##CALL##_V15;               \
                                                   \
      default:                                     \
        return 0;                                  \
    }                                              \
  }

parser_error_t _readMethod(parser_context_t* c, uint8_t moduleIdx,
                           uint8_t callIdx, pd_Method_t* method);
uint8_t _getMethod_NumItems(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx);
const char* _getMethod_ModuleName(uint32_t transactionVersion,
                                  uint8_t moduleIdx);
const char* _getMethod_Name(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx);
const char* _getMethod_ItemName(uint32_t transactionVersion, uint8_t moduleIdx,
                                uint8_t callIdx, uint8_t itemIdx);

parser_error_t _getMethod_ItemValue(uint32_t transactionVersion, pd_Method_t* m,
                                    uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

bool _getMethod_ItemIsExpert(uint32_t transactionVersion, uint8_t moduleIdx,
                             uint8_t callIdx, uint8_t itemIdx);
bool _getMethod_IsNestingSupported(uint32_t transactionVersion,
                                   uint8_t moduleIdx, uint8_t callIdx);

#endif
