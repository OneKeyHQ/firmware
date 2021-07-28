#include "chinese.h"
#include "font.h"
#include "oled.h"

int oledStringWidthAdapter(const char *text, uint8_t font) {
  if (!text) return 0;
  const struct font_desc *font_dese = find_cur_font();
  int l = 0;
  while (*text) {
    if ((uint8_t)*text < 0x80) {
      l += fontCharWidth(font & 0x7f, (uint8_t)*text) +
           ((font & FONT_DOUBLE) ? 2 : 1);
      text++;
    } else {
      if (font_dese->idx == DEFAULT_IDX) {
        font_dese = find_font("dingmao_9x9");
      }
      l += font_dese->width + ((font & FONT_DOUBLE) ? 2 : 1);
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
      l = fontCharWidth(font & 0x7f, *text) + space;
      if ((x + l > OLED_WIDTH) || (*text == '\n')) {
        x = 0;
        y += font_desc->pixel + 1;
      }
      if (y > OLED_HEIGHT) y = 0;
      oledDrawChar(x, y + font_desc->pixel - 8, *text, font);
      x += l;
      text++;
    } else {
      if (font_desc_bak->idx == DEFAULT_IDX) {
        font_desc_bak = find_font("dingmao_9x9");
      }
      if (x + HZ_CODE_LEN > OLED_WIDTH) {
        x = 0;
        y += font_desc_bak->pixel + 1;
      }
      if (y > OLED_HEIGHT) y = 0;
      oledDrawChar_zh(x, y, text, font, font_desc_bak);
      x += font_desc_bak->width + ((font & FONT_DOUBLE) ? 2 : 1);
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
