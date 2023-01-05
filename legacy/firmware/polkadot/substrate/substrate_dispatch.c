#include "substrate_dispatch.h"
#include "../parser_impl.h"

#include <stdint.h>

parser_error_t _readMethod(parser_context_t* c, uint8_t moduleIdx,
                           uint8_t callIdx, pd_Method_t* method) {
  switch (c->tx_obj->transactionVersion) {
    case 15:
      return _readMethod_V15(c, moduleIdx, callIdx, &method->V15);
    default:
      return parser_tx_version_not_supported;
  }
}

uint8_t _getMethod_NumItems(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_NumItems_V15(moduleIdx, callIdx);
    default:
      return parser_tx_version_not_supported;
  }
}

const char* _getMethod_ModuleName(uint32_t transactionVersion,
                                  uint8_t moduleIdx) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_ModuleName_V15(moduleIdx);
    default:
      return NULL;
  }
}

const char* _getMethod_Name(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_Name_V15(moduleIdx, callIdx);
    default:
      return 0;
  }
}

const char* _getMethod_ItemName(uint32_t transactionVersion, uint8_t moduleIdx,
                                uint8_t callIdx, uint8_t itemIdx) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_ItemName_V15(moduleIdx, callIdx, itemIdx);
    default:
      return NULL;
  }
}

parser_error_t _getMethod_ItemValue(uint32_t transactionVersion, pd_Method_t* m,
                                    uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_ItemValue_V15(&m->V15, moduleIdx, callIdx, itemIdx,
                                      outValue, outValueLen, pageIdx,
                                      pageCount);
    default:
      return parser_tx_version_not_supported;
  }
}

bool _getMethod_ItemIsExpert(uint32_t transactionVersion, uint8_t moduleIdx,
                             uint8_t callIdx, uint8_t itemIdx) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_ItemIsExpert_V15(moduleIdx, callIdx, itemIdx);
    default:
      return false;
  }
}

bool _getMethod_IsNestingSupported(uint32_t transactionVersion,
                                   uint8_t moduleIdx, uint8_t callIdx) {
  switch (transactionVersion) {
    case 15:
      return _getMethod_IsNestingSupported_V15(moduleIdx, callIdx);
    default:
      return false;
  }
}
