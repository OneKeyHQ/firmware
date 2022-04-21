from storage import device
from trezor import workflow
from trezor.langs import langs
from trezor.lvglui.i18n import gettext as _, i18n_refresh, keys as i18n_keys

from . import font_PJSBOLD24
from .common import Screen, load_scr_with_animation, lv  # noqa: F401, F403, F405
from .components.button import ListItemBtn, ListItemBtnWithSwitch
from .components.container import ContainerFlexCol, ContanierGrid
from .components.imgbtn import ImgBottonGridItem


class MainScreen(Screen):
    def __init__(self, dev_state=None):
        homescreen = device.get_homescreen() or "A:/res/wallpaper_light.png"
        if not hasattr(self, "_init"):
            self._init = True
        else:
            if dev_state:
                self.dev_state_text.set_text(dev_state)
            elif hasattr(self, "dev_state") and dev_state is None:
                self.dev_state.delete()
            self.set_style_bg_img_src(homescreen, lv.PART.MAIN | lv.STATE.DEFAULT)
            if not self.is_visible():
                load_scr_with_animation(self)
            return
        super().__init__()
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
                lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.dev_state_text.set_style_text_font(
                font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.dev_state_text.center()
        self.set_style_bg_img_src(homescreen, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_img_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.btn_settings = lv.btn(self)
        self.btn_settings.set_size(144, 88)
        self.btn_settings.set_pos(64, 112)
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

    def on_click(self, event_obj):
        target = event_obj.get_target()
        code = event_obj.code
        if code == lv.EVENT.CLICKED:
            if target == self.btn_settings:
                self.load_screen(SettingsScreen(self))
            elif target == self.btn_info:
                if __debug__:
                    print("Info")
            else:
                if __debug__:
                    print("Unknown")


class SettingsScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        kwargs = {
            "prev_scr": prev_scr,
            "title": _(i18n_keys.TITLE__SETTING),
            "nav_back": True,
        }
        super().__init__(**kwargs)
        self.container = ContainerFlexCol(self, self.title)
        self.general = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__GENERAL),
            left_img_src="A:/res/general.png",
        )
        self.connect = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__CONNECT),
            left_img_src="A:/res/connect.png",
        )
        self.home_scr = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__HOME_SCREEN),
            left_img_src="A:/res/homescreen.png",
        )
        self.security = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__SECURITY),
            left_img_src="A:/res/security.png",
        )
        self.crypto = ListItemBtn(
            self.container, _(i18n_keys.FORM__CRYPTO), left_img_src="A:/res/crypto.png"
        )
        self.about = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__ABOUT_DEVICE),
            left_img_src="A:/res/about.png",
        )
        self.power = ListItemBtn(
            self.container,
            _(i18n_keys.ACTION__POWER_OFF),
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
                self.load_screen(CryptoScreen(self))
            elif target == self.about:
                self.load_screen(AboutSetting(self))
            elif target == self.power:
                pass
            else:
                if __debug__:
                    print("Unknown")


class GeneralScreen(Screen):
    cur_auto_lock = ""
    cur_language = ""

    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__GENERAL), nav_back=True
        )

        auto_lock_time = device.get_autolock_delay_ms() / 1000 // 60
        if auto_lock_time > 60:
            # TODO: i18n missing
            right_text = f"{str(auto_lock_time // 60).split('.')[0]} hours"
        else:
            right_text = _(i18n_keys.FORM__STATUS__STR_MINUTES).format(
                int(auto_lock_time)
            )
        GeneralScreen.cur_auto_lock = right_text
        self.container = ContainerFlexCol(self, self.title)
        self.auto_lock = ListItemBtn(
            self.container, _(i18n_keys.FORM__AUTO_LOCK), right_text
        )
        GeneralScreen.cur_language = langs[device.get_language()][1]
        self.language = ListItemBtn(
            self.container, _(i18n_keys.FORM__LANGUAGE), GeneralScreen.cur_language
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


# pyright: off
class AutoLockSetting(Screen):
    # TODO: i18n
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__AUTO_LOCK), nav_back=True
        )
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
            self.btns[-1].add_check_img()  # type: ignore[Cannot access member "add_check_img" for type "None"]
            self.btns[-1].set_checked()  # type: ignore[Cannot access member "set_checked" for type "None"]
            self.checked_index = -1
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target in self.btns:
                for index, item in enumerate(self.btns):
                    if item == target and self.checked_index != index:
                        item.set_checked()  # type: ignore[Cannot access member "set_checked" for type "None"]
                        self.btns[self.checked_index].set_uncheck()  # type: ignore[Cannot access member "set_uncheck" for type "None"]
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


