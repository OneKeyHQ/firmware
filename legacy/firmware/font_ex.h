#ifndef _FONT_EX_H_
#define _FONT_EX_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct __attribute__((packed)) {
  uint8_t magic[4];
  uint32_t total_len;
  uint8_t sections;
  uint8_t height;
  uint16_t fonts_flag;
  uint16_t count;
  uint8_t rfu[2];
} FontHeader;

typedef struct __attribute__((packed)) {
  uint16_t start;
  uint16_t end;
  uint32_t offset;
} FontSection;

typedef struct __attribute__((packed)) {
  uint8_t width;
  uint32_t offset;
} FontInfo;

#define FONT_HEADER_LEN sizeof(FontHeader)
#define FONT_SECTION_LEN sizeof(FontSection)

int utf8_get_size(const uint8_t ch);
void font_init(void);
bool font_imported(void);
int font_get_width(const uint8_t *ch);
int font_get_height(void);
uint8_t *font_get_data(const uint8_t *ch, uint8_t *width);

#endif
