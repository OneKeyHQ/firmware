#ifndef _EMMC_DEBUG_UTILS_H_
#define _EMMC_DEBUG_UTILS_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memzero.h"

#include "display.h"
#include "touch.h"

void __debug_print(bool wait_click, const char* file, int line, const char* func, const char* fmt, ...);

#define dbgprintf_Wait(...) __debug_print(true, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define dbgprintf(...)      __debug_print(false, __FILE__, __LINE__, __func__, __VA_ARGS__)

bool buffer_to_hex_string(const void* buff, size_t buff_len, char* str, size_t str_len, size_t* processed);

// void __print_buffer(void* buff, size_t buff_len);

#define print_buffer(buff, buff_len)                                                              \
  {                                                                                               \
    size_t byte_str_len = buff_len * 3 + 1;                                                       \
    char str[byte_str_len];                                                                       \
    size_t processed = 0;                                                                         \
    memzero(str, byte_str_len);                                                                   \
    if ( buffer_to_hex_string(buff, buff_len, str, byte_str_len, &processed) )                    \
      dbgprintf_Wait("buffer=%s\nprocessed=%lu\nbyte_str_len=%lu", str, processed, byte_str_len); \
    else                                                                                          \
      dbgprintf_Wait("failed, processed=%lu\n", processed);                                       \
  }

#endif // _EMMC_DEBUG_UTILS_H_