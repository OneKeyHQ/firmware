from storage import device
from trezor import ui, utils
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

from . import font_PJSREG24, font_PJSREG30
from .common import Screen, lv, lv_colors
from .widgets.style import StyleWrapper


class LockScreen(Screen):
    def __init__(self, device_name, ble_name="", dev_state=None):
        lockscreen = device.get_homescreen()
        if not hasattr(self, "_init"):
            self._init = True
            super().__init__(
                title=device_name, subtitle=ble_name, icon_path="A:/res/lock.png"
            )
        else:
            if ble_name:
                self.subtitle.set_text(ble_name)
            self.set_style_bg_img_src(lockscreen, 0)
            return
        self.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        self.icon.align(lv.ALIGN.TOP_MID, 0, 52)
        self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_MID, -4, 24)
        self.subtitle.set_style_text_color(lv_colors.WHITE, 0)
        self.subtitle.set_style_text_font(font_PJSREG30, 0)

        self.add_style(
            StyleWrapper().bg_img_src(lockscreen).bg_img_opa(int(lv.OPA.COVER * 0.72)),
            0,
        )
        self.tap_tip = lv.label(self)
        self.tap_tip.set_long_mode(lv.label.LONG.WRAP)
        self.tap_tip.set_text(_(i18n_keys.LOCKED_TEXT__TAP_TO_UNLOCK))
        self.tap_tip.align(lv.ALIGN.BOTTOM_MID, 0, -16)
        self.tap_tip.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .text_letter_space(-1)
            .max_width(464)
            .text_align_center(),
            0,
        )

    def eventhandler(self, event_obj: lv.event_t):
        code = event_obj.code
        if code == lv.EVENT.CLICKED:
            if self.channel.takers:
                self.channel.publish("clicked")
            else:
                if not ui.display.backlight() and not device.is_tap_awake_enabled():
                    return
                if utils.turn_on_lcd_if_possible():
                    return
                from trezor import workflow
                from apps.base import unlock_device

                workflow.spawn(unlock_device())

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)
