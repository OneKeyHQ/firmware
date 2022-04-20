from .. import font_PJSMID20, lv


class ListItemWithLeadingCheckbox(lv.obj):
    def __init__(self, parent, text):
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.checkbox = lv.checkbox(self)
        self.checkbox.set_size(40, 40)
        self.checkbox.set_align(lv.ALIGN.TOP_LEFT)
        self.checkbox.set_text("")
        self.checkbox.set_style_radius(8, lv.PART.INDICATOR | lv.STATE.DEFAULT)
        self.checkbox.set_style_border_color(
            lv.color_hex(0x1E1E1E), lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.checkbox.set_style_border_opa(255, lv.PART.INDICATOR | lv.STATE.DEFAULT)
        self.checkbox.set_style_bg_color(
            lv.color_hex(0x1BAC44), lv.PART.INDICATOR | lv.STATE.CHECKED
        )
        self.checkbox.set_style_bg_opa(255, lv.PART.INDICATOR | lv.STATE.CHECKED)
        self.checkbox.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.label = lv.label(self)
        self.label.set_size(340, lv.SIZE.CONTENT)
        self.label.align_to(self.checkbox, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)
        self.label.set_text(text)
        self.label.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label.set_style_text_font(font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def get_checkbox(self):
        return self.checkbox

    def get_label(self):
        return self.label

    def enable_bg_color(self, enable: bool = True):
        if enable:
            self.set_style_bg_color(
                lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
            )
        else:
            self.set_style_bg_color(
                lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
            )


class DisplayItem(lv.obj):
    def __init__(self, parent, title, content):
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_all(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label_top = lv.label(self)
        self.label_top.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.label_top.set_style_text_color(
            lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label_top.set_text(title)
        self.label_top.set_align(lv.ALIGN.TOP_LEFT)

        self.label = lv.label(self)
        self.label.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.label.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label.set_text(content)
        self.label.align_to(self.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 5)
        self.set_style_text_font(font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT)
