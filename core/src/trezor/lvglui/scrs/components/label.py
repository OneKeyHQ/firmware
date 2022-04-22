from .. import font_PJSBOLD36, font_PJSREG24, lv


class Title(lv.label):
    def __init__(self, parent, align_base, width, relative_pos: tuple, text: str):
        super().__init__(parent)
        self.set_long_mode(lv.label.LONG.WRAP)
        self.set_text(text)
        self.set_size(width, lv.SIZE.CONTENT)
        if align_base:
            self.align_to(
                align_base, lv.ALIGN.OUT_BOTTOM_MID, relative_pos[0], relative_pos[1]
            )
        else:
            self.align(lv.ALIGN.TOP_MID, 0, 132)
        self.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT)


class SubTitle(lv.label):
    def __init__(
        self, parent, align_base, width, relative_pos: tuple, text: str
    ) -> None:
        super().__init__(parent)
        self.set_long_mode(lv.label.LONG.WRAP)
        self.set_text(text)
        self.set_size(width, lv.SIZE.CONTENT)
        self.align_to(
            align_base, lv.ALIGN.OUT_BOTTOM_MID, relative_pos[0], relative_pos[1]
        )
        self.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_text_font(font_PJSREG24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT)
