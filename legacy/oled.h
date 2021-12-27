/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __OLED_H__
#define __OLED_H__

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <stdbool.h>
#include <stdint.h>

#include "bitmaps.h"
#if ONEKEY_MINI
#include "rgbicon.h"
#endif
#include "fonts.h"

#define SPI_BASE SPI1

#ifdef OLD_PCB
#define OLED_DC_PORT GPIOA
#define OLED_DC_PIN GPIO2  // PA2 | Data/Command
#define OLED_CS_PORT GPIOA
#define OLED_CS_PIN GPIO4  // PA4 | SPI Select
#define OLED_RST_PORT GPIOA
#define OLED_RST_PIN GPIO3  // PA3 | Reset display
#else
#define OLED_DC_PORT GPIOB
#define OLED_DC_PIN GPIO0  // PB0 | Data/Command
#define OLED_CS_PORT GPIOA
#define OLED_CS_PIN GPIO4  // PA4 | SPI Select
#define OLED_RST_PORT GPIOB
#define OLED_RST_PIN GPIO1  // PB1 | Reset display
#endif

#if ONEKEY_MINI
#define OLED_CTRL_PORT GPIOA
#define OLED_CTRL_PIN GPIO7  // PA7 | Back light

#define OLED_WIDTH 128
#define OLED_HEIGHT 128

#define OLED_X_OFFSET 2
#define OLED_Y_OFFSET 3

#define MINI_ADJUST 2

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xffff
#define COLOR_RED 0xf800
#define COLOR_GREEN 0x07e0
#define COLOR_BLUE 0x001f

#define COLOR_FONT COLOR_WHITE
#define COLOR_BACKGROUND COLOR_BLACK

typedef struct {
  bool valid;
  uint16_t x;
  uint16_t y;
  BITMAP_EX bitmap;
} PICTURE;
#else
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#endif
#define OLED_BUFSIZE (OLED_WIDTH * OLED_HEIGHT / 8)

#if ONEKEY_MINI
void SPISendCmd(uint8_t cmd);
void SPISendData(uint8_t data);
void oledSetAddress(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);
void oledDrawRgbBitmap(int x, int y, const BITMAP *bmp);
void setRgbBitmap(bool valid);
bool isRgbBitmap(void);
void oledRefreshRegion(int x1, int y1, int x2, int y2);
#endif

void oledInit(void);
void oledClear(void);
void oledClearPart(void);

void oledUpdateClk(void);

void oledRefresh(void);

void oledSetDebugLink(bool set);
void oledInvertDebugLink(void);

void oledBufferBak(void);
void oledBufferResume(void);
void oledBufferLoad(uint8_t *buffer);
void oledBufferRestore(uint8_t *buffer);
void oledSetBuffer(uint8_t *buf, uint16_t usLen);
void oledclearLine(uint8_t line);
const uint8_t *oledGetBuffer(void);
bool oledGetPixel(int x, int y);
void oledDrawPixel(int x, int y);
void oledClearPixel(int x, int y);
void oledInvertPixel(int x, int y);
void oledDrawChar(int x, int y, char c, uint8_t font);
int oledStringWidth(const char *text, uint8_t font);
void oledDrawString(int x, int y, const char *text, uint8_t font);
void oledDrawStringCenter(int x, int y, const char *text, uint8_t font);
void oledDrawStringRight(int x, int y, const char *text, uint8_t font);
void oledDrawBitmap(int x, int y, const BITMAP *bmp);
void oledClearBitmap(int x, int y, const BITMAP *bmp);
void oledDrawBitmapFlip(int x, int y, const BITMAP *bmp);
void oledInvert(int x1, int y1, int x2, int y2);
void oledBox(int x1, int y1, int x2, int y2, bool set);
void oledHLine(int y);
void oledFrame(int x1, int y1, int x2, int y2);
void oledSwipeLeft(void);
void oledSwipeRight(void);
void oledSCA(int y1, int y2, int val);
void oledSCAInside(int y1, int y2, int val, int a, int b);
void oledBackligthCtl(bool state);

#endif
