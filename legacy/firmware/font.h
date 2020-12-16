#ifndef __FONT_H_
#define __FONT_H_

#include <stdint.h>
#include <string.h>

#define DEFAULT_IDX 0
#define SONGTI_12_IDX 1
#define DINGMAO_9_IDX 2

struct font_desc {
  int idx;
  const char *name;
  int pixel;
  int width;
  const uint8_t *data;
  const uint8_t *empty;
};
const struct font_desc *find_font(const char *name);
const struct font_desc *find_cur_font(void);
void font_set(const char *name);
const uint8_t *get_font_data(const char *zh);

#endif
