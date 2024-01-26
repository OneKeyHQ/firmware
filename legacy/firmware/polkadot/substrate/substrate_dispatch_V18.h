#ifndef __POLKADOT_SUBSTRATE_DISPATCH_V18_H__
#define __POLKADOT_SUBSTRATE_DISPATCH_V18_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../parser_impl.h"
#include "substrate_functions.h"
#include "substrate_functions_V18.h"

parser_error_t _readMethod_V18(parser_context_t* c, uint8_t moduleIdx,
                               uint8_t callIdx, pd_Method_V18_t* method);
parser_error_t _readMethod_V18_kusama(parser_context_t* c, uint8_t moduleIdx,
                                      uint8_t callIdx, pd_Method_V18_t* method);
parser_error_t _readMethod_V18_westend(parser_context_t* c, uint8_t moduleIdx,
                                       uint8_t callIdx,
                                       pd_Method_V18_t* method);
parser_error_t _readMethod_astar(parser_context_t* c, uint8_t moduleIdx,
                                 uint8_t callIdx, pd_Method_V18_t* method);
parser_error_t _readMethod_joystream(parser_context_t* c, uint8_t moduleIdx,
                                     uint8_t callIdx, pd_Method_V18_t* method);
parser_error_t _readMethod_manta(parser_context_t* c, uint8_t moduleIdx,
                                 uint8_t callIdx, pd_Method_V18_t* method);

const char* _getMethod_ModuleName_V18(uint8_t moduleIdx);

const char* _getMethod_Name_V18(uint8_t moduleIdx, uint8_t callIdx);
const char* _getMethod_Name_V18_ParserFull(uint16_t callPrivIdx);

const char* _getMethod_ItemName_V18(uint8_t moduleIdx, uint8_t callIdx,
                                    uint8_t itemIdx);

uint8_t _getMethod_NumItems_V18(uint8_t moduleIdx, uint8_t callIdx);

parser_error_t _getMethod_ItemValue_V18(pd_Method_V18_t* m, uint8_t moduleIdx,
                                        uint8_t callIdx, uint8_t itemIdx,
                                        char* outValue, uint16_t outValueLen,
                                        uint8_t pageIdx, uint8_t* pageCount);

bool _getMethod_ItemIsExpert_V18(uint8_t moduleIdx, uint8_t callIdx,
                                 uint8_t itemIdx);
bool _getMethod_IsNestingSupported_V18(uint8_t moduleIdx, uint8_t callIdx);

#endif
