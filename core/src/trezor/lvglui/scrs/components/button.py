from .. import font_PJSBOLD24, lv

# class NormalButton(lv.btn):
#     def __init__(self, parent, text="Next", pos=(-6, 300), enable=True) -> None:
#         super().__init__(parent)
#         self.set_size(320, 62)
#         self.set_pos(pos[0], pos[1])
#         self.set_align(lv.ALIGN.CENTER)
#         self.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.set_style_text_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         if enable:
#             self.enable()
#         else:
#             self.disable()
#         # the next btn label
#         self.label = lv.label(self)
#         self.label.set_long_mode(lv.label.LONG.WRAP)
#         self.label.set_text(text)
#         self.label.set_align(lv.ALIGN.CENTER)

#     def disable(self) -> None:
#         self.set_style_bg_color(lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.clear_flag(lv.btn.FLAG.CLICKABLE)

#     def enable(self) -> None:
#         self.set_style_bg_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.add_flag(lv.btn.FLAG.CLICKABLE)


class NormalButton(lv.btn):
    def __init__(self, parent, text="Next", enable=True) -> None:
        super().__init__(parent)
        self.set_size(320, 62)
        self.align_to(parent, lv.ALIGN.BOTTOM_MID, 0, -64)
        self.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_color(
            lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        if enable:
            self.enable()
        else:
            self.disable()
        # the next btn label
        self.label = lv.label(self)
        self.label.set_long_mode(lv.label.LONG.WRAP)
        self.label.set_text(text)
        self.label.set_align(lv.ALIGN.CENTER)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def disable(
        self, bg_color=lv.color_hex(0x323232), text_color=lv.color_hex(0x000000)
    ) -> None:
        self.set_style_bg_color(bg_color, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_color(text_color, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.clear_flag(lv.btn.FLAG.CLICKABLE)

    def enable(
        self, bg_color=lv.color_hex(0xFFFFFF), text_color=lv.color_hex(0x000000)
    ) -> None:
        self.set_style_bg_color(bg_color, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_color(text_color, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.add_flag(lv.btn.FLAG.CLICKABLE)


class ListItemBtn(lv.btn):
    def __init__(
        self,
        parent,
        text: str,
        right_text="",
        left_img_src: str = "",
        has_next: bool = True,
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        if left_img_src:
            img_left = lv.img(self)
            img_left.set_src(left_img_src)
            img_left.set_align(lv.ALIGN.LEFT_MID)
        if has_next:
            self.img_right = lv.img(self)
            self.img_right.set_src("A:/res/arrow-right.png")
            self.img_right.set_align(lv.ALIGN.RIGHT_MID)
        self.label_left = lv.label(self)
        self.label_left.set_text(text)
        self.label_left.set_style_text_font(
            font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label_left.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        if left_img_src:
            self.label_left.align_to(img_left, lv.ALIGN.OUT_RIGHT_MID, 10, 0)  # type: ignore["img_left" is possibly unbound]
        else:
            self.label_left.set_align(lv.ALIGN.LEFT_MID)
        if right_text:
            self.label_right = lv.label(self)
            self.label_right.set_text(right_text)
            self.label_right.set_style_text_font(
                font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.label_right.set_style_text_color(
                lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            if has_next:
                self.label_right.align_to(self.img_right, lv.ALIGN.OUT_LEFT_MID, -10, 0)
            else:
                self.label_right.set_align(lv.ALIGN.RIGHT_MID)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def add_check_img(self) -> None:
        self.img_right = lv.img(self)
        self.img_right.set_src("A:/res/check.png")
        self.img_right.set_align(lv.ALIGN.RIGHT_MID)
        self.img_right.add_flag(lv.obj.FLAG.HIDDEN)

    def set_checked(self) -> None:
        self.img_right.clear_flag(lv.obj.FLAG.HIDDEN)
        self.set_style_bg_color(lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT)

    def set_uncheck(self) -> None:
        self.img_right.add_flag(lv.obj.FLAG.HIDDEN)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)


class ListItemBtnWithSwitch(lv.btn):
    def __init__(self, parent, text: str) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        label_left = lv.label(self)
        label_left.set_text(text)
        label_left.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        label_left.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        label_left.set_align(lv.ALIGN.LEFT_MID)
        self.switch = lv.switch(self)
        self.switch.set_size(48, 24)
        self.switch.set_style_bg_color(
            lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.switch.set_style_bg_color(
            lv.color_hex(0x1BAC44), lv.PART.INDICATOR | lv.STATE.CHECKED
        )
        self.switch.set_style_bg_color(
            lv.color_hex(0xFFFFFF), lv.PART.KNOB | lv.STATE.DEFAULT
        )
        self.switch.set_align(lv.ALIGN.RIGHT_MID)
        self.switch.add_state(lv.STATE.CHECKED)
        self.switch.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def clear_state(self) -> None:
        self.switch.clear_state(lv.STATE.CHECKED)

    def add_state(self) -> None:
        self.switch.add_state(lv.STATE.CHECKED)
