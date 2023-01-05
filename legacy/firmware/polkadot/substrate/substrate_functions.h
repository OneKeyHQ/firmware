#ifndef __POLKADOT_SUBSTRATE_FUNCTIONS_H__
#define __POLKADOT_SUBSTRATE_FUNCTIONS_H__

#include <stddef.h>
#include <stdint.h>
#include "../parser_common.h"

// Common read functions

parser_error_t _readbool(parser_context_t* c, pd_bool_t* v);
parser_error_t _readu8(parser_context_t* c, pd_u8_t* v);
parser_error_t _readu16(parser_context_t* c, pd_u16_t* v);
parser_error_t _readu32(parser_context_t* c, pd_u32_t* v);
parser_error_t _readu64(parser_context_t* c, pd_u64_t* v);
parser_error_t _readBlockNumber(parser_context_t* c, pd_BlockNumber_t* v);
parser_error_t _readCompactu32(parser_context_t* c, pd_Compactu32_t* v);
parser_error_t _readCompactu64(parser_context_t* c, pd_Compactu64_t* v);
parser_error_t _readCallImpl(parser_context_t* c, pd_Call_t* v,
                             pd_MethodNested_t* m);

parser_error_t _readBalance(parser_context_t* c, pd_Balance_t* v);
parser_error_t _readBalanceOf(parser_context_t* c, pd_BalanceOf_t* v);
parser_error_t _substrate_readBytes(parser_context_t* c, pd_Bytes_t* v);
parser_error_t _readCall(parser_context_t* c, pd_Call_t* v);
parser_error_t _readProposal(parser_context_t* c, pd_Proposal_t* v);
parser_error_t _readVecCall(parser_context_t* c, pd_VecCall_t* v);
parser_error_t _readCompactu128(parser_context_t* c, pd_Compactu128_t* v);
parser_error_t _readH256(parser_context_t* c, pd_H256_t* v);
parser_error_t _readHash(parser_context_t* c, pd_Hash_t* v);
parser_error_t _readOptionu32(parser_context_t* c, pd_Optionu32_t* v);
parser_error_t _readVecu32(parser_context_t* c, pd_Vecu32_t* v);
parser_error_t _readVecu8(parser_context_t* c, pd_Vecu8_t* v);

// Common toString functions

parser_error_t _toStringu8(const pd_u8_t* v, char* outValue,
                           uint16_t outValueLen, uint8_t pageIdx,
                           uint8_t* pageCount);

parser_error_t _toStringu16(const pd_u16_t* v, char* outValue,
                            uint16_t outValueLen, uint8_t pageIdx,
                            uint8_t* pageCount);

parser_error_t _toStringu32(const pd_u32_t* v, char* outValue,
                            uint16_t outValueLen, uint8_t pageIdx,
                            uint8_t* pageCount);

parser_error_t _toStringu64(const pd_u64_t* v, char* outValue,
                            uint16_t outValueLen, uint8_t pageIdx,
                            uint8_t* pageCount);

parser_error_t _toStringbool(const pd_bool_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t* pageCount);

parser_error_t _toStringBlockNumber(const pd_BlockNumber_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringCompactu32(const pd_Compactu32_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _toStringCompactu64(const pd_Compactu64_t* v, char* outValue,
                                   uint16_t outValueLen, uint8_t pageIdx,
                                   uint8_t* pageCount);

parser_error_t _substrate_toStringBalance(const pd_Balance_t* v, char* outValue,
                                          uint16_t outValueLen, uint8_t pageIdx,
                                          uint8_t* pageCount);

parser_error_t _substrate_toStringBalanceOf(const pd_BalanceOf_t* v,
                                            char* outValue,
                                            uint16_t outValueLen,
                                            uint8_t pageIdx,
                                            uint8_t* pageCount);

parser_error_t _toStringBytes(const pd_Bytes_t* v, char* outValue,
                              uint16_t outValueLen, uint8_t pageIdx,
                              uint8_t* pageCount);

parser_error_t _toStringCall(const pd_Call_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t pageIdx,
                             uint8_t* pageCount);

parser_error_t _toStringProposal(const pd_Proposal_t* v, char* outValue,
                                 uint16_t outValueLen, uint8_t pageIdx,
                                 uint8_t* pageCount);

parser_error_t _toStringVecCall(const pd_VecCall_t* v, char* outValue,
                                uint16_t outValueLen, uint8_t pageIdx,
                                uint8_t* pageCount);

parser_error_t _toStringCompactu128(const pd_Compactu128_t* v, char* outValue,
                                    uint16_t outValueLen, uint8_t pageIdx,
                                    uint8_t* pageCount);

parser_error_t _toStringH256(const pd_H256_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t pageIdx,
                             uint8_t* pageCount);

parser_error_t _toStringHash(const pd_Hash_t* v, char* outValue,
                             uint16_t outValueLen, uint8_t pageIdx,
                             uint8_t* pageCount);

parser_error_t _toStringOptionu32(const pd_Optionu32_t* v, char* outValue,
                                  uint16_t outValueLen, uint8_t pageIdx,
                                  uint8_t* pageCount);

parser_error_t _toStringVecu32(const pd_Vecu32_t* v, char* outValue,
                               uint16_t outValueLen, uint8_t pageIdx,
                               uint8_t* pageCount);

parser_error_t _toStringVecu8(const pd_Vecu8_t* v, char* outValue,
                              uint16_t outValueLen, uint8_t pageIdx,
                              uint8_t* pageCount);

#endif
