#include "ff.h"
#include "lv_font_ex.h"
#include "lvgl.h"

static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0xffff,
    .bpp = 4,
};

static SRAM1 uint8_t __g_font_buf[480] = {0};
static SRAM1 DWORD clmt[SZ_TBL];
static FIL font_f;
static FRESULT res;
static UINT nums = 0;
static bool is_opend = false;
static uint32_t glyph_location = 0;

static uint8_t *__user_font_getdata(int offset, int size) {
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
        return NULL;
      }
      if (f_read(&font_f, &len, 4, &nums) != FR_OK ||
          f_read(&font_f, buf, 4, &nums) != FR_OK ||
          memcmp("loca", buf, 4) != 0) {
        return NULL;
      }
      glyph_location = len + LOCA_OFFSET;

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

  uint32_t unicode_offset = (unicode_letter - __g_xbf_hd.min + 1) * 4;
  uint32_t *p_pos = (uint32_t *)__user_font_getdata(
      unicode_offset + LOCA_OFFSET + LOCA_VALUE_OFFSET, 4);

  if (p_pos[0] != 0) {
    uint32_t pos = p_pos[0] + glyph_location;
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

  uint32_t unicode_offset = (unicode_letter - __g_xbf_hd.min + 1) * 4;
  uint32_t *p_pos = (uint32_t *)__user_font_getdata(
      unicode_offset + LOCA_OFFSET + LOCA_VALUE_OFFSET, 4);
  if (p_pos[0] != 0) {
    glyph_dsc_t *gdsc = (glyph_dsc_t *)__user_font_getdata(
        glyph_location + p_pos[0], sizeof(glyph_dsc_t));

    dsc_out->adv_w = (gdsc->adv_w + (1 << 3)) >> 4;
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
    .line_height = 24,
    .base_line = 3,
};
