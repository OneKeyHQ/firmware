// clang-format off

/*******************************************************************************
 * Size: 20 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_STATUS_BAR
#define LV_FONT_STATUS_BAR 1
#endif

#if LV_FONT_STATUS_BAR

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0025 "%" */
    0x3, 0xbf, 0xea, 0x10, 0x0, 0x0, 0x7, 0xf7,
    0x0, 0x3f, 0xe8, 0x8e, 0xe1, 0x0, 0x0, 0x4f,
    0xa0, 0x0, 0xbe, 0x10, 0x2, 0xf9, 0x0, 0x2,
    0xed, 0x0, 0x0, 0xe9, 0x0, 0x0, 0xbc, 0x0,
    0xd, 0xe2, 0x0, 0x0, 0xdc, 0x0, 0x0, 0xea,
    0x0, 0xbf, 0x40, 0x0, 0x0, 0x6f, 0xa3, 0x3b,
    0xf4, 0x8, 0xf6, 0x0, 0x0, 0x0, 0x7, 0xff,
    0xfe, 0x50, 0x5f, 0x90, 0x1, 0x0, 0x0, 0x0,
    0x4, 0x30, 0x2, 0xfc, 0x7, 0xef, 0xf8, 0x0,
    0x0, 0x0, 0x0, 0x1d, 0xe1, 0x8f, 0xa5, 0x9f,
    0x90, 0x0, 0x0, 0x0, 0xbf, 0x31, 0xf9, 0x0,
    0x8, 0xf2, 0x0, 0x0, 0x9, 0xf5, 0x4, 0xf3,
    0x0, 0x2, 0xf5, 0x0, 0x0, 0x6f, 0x80, 0x4,
    0xf3, 0x0, 0x2, 0xf5, 0x0, 0x3, 0xfb, 0x0,
    0x1, 0xfb, 0x0, 0xa, 0xf1, 0x0, 0x1e, 0xd1,
    0x0, 0x0, 0x6f, 0xb7, 0xbf, 0x80, 0x0, 0xcf,
    0x20, 0x0, 0x0, 0x5, 0xdf, 0xd6, 0x0,

    /* U+0030 "0" */
    0x0, 0x3, 0xae, 0xfd, 0x71, 0x0, 0x0, 0x5,
    0xff, 0xa8, 0xbf, 0xd1, 0x0, 0x3, 0xfc, 0x10,
    0x0, 0x4f, 0xd0, 0x0, 0xcf, 0x10, 0x0, 0x0,
    0x6f, 0x60, 0x1f, 0x80, 0x0, 0x0, 0x0, 0xeb,
    0x5, 0xf4, 0x0, 0x0, 0x0, 0x9, 0xf0, 0x7f,
    0x10, 0x0, 0x0, 0x0, 0x7f, 0x18, 0xf1, 0x0,
    0x0, 0x0, 0x6, 0xf2, 0x7f, 0x10, 0x0, 0x0,
    0x0, 0x7f, 0x15, 0xf4, 0x0, 0x0, 0x0, 0x9,
    0xf0, 0x1f, 0x80, 0x0, 0x0, 0x0, 0xeb, 0x0,
    0xcf, 0x10, 0x0, 0x0, 0x6f, 0x60, 0x3, 0xfc,
    0x10, 0x0, 0x4f, 0xd0, 0x0, 0x5, 0xff, 0xa8,
    0xbf, 0xd2, 0x0, 0x0, 0x2, 0xae, 0xfd, 0x81,
    0x0, 0x0,

    /* U+0031 "1" */
    0x2f, 0xff, 0xf3, 0x16, 0x6a, 0xf3, 0x0, 0x5,
    0xf3, 0x0, 0x5, 0xf3, 0x0, 0x5, 0xf3, 0x0,
    0x5, 0xf3, 0x0, 0x5, 0xf3, 0x0, 0x5, 0xf3,
    0x0, 0x5, 0xf3, 0x0, 0x5, 0xf3, 0x0, 0x5,
    0xf3, 0x0, 0x5, 0xf3, 0x0, 0x5, 0xf3, 0x0,
    0x5, 0xf3, 0x0, 0x5, 0xf3,

    /* U+0032 "2" */
    0x0, 0x6c, 0xff, 0xc4, 0x0, 0xa, 0xfb, 0x77,
    0xcf, 0x70, 0x6f, 0x70, 0x0, 0xa, 0xf1, 0xcc,
    0x0, 0x0, 0x4, 0xf5, 0x1, 0x0, 0x0, 0x3,
    0xf6, 0x0, 0x0, 0x0, 0x7, 0xf2, 0x0, 0x0,
    0x0, 0x2f, 0xa0, 0x0, 0x0, 0x1, 0xdd, 0x10,
    0x0, 0x0, 0xc, 0xf2, 0x0, 0x0, 0x0, 0xbf,
    0x30, 0x0, 0x0, 0xa, 0xf4, 0x0, 0x0, 0x0,
    0x8f, 0x60, 0x0, 0x0, 0x7, 0xf7, 0x0, 0x0,
    0x0, 0x6f, 0xd6, 0x66, 0x66, 0x65, 0xdf, 0xff,
    0xff, 0xff, 0xfd,

    /* U+0033 "3" */
    0x5, 0xff, 0xff, 0xff, 0xff, 0x40, 0x26, 0x66,
    0x66, 0x6d, 0xf2, 0x0, 0x0, 0x0, 0x6, 0xf6,
    0x0, 0x0, 0x0, 0x3, 0xf9, 0x0, 0x0, 0x0,
    0x1, 0xec, 0x0, 0x0, 0x0, 0x0, 0xce, 0x10,
    0x0, 0x0, 0x0, 0x6f, 0xff, 0x91, 0x0, 0x0,
    0x0, 0x12, 0x7f, 0xd0, 0x0, 0x0, 0x0, 0x0,
    0x4f, 0x80, 0x0, 0x0, 0x0, 0x0, 0xdd, 0x0,
    0x0, 0x0, 0x0, 0xb, 0xe0, 0x86, 0x0, 0x0,
    0x0, 0xeb, 0xa, 0xf3, 0x0, 0x0, 0x8f, 0x50,
    0x1c, 0xfa, 0x77, 0xcf, 0x90, 0x0, 0x7, 0xcf,
    0xeb, 0x50, 0x0,

    /* U+0034 "4" */
    0x0, 0x0, 0x0, 0x0, 0xde, 0x0, 0x0, 0x0,
    0x0, 0xa, 0xfe, 0x0, 0x0, 0x0, 0x0, 0x6f,
    0xbe, 0x0, 0x0, 0x0, 0x3, 0xf7, 0x7e, 0x0,
    0x0, 0x0, 0x1d, 0xb0, 0x7e, 0x0, 0x0, 0x0,
    0xbe, 0x10, 0x7e, 0x0, 0x0, 0x7, 0xf3, 0x0,
    0x7e, 0x0, 0x0, 0x3f, 0x60, 0x0, 0x7e, 0x0,
    0x1, 0xea, 0x0, 0x0, 0x7e, 0x0, 0xb, 0xe1,
    0x0, 0x0, 0x8e, 0x0, 0x1f, 0xff, 0xff, 0xff,
    0xff, 0xfb, 0x3, 0x33, 0x33, 0x33, 0x9e, 0x32,
    0x0, 0x0, 0x0, 0x0, 0x7e, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x7e, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x7e, 0x0,

    /* U+0035 "5" */
    0x2, 0xff, 0xff, 0xff, 0xf6, 0x0, 0x5f, 0x66,
    0x66, 0x66, 0x20, 0x7, 0xd0, 0x0, 0x0, 0x0,
    0x0, 0x9a, 0x0, 0x0, 0x0, 0x0, 0xc, 0x80,
    0x1, 0x0, 0x0, 0x0, 0xea, 0xcf, 0xfe, 0x70,
    0x0, 0xf, 0xfb, 0x65, 0xaf, 0xb0, 0x1, 0xc6,
    0x0, 0x0, 0x7f, 0x60, 0x0, 0x0, 0x0, 0x0,
    0xdd, 0x0, 0x0, 0x0, 0x0, 0x9, 0xf0, 0x0,
    0x0, 0x0, 0x0, 0x9f, 0x9, 0x80, 0x0, 0x0,
    0xe, 0xc0, 0x7f, 0x50, 0x0, 0x8, 0xf5, 0x0,
    0xaf, 0xc7, 0x7c, 0xf9, 0x0, 0x0, 0x5c, 0xef,
    0xc5, 0x0, 0x0,

    /* U+0036 "6" */
    0x0, 0x0, 0x4, 0xfa, 0x0, 0x0, 0x0, 0x1e,
    0xd0, 0x0, 0x0, 0x0, 0xcf, 0x30, 0x0, 0x0,
    0x8, 0xf7, 0x0, 0x0, 0x0, 0x4f, 0xb0, 0x0,
    0x0, 0x1, 0xee, 0x11, 0x0, 0x0, 0xb, 0xfe,
    0xff, 0xe8, 0x0, 0x3f, 0xfa, 0x55, 0xaf, 0xb0,
    0x8f, 0x70, 0x0, 0x7, 0xf5, 0xbe, 0x0, 0x0,
    0x0, 0xfa, 0xcd, 0x0, 0x0, 0x0, 0xdc, 0x9f,
    0x0, 0x0, 0x0, 0xfa, 0x3f, 0x90, 0x0, 0x9,
    0xf4, 0x8, 0xfc, 0x77, 0xcf, 0x90, 0x0, 0x5c,
    0xff, 0xc5, 0x0,

    /* U+0037 "7" */
    0x2f, 0xff, 0xff, 0xff, 0xff, 0x6, 0x66, 0x66,
    0x66, 0xed, 0x0, 0x0, 0x0, 0x3, 0xf6, 0x0,
    0x0, 0x0, 0xb, 0xe0, 0x0, 0x0, 0x0, 0x3f,
    0x60, 0x0, 0x0, 0x0, 0xbe, 0x0, 0x0, 0x0,
    0x3, 0xf7, 0x0, 0x0, 0x0, 0xb, 0xe0, 0x0,
    0x0, 0x0, 0x3f, 0x70, 0x0, 0x0, 0x0, 0xbe,
    0x0, 0x0, 0x0, 0x3, 0xf7, 0x0, 0x0, 0x0,
    0xb, 0xe0, 0x0, 0x0, 0x0, 0x3f, 0x70, 0x0,
    0x0, 0x0, 0xaf, 0x0, 0x0, 0x0, 0x2, 0xf8,
    0x0, 0x0, 0x0,

    /* U+0038 "8" */
    0x0, 0x2a, 0xef, 0xd7, 0x0, 0x0, 0x2f, 0xe8,
    0x7b, 0xfb, 0x0, 0xb, 0xe1, 0x0, 0x8, 0xf4,
    0x0, 0xeb, 0x0, 0x0, 0x2f, 0x70, 0xc, 0xd0,
    0x0, 0x5, 0xf5, 0x0, 0x4f, 0xb3, 0x26, 0xec,
    0x0, 0x0, 0x8f, 0xff, 0xff, 0x20, 0x0, 0x8f,
    0xc6, 0x58, 0xee, 0x20, 0x3f, 0x90, 0x0, 0x1,
    0xec, 0x9, 0xf0, 0x0, 0x0, 0x7, 0xf2, 0xae,
    0x0, 0x0, 0x0, 0x5f, 0x39, 0xf1, 0x0, 0x0,
    0x8, 0xf2, 0x3f, 0xa0, 0x0, 0x2, 0xfb, 0x0,
    0x7f, 0xd8, 0x79, 0xfe, 0x10, 0x0, 0x4b, 0xef,
    0xd8, 0x10, 0x0,

    /* U+0039 "9" */
    0x0, 0x5c, 0xfe, 0xb4, 0x0, 0x9, 0xfc, 0x77,
    0xdf, 0x70, 0x5f, 0x80, 0x0, 0xa, 0xf2, 0xbe,
    0x0, 0x0, 0x1, 0xf8, 0xdc, 0x0, 0x0, 0x0,
    0xea, 0xcd, 0x0, 0x0, 0x0, 0xfa, 0x7f, 0x40,
    0x0, 0x7, 0xf7, 0xd, 0xf7, 0x23, 0x8f, 0xf2,
    0x1, 0xbf, 0xff, 0xff, 0xa0, 0x0, 0x2, 0x44,
    0xde, 0x10, 0x0, 0x0, 0xa, 0xf3, 0x0, 0x0,
    0x0, 0x6f, 0x70, 0x0, 0x0, 0x3, 0xfb, 0x0,
    0x0, 0x0, 0x1e, 0xe1, 0x0, 0x0, 0x0, 0xbf,
    0x30, 0x0, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 312, .box_w = 18, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 234, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 119, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 278, .adv_w = 192, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 353, .adv_w = 195, .box_w = 11, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 202, .box_w = 12, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 197, .box_w = 11, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 609, .adv_w = 191, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 684, .adv_w = 174, .box_w = 10, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 759, .adv_w = 201, .box_w = 11, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 842, .adv_w = 191, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 37, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 2,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    3, 8,
    4, 6,
    8, 11,
    9, 6,
    9, 7,
    9, 8,
    11, 6,
    11, 8
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -3, -3, -6, -29, -6, -16, -10, -6
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 8,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t lv_font_status_bar = {
#else
lv_font_t lv_font_status_bar = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -3,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_FONT_STATUS_BAR*/

