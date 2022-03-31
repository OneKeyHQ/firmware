import lvgl as lv
from ..common import *
class SubTitleStyle(lv.style_t):
    def __init__(self) -> None:
        super().__init__()
        self.set_align(lv.ALIGN.TOP_MID)
        self.set_text_color(lv.color_hex(0xCCCCCC))
        self.set_text_opa(255)
        self.set_text_align(lv.TEXT_ALIGN.CENTER)
        self.set_text_font(font_PJSMID20)


