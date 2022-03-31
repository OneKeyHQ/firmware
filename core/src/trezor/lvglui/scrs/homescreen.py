from storage import device
from trezor import workflow
from trezor.messages import ChangePin
from trezor.wire import DUMMY_CONTEXT

from apps.management.change_pin import change_pin

from .common import *
from .components.button import ListItemBtn, ListItemBtnWithSwitch
from .components.container import ContainerFlexCol, ContanierGrid
from .components.imgbtn import ImgBottonGridItem


class MainScreen(Screen):
    def __init__(self):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            if not self.is_visible():
                load_scr_with_animation(self)
            return
        super().__init__()
        self.set_style_bg_img_src(
            "A:/res/wallpaper_light.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_bg_img_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.btn_settings = lv.btn(self)
        self.btn_settings.set_size(144, 88)
        self.btn_settings.set_pos(64, 92)
        self.btn_settings.set_style_radius(44, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btn_settings.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.btn_settings.set_style_bg_img_src(
            "A:/res/settings.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.btn_settings.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.btn_info = lv.btn(self)
        self.btn_info.set_size(144, 88)
        self.btn_info.align_to(self.btn_settings, lv.ALIGN.OUT_RIGHT_MID, 64, 0)
        self.btn_info.set_style_radius(44, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btn_info.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.btn_info.set_style_bg_img_src(
            "A:/res/book.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.btn_info.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        lv.scr_load(self)

    def on_click(self, event_obj):
        target = event_obj.get_target()
        code = event_obj.code
        if code == lv.EVENT.CLICKED:
            if target == self.btn_settings:
                self.load_screen(SettingsScreen(self))
            elif target == self.btn_info:
                print("Info")
            else:
                print("Unknown")


class SettingsScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        kwargs = {"prev_scr": prev_scr, "title": "Settings", "nav_back": True}
        super().__init__(**kwargs)
        self.container = ContainerFlexCol(self, self.title)
        self.general = ListItemBtn(
            self.container, "General", left_img_src="A:/res/general.png"
        )
        self.connect = ListItemBtn(
            self.container, "Connect", left_img_src="A:/res/connect.png"
        )
        self.home_scr = ListItemBtn(
            self.container, "Home Screen", left_img_src="A:/res/homescreen.png"
        )
        self.security = ListItemBtn(
            self.container, "Security", left_img_src="A:/res/security.png"
        )
        self.crypto = ListItemBtn(
            self.container, "Crypto", left_img_src="A:/res/crypto.png"
        )
        self.about = ListItemBtn(
            self.container, "About Device", left_img_src="A:/res/about.png"
        )
        self.power = ListItemBtn(
            self.container,
            "PowerOff",
            left_img_src="A:/res/poweroff.png",
            has_next=False,
        )
        self.power.label_left.set_style_text_color(
            lv.color_hex(0xFF0000), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.general:
                self.load_screen(GeneralScreen(self))
            elif target == self.connect:
                self.load_screen(ConnectSetting(self))
            elif target == self.home_scr:
                self.load_screen(HomeScreenSetting(self))
            elif target == self.security:
                self.load_screen(SecurityScreen(self))
            elif target == self.crypto:
                print("Crypto")
            elif target == self.about:
                self.load_screen(AboutSetting(self))
            elif target == self.power:
                print("PowerOff")
            else:
                print("Unknown")


class GeneralScreen(Screen):
    cur_auto_lock = ""
    cur_language = ""

    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr=prev_scr, title="General", nav_back=True)

        auto_lock_time = device.get_autolock_delay_ms() / 1000 // 60
        if auto_lock_time > 60:
            right_text = f"{str(auto_lock_time // 60).split('.')[0]} hours"
        else:
            right_text = f"{str(auto_lock_time).split('.')[0]} minutes"
        GeneralScreen.cur_auto_lock = right_text
        self.container = ContainerFlexCol(self, self.title)
        self.auto_lock = ListItemBtn(self.container, "Auto Lock", right_text)
        GeneralScreen.cur_language = device.get_language()
        self.language = ListItemBtn(
            self.container, "Language", GeneralScreen.cur_language
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.auto_lock:
                self.load_screen(AutoLockSetting(self))
            elif target == self.language:
                self.load_screen(LanguageSetting(self))


class AutoLockSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr=prev_scr, title="Auto-Lock", nav_back=True)
        self.container = ContainerFlexCol(self, self.title)
        self.setting_items = [1, 2, 5, 10, 30, "Never"]
        has_custom = True
        self.checked_index = 0
        self.btns = [None] * (len(self.setting_items) + 1)
        for index, item in enumerate(self.setting_items):
            if not item == "Never":  # last item
                item = f"{item} minutes"
            self.btns[index] = ListItemBtn(self.container, item, has_next=False)
            self.btns[index].add_check_img()
            if item == GeneralScreen.cur_auto_lock:
                has_custom = False
                self.btns[index].set_checked()
                self.checked_index = index

        if has_custom:
            self.btns[-1] = ListItemBtn(
                self.container, f"{GeneralScreen.cur_auto_lock}(Custom)", has_next=False
            )
            self.btns[-1].add_check_img()
            self.btns[-1].set_checked()
            self.checked_index = -1
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target in self.btns:
                for index, item in enumerate(self.btns):
                    if item == target and self.checked_index != index:
                        item.set_checked()
                        self.btns[self.checked_index].set_uncheck()
                        self.checked_index = index
                        if index == 5:
                            device.set_autolock_delay_ms(device.AUTOLOCK_DELAY_MAXIMUM)
                            return
                        if index == 6:
                            auto_lock_time = int(GeneralScreen.cur_auto_lock)
                        else:
                            auto_lock_time = self.setting_items[index]
                        device.set_autolock_delay_ms(auto_lock_time * 60 * 1000)
                        return


class LanguageSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        self.check_index = 0
        super().__init__(prev_scr=prev_scr, title="Language", nav_back=True)
        self.container = ContainerFlexCol(self, self.title)
        self.lan_en = ListItemBtn(self.container, "English", has_next=False)
        self.lan_en.add_check_img()
        self.lan_ch = ListItemBtn(self.container, "Chinese", has_next=False)
        self.lan_ch.add_check_img()
        if GeneralScreen.cur_language == "English":
            self.lan_en.set_checked()
        else:
            self.lan_ch.set_checked()
            self.check_index = 1
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.lan_en and self.check_index:
                self.lan_en.set_checked()
                self.lan_ch.set_uncheck()
                device.set_language("English")
                self.check_index = 0
            elif target == self.lan_ch and not self.check_index:
                self.lan_ch.set_checked()
                self.lan_en.set_uncheck()
                device.set_language("Chinese")
                self.check_index = 1


class ConnectSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr=prev_scr, title="Connect", nav_back=True)
        self.container = ContainerFlexCol(self, self.title, size=(lv.pct(100), 160))
        self.ble = ListItemBtnWithSwitch(self.container, "Bluetooth")
        self.usb = ListItemBtnWithSwitch(self.container, "USB")
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.ble.switch:
                if target.has_state(lv.STATE.CHECKED):
                    print("Bluetooth is on")
                else:
                    print("Bluetooth is off")
            else:
                if target.has_state(lv.STATE.CHECKED):
                    print("USB is on")
                else:
                    print("USB is off")


class AboutSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        # model = device.get_model()
        # version = device.get_version()
        # serial = device.get_serial()
        # ble_mac = device.get_ble_mac()
        # storage = device.get_storage()
        super().__init__(prev_scr=prev_scr, title="About Device", nav_back=True)
        self.container = ContainerFlexCol(self, self.title, size=(lv.pct(100), 300))
        self.model = ListItemBtn(
            self.container, "Model", right_text="OneKey Touch", has_next=False
        )
        self.version = ListItemBtn(
            self.container, "System Version", right_text="1.0.0", has_next=False
        )
        self.serial = ListItemBtn(
            self.container, "Serial", right_text="FK1W2Y84JCDR", has_next=False
        )
        self.ble_mac = ListItemBtn(
            self.container, "Bluetooth", right_text="B8:C3:16:DE:D8:46", has_next=False
        )
        self.storage = ListItemBtn(
            self.container, "Storage", right_text="16 GB", has_next=False
        )


class HomeScreenSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr=prev_scr, title="Home Screen", nav_back=True)
        self.container = ContanierGrid(self, self.title, size=(lv.pct(100), 400))
        self.light = ImgBottonGridItem(
            self.container, 0, 0, "A:/res/wallpaper_light_thumnail.png"
        )
        self.light.set_checked(True)
        self.dark = ImgBottonGridItem(
            self.container, 1, 0, "A:/res/wallpaper_dark_thumnail.png"
        )
        self.dark.set_checked(False)
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.light:
                self.light.set_checked(True)
                self.dark.set_checked(False)
            elif target == self.dark:
                self.dark.set_checked(True)
                self.light.set_checked(False)


class SecurityScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title="Security", nav_back=True)
        self.container = ContainerFlexCol(self, self.title, size=(lv.pct(100), 400))
        self.rest_pin = ListItemBtn(self.container, "Reset PIN")
        self.recovery_check = ListItemBtn(self.container, "Check Recovery Phrase")
        self.passphrase = ListItemBtn(
            self.container, "Passphrase", right_text="Coming Soon", has_next=False
        )
        self.rest_device = ListItemBtn(self.container, "Reset Device", has_next=False)
        self.rest_device.label_left.set_style_text_color(
            lv.color_hex(0xFF0000), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.rest_pin:
                workflow.spawn(change_pin(DUMMY_CONTEXT, ChangePin(remove=False)))
            elif target == self.recovery_check:
                print("Check Recovery Phrase")
            elif target == self.passphrase:
                print("Passphrase")
            elif target == self.rest_device:
                print("Reset Device")
