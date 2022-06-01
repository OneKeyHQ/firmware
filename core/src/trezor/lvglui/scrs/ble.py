from ..i18n import gettext as _, keys as i18n_keys
from . import font_PJSBOLD36, lv
from .common import FullSizeWindow


class PairCodeDisplay(FullSizeWindow):
    def __init__(self, pair_code: str):
        super().__init__(
            _(i18n_keys.TITLE__BLUETOOTH_PAIR),
            _(i18n_keys.SUBTITLE__BLUETOOTH_PAIR),
            _(i18n_keys.BUTTON__DONE),
            icon_path="a:/res/icon_ble.png",
        )
        self.pair_code = lv.label(self)
        self.pair_code.set_long_mode(lv.label.LONG.WRAP)
        self.pair_code.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.pair_code.set_style_text_font(
            font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.pair_code.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.pair_code.set_style_text_letter_space(8, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.pair_code.set_text(pair_code)
        self.pair_code.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 40)
        self.btn_yes.enable(lv.color_hex(0x323232), lv.color_hex(0xFFFFFF))
        self.destroyed = False

    def destroy(self, delay_ms=100):
        super().destroy(delay_ms)
        self.destroyed = True
