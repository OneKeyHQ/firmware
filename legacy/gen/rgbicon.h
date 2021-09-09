#ifndef __RGBICON_H__
#define __RGBICON_H__

#include <stdint.h>

#include "bitmaps.h"

typedef struct {
  uint8_t width, height;
  uint8_t *data;
} BITMAP_EX;

extern const BITMAP bmp_icon_forbid;
extern const BITMAP bmp_icon_success;
extern const BITMAP bmp_icon_warn;
extern const BITMAP bmp_icon_err;
extern const BITMAP bmp_icon_egg;

#endif
