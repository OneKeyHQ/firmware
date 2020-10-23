#include "chinese.h"
#include "GB2312.h"
#include "oled.h"

static const uint8_t emptySymbol[24] = {
    0x00, 0x00, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0F, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xE0, 0x20, 0x20, 0x20, 0x20, 0x20, 0xE0, 0x00, 0x00, 0x00};

static const uint8_t *fontChineseData(const uint8_t *zh) {
  static uint8_t buf[24];
  memcpy(buf, emptySymbol, sizeof(buf));
  if (zh[0] >= 0xb0 && zh[0] <= 0xd7 && zh[1] >= 0xa1 && zh[1] <= 0xfe) {
    memcpy(buf,
           (uint8_t *)gbk_12x12 + ((zh[0] - 0xb0) * 94 + (zh[1] - 0xa1)) * 22,
           11);
    memcpy(
        buf + 12,
        (uint8_t *)gbk_12x12 + ((zh[0] - 0xb0) * 94 + (zh[1] - 0xa1)) * 22 + 11,
        11);
  }
  return buf;
}

int oledStringWidth_zh(const uint8_t *text, uint8_t font) {
  if (!text) return 0;
  int l = 0;
  while (*text) {
    if (*text < 0x80) {
      l += fontCharWidth(font & 0x7f, (uint8_t)*text) + 1;
      text++;
    } else {
      l += (font & FONT_DOUBLE) ? 2 * HZ_WIDTH : HZ_WIDTH;
      text += HZ_CODE_LEN;
    }
  }
  return l;
}

static void oledDrawChar_zh(int x, int y, const uint8_t *zh, uint8_t font) {
  if (x >= OLED_WIDTH || y >= OLED_HEIGHT || x <= -12 || y <= -12) {
    return;
  }
  int zoom = (font & FONT_DOUBLE) ? 2 : 1;
  const uint8_t *char_data = fontChineseData(zh);

  if (!char_data) return;

  for (int xo = 0; xo < 12; xo++) {
    for (int yo = 0; yo < 8; yo++) {
      if (char_data[xo] & (1 << (8 - 1 - yo))) {
        if (zoom <= 1) {
          oledDrawPixel(x + xo, y + yo);
        } else {
          oledBox(x + xo * zoom, y + yo * zoom, x + (xo + 1) * zoom - 1,
                  y + (yo + 1) * zoom - 1, true);
        }
      }
      if (char_data[xo + 12] & (1 << (8 - 1 - yo))) {
        if (zoom <= 1) {
          oledDrawPixel(x + xo, y + 8 + yo);
        } else {
          oledBox(x + xo * zoom, y + (8 + yo) * zoom, x + (xo + 1) * zoom - 1,
                  y + (yo + 8 + 1) * zoom - 1, true);
        }
      }
    }
  }
}

void oledDrawString_zh(int x, int y, const uint8_t *text, uint8_t font) {
  if (!text) return;
  int space = (font & FONT_DOUBLE) ? 2 : 1;
  int l = 0;
  while (*text) {
    if (*text < 0x80) {
      l = fontCharWidth(font & 0x7f, *text) + space;
      if (x + l > OLED_WIDTH) {
        x = 0;
        y += 13;
      }
      if (y > OLED_HEIGHT) y = 0;
      oledDrawChar(x, y + 3, *text, font);
      x += l;
      text++;
    } else {
      if (x + HZ_CODE_LEN > OLED_WIDTH) {
        x = 0;
        y += 13;
      }
      if (y > OLED_HEIGHT) y = 0;
      oledDrawChar_zh(x, y, text, font);
      x += (font & FONT_DOUBLE) ? 2 * HZ_WIDTH : HZ_WIDTH;
      text += HZ_CODE_LEN;
    }
  }
}

void oledDrawStringCenter_zh(int x, int y, const uint8_t *text, uint8_t font) {
  if (!text) return;
  x = x - oledStringWidth_zh(text, font) / 2;
  oledDrawString_zh(x, y, text, font);
}

void oledDrawStringRight_zh(int x, int y, const uint8_t *text, uint8_t font) {
  if (!text) return;
  x -= oledStringWidth_zh(text, font);
  oledDrawString_zh(x, y, text, font);
}
