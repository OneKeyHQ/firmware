#ifndef __POLKADOT_SUBSTRATE_DISPATCH_V19_H__
#define __POLKADOT_SUBSTRATE_DISPATCH_V19_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../parser_impl.h"
#include "substrate_functions.h"
#include "substrate_functions_V19.h"

parser_error_t _readMethod_V19(parser_context_t* c, uint8_t moduleIdx,
                               uint8_t callIdx, pd_Method_V19_t* method);

const char* _getMethod_ModuleName_V19(uint8_t moduleIdx);

const char* _getMethod_Name_V19(uint8_t moduleIdx, uint8_t callIdx);
const char* _getMethod_Name_V19_ParserFull(uint16_t callPrivIdx);

const char* _getMethod_ItemName_V19(uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx);

uint8_t _getMethod_NumItems_V19(uint8_t moduleIdx, uint8_t callIdx);

parser_error_t _getMethod_ItemValue_V19(pd_Method_V19_t* m, uint8_t moduleIdx,
                                        uint8_t callIdx, uint8_t itemIdx,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

bool _getMethod_ItemIsExpert_V19(uint8_t moduleIdx, uint8_t callIdx,
                                 uint8_t itemIdx);
bool _getMethod_IsNestingSupported_V19(uint8_t moduleIdx, uint8_t callIdx);

#endif