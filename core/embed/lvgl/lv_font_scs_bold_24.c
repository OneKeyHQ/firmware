#include "ff.h"
#include "lvgl.h"

typedef struct {
  uint16_t min;
  uint16_t max;
  uint8_t bpp;
  uint8_t reserved[3];
} x_header_t;

typedef struct {
  uint32_t pos;
} x_table_t;

typedef struct {
  uint8_t adv_w;
  uint8_t box_w;
  uint8_t box_h;
  int8_t ofs_x;
  int8_t ofs_y;
  uint8_t r;
} glyph_dsc_t;

static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0xf2e0,
    .bpp = 4,
};

static uint8_t __g_font_buf[480] = {0};
static FIL font_f;
static FRESULT res;
static UINT nums = 0;
static bool is_opend = false;

static uint8_t *__user_font_getdata(int offset, int size) {
  if (!is_opend) {
    res = f_open(&font_f, "/res/lv_font_scs_bold_24.bin", FA_READ);
    if (FR_OK == res) {
      is_opend = true;
    } else {
      return NULL;
    }
  }
  res = f_lseek(&font_f, offset);
  if (FR_OK != res) {
    return NULL;
  }
  res = f_read(&font_f, __g_font_buf, size, &nums);
  if (FR_OK != res) {
    return NULL;
  }
  return __g_font_buf;
}

static const uint8_t *__user_font_get_bitmap(const lv_font_t *font,
                                             uint32_t unicode_letter) {
  if (unicode_letter > __g_xbf_hd.max || unicode_letter < __g_xbf_hd.min) {
    return NULL;
  }
  uint32_t unicode_offset =
      sizeof(x_header_t) + (unicode_letter - __g_xbf_hd.min) * 4;
  uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
  if (p_pos[0] != 0) {
    uint32_t pos = p_pos[0];
    glyph_dsc_t *gdsc =
        (glyph_dsc_t *)__user_font_getdata(pos, sizeof(glyph_dsc_t));
    return __user_font_getdata(pos + sizeof(glyph_dsc_t),
                               gdsc->box_w * gdsc->box_h * __g_xbf_hd.bpp / 8);
  }
  return NULL;
}

static bool __user_font_get_glyph_dsc(const lv_font_t *font,
                                      lv_font_glyph_dsc_t *dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next) {
  if (unicode_letter > __g_xbf_hd.max || unicode_letter < __g_xbf_hd.min) {
    return NULL;
  }
  uint32_t unicode_offset =
      sizeof(x_header_t) + (unicode_letter - __g_xbf_hd.min) * 4;
  uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
  if (p_pos[0] != 0) {
    glyph_dsc_t *gdsc =
        (glyph_dsc_t *)__user_font_getdata(p_pos[0], sizeof(glyph_dsc_t));
    dsc_out->adv_w = gdsc->adv_w;
    dsc_out->box_h = gdsc->box_h;
    dsc_out->box_w = gdsc->box_w;
    dsc_out->ofs_x = gdsc->ofs_x;
    dsc_out->ofs_y = gdsc->ofs_y;
    dsc_out->bpp = __g_xbf_hd.bpp;
    return true;
  }
  return false;
}

// Source Han Sans,Bold,24
const lv_font_t lv_font_scs_bold_24 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 26,
    .base_line = -5,
};
