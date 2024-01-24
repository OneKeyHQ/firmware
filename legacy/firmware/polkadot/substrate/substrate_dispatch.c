#include "substrate_dispatch.h"
#include "../parser_impl.h"

#include <stdint.h>

extern char polkadot_network[32];

parser_error_t _readMethod(parser_context_t* c, uint8_t moduleIdx,
                           uint8_t callIdx, pd_Method_t* method) {
  if (!strncmp(polkadot_network, "polkadot", 8)) {
    return _readMethod_V18(c, moduleIdx, callIdx, &method->V18);
  } else if (!strncmp(polkadot_network, "kusama", 6)) {
    return _readMethod_V18_kusama(c, moduleIdx, callIdx, &method->V18);
  } else if (!strncmp(polkadot_network, "westend", 7)) {
    return _readMethod_V18_westend(c, moduleIdx, callIdx, &method->V18);
  } else if (!strncmp(polkadot_network, "astar", 5)) {
    return _readMethod_astar(c, moduleIdx, callIdx, &method->V18);
  } else if (!strncmp(polkadot_network, "joystream", 9)) {
    return _readMethod_joystream(c, moduleIdx, callIdx, &method->V18);
  } else if (!strncmp(polkadot_network, "manta", 5)) {
    return _readMethod_manta(c, moduleIdx, callIdx, &method->V18);
  } else {
    return parser_tx_version_not_supported;
  }
}

uint8_t _getMethod_NumItems(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx) {
  (void)transactionVersion;
  return _getMethod_NumItems_V18(moduleIdx, callIdx);
}

const char* _getMethod_ModuleName(uint32_t transactionVersion,
                                  uint8_t moduleIdx) {
  (void)transactionVersion;
  return _getMethod_ModuleName_V18(moduleIdx);
}

const char* _getMethod_Name(uint32_t transactionVersion, uint8_t moduleIdx,
                            uint8_t callIdx) {
  (void)transactionVersion;
  return _getMethod_Name_V18(moduleIdx, callIdx);
}

const char* _getMethod_ItemName(uint32_t transactionVersion, uint8_t moduleIdx,
                                uint8_t callIdx, uint8_t itemIdx) {
  (void)transactionVersion;
  return _getMethod_ItemName_V18(moduleIdx, callIdx, itemIdx);
}

parser_error_t _getMethod_ItemValue(uint32_t transactionVersion, pd_Method_t* m,
                                    uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount) {
  (void)transactionVersion;
  return _getMethod_ItemValue_V18(&m->V18, moduleIdx, callIdx, itemIdx,
                                  outValue, outValueLen, pageIdx, pageCount);
}

bool _getMethod_ItemIsExpert(uint32_t transactionVersion, uint8_t moduleIdx,
                             uint8_t callIdx, uint8_t itemIdx) {
  (void)transactionVersion;
  return _getMethod_ItemIsExpert_V18(moduleIdx, callIdx, itemIdx);
}

bool _getMethod_IsNestingSupported(uint32_t transactionVersion,
                                   uint8_t moduleIdx, uint8_t callIdx) {
  (void)transactionVersion;
  return _getMethod_IsNestingSupported_V18(moduleIdx, callIdx);
}
