/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdbool.h>
#include <stdint.h>

#if PRODUCTION_MODEL == 'H'

#define MAX_DISPLAY_RESX 480
#define MAX_DISPLAY_RESY 800
#define DISPLAY_RESX 480
#define DISPLAY_RESY (800)
#define TREZOR_FONT_BPP 4

#elif TREZOR_MODEL == T

// ILI9341V, GC9307 and ST7789V drivers support 240px x 320px display resolution
#define MAX_DISPLAY_RESX 480
#define MAX_DISPLAY_RESY 800
#define DISPLAY_RESX 480
#define DISPLAY_RESY 800
#define TREZOR_FONT_BPP 4

#elif TREZOR_MODEL == 1

#define MAX_DISPLAY_RESX 128
#define MAX_DISPLAY_RESY 64
#define DISPLAY_RESX 128
#define DISPLAY_RESY 64
#define TREZOR_FONT_BPP 1

#else
#error Unknown Trezor model
#endif

#define AVATAR_IMAGE_SIZE 144
#define LOADER_ICON_SIZE 64

#define RGB16(R, G, B) ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | ((B & 0xF8) >> 3)
#define COLOR_WHITE 0xFFFF
#define COLOR_BLACK 0x0000
#define COLOR_RED RGB16(0xFF, 0x00, 0x00)    // red
#define COLOR_BLUE RGB16(0x00, 0x00, 0xFF)   // blue
#define COLOR_GREEN RGB16(0x00, 0xFF, 0x00)  // green
#define COLOR_GRAY RGB16(0x99, 0x99, 0x99)

#ifdef TREZOR_FONT_NORMAL_ENABLE
#define FONT_NORMAL (-1)
#endif
#ifdef TREZOR_FONT_MEDIUM_ENABLE
#define FONT_MEDIUM (-5)
#endif
#ifdef TREZOR_FONT_BOLD_ENABLE
#define FONT_BOLD (-2)
#endif
#ifdef TREZOR_FONT_MONO_ENABLE
#define FONT_MONO (-3)
#endif
#ifdef TREZOR_FONT_BOLD36_ENABLE
#define FONT_BOLD36 (-4)
#endif

// provided by port

void display_init(void);
void display_init_seq(void);
void display_refresh(void);
const char *display_save(const char *prefix);
void display_clear_save(void);

// provided by common

void display_clear(void);
void display_buffer(int x, int y, int w, int h, uint16_t *c);
void display_init_ex(void);
void display_bar(int x, int y, int w, int h, uint16_t c);
void display_bar_radius(int x, int y, int w, int h, uint16_t c, uint16_t b,
                        uint8_t r);

bool display_toif_info(const uint8_t *buf, uint32_t len, uint16_t *out_w,
                       uint16_t *out_h, bool *out_grayscale);
void display_image(int x, int y, int w, int h, const void *data,
                   uint32_t datalen);
void display_avatar(int x, int y, const void *data, uint32_t datalen,
                    uint16_t fgcolor, uint16_t bgcolor);
void display_icon(int x, int y, int w, int h, const void *data,
                  uint32_t datalen, uint16_t fgcolor, uint16_t bgcolor);
void display_loader(uint16_t progress, bool indeterminate, int yoffset,
                    uint16_t fgcolor, uint16_t bgcolor, const uint8_t *icon,
                    uint32_t iconlen, uint16_t iconfgcolor);
void display_progress(const char *desc, int permil);

#ifndef TREZOR_PRINT_DISABLE
void display_print_color(uint16_t fgcolor, uint16_t bgcolor);
void display_print(const char *text, int textlen);
void display_printf(const char *fmt, ...)
    __attribute__((__format__(__printf__, 1, 2)));
#endif

void display_text(int x, int y, const char *text, int textlen, int font,
                  uint16_t fgcolor, uint16_t bgcolor);
void display_text_center(int x, int y, const char *text, int textlen, int font,
                         uint16_t fgcolor, uint16_t bgcolor);
void display_text_right(int x, int y, const char *text, int textlen, int font,
                        uint16_t fgcolor, uint16_t bgcolor);
int display_text_width(const char *text, int textlen, int font);
int display_text_split(const char *text, int textlen, int font,
                       int requested_width);
int display_text_height(int font);

void display_qrcode(int x, int y, const char *data, uint32_t datalen,
                    uint8_t scale);

void display_offset(int set_xy[2], int *get_x, int *get_y);
int display_orientation(int degrees);
int display_backlight(int val);
void display_fade(int start, int end, int delay);

// helper for locating a substring in buffer with utf-8 string
void display_utf8_substr(const char *buf_start, size_t buf_len, int char_off,
                         int char_len, const char **out_start, int *out_len);

#endif
