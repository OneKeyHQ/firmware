from . import *
from .common import FullSizeWindow
from .components.button import NormalButton
from .components.container import ContainerFlexCol
from .components.listitem import ListItemWithLeadingCheckbox


class WipeDevice(FullSizeWindow):
    def __init__(self):
        title = "Wipe device"
        subtitle = "To remove all data from your device, you can reset your device to factory default."
        confirm_text = "Verify PIN"
        cancel_text = "Cancel"
        icon_path = "A:/res/warning.png"
        super().__init__(title, subtitle, confirm_text, cancel_text, icon_path, None)
        self.btn_yes.set_style_bg_color(
            lv.color_hex(0xAF2B0E), lv.PART.MAIN | lv.STATE.DEFAULT
        )


class WipeDeviceTips(FullSizeWindow):
    def __init__(self):
        title = "Factory Reset"
        subtitle = "This will erase all data on internal storage and Secure Element (SE). Beforehand, you need to know:"
        icon_path = "A:/res/danger.png"
        super().__init__(title, subtitle, icon_path=icon_path)
        self.container = ContainerFlexCol(self, self.subtitle)
        self.item1 = ListItemWithLeadingCheckbox(
            self.container,
            "After reset, the recovery phrase on this device will be permanently deleted.",
        )
        self.item2 = ListItemWithLeadingCheckbox(
            self.container,
            "You can still restore your funds from recovery phrase backup.",
        )
        self.btn_yes = NormalButton(self, "Hold to Reset", enable=False)
        self.btn_yes.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 54)
        self.btn_no = NormalButton(self, "Cancel", enable=True)
        self.btn_no.align_to(self.btn_yes, lv.ALIGN.OUT_BOTTOM_MID, 0, 16)
        self.btn_no.label.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.btn_no.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.container.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(
            self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.LONG_PRESSED, None
        )
        self.cb_cnt = 0

    def btn_enable(self, enable: bool = True):
        if enable:
            self.btn_yes.enable(lv.color_hex(0xAF2B0E), lv.color_hex(0xFFFFFF))
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
        elif code == lv.EVENT.CLICKED:
            if target == self.btn_no:
                self.channel.publish(0)
            elif target == self.btn_yes:
                self.channel.publish(1)
            self.destory()
        elif code == lv.EVENT.LONG_PRESSED:
            if __debug__:
                print("long pressed")
            if target == self.btn_yes:
                self.channel.publish(1)


class WipeDeviceSuccess(FullSizeWindow):
    def __init__(self):
        title = "Reset Complete"
        subtitle = "Device reset successfully, restart system now."
        icon_path = "A:/res/success_icon.png"
        confirm_text = "Done"
        super().__init__(
            title, subtitle, confirm_text=confirm_text, icon_path=icon_path
        )
        self.btn_yes.enable(lv.color_hex(0x323232), lv.color_hex(0xFFFFFF))

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                self.channel.publish(1)
            self.destory()
            from apps.base import set_homescreen

            set_homescreen()
