#include "chinese.h"
#include "buttons.h"
#include "font.h"
#include "layout2.h"
#include "oled.h"
#include "protect.h"

extern void drawScrollbar(int pages, int index);
int oledStringWidthAdapter(const char *text, uint8_t font) {
  if (!text) return 0;
  const struct font_desc *font_dese = find_cur_font();
  int l = 0;
  int zoom = (font & FONT_DOUBLE) ? 2 : 1;

  while (*text) {
    if ((uint8_t)*text < 0x80) {
      if (zoom == 2) {
        l += (fontCharWidth(font & 0x7f, (uint8_t)*text) + 1) * zoom + 1;
      } else {
        l += fontCharWidth(font & 0x7f, (uint8_t)*text) + 1;
      }
      text++;
    } else {
      if (font_dese->idx == DEFAULT_IDX) {
        font_dese = find_font("dingmao_9x9");
      }
      // l += font_dese->width + ((font & FONT_DOUBLE) ? 2 : 1);
      l += font_dese->width + ((font & FONT_DOUBLE) ? 1 : 0);
      text += HZ_CODE_LEN;
    }
  }
  return l;
}

static void oledDrawChar_zh(int x, int y, const char *zh, uint8_t font,
                            const struct font_desc *font_dc) {
  if (x >= OLED_WIDTH || y >= OLED_HEIGHT || x <= -12 || y <= -12) {
    return;
  }
  int zoom = (font & FONT_DOUBLE) ? 2 : 1;
  const uint8_t *char_data = get_font_data(zh);

  if (!char_data) return;

  for (int xo = 0; xo < font_dc->pixel; xo++) {
    for (int yo = 0; yo < 8; yo++) {
      if (char_data[xo] & (1 << (8 - 1 - yo))) {
        if (zoom <= 1) {
          oledDrawPixel(x + xo, y + yo);
        } else {
          oledBox(x + xo, y + yo * zoom, x + (xo + 1) - 1,
                  y + (yo + 1) * zoom - 1, true);
        }
      }
    }
    for (int yo = 0; yo < font_dc->pixel - 8; yo++) {
      if (char_data[xo + font_dc->pixel] & (1 << (8 - 1 - yo))) {
        if (zoom <= 1) {
          oledDrawPixel(x + xo, y + 8 + yo);
        } else {
          oledBox(x + xo * zoom, y + (font_dc->pixel + yo) * zoom,
                  x + (xo + 1) * zoom - 1, y + (yo + 8 + 1) * zoom - 1, true);
        }
      }
    }
  }
}

void oledDrawStringAdapter(int x, int y, const char *text, uint8_t font) {
  if (!text) return;
  const struct font_desc *font_desc, *font_desc_bak;
  font_desc = font_desc_bak = find_cur_font();
  int space = (font & FONT_DOUBLE) ? 2 : 1;
  int l = 0;
  while (*text) {
    if ((uint8_t)*text < 0x80) {
      if (*text == '\n') {
        x = 0;
        if (font_desc->pixel <= 8)
          y += font_desc->pixel + 2;
        else
          y += font_desc->pixel + 1;
        text++;
        continue;
      }
      l = fontCharWidth(font & 0x7f, *text) + space;
      if (x + l > OLED_WIDTH) {
        x = 0;
        y += font_desc->pixel + 1;
      }
      if (y > OLED_HEIGHT) y = 0;
      oledDrawChar(x, y + font_desc->pixel - 8, *text, font);
      if (font & FONT_DOUBLE)
        x += l * space - 1;
      else
        x += l;
      text++;
    } else {
      if (font_desc_bak->idx == DEFAULT_IDX) {
        font_desc_bak = find_font("dingmao_9x9");
      }
      if (x + font_desc_bak->width > OLED_WIDTH) {
        x = 0;
        y += font_desc_bak->pixel + 1;
      }
      if (y > OLED_HEIGHT) y = 0;
      oledDrawChar_zh(x, y, text, font, font_desc_bak);
      // x += font_desc_bak->width + ((font & FONT_DOUBLE) ? 2 : 1);
      x += font_desc_bak->width +
           ((font & FONT_DOUBLE)
                ? 1
                : 0);  // dingmao_9x9: .width = 10 include 1 space
      text += HZ_CODE_LEN;
    }
  }
}

void oledDrawStringCenterAdapter(int x, int y, const char *text, uint8_t font) {
  if (!text) return;
  x = x - oledStringWidthAdapter(text, font) / 2;
  if (x < 0) x = 0;
  oledDrawStringAdapter(x, y, text, font);
}

void oledDrawStringRightAdapter(int x, int y, const char *text, uint8_t font) {
  if (!text) return;
  x -= oledStringWidthAdapter(text, font);
  oledDrawStringAdapter(x, y, text, font);
}
#include "memzero.h"
#include "util.h"

uint8_t oledDrawPageableStringAdapter(int x, int y, const char *text,
                                      uint8_t font, const BITMAP *btn_no_icon,
                                      const BITMAP *btn_yes_icon) {
  size_t text_len = strlen(text);
  size_t rowlen = 21;
  int index = 0, rowcount = text_len / rowlen + 1;
  if (rowcount > 3) {
    char str[rowcount][rowlen + 1];
    memzero(str, sizeof(str));
    for (int i = 0; i < rowcount; ++i) {
      size_t show_len = strnlen((char *)text, MIN(rowlen, text_len));
      memcpy(str[i], (char *)text, show_len);
      str[i][show_len] = '\0';
      text += show_len;
      text_len -= show_len;
    }

  refresh_text:
    oledClear_ext(x, y);
    int y1 = y;
    y1++;
    if (0 == index) {
      oledDrawStringAdapter(x, y1, str[0], font);
      oledDrawStringAdapter(x, y1 + 1 * 10, str[1], font);
      oledDrawStringAdapter(x, y1 + 2 * 10, str[2], font);
      oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                     &bmp_bottom_middle_arrow_down);
    } else {
      oledDrawStringAdapter(x, y1, str[index], font);
      oledDrawStringAdapter(x, y1 + 1 * 10, str[index + 1], font);
      oledDrawStringAdapter(x, y1 + 2 * 10, str[index + 2], font);
      if (index == rowcount - 3) {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
      } else {
        oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_up);
        oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                       &bmp_bottom_middle_arrow_down);
      }
    }
    // scrollbar
    drawScrollbar(rowcount - 2, index);
    // bottom button
    layoutButtonNoAdapter(NULL, btn_no_icon);
    layoutButtonYesAdapter(NULL, btn_yes_icon);
    oledRefresh();
    uint8_t key = KEY_NULL;
    key = protectWaitKey(0, 0);
    switch (key) {
      case KEY_UP:
        if (index > 0) {
          index--;
        }
        goto refresh_text;
      case KEY_DOWN:
        if (index < rowcount - 3) {
          index++;
        }
        goto refresh_text;
      default:
        return key;
    }
  }
  return KEY_NULL;
}
