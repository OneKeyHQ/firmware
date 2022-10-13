import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


def LV_COLOR_MAKE(r, g, b):
    #    return lv.color_hex(r<<16| g<<8 |b)
    return lv.color_make(r, g, b)


class lv_colors:
    WHITE = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF)
    SILVER = LV_COLOR_MAKE(0xC0, 0xC0, 0xC0)
    GRAY = LV_COLOR_MAKE(0x80, 0x80, 0x80)
    BLACK = LV_COLOR_MAKE(0x00, 0x00, 0x00)
    RED = LV_COLOR_MAKE(0xFF, 0x00, 0x00)
    MAROON = LV_COLOR_MAKE(0x80, 0x00, 0x00)
    YELLOW = LV_COLOR_MAKE(0xFF, 0xFF, 0x00)
    OLIVE = LV_COLOR_MAKE(0x80, 0x80, 0x00)
    LIME = LV_COLOR_MAKE(0x00, 0xFF, 0x00)
    GREEN = LV_COLOR_MAKE(0x00, 0x80, 0x00)
    CYAN = LV_COLOR_MAKE(0x00, 0xFF, 0xFF)
    AQUA = CYAN
    TEAL = LV_COLOR_MAKE(0x00, 0x80, 0x80)
    BLUE = LV_COLOR_MAKE(0x00, 0x00, 0xFF)
    NAVY = LV_COLOR_MAKE(0x00, 0x00, 0x80)
    MAGENTA = LV_COLOR_MAKE(0xFF, 0x00, 0xFF)
    PURPLE = LV_COLOR_MAKE(0x80, 0x00, 0x80)
    ORANGE = LV_COLOR_MAKE(0xFF, 0xA5, 0x00)

    ONEKEY_GREEN = LV_COLOR_MAKE(0x00, 0xCC, 0x36)  # ok button bg
    ONEKEY_GREEN_1 = LV_COLOR_MAKE(0x00, 0xB8, 0x12)
    # ONEKEY_RED = LV_COLOR_MAKE(0xAF, 0x2B, 0x0E) #
    ONEKEY_RED_1 = LV_COLOR_MAKE(
        0xDF, 0x32, 0x0C
    )  # danger button list item left/right text/number keyboard delete button
    ONEKEY_BLACK = LV_COLOR_MAKE(
        0x4D, 0x4D, 0x4D
    )  # cancel button bg enable/number keyboard
    ONEKEY_BLACK_1 = LV_COLOR_MAKE(0x33, 0x33, 0x33)  # cancel button bg disable
    ONEKEY_GRAY = LV_COLOR_MAKE(
        0x8D, 0x8D, 0x93
    )  # cancel button disable text/ display item key
    LIGHT_GRAY = LV_COLOR_MAKE(
        0xCC, 0xCC, 0xCC
    )  # list item right gray text/ mnemonic tip words
    GRAY_1 = LV_COLOR_MAKE(0x66, 0x66, 0x66)  # switch main bg
    WHITE_1 = LV_COLOR_MAKE(0x99, 0x99, 0x99)  # checkbox text deselect
    WHITE_2 = LV_COLOR_MAKE(0xCC, 0xCC, 0xCC)  # subtitle text
    WHITE_3 = LV_COLOR_MAKE(0xE5, 0xE5, 0xE5)  # scrollbar
    GRAY_2 = LV_COLOR_MAKE(0xA6, 0xA6, 0xA6)  # page_able gray dot indicator
    ONEKEY_YELLOW = LV_COLOR_MAKE(0xFF, 0xCC, 0x00)
