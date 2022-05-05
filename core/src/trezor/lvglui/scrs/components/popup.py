from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]

from .button import NormalButton
from .container import ContainerFlexCol
from .label import SubTitle, Title
from .listitem import ListItemWithLeadingCheckbox


class Popup(lv.obj):
    def __init__(
        self,
        parent,
        title: str,
        subtitle: str = "",
        icon_path: str = "",
        btn_text: str = "",
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), 750)
        self.set_pos(0, 50)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.close_img = lv.img(self)
        self.close_img.set_src("A:/res/close.png")
        self.close_img.align(lv.ALIGN.TOP_RIGHT, 0, 0)
        self.close_img.add_flag(lv.obj.FLAG.CLICKABLE | lv.obj.FLAG.EVENT_BUBBLE)
        self.icon = lv.img(self)
        self.icon.set_src(icon_path)
        self.icon.align(lv.ALIGN.TOP_MID, 0, 20)
        self.title = Title(self, self.icon, 416, (0, 36), title)
        self.subtitle = SubTitle(self, self.title, 416, (0, 24), subtitle)
        self.container = ContainerFlexCol(self, self.subtitle, pos=(0, 10))
        self.container.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.item1 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.FORM__OPTION__CRYPTO_BLIND_SIGN_ENABLED_1),
        )
        self.item2 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.FORM__OPTION__CRYPTO_BLIND_SIGN_ENABLED_2),
        )
        self.btn = NormalButton(self, btn_text, False)
        self.btn.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 48)
        self.container.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        self.cb_cnt = 0
        self.parent = parent

    def eventhandler(self, event_obj: lv.event_t):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.close_img:
                self.parent.blind_sign.clear_state()
                self.del_delayed(100)
            elif target == self.btn:
                self.delete()
        elif code == lv.EVENT.VALUE_CHANGED:
            if target == self.item1.checkbox:
                if target.get_state() & lv.STATE.CHECKED:
                    self.item1.enable_bg_color()
                    self.cb_cnt += 1
                else:
                    self.item1.enable_bg_color(False)
                    self.cb_cnt -= 1
            elif target == self.item2.checkbox:
                if target.get_state() & lv.STATE.CHECKED:
                    self.item2.enable_bg_color()
                    self.cb_cnt += 1
                else:
                    self.item2.enable_bg_color(False)
                    self.cb_cnt -= 1
            if self.cb_cnt == 2:
                self.btn.enable()
            elif self.cb_cnt < 2:
                self.btn.disable()


class PopupSample(lv.obj):
    def __init__(
        self, title: str, subtitle: str, icon_path: str, del_delay: int
    ) -> None:
        super().__init__(lv.scr_act())
        self.set_size(lv.pct(100), 750)
        self.set_pos(0, 50)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.icon = lv.img(self)
        self.icon.set_src(icon_path)
        self.icon.align(lv.ALIGN.TOP_MID, 0, 20)
        self.title = Title(self, self.icon, 416, (0, 36), title)
        self.subtitle = SubTitle(self, self.title, 416, (0, 24), subtitle)
        self.del_delayed(del_delay)
