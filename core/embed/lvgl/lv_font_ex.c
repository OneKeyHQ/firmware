#include "lv_font_ex.h"

int font_cache_add_letter(font_data_cache *font_cache, uint32_t letter,
                          uint8_t *font_data, uint32_t data_len) {
  font_info *f_info = NULL;
  if (font_cache->cache_index < FONT_CACHE_NUM) {
    f_info = &font_cache->font_infos[font_cache->cache_index];
    font_cache->cache_index++;
  } else {
    f_info = &font_cache->font_infos[font_cache->cache_tail];

    font_free(f_info->data);

    if (font_cache->cache_tail < FONT_CACHE_NUM - 1)
      font_cache->cache_tail++;
    else
      font_cache->cache_tail = 0;
  }
  f_info->unicode_letter = letter;
  f_info->data_size = data_len;
  f_info->data = font_data;

  return 0;
}

uint8_t *font_cache_get_letter(font_data_cache *font_cache, uint32_t letter,
                               uint32_t *buff_size) {
  uint8_t *data = NULL;
  font_info *f_info = NULL;
  for (int i = 0; i < font_cache->cache_index; i++) {
    f_info = &font_cache->font_infos[i];
    if (f_info->unicode_letter == letter) {
      data = f_info->data;
      if (buff_size) *buff_size = f_info->data_size;
    }
  }
  return data;
}
