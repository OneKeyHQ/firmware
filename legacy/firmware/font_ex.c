#include "font_ex.h"
#include "flash_enc.h"

#define FONT_DATA_ADDR 0x00
static FontHeader font_header;
static bool has_font = false;

int utf8_get_size(const uint8_t ch) {
  int n = 0;

  if (ch >= 0xfc) {
    /* 6:<11111100> */
    n = 6;
  } else if (ch >= 0xf8) {
    /* 5:<11111000> */
    n = 5;
  } else if (ch >= 0xf0) {
    /* 4:<11110000> */
    n = 4;
  } else if (ch >= 0xe0) {
    /* 3:<11100000> */
    n = 3;
  } else if (ch >= 0xc0) {
    /* 2:<11000000> */
    n = 2;
  } else {
    n = 1;
  }

  return n;
}

static int utf8_to_unicode_char(const uint8_t *utf8, uint32_t *unicode) {
  uint32_t e = 0;
  int n = 0;
  if (!utf8 || !unicode) return 0;

  if (*utf8 >= 0xfc) {
    /* 6:<11111100> */
    e = (utf8[0] & 0x01) << 30;
    e |= (utf8[1] & 0x3f) << 24;
    e |= (utf8[2] & 0x3f) << 18;
    e |= (utf8[3] & 0x3f) << 12;
    e |= (utf8[4] & 0x3f) << 6;
    e |= (utf8[5] & 0x3f);
    n = 6;
  } else if (*utf8 >= 0xf8) {
    /* 5:<11111000> */
    e = (utf8[0] & 0x03) << 24;
    e |= (utf8[1] & 0x3f) << 18;
    e |= (utf8[2] & 0x3f) << 12;
    e |= (utf8[3] & 0x3f) << 6;
    e |= (utf8[4] & 0x3f);
    n = 5;
  } else if (*utf8 >= 0xf0) {
    /* 4:<11110000> */
    e = (utf8[0] & 0x07) << 18;
    e |= (utf8[1] & 0x3f) << 12;
    e |= (utf8[2] & 0x3f) << 6;
    e |= (utf8[3] & 0x3f);
    n = 4;
  } else if (*utf8 >= 0xe0) {
    /* 3:<11100000> */
    e = (utf8[0] & 0x0f) << 12;
    e |= (utf8[1] & 0x3f) << 6;
    e |= (utf8[2] & 0x3f);
    n = 3;
  } else if (*utf8 >= 0xc0) {
    /* 2:<11000000> */
    e = (utf8[0] & 0x1f) << 6;
    e |= (utf8[1] & 0x3f);
    n = 2;
  } else {
    e = utf8[0];
    n = 1;
  }
  *unicode = e;

  return n;
}

static void font_data_read(void *buf, uint32_t offset, uint32_t len) {
  flash_read_enc(buf, FONT_DATA_ADDR + offset, len);
}

void font_init(void) {
  uint8_t end_flag[4] = {0};
  font_data_read(&font_header, 0, FONT_HEADER_LEN);

  if (memcmp(font_header.magic, "U3TP", 4) == 0) {
    font_data_read(&end_flag, font_header.total_len - 4, 4);
    if (memcmp(end_flag, "ENDU", 4) == 0) {
      has_font = true;
    }
  }
}

bool font_imported(void) {
  font_init();
  return has_font;
}

static bool font_get_info(const uint8_t *ch, FontInfo *font_info) {
  uint8_t i;
  uint32_t unicode = 0;
  uint32_t offset = 0;
  uint32_t data = 0;
  FontSection font_section = {0};

  font_info->width = 0;
  font_info->offset = 0;

  utf8_to_unicode_char(ch, &unicode);
  for (i = 0; i < font_header.sections; i++) {
    font_data_read(&font_section, FONT_HEADER_LEN + i * FONT_SECTION_LEN,
                   FONT_SECTION_LEN);
    if (unicode >= font_section.start && unicode <= font_section.end) {
      break;
    }
  }
  if (i >= font_header.sections) {
    return false;
  }
  offset = font_section.offset + (unicode - font_section.start) * 4;
  font_data_read(&data, offset, 4);
  font_info->width = data >> 26;
  font_info->offset = data & 0x3ffffff;
  if (font_info->offset > font_header.total_len) {
    return false;
  }
  return true;
}

int font_get_width(const uint8_t *ch) {
  if (!has_font) {
    return 0;
  }
  FontInfo font_info = {0};
  font_get_info(ch, &font_info);
  return font_info.width;
}

int font_get_addr(const uint8_t *ch) {
  FontInfo font_info = {0};
  font_get_info(ch, &font_info);
  return font_info.offset;
}

int font_get_height(void) {
  if (!has_font) {
    return 0;
  }
  return font_header.height;
}

uint8_t *font_get_data(const uint8_t *ch, uint8_t *width) {
  if (!has_font) {
    return NULL;
  }
  static uint8_t buff[24] = {0};
  uint8_t line_bytes = 0;
  FontInfo font_info;

  *width = 0;
  if (font_get_info(ch, &font_info)) {
    line_bytes = (font_info.width + 7) / 8;
    if (line_bytes * font_header.height > sizeof(buff)) {
      return NULL;
    }
    font_data_read(buff, font_info.offset, line_bytes * font_header.height);
    *width = font_info.width;
  } else {
    *width = 5;
  }

  return buff;
}
