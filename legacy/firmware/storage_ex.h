#ifndef STORAGE_EX_H_
#define STORAGE_EX_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define ADDR_MAX_SIZE 130

typedef struct {
  uint8_t hash[20];
  uint64_t amount;
} utxo_info;

typedef struct {
  uint8_t state;
  uint8_t rfu[5];
  char address[ADDR_MAX_SIZE];
} addr_info;

enum { WHITE_LIST_OK = 0, WHILT_LIST_ADDR_EXIST, WHILT_LIST_FULL };

#if !EMULATOR
void user_data_init(void);
void utxo_cache_init(void);
bool utxo_cache_check(uint8_t* prv_id, uint32_t index, uint64_t amount);
int white_list_add(const char* addr);
uint32_t white_list_get_count(void);
bool white_list_check(const char* addr);
void white_list_delete(const char* addr);
void white_list_inquiry(char data[][130], uint16_t* count);
#else
#define white_list_add(...) 0
#define white_list_delete(...)
#define white_list_inquiry(...)
#define white_list_check(...) false
#endif

#endif
