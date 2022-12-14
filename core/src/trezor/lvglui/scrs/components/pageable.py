from trezor.lvglui.lv_colors import lv_colors

from ...i18n import gettext as _, keys as i18n_keys
from .. import font_PJSREG30, lv
from ..common import FullSizeWindow
from ..widgets.style import StyleWrapper
from .container import ContainerFlexRow

CONTENT_SIZE = 240


class PageAbleMessage(FullSizeWindow):
    def __init__(
        self,
        title: str,
        content: str,
        channel,
        primary_color=lv_colors.ONEKEY_GREEN,
        confirm_text=_(i18n_keys.BUTTON__CONTINUE),
    ):
        super().__init__(
            title,
            None,
            confirm_text=confirm_text,
            cancel_text=_(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            anim_dir=0,
        )
        self.content = content
        self.channel = channel
        # self.set_size(lv.pct(100), lv.pct(100))
        # self.align(lv.ALIGN.TOP_LEFT, 0, 0)
        # self.set_style_pad_all(0, 0)
        # self.set_style_bg_opa(255, 0)
        # self.set_style_bg_color(lv.color_hex(0x000000), 0)
        # self.set_style_border_width(0, 0)
        # self.set_style_radius(0, 0)
        # content
        self.message = lv.label(self.content_area)
        self.message.set_size(464, 468)
        self.message.align_to(self.title, lv.ALIGN.OUT_BOTTOM_MID, 0, 40)
        self.message.set_long_mode(lv.label.LONG.WRAP)
        self.message.add_style(
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_align_left()
            .text_letter_space(3)
            .text_letter_space(-1)
            .text_color(lv_colors.WHITE),
            0,
        )
        self.message.set_text(content[:CONTENT_SIZE])
        # # close button
        # self.close = NormalButton(self, cancel_text)
        self.container = ContainerFlexRow(self, None, padding_col=4)
        self.container.align(lv.ALIGN.BOTTOM_MID, 0, -130)
        self.pages_size = len(content) // CONTENT_SIZE + 1
        if self.pages_size > 1:
            # indicator dots
            self.select_index = 0
            self.indicators = []
            for i in range(self.pages_size):
                self.indicators.append(Indicator(self.container, i))
            self.clear_flag(lv.obj.FLAG.GESTURE_BUBBLE)
            self.add_event_cb(self.on_gesture, lv.EVENT.GESTURE, None)
        # self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def on_gesture(self, event_obj):
        code = event_obj.code
        if code == lv.EVENT.GESTURE:
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
        self.set_size(16, 8)
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_GRAY_1)
            .bg_opa(lv.OPA.COVER)
            .border_width(0)
            .radius(0),
            0,
        )
        if index == 0:
            self.set_active(True)

    def set_active(self, active):
        if active:
            self.active = True
            self.set_style_bg_color(lv_colors.WHITE, 0)
        else:
            self.active = False
            self.set_style_bg_color(lv_colors.ONEKEY_GRAY_1, 0)
