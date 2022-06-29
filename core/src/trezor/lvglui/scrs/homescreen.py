from storage import device
from trezor import workflow
from trezor.langs import langs, langs_keys
from trezor.lvglui.i18n import gettext as _, i18n_refresh, keys as i18n_keys
from trezor.lvglui.lv_colors import lv_colors
from trezor.ui import display

from . import font_LANG_MIX, font_PJSBOLD24, font_PJSBOLD36
from .common import (  # noqa: F401, F403, F405
    FullSizeWindow,
    Screen,
    load_scr_with_animation,
    lv,
)
from .components.button import ListItemBtn, ListItemBtnWithSwitch
from .components.container import ContainerFlexCol, ContanierGrid
from .components.imgbtn import ImgBottonGridItem


def brightness2_percent_str(brightness: int) -> str:
    return f"{int(brightness / 255 * 100)}%"


class MainScreen(Screen):
    def __init__(self, dev_state=None):
        homescreen = device.get_homescreen() or "A:/res/wallpaper_light.png"
        if not hasattr(self, "_init"):
            self._init = True
        else:
            if dev_state:
                self.dev_state.clear_flag(lv.obj.FLAG.HIDDEN)
                self.dev_state_text.set_text(dev_state)
            else:
                from apps.base import get_state

                state = get_state()
                if state:
                    self.dev_state.clear_flag(lv.obj.FLAG.HIDDEN)
                    self.dev_state_text.set_text(state)
                else:
                    self.dev_state.add_flag(lv.obj.FLAG.HIDDEN)
            self.set_style_bg_img_src(homescreen, lv.PART.MAIN | lv.STATE.DEFAULT)
            if not self.is_visible():
                load_scr_with_animation(self)
            return
        super().__init__()
        self.dev_state = lv.btn(self)
        self.dev_state.set_size(lv.pct(96), lv.SIZE.CONTENT)
        self.dev_state.set_style_bg_color(
            lv.color_hex(0xDCA312), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.dev_state.set_style_radius(8, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.dev_state.align(lv.ALIGN.TOP_MID, 0, 52)
        self.dev_state_text = lv.label(self.dev_state)
        self.dev_state_text.set_style_text_color(
            lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.dev_state_text.set_style_text_font(
            font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.dev_state_text.center()
        if dev_state:
            self.dev_state_text.set_text(dev_state)
        else:
            self.dev_state.add_flag(lv.obj.FLAG.HIDDEN)
        self.set_style_bg_img_src(homescreen, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_img_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.btn_settings = lv.imgbtn(self)
        self.btn_settings.set_pos(64, 92)
        self.btn_settings.set_style_bg_img_src(
            "A:/res/settings.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.btn_settings.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.btn_info = lv.imgbtn(self)
        self.btn_info.align_to(self.btn_settings, lv.ALIGN.OUT_RIGHT_MID, 96, 0)
        self.btn_info.set_style_bg_img_src(
            "A:/res/user_guide.png", lv.PART.MAIN | lv.STATE.DEFAULT
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
            self.refresh_text()
            return
        kwargs = {
            "prev_scr": prev_scr,
            "title": _(i18n_keys.TITLE__SETTINGS),
            "nav_back": True,
        }
        super().__init__(**kwargs)
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.general = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__GENERAL),
            left_img_src="A:/res/general.png",
        )
        self.connect = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__CONNECT),
            left_img_src="A:/res/connect.png",
        )
        self.home_scr = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__HOME_SCREEN),
            left_img_src="A:/res/homescreen.png",
        )
        self.security = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__SECURITY),
            left_img_src="A:/res/security.png",
        )
        # self.crypto = ListItemBtn(
        #     self.container, _(i18n_keys.FORM__CRYPTO), left_img_src="A:/res/crypto.png"
        # )
        self.about = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__ABOUT_DEVICE),
            left_img_src="A:/res/about.png",
        )
        self.power = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__POWER_OFF),
            left_img_src="A:/res/poweroff.png",
            has_next=False,
        )
        self.power.label_left.set_style_text_color(
            lv_colors.ONEKEY_RED_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def refresh_text(self):
        self.title.set_text(_(i18n_keys.TITLE__SETTINGS))
        self.general.label_left.set_text(_(i18n_keys.ITEM__GENERAL))
        self.connect.label_left.set_text(_(i18n_keys.ITEM__CONNECT))
        self.home_scr.label_left.set_text(_(i18n_keys.ITEM__HOME_SCREEN))
        self.security.label_left.set_text(_(i18n_keys.ITEM__SECURITY))
        self.about.label_left.set_text(_(i18n_keys.ITEM__ABOUT_DEVICE))
        self.power.label_left.set_text(_(i18n_keys.ITEM__POWER_OFF))

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
            # elif target == self.crypto:
            #     self.load_screen(CryptoScreen(self))
            elif target == self.about:
                self.load_screen(AboutSetting(self))
            elif target == self.power:
                PowerOff()
            else:
                if __debug__:
                    print("Unknown")


class GeneralScreen(Screen):
    cur_auto_lock = ""
    cur_auto_lock_ms = 0
    cur_language = ""

    def __init__(self, prev_scr=None):
        GeneralScreen.cur_auto_lock_ms = device.get_autolock_delay_ms()
        GeneralScreen.cur_auto_lock = self.get_str_from_lock_ms(
            GeneralScreen.cur_auto_lock_ms
        )
        if not hasattr(self, "_init"):
            self._init = True
        else:
            if self.cur_auto_lock_ms:
                self.auto_lock.label_right.set_text(GeneralScreen.cur_auto_lock)
            if self.cur_language:
                self.language.label_right.set_text(self.cur_language)
            self.backlight.label_right.set_text(
                brightness2_percent_str(device.get_brightness())
            )
            self.refresh_text()
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__GENERAL), nav_back=True
        )
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.auto_lock = ListItemBtn(
            self.container, _(i18n_keys.ITEM__AUTO_LOCK), self.cur_auto_lock
        )
        GeneralScreen.cur_language = langs[langs_keys.index(device.get_language())][1]
        self.language = ListItemBtn(
            self.container, _(i18n_keys.ITEM__LANGUAGE), GeneralScreen.cur_language
        )
        self.backlight = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__BRIGHTNESS),
            brightness2_percent_str(device.get_brightness()),
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def refresh_text(self):
        self.title.set_text(_(i18n_keys.TITLE__GENERAL))
        self.auto_lock.label_left.set_text(_(i18n_keys.ITEM__AUTO_LOCK))
        self.language.label_left.set_text(_(i18n_keys.ITEM__LANGUAGE))
        self.backlight.label_left.set_text(_(i18n_keys.ITEM__BRIGHTNESS))

    def get_str_from_lock_ms(self, time_ms) -> str:
        if time_ms == device.AUTOLOCK_DELAY_MAXIMUM:
            return _(i18n_keys.ITEM__STATUS__NEVER)
        auto_lock_time = time_ms / 1000 // 60
        if auto_lock_time > 60:
            text = _(i18n_keys.OPTION__STR_HOUR).format(
                str(auto_lock_time // 60).split(".")[0]
            )
        else:
            text = _(i18n_keys.ITEM__STATUS__STR_MINUTES).format(
                str(auto_lock_time).split(".")[0]
            )
        return text

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.auto_lock:
                self.load_screen(AutoLockSetting(self))
            elif target == self.language:
                self.load_screen(LanguageSetting(self))
            elif target == self.backlight:
                self.load_screen(BacklightSetting(self))


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
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.setting_items = [1, 2, 5, 10, 30, "Never"]
        has_custom = True
        self.checked_index = 0
        self.btns = [None] * (len(self.setting_items) + 1)
        for index, item in enumerate(self.setting_items):
            if not item == "Never":  # last item
                item = _(i18n_keys.ITEM__STATUS__STR_MINUTES).format(item)
            else:
                item = _(i18n_keys.ITEM__STATUS__NEVER)
            self.btns[index] = ListItemBtn(self.container, item, has_next=False)
            self.btns[index].add_check_img()
            if item == GeneralScreen.cur_auto_lock:
                has_custom = False
                self.btns[index].set_checked()
                self.checked_index = index

        if has_custom:
            self.btns[-1] = ListItemBtn(
                self.container,
                f"{GeneralScreen.cur_auto_lock}({_(i18n_keys.OPTION__CUSTOM__INSERT)})",
                has_next=False,
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
                            auto_lock_time = device.AUTOLOCK_DELAY_MAXIMUM
                        elif index == 6:
                            auto_lock_time = int(GeneralScreen.cur_auto_lock_ms)
                        else:
                            auto_lock_time = self.setting_items[index] * 60 * 1000
                        device.set_autolock_delay_ms(auto_lock_time)
                        GeneralScreen.cur_auto_lock_ms = auto_lock_time
                        from apps.base import reload_settings_from_storage

                        reload_settings_from_storage()


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
        self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.lang_buttons = []
        for idx, lang in enumerate(langs):
            lang_button = ListItemBtn(self.container, lang[1], has_next=False)
            lang_button.label_left.set_style_text_font(
                font_LANG_MIX, lv.PART.MAIN | lv.STATE.DEFAULT
            )
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
                    device.set_language(langs_keys[idx])
                    GeneralScreen.cur_language = langs[idx][1]
                    i18n_refresh()
                    self.title.set_text(_(i18n_keys.TITLE__LANGUAGE))
                    self.check_index = idx
                    button.set_checked()


class BacklightSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__BRIGHTNESS), nav_back=True
        )
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.item1 = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__BRIGHTNESS),
            brightness2_percent_str(device.get_brightness()),
            has_next=False,
        )
        self.slider = lv.slider(self)
        self.slider.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.slider.set_size(424, 8)
        self.slider.set_range(20, 255)
        self.slider.set_value(display.backlight(), lv.ANIM.OFF)
        self.slider.align_to(self.container, lv.ALIGN.BOTTOM_MID, 0, 33)
        self.slider.set_style_bg_color(
            lv_colors.GRAY_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.slider.set_style_bg_color(lv_colors.WHITE, lv.PART.KNOB | lv.STATE.DEFAULT)
        self.slider.set_style_bg_color(
            lv_colors.WHITE, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.slider.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        target = event_obj.get_target()
        if target == self.slider:
            value = target.get_value()
            display.backlight(value)
            self.item1.label_right.set_text(brightness2_percent_str(value))
            device.set_brightness(value)


class ConnectSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__CONNECT), nav_back=True
        )
        self.container = ContainerFlexCol(self, self.title, padding_row=20)
        self.ble = ListItemBtnWithSwitch(self.container, _(i18n_keys.ITEM__BLUETOOTH))
        if device.ble_enabled():
            self.ble.add_state()
        else:
            self.ble.clear_state()
        # self.usb = ListItemBtnWithSwitch(self.container, _(i18n_keys.ITEM__USB))
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            from trezor import uart

            if target == self.ble.switch:
                if target.has_state(lv.STATE.CHECKED):
                    uart.ctrl_ble(enable=True)
                else:
                    uart.ctrl_ble(enable=False)
            # else:
            #     if target.has_state(lv.STATE.CHECKED):
            #         print("USB is on")
            #     else:
            #         print("USB is off")


class AboutSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        model = device.get_model()
        version = device.get_firmware_version()
        serial = device.get_serial()
        from trezor import uart

        ble_name = uart.get_ble_name()
        storage = device.get_storage()
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__ABOUT_DEVICE), nav_back=True
        )
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.model = ListItemBtn(
            self.container, _(i18n_keys.ITEM__MODEL), right_text=model, has_next=False
        )
        self.version = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__SYSTEM_VERSION),
            right_text=version,
            has_next=False,
        )
        self.serial = ListItemBtn(
            self.container, _(i18n_keys.ITEM__SERIAL), right_text=serial, has_next=False
        )
        self.ble_mac = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__BLUETOOTH),
            right_text=ble_name,
            has_next=False,
        )
        self.storage = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__STORAGE),
            right_text=storage,
            has_next=False,
        )


class PowerOff(FullSizeWindow):
    def __init__(self, clear_loop:bool=False):
        super().__init__(
            title=_(i18n_keys.TITLE__POWER_OFF),
            subtitle=None,
            confirm_text=_(i18n_keys.ITEM__POWER_OFF),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            top_layer=True,
        )
        self.clear_loop = clear_loop
        self.btn_yes.enable(bg_color=lv_colors.ONEKEY_RED_1)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        from trezor import config

        self.has_pin = config.has_pin()
        if self.has_pin:
            config.lock()

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                self.destroy()
                ShutingDown()
            elif target == self.btn_no:
                self.destroy(500)
                if self.has_pin:
                    from apps.common.request_pin import verify_user_pin

                    workflow.spawn(verify_user_pin(clear_loop=self.clear_loop))


