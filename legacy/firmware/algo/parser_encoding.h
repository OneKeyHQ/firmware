#ifndef __ALGO_PARSER_ENCODING_H__
#define __ALGO_PARSER_ENCODING_H__

#include "parser_common.h"
#include "stdbool.h"
#include "stdint.h"

char* encodePubKey(uint8_t* buffer, uint16_t bufferLen,
                   const uint8_t* publicKey);

parser_error_t b64hash_data(unsigned char* data, size_t data_len, char* b64hash,
                            size_t b64hashLen);

parser_error_t _toStringBalance(uint64_t* amount, uint8_t decimalPlaces,
                                char postfix[], char prefix[], char* outValue,
                                uint16_t outValueLen, uint8_t pageIdx,
                                uint8_t* pageCount);

parser_error_t _toStringAddress(uint8_t* address, char* outValue,
                                uint16_t outValueLen, uint8_t pageIdx,
                                uint8_t* pageCount);

parser_error_t _toStringSchema(const state_schema* schema, char* outValue,
                               uint16_t outValueLen, uint8_t pageIdx,
                               uint8_t* pageCount);

bool all_zero_key(uint8_t* buff);
bool is_opt_in_tx(parser_tx_t* tx_obj);

#endif