from .common import *  # noqa: F401,F403
from .components.button import NormalButton
from .components.container import ContainerFlexCol
from .components.keyboard import NumberKeyboard
from .components.listitem import ListItemWithLeadingCheckbox


class PinTip(FullSizeWindow):
    def __init__(self):
        super().__init__(
            "Set a PIN",
            "Set a PIN to protect you wallet. OneKey will ask for PIN eachtime when unlock your device. So you need to know:",
        )
        self.container = ContainerFlexCol(self, self.subtitle, pos=(0, 10))
        self.container.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.item1 = ListItemWithLeadingCheckbox(
            self.container,
            "Using a strong PIN to protects your wallet from unauthorized physical access.",
        )
        self.item2 = ListItemWithLeadingCheckbox(
            self.container,
            "Keeping your PIN secured, be sure to store it separate from recovery phrase.",
        )
        self.btn = NormalButton(self, "Continue", False)
        self.container.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        self.cb_cnt = 0

    def eventhandler(self, event_obj: lv.event_t):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn:
                self.channel.publish(1)
                self.destory()
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
                self.btn.enable(
                    bg_color=lv.color_hex(0x1B7735), text_color=lv.color_hex(0xFFFFFF)
                )
            elif self.cb_cnt < 2:
                self.btn.disable()


class InputPin(FullSizeWindow):
    def __init__(self, **kwargs):
        super().__init__(
            title=kwargs.get("title", "Enter PIN"), subtitle=kwargs.get("subtitle", "")
        )
        self.keyboard = NumberKeyboard(self)
        self.keyboard.add_event_cb(self.on_event, lv.EVENT.READY, None)
        self.keyboard.ta.add_event_cb(self.on_event, lv.EVENT.VALUE_CHANGED, None)

    def on_event(self, event_obj):
        code = event_obj.code
        if code == lv.EVENT.VALUE_CHANGED:
            if self.keyboard.ta.get_text() != "":
                self.subtitle.set_text("")
            return
        input = self.keyboard.ta.get_text()
        if input == "" or len(input) < 5:
            self.subtitle.set_text("Length must be 4 digits or more")
            self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
            return
        self.destory()
        self.channel.publish(input)


class SetupComplete(FullSizeWindow):
    def __init__(self, subtitle=""):
        super().__init__(
            title="Setup Complete",
            subtitle=subtitle,
            confirm_text="Done",
            icon_path="A:/res/success_icon.png",
        )
        self.btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event_obj: lv.event_t):
        self.channel.publish(1)
        self.destory()
        lv.scr_act().del_delayed(500)
        from apps.base import set_homescreen

        set_homescreen()
