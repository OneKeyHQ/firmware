from trezor.lvglui.lv_colors import lv_colors

from .. import font_PJSMID24, lv
from .button import NormalButton
from .container import ContainerFlexRow

CONTENT_SIZE = 260


class PageAbleMessage(lv.obj):
    def __init__(self, content: str, cancel_text: str):
        self.content = content
        super().__init__(lv.scr_act())
        self.set_size(lv.pct(100), lv.pct(100))
        self.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.set_style_pad_all(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        # content
        self.message = lv.label(self)
        self.message.set_size(432, lv.SIZE.CONTENT)
        self.message.set_style_max_height(560, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.message.align(lv.ALIGN.TOP_MID, 0, 68)
        self.message.set_style_text_color(
            lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.message.set_style_text_font(font_PJSMID24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.message.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.message.set_long_mode(lv.label.LONG.WRAP)
        self.message.set_style_text_line_space(6, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.message.set_text(content[:CONTENT_SIZE])
        # close button
        self.close = NormalButton(self, cancel_text)
        self.container = ContainerFlexRow(self, self.close)
        self.pages_size = len(content) // CONTENT_SIZE + 1
        if self.pages_size > 1:
            # indicator dots
            self.select_index = 0
            self.indicators = []
            for i in range(self.pages_size):
                self.indicators.append(Indicator(self.container, i))
            self.clear_flag(lv.obj.FLAG.GESTURE_BUBBLE)
            self.add_event_cb(self.eventhandler, lv.EVENT.GESTURE, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.close:
                self.clean()
                self.del_delayed(100)
        elif code == lv.EVENT.GESTURE:
            indev = lv.indev_get_act()
            _dir = indev.get_gesture_dir()
            if _dir not in [lv.DIR.RIGHT, lv.DIR.LEFT]:
                return
            self.indicators[self.select_index].set_active(False)
            if _dir == lv.DIR.LEFT:
                self.select_index = (self.select_index + 1) % self.pages_size

            elif _dir == lv.DIR.RIGHT:
                self.select_index = (
                    self.select_index - 1 + self.pages_size
                ) % self.pages_size
            else:
                return
            self.indicators[self.select_index].set_active(True)
            self.message.set_text(
                self.content[
                    CONTENT_SIZE
                    * self.select_index : CONTENT_SIZE
                    * (self.select_index + 1)
                ]
            )


class Indicator(lv.btn):
    def __init__(self, parent, index):
        super().__init__(parent)
        self.index = index
        self.set_size(8, 8)
        self.set_style_radius(8, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv_colors.GRAY_2, lv.PART.MAIN | lv.STATE.DEFAULT)
        if index == 0:
            self.set_active(True)

    def set_active(self, active):
        if active:
            self.active = True
            self.set_style_bg_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        else:
            self.active = False
            self.set_style_bg_color(lv_colors.GRAY_2, lv.PART.MAIN | lv.STATE.DEFAULT)
