from trezor import utils
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.lv_colors import lv_colors

from .common import FullSizeWindow, lv
from .components.button import NormalButton
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
        super().__init__(title, subtitle, icon_path=icon_path)
        self.container = ContainerFlexCol(self, self.subtitle, padding_row=10)
        self.item1 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_WIPE_DEVICE_FACTORY_RESET_1),
        )
        self.item2 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_WIPE_DEVICE_FACTORY_RESET_2),
        )
        self.btn_yes = NormalButton(
            self, _(i18n_keys.BUTTON__HOLD_TO_RESET), enable=False
        )
        self.btn_yes.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 54)
        self.btn_no = NormalButton(self, _(i18n_keys.BUTTON__CANCEL), enable=True)
        self.btn_no.align_to(self.btn_yes, lv.ALIGN.OUT_BOTTOM_MID, 0, 16)
        self.container.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.LONG_PRESSED, None)
        self.cb_cnt = 0

    def btn_enable(self, enable: bool = True):
        if enable:
            self.btn_yes.enable(lv_colors.ONEKEY_RED_1)
        else:
            self.btn_yes.disable()

    def eventhandler(self, event_obj):
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
                self.btn_enable()
            elif self.cb_cnt < 2:
                self.btn_enable(False)
            return
        elif code == lv.EVENT.CLICKED:
            if target == self.btn_no:
                self.channel.publish(0)
            elif target == self.btn_yes:
                if utils.EMULATOR:
                    self.channel.publish(1)
                else:
                    return
            else:
                return
        elif code == lv.EVENT.LONG_PRESSED:
            if target == self.btn_yes:
                self.channel.publish(1)
        self.destroy()


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
