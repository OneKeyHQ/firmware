#include "ff.h"
#include "lv_font_ex.h"
#include "lvgl.h"

static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0xffff,
    .bpp = 4,
};

static SRAM1 DWORD clmt[SZ_TBL];
static FIL font_f;
static FRESULT res;
static UINT nums = 0;
static bool is_opend = false;
static uint32_t glyph_location = 0;

static font_data_cache font_cache = {0};

static int __user_font_getdata(uint8_t *data_buf, int offset, int size) {
  if (!is_opend) {
    uint8_t buf[4] = {0};
    uint32_t len = 0;
    res = f_open(&font_f, "/res/lv_font_scs_bold_24.bin", FA_READ);
    if (FR_OK == res) {
      font_f.cltbl = clmt; /* Enable fast seek mode (cltbl != NULL) */
      clmt[0] = SZ_TBL;    /* Set table size */
      f_lseek(&font_f, CREATE_LINKMAP); /* Create CLMT */

      res = f_lseek(&font_f, LOCA_OFFSET);
      if (FR_OK != res) {
        return -1;
      }
      if (f_read(&font_f, &len, 4, &nums) != FR_OK ||
          f_read(&font_f, buf, 4, &nums) != FR_OK ||
          memcmp("loca", buf, 4) != 0) {
        return -1;
      }
      glyph_location = len + LOCA_OFFSET;

      is_opend = true;
    } else {
      return -1;
    }
  }
  res = f_lseek(&font_f, offset);
  if (FR_OK != res) {
    return -1;
  }
  res = f_read(&font_f, data_buf, size, &nums);
  if (FR_OK != res) {
    return -1;
  }
  return 0;
}

static const uint8_t *__user_font_get_bitmap(const lv_font_t *font,
                                             uint32_t unicode_letter) {
  if (unicode_letter > __g_xbf_hd.max || unicode_letter < __g_xbf_hd.min) {
    return NULL;
  }
  uint8_t *font_data = NULL;
  uint32_t data_len = 0;
  font_data = font_cache_get_letter(&font_cache, unicode_letter, &data_len);
  if (font_data) {
    return font_data + sizeof(glyph_dsc_t);
  } else {
    uint32_t unicode_offset = (unicode_letter - __g_xbf_hd.min + 1) * 4;
    uint32_t len = 0;
    uint8_t buf[8] = {0};
    __user_font_getdata(buf, unicode_offset + LOCA_OFFSET + LOCA_VALUE_OFFSET,
                        8);
    uint32_t *p_pos = (uint32_t *)buf;
    if (p_pos[0] != 0) {
      len = p_pos[1] - p_pos[0];
      font_data = font_malloc(len);
      if (__user_font_getdata(font_data, glyph_location + p_pos[0], len) != 0) {
        return NULL;
      }
      font_cache_add_letter(&font_cache, unicode_letter, font_data, len);
      return font_data + sizeof(glyph_dsc_t);
    } else {
      return NULL;
    }
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

  uint8_t *font_data = NULL;
  uint32_t data_len = 0;
  font_data = font_cache_get_letter(&font_cache, unicode_letter, &data_len);
  if (font_data) {
  } else {
    uint32_t unicode_offset = (unicode_letter - __g_xbf_hd.min + 1) * 4;
    uint32_t len = 0;
    uint8_t buf[8] = {0};
    __user_font_getdata(buf, unicode_offset + LOCA_OFFSET + LOCA_VALUE_OFFSET,
                        8);
    uint32_t *p_pos = (uint32_t *)buf;
    if (p_pos[0] != 0) {
      len = p_pos[1] - p_pos[0];
      font_data = font_malloc(len);
      if (__user_font_getdata(font_data, glyph_location + p_pos[0], len) != 0) {
        return false;
      }
      font_cache_add_letter(&font_cache, unicode_letter, font_data, len);
    } else {
      return false;
    }
  }
  glyph_dsc_t *gdsc = (glyph_dsc_t *)font_data;
  dsc_out->adv_w = (gdsc->adv_w + (1 << 3)) >> 4;
  dsc_out->box_h = gdsc->box_h;
  dsc_out->box_w = gdsc->box_w;
  dsc_out->ofs_x = gdsc->ofs_x;
  dsc_out->ofs_y = gdsc->ofs_y;
  dsc_out->bpp = __g_xbf_hd.bpp;
  return true;
}

// Source Han Sans,Bold,24
const lv_font_t lv_font_scs_bold_24 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 24,
    .base_line = 3,
};
