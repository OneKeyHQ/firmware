from storage import device
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.ui import display

from . import font_PJSBOLD24
from .common import Screen, load_scr_with_animation, lv, lv_colors


class LockScreen(Screen):
    def __init__(self, device_name, dev_state=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            if dev_state:
                self.dev_state_text.set_text(dev_state)
            elif hasattr(self, "dev_state") and dev_state is None:
                self.dev_state.delete()
            if not self.is_visible():
                load_scr_with_animation(self)
            return
        super().__init__(title=device_name or "My OneKey")
        if dev_state:
            self.dev_state = lv.btn(self)
            self.dev_state.set_size(lv.pct(96), lv.SIZE.CONTENT)
            self.dev_state.set_style_bg_color(
                lv.color_hex(0xDCA312), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.dev_state.set_style_radius(8, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.dev_state.align(lv.ALIGN.TOP_MID, 0, 52)
            self.dev_state_text = lv.label(self.dev_state)
            self.dev_state_text.set_text(dev_state)
            self.dev_state_text.set_style_text_color(
                lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.dev_state_text.set_style_text_font(
                font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.dev_state_text.center()
        self.set_style_bg_img_src(
            "A:/res/wallpaper_dark.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_bg_img_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.tap_tip = lv.label(self)
        self.tap_tip.set_long_mode(lv.label.LONG.WRAP)
        self.tap_tip.set_text(_(i18n_keys.LOCKED_TEXT__TAP_TO_UNLOCK))
        self.tap_tip.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
        self.tap_tip.align(lv.ALIGN.BOTTOM_MID, 0, -100)
        self.tap_tip.set_style_text_font(
            font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event_obj: lv.event_t):
        code = event_obj.code
        if code == lv.EVENT.CLICKED:
            if self.channel.takers:
                self.channel.publish("clicked")
            else:
                if not display.backlight():
                    from apps.base import reload_settings_from_storage

                    display.backlight(device.get_brightness())
                    # reschedule auto_lock task to avoid never turn off lcd (turn the brightness of the lcd to zero)
                    reload_settings_from_storage()
                    return
                from trezor import workflow
                from apps.base import unlock_device

                workflow.spawn(unlock_device())