# pyright: on
class LanguageSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__LANGUAGE), nav_back=True
        )
        self.check_index = 0
        self.container = ContainerFlexCol(self, self.title)
        self.lang_buttons = []
        for idx, lang in enumerate(langs):
            lang_button = ListItemBtn(self.container, lang[1], has_next=False)
            lang_button.add_check_img()
            self.lang_buttons.append(lang_button)
            if GeneralScreen.cur_language == lang[1]:
                lang_button.set_checked()
                self.check_index = idx
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            last_checked = self.check_index
            for idx, button in enumerate(self.lang_buttons):
                if target != button and idx == last_checked:
                    button.set_uncheck()
                if target == button and idx != last_checked:
                    device.set_language(idx)
                    i18n_refresh()
                    self.check_index = idx
                    button.set_checked()


class ConnectSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__CONNECT), nav_back=True
        )
        self.container = ContainerFlexCol(self, self.title)
        self.ble = ListItemBtnWithSwitch(self.container, _(i18n_keys.FORM__BLUETOOTH))
        self.usb = ListItemBtnWithSwitch(self.container, _(i18n_keys.FORM__USB))
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if __debug__:
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
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.FORM__ABOUT_DEVICE), nav_back=True
        )
        self.container = ContainerFlexCol(self, self.title)
        # TODO: i18n missing
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
            self.container,
            _(i18n_keys.FORM__BLUETOOTH),
            right_text="B8:C3:16:DE:D8:46",
            has_next=False,
        )
        self.storage = ListItemBtn(
            self.container, "Storage", right_text="16 GB", has_next=False
        )


