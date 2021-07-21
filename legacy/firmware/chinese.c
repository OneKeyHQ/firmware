#include "chinese.h"
#include "font.h"
#include "font_ex.h"
#include "oled.h"

#if ONEKEY_MINI

static int oledDrawCharEx(int x, int y, const char *c, uint8_t font) {
  if (x >= OLED_WIDTH || y >= OLED_HEIGHT || y <= -FONT_HEIGHT) {
    return 0;
  }

  uint8_t empty[] = {0x00, 0x00, 0x00, 0x7E, 0x42, 0x42, 0x42, 0x7E};

  int height = font_get_height();
  int zoom = (font & FONT_DOUBLE) ? 2 : 1;
  uint8_t char_width = 0;
  uint8_t line_bytes = 0;
  const uint8_t *char_data = font_get_data((uint8_t *)c, &char_width);
  if (!char_data) {
    char_data = empty;
    height = char_width = 8;
  }
  line_bytes = (char_width + 7) / 8;

  if (x <= -char_width) {
    return 0;
  }

  for (int yo = 0; yo < height; yo++) {
    for (int xo = 0; xo < char_width; xo++) {
      if (char_data[yo * line_bytes + xo / 8] & (1 << (7 - xo % 8))) {
        if (zoom <= 1) {
          oledDrawPixel(x + xo, y + yo);
        } else {
          oledBox(x + xo, y + yo * zoom, x + (xo + 1) - 1,
                  y + (yo + 1) * zoom - 1, true);
        }
      }
    }
  }
  return char_width;
}

int oledStringWidthEx(const char *text, uint8_t font) {
  if (!text) return 0;
  int steps = 0;
  int l = 0;
  while (*text) {
    if ((uint8_t)*text < 0x80) {
      l += fontCharWidth(font & 0x7f, (uint8_t)*text) +
           ((font & FONT_DOUBLE) ? 2 : 1);
      text++;
    } else {
      steps = utf8_get_size(*text);
      l += font_get_width((uint8_t *)text) + ((font & FONT_DOUBLE) ? 1 : 0);
      text += steps;
    }
  }
  return l;
}

void oledDrawStringEx(int x, int y, const char *text, uint8_t font) {
  int steps = 0;
  int l = 0;
  while (*text) {
    if ((uint8_t)*text < 0x80) {
      l = fontCharWidth(font & 0x7f, *text) + ((font & FONT_DOUBLE) ? 2 : 1);
      oledDrawChar(x, y, *text, font);
      x += l;
      text++;
    } else {
      steps = utf8_get_size(*text);
      l = oledDrawCharEx(x, y, text, font);
      text += steps;
      x += l;
    }
  }
}
#endif

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

int oledStringWidthAdapter(const char *text, uint8_t font) {
  if (!text) return 0;
#if ONEKEY_MINI
  if (font_imported()) {
    return oledStringWidthEx(text, font);
  } else
#endif
  {
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
}

void oledDrawStringAdapter(int x, int y, const char *text, uint8_t font) {
  if (!text) return;
#if ONEKEY_MINI
  if (font_imported()) {
    oledDrawStringEx(x, y, text, font);
  } else
#endif
  {
    const struct font_desc *font_desc, *font_desc_bak;
    font_desc = font_desc_bak = find_cur_font();
    int space = (font & FONT_DOUBLE) ? 2 : 1;
    int l = 0;
    while (*text) {
      if ((uint8_t)*text < 0x80) {
        l = fontCharWidth(font & 0x7f, *text) + space;
        if (x + l > OLED_WIDTH || (*text == '\n')) {
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
        if (x + font_desc_bak->width > OLED_WIDTH || (*text == '\n')) {
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
