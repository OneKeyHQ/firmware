#include "layout_boot.h"

void layoutBootHome(void) {
  if (layoutNeedRefresh()) {
    oledClear();
    oledDrawBitmap(30, 20, &bmp_BiXin_logo32);
    oledDrawStringCenter(85, 20, "BiXin", FONT_STANDARD);
    oledDrawStringCenter(85, 30, "Bootloader", FONT_STANDARD);
    oledDrawStringCenter(85, 40,
                         VERSTR(VERSION_MAJOR) "." VERSTR(
                             VERSION_MINOR) "." VERSTR(VERSION_PATCH),
                         FONT_STANDARD);
    layoutFillBleName(6);
    oledRefresh();
  }
}