class HomeScreenSetting(Screen):
    LIGHT_WALLPAPER = "A:/res/wallpaper_light.png"
    DARK_WALLPAPER = "A:/res/wallpaper_dark.png"

    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        cur = device.get_homescreen() or self.LIGHT_WALLPAPER
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__HOME_SCREEN), nav_back=True
        )
        self.container = ContanierGrid(self, self.title)
        self.light = ImgBottonGridItem(
            self.container, 0, 0, "A:/res/wallpaper_light_thumnail.png"
        )
        self.dark = ImgBottonGridItem(
            self.container, 1, 0, "A:/res/wallpaper_dark_thumnail.png"
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

        if cur == self.DARK_WALLPAPER:
            self.dark.set_checked(True)
            self.light.set_checked(False)
        else:
            self.light.set_checked(True)
            self.dark.set_checked(False)
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.light:
                self.light.set_checked(True)
                self.dark.set_checked(False)
                device.set_homescreen(self.LIGHT_WALLPAPER)
            elif target == self.dark:
                self.dark.set_checked(True)
                self.light.set_checked(False)
                device.set_homescreen(self.DARK_WALLPAPER)


class SecurityScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__SECURITY), nav_back=True)
        self.container = ContainerFlexCol(self, self.title)
        self.rest_pin = ListItemBtn(self.container, _(i18n_keys.FORM__RESET_PIN))
        self.recovery_check = ListItemBtn(
            self.container, _(i18n_keys.FORM__CHECK_RECOVERY_PHRASE)
        )
        self.passphrase = ListItemBtn(self.container, _(i18n_keys.FORM__PASSPHRASE))
        self.rest_device = ListItemBtn(
            self.container, _(i18n_keys.ACTION__RESET_DEVICE), has_next=False
        )
        self.rest_device.label_left.set_style_text_color(
            lv.color_hex(0xFF0000), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        # pyright: off
        if code == lv.EVENT.CLICKED:
            from trezor.wire import DUMMY_CONTEXT

            if target == self.rest_pin:
                from apps.management.change_pin import change_pin
                from trezor.messages import ChangePin

                workflow.spawn(change_pin(DUMMY_CONTEXT, ChangePin(remove=False)))
            elif target == self.recovery_check:
                from apps.management.recovery_device import recovery_device
                from trezor.messages import RecoveryDevice

                workflow.spawn(
                    recovery_device(
                        DUMMY_CONTEXT,
                        RecoveryDevice(dry_run=True, enforce_wordlist=True),
                    )
                )
            elif target == self.passphrase:
                from apps.management.apply_settings import apply_settings
                from trezor.messages import ApplySettings

                passphrase_enable = not device.is_passphrase_enabled()
                if passphrase_enable:
                    on_device = True
                else:
                    on_device = None
                workflow.spawn(
                    apply_settings(
                        DUMMY_CONTEXT,
                        ApplySettings(
                            use_passphrase=passphrase_enable,
                            passphrase_always_on_device=on_device,
                        ),
                    )
                )
            elif target == self.rest_device:
                from apps.management.wipe_device import wipe_device
                from trezor.messages import WipeDevice

                workflow.spawn(wipe_device(DUMMY_CONTEXT, WipeDevice()))
        # pyright: on


class CryptoScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__CRYPTO), nav_back=True)
        self.container = ContainerFlexCol(self, self.title)
        self.ethereum = ListItemBtn(self.container, _(i18n_keys.TITLE__ETHEREUM))
        self.solana = ListItemBtn(self.container, _(i18n_keys.TITLE__SOLANA))
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.ethereum:
                self.load_screen(EthereumSetting(self))
            elif target == self.solana:
                self.load_screen(SolanaSetting(self))


class EthereumSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__ETHEREUM), nav_back=True)
        self.container = ContainerFlexCol(self, self.title)
        self.blind_sign = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__BLIND_SIGNING),
            right_text=_(i18n_keys.FORM__STATUS__OFF),
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.blind_sign:
                self.load_screen(
                    BlindSign(self, coin_type=_(i18n_keys.TITLE__ETHEREUM))
                )


class SolanaSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__SOLANA), nav_back=True)
        self.container = ContainerFlexCol(self, self.title)
        self.blind_sign = ListItemBtn(
            self.container,
            _(i18n_keys.FORM__BLIND_SIGNING),
            right_text=_(i18n_keys.FORM__STATUS__OFF),
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.blind_sign:
                self.load_screen(BlindSign(self, coin_type=_(i18n_keys.TITLE__SOLANA)))


class BlindSign(Screen):
    def __init__(self, prev_scr=None, coin_type: str = _(i18n_keys.TITLE__ETHEREUM)):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            self.coin_type = coin_type
            return
        super().__init__(
            prev_scr, title=_(i18n_keys.TITLE__BLIND_SIGNING), nav_back=True
        )
        self.coin_type = coin_type
        self.container = ContainerFlexCol(self, self.title)
        self.blind_sign = ListItemBtnWithSwitch(
            self.container, f"{coin_type} Blind Signing"
        )
        self.blind_sign.clear_state()
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)
        self.popup = None

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.blind_sign.switch:
                if target.has_state(lv.STATE.CHECKED):
                    from .components.popup import Popup

                    self.popup = Popup(
                        self,
                        _(i18n_keys.TITLE__ENABLE_STR_BLIND_SIGNING).format(
                            self.coin_type
                        ),
                        _(i18n_keys.SUBTITLE_SETTING_CRYPTO_BLIND_SIGN_ENABLED),
                        icon_path="A:/res/warning.png",
                        btn_text=_(i18n_keys.ACTION__ENABLE),
                    )
                else:
                    if __debug__:
                        print("Bluetooth is off")
