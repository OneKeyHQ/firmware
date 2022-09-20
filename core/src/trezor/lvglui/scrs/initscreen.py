# import storage
# from trezor.crypto import bip39, hashlib, random
# from trezor.enums import BackupType

from trezor import utils
from trezor.langs import langs
from trezor.lvglui.i18n import gettext as _, i18n_refresh, keys as i18n_keys
from trezor.lvglui.scrs import font_LANG_MIX, font_PJSBOLD24, font_PJSBOLD36

from .common import FullSizeWindow, Screen, lv, lv_colors  # noqa: F401,F403,F405
from .components.container import ContainerFlexCol
from .components.label import Title
from .components.radio import ButtonCell

# from .components.keyboard import BIP39Keyboard
# from .components.style import SubTitleStyle
# from .pinscreen import PinTip

word_cnt_strength_map = {
    12: 128,
    18: 192,
    24: 256,
}

language = "en"


class InitScreen(Screen):
    def __init__(self):
        super().__init__(
            btn_text=_(i18n_keys.BUTTON__CONTINUE),
        )
        self.content_area = lv.obj(self)
        self.content_area.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.content_area.align(lv.ALIGN.TOP_LEFT, 0, 44)
        self.content_area.set_style_bg_color(
            lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.content_area.set_style_bg_color(
            lv_colors.WHITE_3, lv.PART.SCROLLBAR | lv.STATE.DEFAULT
        )
        self.content_area.set_style_pad_all(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.content_area.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.content_area.set_style_radius(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.content_area.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        self.content_area.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.content_area.set_style_max_height(646, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.content_area.set_style_min_height(400, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.icon = lv.img(self.content_area)
        self.icon.set_src("A:/res/language.png")
        self.icon.align(lv.ALIGN.TOP_MID, 0, 24)
        self.title = Title(
            self.content_area,
            None,
            452,
            (),
            _(i18n_keys.TITLE__SELECT_LANGUAGE),
            pos_y=48,
        )
        self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
        self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.container = ContainerFlexCol(self.content_area, self.title, padding_row=0)
        self.lang_buttons: list[ButtonCell] = []
        self.check_index = 0
        for _idx, lang in enumerate(langs):
            lang_button = ButtonCell(self.container, lang[1])
            lang_button.label.set_style_text_font(
                font_LANG_MIX, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            if _idx == 0:
                lang_button.set_checked()
            self.lang_buttons.append(lang_button)
        self.container.add_event_cb(self.on_selected_changed, lv.EVENT.CLICKED, None)

        self.btn.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btn.enable(lv_colors.ONEKEY_GREEN)

    def on_selected_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            last_checked = self.check_index
            for idx, button in enumerate(self.lang_buttons):
                if target != button and idx == last_checked:
                    button.set_uncheck()
                if target == button and idx != last_checked:
                    self.check_index = idx
                    button.set_checked()
                    global language
                    lang_key = langs[idx][0]
                    i18n_refresh(lang_key)
                    self.title.set_text(_(i18n_keys.TITLE__SELECT_LANGUAGE))
                    self.btn.label.set_text(_(i18n_keys.BUTTON__CONTINUE))
                    language = lang_key

    def on_click(self, event_obj):
        QuickStart()


class QuickStart(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__QUICK_START),
            _(i18n_keys.SUBTITLE__SETUP_QUICK_START),
            _(i18n_keys.BUTTON__START),
            options="\n".join(
                [
                    _(i18n_keys.OPTION__CREATE_NEW_WALLET),
                    _(i18n_keys.OPTION__RESTORE_WALLET),
                ]
            ),
        )

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                if self.selector.get_selected_index() == 0:
                    from trezor import workflow
                    from trezor.wire import DUMMY_CONTEXT
                    from apps.management.reset_device import reset_device
                    from trezor.messages import ResetDevice

                    # pyright: off
                    workflow.spawn(
                        reset_device(
                            DUMMY_CONTEXT,
                            ResetDevice(
                                strength=128,
                                language=language,
                                pin_protection=True,
                            ),
                        )
                    )
                elif self.selector.get_selected_index() == 1:
                    from apps.management.recovery_device import recovery_device
                    from trezor.messages import RecoveryDevice
                    from trezor import workflow
                    from trezor.wire import DUMMY_CONTEXT

                    workflow.spawn(
                        recovery_device(
                            DUMMY_CONTEXT,
                            RecoveryDevice(
                                enforce_wordlist=True,
                                language=language,
                                pin_protection=True,
                            ),
                        )
                    )  # pyright: on
                else:
                    return
            else:
                return
            self.destroy()


class SelectMnemonicNum(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__READY_TO_CREATE),
            _(i18n_keys.TITLE__SELECT_NUMBER_OF_WORDS),
            _(i18n_keys.BUTTON__CONTINUE),
            options="12\n18\n24",
        )
        self.num = 12

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                self.channel.publish(
                    word_cnt_strength_map[self.selector.get_selected_index()]
                )
                self.destroy()


# class InitScreen(Screen):
#     """Language and Init way select screens."""

#     def __init__(self) -> None:
#         if not hasattr(self, "_init"):
#             self._init = True
#         else:
#             return
#         kwargs = {
#             "title": "Select Language",
#             "title_pos": (lv.pct(0), lv.pct(-5)),
#             "title_align": lv.ALIGN.CENTER,
#         }
#         super().__init__(**kwargs)

#         # earth icon
#         self.icon = lv.img(self)
#         self.icon.set_src("A:/res/language.png")
#         self.icon.set_width(lv.SIZE.CONTENT)  # 100
#         self.icon.set_height(lv.SIZE.CONTENT)  # 100
#         self.icon.set_x(lv.pct(0))
#         self.icon.set_y(lv.pct(-30))
#         self.icon.set_align(lv.ALIGN.CENTER)
#         self.icon.set_pivot(0, 0)
#         self.icon.set_angle(0)
#         self.icon.set_zoom(255)
#         # the roller of select language
#         self.roller = lv.roller(self)
#         self.roller.set_options("English\nChinese", lv.roller.MODE.NORMAL)
#         self.roller.set_width(448)
#         self.roller.set_height(157)
#         self.roller.set_x(0)
#         self.roller.set_y(lv.pct(15))
#         self.roller.set_align(lv.ALIGN.CENTER)
#         self.roller.set_style_text_color(
#             lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_line_space(40, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_radius(24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_bg_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_border_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_border_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_radius(24, lv.PART.SELECTED | lv.STATE.DEFAULT)
#         self.roller.set_style_bg_color(
#             lv.color_hex(0x191919), lv.PART.SELECTED | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_bg_opa(255, lv.PART.SELECTED | lv.STATE.DEFAULT)
#         # add callback for roller
#         self.roller.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
#         # the btn next
#         self.btn = lv.btn(self)
#         self.btn.set_width(320)
#         self.btn.set_height(62)
#         self.btn.set_x(-6)
#         self.btn.set_y(300)
#         self.btn.set_align(lv.ALIGN.CENTER)
#         self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_bg_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         # add callback for btn next
#         self.btn.add_event_cb(
#             self.eventhandler, lv.EVENT.PRESSED | lv.EVENT.CLICKED, None
#         )
#         # the next btn label
#         self.btn_label = lv.label(self.btn)
#         self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#         self.btn_label.set_text("Next")
#         self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_align(lv.ALIGN.CENTER)
#         self.btn_label.set_style_text_font(
#             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         lv.scr_load(self)

#     def on_click(self, target):
#         self.load_screen(self.QuickStart(self))

#     def on_value_changed(self, target):
#         str = " " * 10
#         target.get_selected_str(str, len(str))

#     class QuickStart(Screen):
#         def __init__(self, prev_scr):
#             if not hasattr(self, "_init"):
#                 self._init = True
#             else:
#                 return
#             kwargs = {"title": "Quick Start"}
#             super().__init__(prev_scr, nav_back=True, **kwargs)
#             self.selected_index = 0
#             self.selected_str = ""

#             # the init type roller
#             self.roller = lv.roller(self)
#             self.roller.set_options(
#                 "Create New Wallet\nRestore Wallet", lv.roller.MODE.NORMAL
#             )
#             self.roller.set_width(448)
#             self.roller.set_height(157)
#             self.roller.set_x(0)
#             self.roller.set_y(lv.pct(15))
#             self.roller.set_align(lv.ALIGN.CENTER)
#             self.roller.set_style_text_color(
#                 lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.roller.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.roller.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.roller.set_style_text_line_space(40, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.roller.set_style_text_font(
#                 font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.roller.set_style_radius(24, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.roller.set_style_bg_color(
#                 lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.roller.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.roller.set_style_border_color(
#                 lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.roller.set_style_border_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.roller.set_style_radius(24, lv.PART.SELECTED | lv.STATE.DEFAULT)
#             self.roller.set_style_bg_color(
#                 lv.color_hex(0x191919), lv.PART.SELECTED | lv.STATE.DEFAULT
#             )
#             self.roller.set_style_bg_opa(255, lv.PART.SELECTED | lv.STATE.DEFAULT)
#             self.roller.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
#             # the btn start
#             self.btn = lv.btn(self)
#             self.btn.set_width(320)
#             self.btn.set_height(62)
#             self.btn.set_x(-6)
#             self.btn.set_y(300)
#             self.btn.set_align(lv.ALIGN.CENTER)
#             self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.btn.set_style_bg_color(
#                 lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.btn.set_style_text_color(
#                 lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.btn.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.btn.set_style_text_font(
#                 font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.btn.add_event_cb(
#                 self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.PRESSED, None
#             )
#             # the btn start label
#             self.btn_label = lv.label(self.btn)
#             self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#             self.btn_label.set_text("Start")
#             self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#             self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#             self.btn_label.set_x(0)
#             self.btn_label.set_y(0)
#             self.btn_label.set_align(lv.ALIGN.CENTER)
#             self.btn_label.set_style_text_font(
#                 font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             # the sub title
#             self.subtitle = lv.label(self)
#             self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#             self.subtitle.set_text(
#                 "Create a new wallet, or restore wallet used before from a backup."
#             )
#             self.subtitle.set_width(lv.pct(80))
#             self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#             self.subtitle.set_x(0)
#             self.subtitle.set_y(lv.pct(25))
#             self.subtitle.set_align(lv.ALIGN.TOP_MID)
#             self.subtitle.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
#             self.subtitle.set_scroll_dir(lv.DIR.ALL)
#             self.subtitle.set_style_text_color(
#                 lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.subtitle.set_style_text_align(
#                 lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.subtitle.set_style_text_font(
#                 font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#             )

#         def on_click(self, target):
#             if self.selected_index == 0:
#                 self.load_screen(CreateNew(self))
#             elif self.selected_index == 1:
#                 self.load_screen(Recovery(self))
#             else:
#                 pass

#         def on_value_changed(self, target):
#             self.selected_index = target.get_selected()
#             self.selected_str = " " * 20
#             target.get_selected_str(self.selected_str, len(self.selected_str))


# class CreateNew(Screen):
#     """Screens associated with creating a new wallet"""

#     def __init__(self, prev_scr):
#         if not hasattr(self, "_init"):
#             self._init = True
#         else:
#             return
#         kwargs = {"title": "Create a new wallet"}
#         super().__init__(prev_scr, nav_back=True, **kwargs)

#         # the roller
#         self.roller = lv.roller(self)
#         self.roller.set_options("12\n18\n24", lv.roller.MODE.NORMAL)
#         self.roller.set_width(448)
#         self.roller.set_height(157)
#         self.roller.set_x(0)
#         self.roller.set_y(lv.pct(15))
#         self.roller.set_align(lv.ALIGN.CENTER)
#         self.roller.set_style_text_color(
#             lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_line_space(40, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_radius(24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_bg_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_border_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_border_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_radius(24, lv.PART.SELECTED | lv.STATE.DEFAULT)
#         self.roller.set_style_bg_color(
#             lv.color_hex(0x191919), lv.PART.SELECTED | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_bg_opa(255, lv.PART.SELECTED | lv.STATE.DEFAULT)
#         self.roller.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
#         # the btn
#         self.btn = lv.btn(self)
#         self.btn.set_width(320)
#         self.btn.set_height(62)
#         self.btn.set_x(-6)
#         self.btn.set_y(300)
#         self.btn.set_align(lv.ALIGN.CENTER)
#         self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_bg_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
#         # the btn label
#         self.btn_label = lv.label(self.btn)
#         self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#         self.btn_label.set_text("Create")
#         self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_x(0)
#         self.btn_label.set_y(0)
#         self.btn_label.set_align(lv.ALIGN.CENTER)
#         self.btn_label.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
#         self.btn_label.set_scroll_dir(lv.DIR.ALL)
#         self.btn_label.set_style_text_font(
#             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         # the subtitle
#         self.subtitle = lv.label(self)
#         self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#         self.subtitle.set_text("Select the number of words.")
#         self.subtitle.set_width(lv.pct(80))
#         self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#         self.subtitle.set_x(0)
#         self.subtitle.set_y(lv.pct(25))
#         self.subtitle.set_align(lv.ALIGN.TOP_MID)
#         self.subtitle.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
#         self.subtitle.set_scroll_dir(lv.DIR.ALL)
#         self.subtitle.set_style_text_color(
#             lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.subtitle.set_style_text_align(
#             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_font(
#             font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#         )

#         self.word_cnt_list = [12, 18, 24]
#         global select_word_cnt
#         select_word_cnt = 12

#     def on_click(self, target):
#         self.load_screen(self.BackupTip(self))

#     def on_value_changed(self, target):
#         global select_word_cnt
#         selected_index = int(target.get_selected())
#         select_word_cnt = self.word_cnt_list[selected_index]

#     class BackupTip(Screen):
#         def __init__(self, prev_scr):
#             if not hasattr(self, "_init"):
#                 self._init = True
#             else:
#                 return
#             kwargs = {"title": "BackUp Recovery Phrase"}
#             super().__init__(prev_scr, nav_back=True, **kwargs)

#             self.btn = NormalButton(self, text="Continue")
#             self.btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
#             self.btn.disable()

#             self.subtitle = lv.label(self)
#             self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#             self.subtitle.set_text(
#                 "Next, OneKey will display a list of words, which is called the recovery phrase of wallet. So you need to know:"
#             )
#             self.subtitle.set_width(lv.pct(80))
#             self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#             self.subtitle.set_x(0)
#             self.subtitle.set_y(lv.pct(30))
#             self.subtitle.set_align(lv.ALIGN.TOP_MID)
#             self.subtitle.set_style_text_color(
#                 lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.subtitle.set_style_text_align(
#                 lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.subtitle.set_style_text_font(
#                 font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#             )

#             self.cb_bg1 = lv.obj(self)
#             self.cb_bg1.set_width(400)
#             self.cb_bg1.set_height(80)
#             self.cb_bg1.set_x(-10)
#             self.cb_bg1.set_y(0)
#             self.cb_bg1.set_align(lv.ALIGN.CENTER)
#             self.cb_bg1.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg1.set_style_bg_color(
#                 lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.cb_bg1.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg1.set_style_border_color(
#                 lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.cb_bg1.set_style_border_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg1.add_flag(lv.obj.FLAG.HIDDEN)

#             self.cb1 = lv.checkbox(self)
#             self.cb1.set_width(40)
#             self.cb1.set_height(40)
#             self.cb1.set_x(40)
#             self.cb1.set_y(380)
#             self.cb1.set_text("")
#             self.cb1.set_style_radius(8, lv.PART.INDICATOR | lv.STATE.DEFAULT)
#             self.cb1.set_style_border_color(
#                 lv.color_hex(0x1E1E1E), lv.PART.INDICATOR | lv.STATE.DEFAULT
#             )
#             self.cb1.set_style_border_opa(255, lv.PART.INDICATOR | lv.STATE.DEFAULT)
#             self.cb1.set_style_bg_color(
#                 lv.color_hex(0x1BAC44), lv.PART.INDICATOR | lv.STATE.CHECKED
#             )
#             self.cb1.set_style_bg_opa(255, lv.PART.INDICATOR | lv.STATE.CHECKED)
#             self.cb1.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)

#             self.tip1 = lv.label(self)
#             self.tip1.set_long_mode(lv.label.LONG.WRAP)
#             self.tip1.set_text(
#                 "If you lose recovery phrase, you will lose all your fund."
#             )
#             self.tip1.set_width(lv.pct(70))
#             self.tip1.set_height(lv.SIZE.CONTENT)  # 1
#             self.tip1.set_x(0)
#             self.tip1.set_y(380)
#             self.tip1.set_align(lv.ALIGN.TOP_MID)
#             self.tip1.set_style_text_color(
#                 lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.tip1.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.tip1.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.tip1.set_style_text_line_space(5, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.tip1.set_style_text_align(
#                 lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.tip1.set_style_text_font(
#                 font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#             )

#             self.cb_bg2 = lv.obj(self)
#             self.cb_bg2.set_width(400)
#             self.cb_bg2.set_height(80)
#             self.cb_bg2.set_x(-10)
#             self.cb_bg2.set_y(100)
#             self.cb_bg2.set_align(lv.ALIGN.CENTER)
#             self.cb_bg2.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg2.set_style_bg_color(
#                 lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.cb_bg2.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg2.set_style_border_color(
#                 lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.cb_bg2.set_style_border_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg2.add_flag(lv.obj.FLAG.HIDDEN)

#             self.cb2 = lv.checkbox(self)
#             self.cb2.set_width(40)
#             self.cb2.set_height(40)
#             self.cb2.set_x(40)
#             self.cb2.set_y(100)
#             self.cb2.set_align(lv.ALIGN.LEFT_MID)
#             self.cb2.set_text("")
#             self.cb2.set_style_radius(8, lv.PART.INDICATOR | lv.STATE.DEFAULT)
#             self.cb2.set_style_border_color(
#                 lv.color_hex(0x1E1E1E), lv.PART.INDICATOR | lv.STATE.DEFAULT
#             )
#             self.cb2.set_style_border_opa(255, lv.PART.INDICATOR | lv.STATE.DEFAULT)
#             self.cb2.set_style_bg_color(
#                 lv.color_hex(0x1BAC44), lv.PART.INDICATOR | lv.STATE.CHECKED
#             )
#             self.cb2.set_style_bg_opa(255, lv.PART.INDICATOR | lv.STATE.CHECKED)
#             self.cb2.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)

#             tip2 = lv.label(self)
#             tip2.set_long_mode(lv.label.LONG.WRAP)
#             tip2.set_text(
#                 "Never take photo or make digital copys, and never upload it online."
#             )
#             tip2.set_width(lv.pct(70))
#             tip2.set_height(lv.SIZE.CONTENT)  # 1
#             tip2.set_x(0)
#             tip2.set_y(480)
#             tip2.set_align(lv.ALIGN.TOP_MID)
#             tip2.set_style_text_color(
#                 lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             tip2.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             tip2.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#             tip2.set_style_text_line_space(5, lv.PART.MAIN | lv.STATE.DEFAULT)
#             tip2.set_style_text_align(
#                 lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             tip2.set_style_text_font(font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT)

#             self.cb_bg3 = lv.obj(self)
#             self.cb_bg3.set_width(400)
#             self.cb_bg3.set_height(80)
#             self.cb_bg3.set_x(-10)
#             self.cb_bg3.set_y(200)
#             self.cb_bg3.set_align(lv.ALIGN.CENTER)
#             self.cb_bg3.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg3.set_style_bg_color(
#                 lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.cb_bg3.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg3.set_style_border_color(
#                 lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.cb_bg3.set_style_border_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.cb_bg3.add_flag(lv.obj.FLAG.HIDDEN)

#             self.cb3 = lv.checkbox(self)
#             self.cb3.set_width(40)
#             self.cb3.set_height(40)
#             self.cb3.set_x(40)
#             self.cb3.set_y(200)
#             self.cb3.set_text("")
#             self.cb3.set_align(lv.ALIGN.LEFT_MID)
#             self.cb3.set_style_radius(8, lv.PART.INDICATOR | lv.STATE.DEFAULT)
#             self.cb3.set_style_border_color(
#                 lv.color_hex(0x1E1E1E), lv.PART.INDICATOR | lv.STATE.DEFAULT
#             )
#             self.cb3.set_style_border_opa(255, lv.PART.INDICATOR | lv.STATE.DEFAULT)
#             self.cb3.set_style_bg_color(
#                 lv.color_hex(0x1BAC44), lv.PART.INDICATOR | lv.STATE.CHECKED
#             )
#             self.cb3.set_style_bg_opa(255, lv.PART.INDICATOR | lv.STATE.CHECKED)
#             self.cb3.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)

#             tip3 = lv.label(self)
#             tip3.set_long_mode(lv.label.LONG.WRAP)
#             tip3.set_text("Keeping your backup secured and never send it to anyone.")
#             tip3.set_width(lv.pct(70))
#             tip3.set_height(lv.SIZE.CONTENT)  # 1
#             tip3.set_x(0)
#             tip3.set_y(580)
#             tip3.set_align(lv.ALIGN.TOP_MID)
#             tip3.set_style_text_color(
#                 lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             tip3.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             tip3.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#             tip3.set_style_text_line_space(5, lv.PART.MAIN | lv.STATE.DEFAULT)
#             tip3.set_style_text_align(
#                 lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             tip3.set_style_text_font(font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT)

#             self.cb_cnt = 0

#         def on_click(self, target):
#             self.load_screen(self.ManualBackup(self))

#         def on_value_changed(self, target):
#             if target == self.cb1:
#                 if target.get_state() & lv.STATE.CHECKED:
#                     self.cb_bg1.clear_flag(lv.obj.FLAG.HIDDEN)
#                     self.cb_cnt += 1
#                 else:
#                     self.cb_bg1.add_flag(lv.obj.FLAG.HIDDEN)
#                     self.cb_cnt -= 1
#             elif target == self.cb2:
#                 if target.get_state() & lv.STATE.CHECKED:
#                     self.cb_bg2.clear_flag(lv.obj.FLAG.HIDDEN)
#                     self.cb_cnt += 1
#                 else:
#                     self.cb_bg2.add_flag(lv.obj.FLAG.HIDDEN)
#                     self.cb_cnt -= 1
#             elif target == self.cb3:
#                 if target.get_state() & lv.STATE.CHECKED:
#                     self.cb_bg3.clear_flag(lv.obj.FLAG.HIDDEN)
#                     self.cb_cnt += 1
#                 else:
#                     self.cb_bg3.add_flag(lv.obj.FLAG.HIDDEN)
#                     self.cb_cnt -= 1
#             if self.cb_cnt == 3:
#                 self.btn.enable()
#             elif self.cb_cnt < 3:
#                 self.btn.disable()

#         class ManualBackup(Screen):
#             def __init__(self, prev_scr):
#                 if not hasattr(self, "_init"):
#                     self._init = True
#                 else:
#                     return
#                 kwargs = {"title": "Manual Backup"}
#                 super().__init__(prev_scr, nav_back=True, **kwargs)

#                 self.subtitle = lv.label(self)
#                 self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#                 global select_word_cnt
#                 self.subtitle.set_text(
#                     f"Write down the following {select_word_cnt} words in order."
#                 )
#                 self.subtitle.set_width(lv.pct(80))
#                 self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#                 self.subtitle.set_x(0)
#                 self.subtitle.set_y(lv.pct(22))
#                 self.subtitle.set_align(lv.ALIGN.TOP_MID)
#                 self.subtitle.set_style_text_color(
#                     lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#                 self.subtitle.set_style_text_align(
#                     lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.subtitle.set_style_text_font(
#                     font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )

#                 self.btn = NormalButton(self)
#                 self.btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
#                 self.words_panel = lv.obj(self)
#                 self.words_panel.set_width(lv.SIZE.CONTENT)
#                 self.words_panel.set_height(lv.SIZE.CONTENT)
#                 self.words_panel.align(lv.ALIGN.CENTER, 0, 0)
#                 self.words_panel.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
#                 self.words_panel.set_style_bg_color(
#                     lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.words_panel.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#                 self.words_panel.set_style_border_color(
#                     lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.words_panel.set_style_border_opa(
#                     255, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 # label display the first column
#                 self.word_col = lv.label(self.words_panel)
#                 self.word_col.set_pos(0, 0)
#                 self.word_col.set_width(180)
#                 self.word_col.set_height(lv.SIZE.CONTENT)
#                 self.word_col.set_style_text_font(
#                     font_MONO20, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.word_col.set_style_text_align(
#                     lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.word_col.set_style_pad_all(10, lv.PART.MAIN | lv.STATE.DEFAULT)
#                 self.word_col.set_style_text_line_space(
#                     10, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 # label display the second column
#                 self.word_col2 = lv.label(self.words_panel)
#                 self.word_col2.set_pos(210, 0)
#                 self.word_col2.set_width(180)
#                 self.word_col2.set_height(lv.SIZE.CONTENT)
#                 self.word_col2.set_style_text_font(
#                     font_MONO20, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.word_col2.set_style_text_align(
#                     lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )
#                 self.word_col2.set_style_pad_ver(10, lv.PART.MAIN | lv.STATE.DEFAULT)
#                 self.word_col2.set_style_pad_left(10, lv.PART.MAIN | lv.STATE.DEFAULT)
#                 self.word_col2.set_style_text_line_space(
#                     10, lv.PART.MAIN | lv.STATE.DEFAULT
#                 )

#                 global mnemonics
#                 mnemonics = init_device(word_cnt_strength_map[select_word_cnt])
#                 text_col = ""
#                 text_col2 = ""
#                 for index in range(0, select_word_cnt / 2):
#                     text_col += f"{index+1:>2}. {mnemonics[index]}\n"
#                     text_col2 += f"{int(index+select_word_cnt/2+1):>2}. {mnemonics[int(index+select_word_cnt/2)]}\n"
#                 self.word_col.set_text(text_col.rstrip())
#                 self.word_col2.set_text(text_col2.rstrip())

#             def on_click(self, target):
#                 self.load_screen(self.CheckRecoveryPhrase(self))

#             class CheckRecoveryPhrase(Screen):
#                 def __init__(self, prev_scr):
#                     if not hasattr(self, "_init"):
#                         self._init = True
#                     else:
#                         return
#                     kwargs = {"title": "Check Recovery Phrase", "subtitle": ""}
#                     super().__init__(prev_scr, nav_back=True, **kwargs)

#                     self.btn = NormalButton(self, text="Continue")
#                     self.btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

#                     self.subtitle = lv.label(self)
#                     self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#                     global select_word_cnt
#                     self.subtitle.set_text(
#                         f"Check {select_word_cnt} words again, make sure they are exactly the same as the backup you just wrote down."
#                     )
#                     self.subtitle.set_width(lv.pct(80))
#                     self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#                     self.subtitle.set_x(0)
#                     self.subtitle.set_y(lv.pct(30))
#                     self.subtitle.set_align(lv.ALIGN.TOP_MID)
#                     self.subtitle.set_style_text_color(
#                         lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#                     )
#                     self.subtitle.set_style_text_opa(
#                         255, lv.PART.MAIN | lv.STATE.DEFAULT
#                     )
#                     self.subtitle.set_style_text_align(
#                         lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#                     )
#                     self.subtitle.set_style_text_font(
#                         font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#                     )

#                 def on_click(self, target):
#                     self.load_screen(self.CheckWord(self))

#                 class CheckWord(Screen):
#                     def __init__(self, prev_scr):
#                         if not hasattr(self, "_init"):
#                             self._init = True
#                         else:
#                             return
#                         kwargs = {"title": "Check Word #1", "subtitle": ""}
#                         super().__init__(prev_scr, nav_back=True, **kwargs)
#                         # the roller
#                         self.roller = lv.roller(self)
#                         choices = get_choices()
#                         global check_selected
#                         check_selected = choices[1]
#                         self.roller.set_options(
#                             f"{choices[0]}\n{choices[1]}\n{choices[2]}",
#                             lv.roller.MODE.NORMAL,
#                         )
#                         self.roller.set_width(448)
#                         self.roller.set_height(157)
#                         self.roller.set_x(0)
#                         self.roller.set_y(lv.pct(15))
#                         self.roller.set_align(lv.ALIGN.CENTER)
#                         self.roller.set_style_text_color(
#                             lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_text_opa(
#                             255, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_text_letter_space(
#                             0, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_text_line_space(
#                             40, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_text_font(
#                             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_radius(
#                             24, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_bg_color(
#                             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#                         self.roller.set_style_border_color(
#                             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_border_opa(
#                             0, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_radius(
#                             24, lv.PART.SELECTED | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_style_bg_color(
#                             lv.color_hex(0x191919), lv.PART.SELECTED | lv.STATE.DEFAULT
#                         )
#                         self.roller.set_visible_row_count(3)
#                         self.roller.set_selected(1, lv.ANIM.OFF)
#                         self.roller.set_style_bg_opa(
#                             255, lv.PART.SELECTED | lv.STATE.DEFAULT
#                         )
#                         self.roller.add_event_cb(
#                             self.eventhandler, lv.EVENT.VALUE_CHANGED, None
#                         )
#                         # the btn
#                         self.btn = lv.btn(self)
#                         self.btn.set_width(320)
#                         self.btn.set_height(62)
#                         self.btn.set_x(-6)
#                         self.btn.set_y(300)
#                         self.btn.set_align(lv.ALIGN.CENTER)
#                         self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#                         self.btn.set_style_bg_color(
#                             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#                         self.btn.set_style_text_color(
#                             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.btn.set_style_text_opa(
#                             255, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.btn.set_style_text_font(
#                             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
#                         # the btn label
#                         self.btn_label = lv.label(self.btn)
#                         self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#                         self.btn_label.set_text("Next")
#                         self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#                         self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#                         self.btn_label.set_align(lv.ALIGN.CENTER)
#                         self.btn_label.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
#                         self.btn_label.set_scroll_dir(lv.DIR.ALL)
#                         self.btn_label.set_style_text_font(
#                             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         # the subtitle
#                         self.subtitle = lv.label(self)
#                         self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#                         self.subtitle.set_text("Choose the correct word.")
#                         self.subtitle.set_width(lv.pct(80))
#                         self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#                         self.subtitle.set_x(0)
#                         self.subtitle.set_y(lv.pct(25))
#                         self.subtitle.set_align(lv.ALIGN.TOP_MID)
#                         self.subtitle.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
#                         self.subtitle.set_scroll_dir(lv.DIR.ALL)
#                         self.subtitle.set_style_text_color(
#                             lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.subtitle.set_style_text_opa(
#                             255, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.subtitle.set_style_text_align(
#                             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )
#                         self.subtitle.set_style_text_font(
#                             font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#                         )

#                     def on_click(self, target):
#                         global check_index, check_selected
#                         if check_selected == mnemonics[check_index]:
#                             check_index += 1
#                             if check_index >= len(mnemonics):
#                                 save_mnemonics()
#                                 self.load_screen(
#                                     WalletReady(
#                                         "You have successfully created your wallet."
#                                     )
#                                 )
#                             else:
#                                 choices = get_choices()
#                                 check_selected = choices[1]
#                                 self.roller.set_options(
#                                     f"{choices[0]}\n{choices[1]}\n{choices[2]}",
#                                     lv.roller.MODE.NORMAL,
#                                 )
#                                 self.roller.set_selected(1, lv.ANIM.OFF)
#                                 self.title.set_text(f"Check Word #{check_index+1}")
#                                 self.subtitle.set_text(f"Choose the correct word.")
#                                 self.subtitle.set_style_text_color(
#                                     lv.color_hex(0xCCCCCC),
#                                     lv.PART.MAIN | lv.STATE.DEFAULT,
#                                 )
#                         else:
#                             self.subtitle.set_text("Incorrect word, try again.")
#                             self.subtitle.set_style_text_color(
#                                 lv_colors.RED, lv.PART.MAIN | lv.STATE.DEFAULT
#                             )

#                     def on_value_changed(self, target):
#                         global check_selected
#                         check_selected = " " * 11
#                         target.get_selected_str(check_selected, len(check_selected))
#                         check_selected = check_selected.strip()[:-1]


# class Recovery(Screen):
#     """Screens associated with importing a wallet"""

#     def __init__(self, prev_scr):
#         if not hasattr(self, "_init"):
#             self._init = True
#         else:
#             return
#         kwargs = {"title": "Import Wallet", "subtitle": ""}
#         super().__init__(prev_scr, nav_back=True, **kwargs)

#         # the roller
#         self.roller = lv.roller(self)
#         self.roller.set_options("12\n18\n24", lv.roller.MODE.NORMAL)
#         self.roller.set_width(448)
#         self.roller.set_height(157)
#         self.roller.set_x(0)
#         self.roller.set_y(lv.pct(15))
#         self.roller.set_align(lv.ALIGN.CENTER)
#         self.roller.set_style_text_color(
#             lv.color_hex(0x666666), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_line_space(40, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_radius(24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_bg_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_border_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_border_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.roller.set_style_radius(24, lv.PART.SELECTED | lv.STATE.DEFAULT)
#         self.roller.set_style_bg_color(
#             lv.color_hex(0x191919), lv.PART.SELECTED | lv.STATE.DEFAULT
#         )
#         self.roller.set_style_bg_opa(255, lv.PART.SELECTED | lv.STATE.DEFAULT)
#         self.roller.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
#         # the btn
#         self.btn = lv.btn(self)
#         self.btn.set_width(320)
#         self.btn.set_height(62)
#         self.btn.set_x(-6)
#         self.btn.set_y(300)
#         self.btn.set_align(lv.ALIGN.CENTER)
#         self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_bg_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.add_event_cb(
#             self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.PRESSED, None
#         )
#         # the btn label
#         self.btn_label = lv.label(self.btn)
#         self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#         self.btn_label.set_text("Continue")
#         self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_x(0)
#         self.btn_label.set_y(0)
#         self.btn_label.set_align(lv.ALIGN.CENTER)
#         self.btn_label.set_style_text_font(
#             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         # the sub title
#         self.subtitle = lv.label(self)
#         self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#         self.subtitle.set_text("Select the number of words.")
#         self.subtitle.set_width(lv.pct(80))
#         self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#         self.subtitle.set_x(0)
#         self.subtitle.set_y(lv.pct(25))
#         self.subtitle.set_align(lv.ALIGN.TOP_MID)
#         self.subtitle.set_style_text_color(
#             lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.subtitle.set_style_text_align(
#             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_font(
#             font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         global select_word_cnt
#         select_word_cnt = 12

#     def on_click(self, target):
#         self.load_screen(self.EnterWord(self))

#     def on_value_changed(self, target):
#         global select_word_cnt
#         # self.selected_index = target.get_selected()
#         selected_str = " " * 3
#         target.get_selected_str(selected_str, len(selected_str))
#         select_word_cnt = int(selected_str.strip()[:-1])

#     class EnterWord(Screen):
#         def __init__(self, prev_scr):
#             if not hasattr(self, "_init"):
#                 self._init = True
#             else:
#                 return
#             super().__init__(prev_scr, nav_back=True)

#             self.title = lv.label(self)
#             self.title.set_long_mode(lv.label.LONG.WRAP)
#             self.title.set_text(f"Enter Word #1")
#             self.title.set_width(lv.SIZE.CONTENT)  # 1
#             self.title.set_height(lv.SIZE.CONTENT)  # 1
#             self.title.set_x(lv.pct(0))
#             self.title.set_y(lv.pct(15))

#             self.title.set_align(lv.ALIGN.TOP_MID)

#             self.title.set_style_text_color(
#                 lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.title.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#             self.title.set_style_text_font(
#                 font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT
#             )
#             self.keyboard = BIP39Keyboard(self)
#             self.keyboard.add_event_cb(self.on_ready, lv.EVENT.READY, None)
#             self.num_th = 1

#         def on_ready(self, target):
#             global mnemonics, select_word_cnt
#             input = self.keyboard.ta.get_text()
#             if input == "" or self.num_th > select_word_cnt + 1:
#                 return
#             # if wordlist.index(select) != -1:
#             self.num_th += 1
#             mnemonics.append(input)
#             print(f"{input} ==== {self.num_th}== {select_word_cnt}")
#             if self.num_th == select_word_cnt + 1:
#                 global mnemonic_str
#                 mnemonic_str = " ".join(mnemonics)
#                 if bip39.check(mnemonic_str):
#                     save_mnemonics()
#                     self.load_screen(
#                         WalletReady("You have successfully imported your wallet."),
#                         destory_self=True,
#                     )
#                 else:
#                     print("mnemonic is not valid")
#                     mnemonics.clear()
#                     return
#             elif self.num_th > select_word_cnt + 1:
#                 return
#             else:
#                 self.title.set_text(f"Enter Word #{self.num_th}")
#                 self.keyboard.ta.set_text("")


# class WalletReady(Screen):
#     """Screen for the wallet ready"""

#     def __init__(self, subtitle=""):
#         if not hasattr(self, "_init"):
#             self._init = True
#         else:
#             return
#         super().__init__()
#         self.title = lv.label(self)
#         self.title.set_long_mode(lv.label.LONG.WRAP)
#         self.title.set_text("Wallet is Ready")
#         self.title.set_width(lv.pct(80))
#         self.title.set_height(lv.SIZE.CONTENT)  # 1
#         self.title.set_x(lv.pct(0))
#         self.title.set_y(lv.pct(35))
#         self.title.set_align(lv.ALIGN.TOP_MID)
#         self.title.set_style_text_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.title.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.title.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.title.set_style_text_line_space(10, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.title.set_style_text_align(
#             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)

#         self.subtitle = lv.label(self)
#         self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#         self.subtitle.set_text(subtitle)
#         self.subtitle.set_width(lv.pct(80))
#         self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#         self.subtitle.set_x(0)
#         self.subtitle.set_y(lv.pct(42))
#         self.subtitle.set_align(lv.ALIGN.TOP_MID)
#         self.subtitle.set_style_text_color(
#             lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.subtitle.set_style_text_align(
#             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_font(
#             font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#         )

#         self.btn = lv.btn(self)
#         self.btn.set_width(320)
#         self.btn.set_height(62)
#         self.btn.set_x(-6)
#         self.btn.set_y(300)
#         self.btn.set_align(lv.ALIGN.CENTER)
#         self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_bg_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.add_event_cb(
#             self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.PRESSED, None
#         )

#         self.btn_label = lv.label(self.btn)
#         self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#         self.btn_label.set_text("Next")
#         self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_align(lv.ALIGN.CENTER)
#         self.btn_label.set_style_text_font(
#             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#         )

#         self.icon = lv.img(self)
#         self.icon.set_src("A:/res/success_icon.png")
#         self.icon.set_width(lv.SIZE.CONTENT)  # 1
#         self.icon.set_height(lv.SIZE.CONTENT)  # 1
#         self.icon.set_align(lv.ALIGN.CENTER)
#         self.icon.set_x(lv.pct(0))
#         self.icon.set_y(lv.pct(-30))
#         self.icon.set_pivot(0, 0)
#         self.icon.set_angle(0)
#         self.icon.set_zoom(256)

#     def on_click(self, target):
#         self.load_screen(PinTip())


# class RecoveryFail(Screen):
#     def __init__(
#         self,
#         subtitle="The recovery phrase you entered is invalid. Check your backup carefully and try again.",
#     ):
#         if not hasattr(self, "_init"):
#             self._init = True
#         else:
#             return
#         super().__init__()
#         self.title = lv.label(self)
#         self.title.set_long_mode(lv.label.LONG.WRAP)
#         self.title.set_text("Invalid Recovery Phrase")
#         self.title.set_width(lv.pct(80))
#         self.title.set_height(lv.SIZE.CONTENT)  # 1
#         self.title.set_x(lv.pct(0))
#         self.title.set_y(lv.pct(35))
#         self.title.set_align(lv.ALIGN.TOP_MID)
#         self.title.set_style_text_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.title.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.title.set_style_text_letter_space(0, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.title.set_style_text_line_space(10, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.title.set_style_text_align(
#             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)

#         self.subtitle = lv.label(self)
#         self.subtitle.set_long_mode(lv.label.LONG.WRAP)
#         self.subtitle.set_text(subtitle)
#         self.subtitle.set_width(lv.pct(80))
#         self.subtitle.set_height(lv.SIZE.CONTENT)  # 1
#         self.subtitle.set_x(0)
#         self.subtitle.set_y(lv.pct(42))
#         self.subtitle.set_align(lv.ALIGN.TOP_MID)
#         self.subtitle.set_style_text_color(
#             lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.subtitle.set_style_text_align(
#             lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.subtitle.set_style_text_font(
#             font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT
#         )

#         self.btn = lv.btn(self)
#         self.btn.set_width(320)
#         self.btn.set_height(62)
#         self.btn.set_x(-6)
#         self.btn.set_y(300)
#         self.btn.set_align(lv.ALIGN.CENTER)
#         self.btn.set_style_radius(32, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_bg_color(
#             lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_color(
#             lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
#         )
#         self.btn.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
#         self.btn.add_event_cb(
#             self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.PRESSED, None
#         )

#         self.btn_label = lv.label(self.btn)
#         self.btn_label.set_long_mode(lv.label.LONG.WRAP)
#         self.btn_label.set_text("Try Again")
#         self.btn_label.set_width(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_height(lv.SIZE.CONTENT)  # 1
#         self.btn_label.set_align(lv.ALIGN.CENTER)
#         self.btn_label.set_style_text_font(
#             font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
#         )

#         self.icon = lv.img(self)
#         self.icon.set_src("A:/res/danger.png")
#         self.icon.set_width(lv.SIZE.CONTENT)  # 1
#         self.icon.set_height(lv.SIZE.CONTENT)  # 1
#         self.icon.set_align(lv.ALIGN.CENTER)
#         self.icon.set_x(lv.pct(0))
#         self.icon.set_y(lv.pct(-30))
#         self.icon.set_pivot(0, 0)
#         self.icon.set_angle(0)
#         self.icon.set_zoom(256)

#     def on_click(self, event_obj):
#         self.load_screen(Recovery(), destory_self=True)


# def init_device(strength=128) -> list[str]:
#     """
#     Initialize the device
#     :return: the mnemonic string
#     """
#     # wipe storage to make sure the device is in a clear state
#     storage.reset()
#     # generate internal entropy
#     int_entropy = random.bytes(32)
#     ext_entropy = random.bytes(32)

#     def _compute_secret_from_entropy(
#         int_entropy: bytes, ext_entropy: bytes, strength_in_bytes: int
#     ) -> bytes:
#         # combine internal and external entropy
#         ehash = hashlib.sha256()
#         ehash.update(int_entropy)
#         ehash.update(ext_entropy)
#         entropy = ehash.digest()
#         # take a required number of bytes
#         strength = strength_in_bytes // 8
#         secret = entropy[:strength]
#         return secret

#     # For SLIP-39 this is the Encrypted Master Secret
#     secret = _compute_secret_from_entropy(int_entropy, ext_entropy, strength)
#     # in BIP-39 we store mnemonic string instead of the secret
#     secret = bip39.from_data(secret).encode()
#     global mnemonic_str
#     mnemonic_str = secret
#     return secret.decode("utf-8").strip(" ").split(" ")


# def save_mnemonics():
#     # write settings and master secret into storage
#     storage.device.set_passphrase_enabled(False)
#     storage.device.store_mnemonic_secret(
#         mnemonic_str,  # for SLIP-39, this is the EMS
#         BackupType.Bip39,
#         needs_backup=False,
#         no_backup=False,
#     )


# def get_choices() -> list[str]:
#     """
#     Get the choices for the number of words
#     :param mnemonic_str: mnemonic string
#     :return: the choices
#     """
#     global mnemonics
#     dummy_mnemonics = list(set(mnemonics))
#     dummy_mnemonics.remove(mnemonics[check_index])
#     random.shuffle(dummy_mnemonics)
#     choices = [mnemonics[check_index]] + dummy_mnemonics[-2:]
#     random.shuffle(choices)
#     return choices
