#include "substrate_dispatch.h"
#include "../parser_impl.h"

#include <stdint.h>

parser_error_t _readMethod(parser_context_t* c, uint8_t moduleIdx,
                           uint8_t callIdx, pd_Method_t* method) {
  switch (c->tx_obj->transactionVersion) {
    case 19:
      return _readMethod_V19(c, moduleIdx, callIdx, &method->V19);
    case 18:
      return _readMethod_V18(c, moduleIdx, callIdx, &method->V18);
    default:
      return parser_tx_version_not_supported;
  }
}

uint8_t _getMethod_NumItems(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx) {
  switch (transactionVersion) {
    case 19:
      return _getMethod_NumItems_V19(moduleIdx, callIdx);
    case 18:
      return _getMethod_NumItems_V18(moduleIdx, callIdx);
    default:
      return parser_tx_version_not_supported;
  }
}

const char* _getMethod_ModuleName(uint32_t transactionVersion,
                                  uint8_t moduleIdx) {
  switch (transactionVersion) {
    case 19:
      return _getMethod_ModuleName_V19(moduleIdx);
    case 18:
      return _getMethod_ModuleName_V18(moduleIdx);
    default:
      return NULL;
  }
}

const char* _getMethod_Name(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx) {
  switch (transactionVersion) {
    case 19:
      return _getMethod_Name_V19(moduleIdx, callIdx);
    case 18:
      return _getMethod_Name_V18(moduleIdx, callIdx);
    default:
      return 0;
  }
}

const char* _getMethod_ItemName(uint32_t transactionVersion, uint8_t moduleIdx,
                                uint8_t callIdx, uint8_t itemIdx) {
  switch (transactionVersion) {
    case 19:
      return _getMethod_ItemName_V19(moduleIdx, callIdx, itemIdx);
    case 18:
      return _getMethod_ItemName_V18(moduleIdx, callIdx, itemIdx);
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
    case 19:
      return _getMethod_ItemValue_V19(&m->V19, moduleIdx, callIdx, itemIdx,
                                      outValue, outValueLen, pageIdx,
                                      pageCount);
    case 18:
      return _getMethod_ItemValue_V18(&m->V18, moduleIdx, callIdx, itemIdx,
                                      outValue, outValueLen, pageIdx,
                                      pageCount);
    default:
      return parser_tx_version_not_supported;
  }
}

bool _getMethod_ItemIsExpert(uint32_t transactionVersion, uint8_t moduleIdx,
                             uint8_t callIdx, uint8_t itemIdx) {
  switch (transactionVersion) {
    case 19:
      return _getMethod_ItemIsExpert_V19(moduleIdx, callIdx, itemIdx);
    case 18:
      return _getMethod_ItemIsExpert_V18(moduleIdx, callIdx, itemIdx);
    default:
      return false;
  }
}

bool _getMethod_IsNestingSupported(uint32_t transactionVersion,
                                   uint8_t moduleIdx, uint8_t callIdx) {
  switch (transactionVersion) {
    case 19:
      return _getMethod_IsNestingSupported_V19(moduleIdx, callIdx);
    case 18:
      return _getMethod_IsNestingSupported_V18(moduleIdx, callIdx);
    default:
      return false;
  }
}
