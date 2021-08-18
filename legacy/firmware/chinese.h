#ifndef __CHINESE_H__
#define __CHINESE_H__

#include <stdint.h>
#include <string.h>

// #define HZ_CODE_LEN 2  // GBK
#define HZ_CODE_LEN 3  // UTF-8

int oledStringWidthAdapter(const char *text, uint8_t font);
void oledDrawNumber_zh(int x, int y, const char font);
void oledDrawStringAdapter(int x, int y, const char *text, uint8_t font);
void oledDrawStringCenterAdapter(int x, int y, const char *text, uint8_t font);
#if ONEKEY_MINI
void oledDrawStringCenterAdapterEx(int x, int y, const char *text,
                                   uint8_t font);
#endif
void oledDrawStringRightAdapter(int x, int y, const char *text, uint8_t font);

#endif
