from ..i18n import gettext as _, keys as i18n_keys
from . import font_PJSBOLD48, lv, lv_colors
from .common import FullSizeWindow


class PairCodeDisplay(FullSizeWindow):
    def __init__(self, pair_code: str):
        super().__init__(
            _(i18n_keys.TITLE__BLUETOOTH_PAIR),
            _(i18n_keys.SUBTITLE__BLUETOOTH_PAIR),
            _(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/ble-blue.png",
        )
        self.pair_code = lv.label(self)
        self.pair_code.set_long_mode(lv.label.LONG.WRAP)
        self.pair_code.set_style_text_color(
            lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.pair_code.set_style_text_font(
            font_PJSBOLD48, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.pair_code.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.pair_code.set_style_text_letter_space(-2, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.pair_code.set_text(pair_code)
        self.pair_code.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 40)
        self.btn_yes.enable(lv_colors.ONEKEY_BLACK)
        self.destroyed = False

    def destroy(self, delay_ms=100):
        super().destroy(delay_ms)
        self.destroyed = True
