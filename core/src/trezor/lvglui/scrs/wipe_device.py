from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.lv_colors import lv_colors

from .common import FullSizeWindow, lv
from .components.container import ContainerFlexCol
from .components.listitem import ListItemWithLeadingCheckbox


class WipeDevice(FullSizeWindow):
    def __init__(self):
        title = _(i18n_keys.TITLE__WIPE_DEVICE)
        subtitle = _(i18n_keys.SUBTITLE_WIPE_DEVICE_WIPE_DEVICE)
        confirm_text = _(i18n_keys.BUTTON__VERIFY_PIN)
        cancel_text = _(i18n_keys.BUTTON__CANCEL)
        icon_path = "A:/res/warning.png"
        super().__init__(title, subtitle, confirm_text, cancel_text, icon_path, None)
        self.btn_yes.set_style_bg_color(
            lv_colors.ONEKEY_RED_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )


class WipeDeviceTips(FullSizeWindow):
    def __init__(self):
        title = _(i18n_keys.TITLE__FACTORY_RESET)
        subtitle = _(i18n_keys.SUBTITLE__DEVICE_WIPE_DEVICE_FACTORY_RESET)
        icon_path = "A:/res/danger.png"
        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__SLIDE_TO_RESET),
            _(i18n_keys.BUTTON__CANCEL),
            icon_path=icon_path,
            hold_confirm=True,
        )
        self.container = ContainerFlexCol(
            self.content_area, self.subtitle, padding_row=10
        )
        self.item1 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_WIPE_DEVICE_FACTORY_RESET_1),
        )
        self.item2 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_WIPE_DEVICE_FACTORY_RESET_2),
        )
        self.slider_enable(False)
        self.btn_no.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)
        self.cb_cnt = 0

    def slider_enable(self, enable: bool = True):
        if enable:
            self.slider.add_flag(lv.obj.FLAG.CLICKABLE)
            self.slider.enable()
        else:
            self.slider.clear_flag(lv.obj.FLAG.CLICKABLE)
            self.slider.enable(False)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
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
                self.slider_enable()
            elif self.cb_cnt < 2:
                self.slider_enable(False)


class WipeDeviceSuccess(FullSizeWindow):
    def __init__(self):
        title = _(i18n_keys.TITLE__RESET_COMPLETE)
        subtitle = _(i18n_keys.SUBTITLE__DEVICE_WIPE_DEVICE_RESET_COMPLETE)
        icon_path = "A:/res/success.png"
        confirm_text = _(i18n_keys.BUTTON__RESTART)
        super().__init__(
            title, subtitle, confirm_text=confirm_text, icon_path=icon_path
        )

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                self.channel.publish(1)
            self.destroy()
            from apps.base import set_homescreen

            set_homescreen()
