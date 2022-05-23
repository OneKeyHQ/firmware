#ifndef LV_FONT_EX_H
#define LV_FONT_EX_H

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

#endif