class ShutingDown(FullSizeWindow):
    def __init__(self):
        super().__init__(
            title=_(i18n_keys.TITLE__SHUTTING_DOWN), subtitle=None, top_layer=True
        )
        from trezor import loop, uart

        async def shutdown_delay():
            await loop.sleep(3000)
            uart.ctrl_power_off()

        self.destroy(3000)
        workflow.spawn(shutdown_delay())


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
            self.clean()
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__SECURITY), nav_back=True)
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.change_pin = ListItemBtn(self.container, _(i18n_keys.ITEM__CHANGE_PIN))
        self.recovery_check = ListItemBtn(
            self.container, _(i18n_keys.ITEM__CHECK_RECOVERY_PHRASE)
        )
        # self.passphrase = ListItemBtn(self.container, _(i18n_keys.ITEM__PASSPHRASE))
        self.rest_device = ListItemBtn(
            self.container, _(i18n_keys.ITEM__RESET_DEVICE), has_next=False
        )
        self.rest_device.label_left.set_style_text_color(
            lv_colors.ONEKEY_RED_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        # pyright: off
        if code == lv.EVENT.CLICKED:
            from trezor.wire import DUMMY_CONTEXT

            if target == self.change_pin:
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
            # elif target == self.passphrase:
            #     from apps.management.apply_settings import apply_settings
            #     from trezor.messages import ApplySettings

            #     passphrase_enable = not device.is_passphrase_enabled()
            #     if passphrase_enable:
            #         on_device = True
            #     else:
            #         on_device = None
            #     workflow.spawn(
            #         apply_settings(
            #             DUMMY_CONTEXT,
            #             ApplySettings(
            #                 use_passphrase=passphrase_enable,
            #                 passphrase_always_on_device=on_device,
            #             ),
            #         )
            #     )
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
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
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
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.blind_sign = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__BLIND_SIGNING),
            right_text=_(i18n_keys.ITEM__STATUS__OFF),
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
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.blind_sign = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__BLIND_SIGNING),
            right_text=_(i18n_keys.ITEM__STATUS__OFF),
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
        self.container = ContainerFlexCol(self, self.title, padding_row=10)
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
                        btn_text=_(i18n_keys.BUTTON__ENABLE),
                    )
                else:
                    pass
