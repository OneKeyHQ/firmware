#ifndef LV_FONT_EX_H
#define LV_FONT_EX_H

#include "secure_heap.h"

#define font_malloc pvPortMalloc
#define font_free vPortFree

#define SRAM1 __attribute__((section("sram1")))
#define EXRAM __attribute__((section("exram")))
#define SZ_TBL 5 * 1024

#define LOCA_OFFSET 0x4C
#define LOCA_VALUE_OFFSET 0x0C
#define GLYPH_OFFSET 0x08

typedef struct {
  uint16_t min;
  uint16_t max;
  uint8_t bpp;
  uint8_t reserved[3];
} x_header_t;

typedef struct __attribute__((packed)) {
  uint16_t adv_w;
  uint8_t box_w;
  uint8_t box_h;
  int8_t ofs_x;
  int8_t ofs_y;
} glyph_dsc_t;

#define FONT_CACHE_NUM 64

typedef struct {
  uint32_t unicode_letter;
  uint32_t data_size;
  uint8_t *data;
} font_info;

typedef struct {
  uint32_t cache_num;
  uint32_t cache_index;
  uint32_t cache_tail;
  font_info font_infos[FONT_CACHE_NUM];
} font_data_cache;

int font_cache_add_letter(font_data_cache *font_cache, uint32_t letter,
                          uint8_t *font_data, uint32_t data_len);
uint8_t *font_cache_get_letter(font_data_cache *font_cache, uint32_t letter,
                               uint32_t *buff_size);

#endif
