#ifndef __CHINESE_H__
#define __CHINESE_H__

#include <stdint.h>
#include <string.h>

#define HZ_WIDTH 12
#define HZ_CODE_LEN 3  // UTF-8

int oledStringWidth_zh(const uint8_t *text);
void oledDrawNumber_zh(int x, int y, const char font);
void oledDrawString_zh(int x, int y, const uint8_t *text);
void oledDrawStringCenter_zh(int x, int y, const uint8_t *text);
void oledDrawStringRight_zh(int x, int y, const uint8_t *text);

#endif