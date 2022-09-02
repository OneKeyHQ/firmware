from ..i18n import gettext as _, keys as i18n_keys
from .common import FullSizeWindow, lv
from .components.keyboard import PassphraseKeyboard


class PassphraseRequest(FullSizeWindow):
    def __init__(self, max_len):
        super().__init__(_(i18n_keys.TITLE__ENTER_PASSPHRASE), None)
        self.keyboard = PassphraseKeyboard(self, max_len)
        self.keyboard.add_event_cb(self.on_ready, lv.EVENT.READY, None)

        self.nav_back = lv.imgbtn(self)
        self.nav_back.set_size(48, 48)
        self.nav_back.set_pos(24, 92)
        self.nav_back.set_ext_click_area(100)
        self.nav_back.set_style_bg_img_src(
            "A:/res/nav-back.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.nav_back.add_event_cb(self.on_cancel, lv.EVENT.CLICKED, None)

    def on_ready(self, event_obj):
        input = self.keyboard.ta.get_text()
        self.channel.publish(input)
        self.keyboard.ta.set_text("")
        self.destroy()

    def on_cancel(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED and target == self.nav_back:
            self.channel.publish(None)
