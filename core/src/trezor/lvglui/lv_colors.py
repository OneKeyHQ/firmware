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

    ONEKEY_GREEN = LV_COLOR_MAKE(0x00, 0xFF, 0x33)  # ok button bg
    ONEKEY_GREEN_1 = LV_COLOR_MAKE(0x00, 0xB8, 0x12)
    ONEKEY_GREEN_2 = LV_COLOR_MAKE(0x00, 0xBE, 0x2D)
    ONEKEY_RED = LV_COLOR_MAKE(0x29, 0x07, 0x00)  #
    ONEKEY_RED_1 = LV_COLOR_MAKE(
        0xFF, 0x11, 0x00
    )  # danger button list item left/right text/number keyboard delete button
    ONEKEY_RED_2 = LV_COLOR_MAKE(0xDE, 0x12, 0x00)
    ONEKEY_RED_3 = LV_COLOR_MAKE(0xBE, 0x13, 0x00)
    ONEKEY_BLACK = LV_COLOR_MAKE(
        0x4B, 0x4B, 0x4B
    )  # cancel button bg enable/number keyboard
    ONEKEY_BLACK_1 = LV_COLOR_MAKE(0x33, 0x33, 0x33)  # cancel button bg disable
    ONEKEY_BLACK_2 = LV_COLOR_MAKE(0x1A, 0x1A, 0x1A)
    ONEKEY_BLACK_3 = LV_COLOR_MAKE(0x1E, 0x1E, 0x1E)
    ONEKEY_BLACK_4 = LV_COLOR_MAKE(0x0F, 0x0F, 0x0F)  # slider main disable color
    ONEKEY_GRAY = LV_COLOR_MAKE(
        0x96, 0x96, 0x96
    )  # cancel button disable text/ display item key switch enable border color
    ONEKEY_GRAY_3 = LV_COLOR_MAKE(0x2D, 0x2D, 0x2D)
    ONEKEY_GRAY_4 = LV_COLOR_MAKE(0xB4, 0xB4, 0xB4)
    LIGHT_GRAY = LV_COLOR_MAKE(
        0xD2, 0xD2, 0xD2
    )  # list item right gray text/ mnemonic tip words
    GRAY_1 = LV_COLOR_MAKE(0x61, 0x61, 0x63)  # switch main bg
    WHITE_1 = LV_COLOR_MAKE(0x99, 0x99, 0x99)  # checkbox text deselect
    WHITE_2 = LV_COLOR_MAKE(0x87, 0x87, 0x87)  # subtitle text
    WHITE_3 = LV_COLOR_MAKE(0xE5, 0xE5, 0xE5)  # scrollbar
    ONEKEY_WHITE_4 = LV_COLOR_MAKE(0xF0, 0xF0, 0xF0)  # slider enable text color
    GRAY_2 = LV_COLOR_MAKE(0xA6, 0xA6, 0xA6)  # page_able gray dot indicator
    ONEKEY_YELLOW = LV_COLOR_MAKE(0xFF, 0xD5, 0x00)
    ONEKEY_GRAY_1 = LV_COLOR_MAKE(0x69, 0x69, 0x69)  # slider border color disable
    ONEKEY_GRAY_2 = LV_COLOR_MAKE(0x3C, 0x3C, 0x3C)  # switch border color disable
    ONEKEY_PURPLE = LV_COLOR_MAKE(0x9F, 0x00, 0xFF)
    ONEKEY_DARK_BLUE = LV_COLOR_MAKE(0x00, 0x18, 0x47)
    ONEKEY_BLUE = LV_COLOR_MAKE(0x41, 0x78, 0xFF)
    ONEKEY_YELLOW_1 = LV_COLOR_MAKE(0xE0, 0xBC, 0x00)
    ONEKEY_YELLOW_2 = LV_COLOR_MAKE(0x33, 0x2C, 0x00)
