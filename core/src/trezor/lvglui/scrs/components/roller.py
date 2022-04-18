from .. import *


class Roller(lv.roller):
    def __init__(self, parent, options) -> None:
        super().__init__(parent)
        self.set_options(options, lv.roller.MODE.NORMAL)  # lv.roller.MODE.INFINITE
        self.set_size(448, 157)
        self.align(lv.ALIGN.BOTTOM_MID, 0, -176)
        self.set_style_text_color(
            lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_line_space(40, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(24, lv.PART.MAIN | lv.PART.SELECTED | lv.STATE.DEFAULT)
        # self.set_style_radius(24, lv.PART.SELECTED | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(
            lv.color_hex(0x191919), lv.PART.SELECTED | lv.STATE.DEFAULT
        )
        self.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.SELECTED | lv.STATE.DEFAULT)
        self.set_style_border_color(
            lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_border_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_visible_row_count(3)
        self.set_selected(1, lv.ANIM.OFF)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
