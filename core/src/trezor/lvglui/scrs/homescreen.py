import math
from micropython import const

from storage import device
from trezor import io, loop, uart, utils, workflow
from trezor.enums import SafetyCheckLevel
from trezor.langs import langs, langs_keys
from trezor.lvglui.i18n import gettext as _, i18n_refresh, keys as i18n_keys
from trezor.lvglui.lv_colors import lv_colors
from trezor.ui import display, style

import ujson as json
from apps.common import safety_checks

from . import font_PJSBOLD30, font_PJSREG24, font_PJSREG30
from .common import FullSizeWindow, Screen, lv  # noqa: F401, F403, F405
from .components.anim import Anim
from .components.button import ListItemBtn, ListItemBtnWithSwitch, NormalButton
from .components.container import ContainerFlexCol, ContainerGrid
from .components.listitem import DisplayItem, ImgGridItem
from .components.navigation import Navigation
from .widgets.style import StyleWrapper


def brightness2_percent_str(brightness: int) -> str:
    return f"{int(brightness / style.BACKLIGHT_MAX * 100)}%"


GRID_CELL_SIZE_ROWS = const(240)
GRID_CELL_SIZE_COLS = const(144)


def change_state(is_busy: bool = False):
    if hasattr(MainScreen, "_instance"):
        if MainScreen._instance:
            MainScreen._instance.change_state(is_busy)


class MainScreen(Screen):
    def __init__(self, device_name=None, ble_name=None, dev_state=None):
        homescreen = device.get_homescreen()
        if not hasattr(self, "_init"):
            self._init = True
            super().__init__(
                title=device_name, subtitle=ble_name or uart.get_ble_name()
            )
        else:
            if hasattr(self, "dev_state"):
                from apps.base import get_state

                state = get_state()
                if state:
                    self.dev_state.show(state)
                    self.title.align_to(self.dev_state, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)
                    self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
                else:
                    self.dev_state.delete()
                    del self.dev_state
                    self.title.align(lv.ALIGN.TOP_LEFT, 8, 52)
                    self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
            self.set_style_bg_img_src(homescreen, 0)
            if self.bottom_tips:
                self.bottom_tips.set_text(_(i18n_keys.BUTTON__SWIPE_TO_SHOW_APPS))
            if self.apps:
                self.apps.tips.set_text(_(i18n_keys.CONTENT__SWIPE_DOWN_TO_CLOSE))
            return
        self.subtitle.set_style_text_color(lv_colors.WHITE, 0)
        if dev_state:
            self.dev_state = MainScreen.DevStateTipsBar(self)
            self.dev_state.show(dev_state)
            self.title.align_to(self.dev_state, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)
            self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.add_style(
            StyleWrapper().bg_img_src(homescreen).bg_img_opa(int(lv.OPA.COVER * 0.92)),
            0,
        )
        # self.add_style(StyleWrapper()
        #     .transform_zoom(512)
        #     , lv.PART.MAIN | lv.STATE.USER_1
        #     )
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.bottom_bar = lv.btn(self)
        self.bottom_bar.remove_style_all()
        self.bottom_bar.set_size(lv.pct(100), 100)
        self.bottom_bar.set_align(lv.ALIGN.BOTTOM_MID)
        # self.up_arrow = lv.img(self.bottom_bar)
        # self.up_arrow.set_src("A:/res/up-home.png")
        # self.up_arrow.set_align(lv.ALIGN.TOP_MID)
        self.bottom_tips = lv.label(self.bottom_bar)
        self.bottom_tips.align(lv.ALIGN.BOTTOM_MID, 0, -16)
        self.bottom_tips.set_width(464)
        self.bottom_tips.set_long_mode(lv.label.LONG.WRAP)
        self.bottom_tips.set_text(_(i18n_keys.BUTTON__SWIPE_TO_SHOW_APPS))
        self.bottom_tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .text_color(lv_colors.WHITE)
            .text_align_center(),
            0,
        )
        self.apps = self.AppDrawer(self)
        self.add_event_cb(self.on_slide_up, lv.EVENT.GESTURE, None)

    def hidden_titles(self, hidden: bool = True):
        if hidden:
            self.subtitle.add_flag(lv.obj.FLAG.HIDDEN)
            self.title.add_flag(lv.obj.FLAG.HIDDEN)

        else:
            self.subtitle.clear_flag(lv.obj.FLAG.HIDDEN)
            self.title.clear_flag(lv.obj.FLAG.HIDDEN)

    def change_state(self, busy: bool):
        if busy:
            self.clear_flag(lv.obj.FLAG.CLICKABLE)
            self.bottom_bar.clear_flag(lv.obj.FLAG.CLICKABLE)
            # self.up_arrow.add_flag(lv.obj.FLAG.HIDDEN)
            self.bottom_tips.set_text(_(i18n_keys.BUTTON__PROCESSING))
        else:
            self.add_flag(lv.obj.FLAG.CLICKABLE)
            self.bottom_bar.add_flag(lv.obj.FLAG.CLICKABLE)
            # self.up_arrow.clear_flag(lv.obj.FLAG.HIDDEN)
            self.bottom_tips.set_text(_(i18n_keys.BUTTON__SWIPE_TO_SHOW_APPS))

    def on_slide_up(self, event_obj):
        code = event_obj.code
        if code == lv.EVENT.GESTURE:
            _dir = lv.indev_get_act().get_gesture_dir()
            if _dir == lv.DIR.TOP:
                # child_cnt == 5 in common if in homepage
                if self.get_child_cnt() > 5:
                    return
                if self.is_visible():
                    # self.hidden_titles()
                    # if hasattr(self, "dev_state"):
                    #     self.dev_state.hidden()
                    self.apps.show()
            elif _dir == lv.DIR.BOTTOM:
                lv.event_send(self.apps, lv.EVENT.GESTURE, None)

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)

    class DevStateTipsBar(lv.obj):
        def __init__(self, parent) -> None:
            super().__init__(parent)
            self.remove_style_all()
            self.set_size(lv.pct(100), 66)
            self.add_style(
                StyleWrapper()
                .bg_color(lv.color_hex(0x6B5C00))
                .bg_opa(lv.OPA.COVER)
                .border_width(0)
                .pad_ver(16)
                .pad_hor(8)
                .radius(0)
                .text_font(font_PJSBOLD30)
                .text_align_left(),
                0,
            )
            self.align(lv.ALIGN.TOP_MID, 0, 44)

            self.icon = lv.img(self)
            self.icon.set_align(lv.ALIGN.LEFT_MID)
            self.icon.set_src("A:/res/icon-warning_bar.png")
            self.warnings = lv.label(self)
            self.warnings.align_to(self.icon, lv.ALIGN.OUT_RIGHT_MID, 8, 0)

        def show(self, text=None):
            self.clear_flag(lv.obj.FLAG.HIDDEN)
            if text:
                self.warnings.set_text(text)

        def hidden(self):
            self.add_flag(lv.obj.FLAG.HIDDEN)

    class AppDrawer(lv.obj):
        def __init__(self, parent) -> None:
            super().__init__(parent)
            self.parent = parent
            self.remove_style_all()
            self.set_pos(0, 800)
            self.set_size(lv.pct(100), lv.pct(100))
            self.add_style(
                StyleWrapper()
                .bg_color(lv_colors.BLACK)
                .bg_opa(lv.OPA.COVER)
                .border_width(0),
                0,
            )
            # # header
            # self.header = lv.obj(self)
            # self.header.remove_style_all()
            # self.header.set_size(lv.pct(100), 100)
            # self.header.set_align(lv.ALIGN.TOP_MID)
            # self.header.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            # self.tips_top = lv.label(self.header)
            # self.tips_top.set_style_text_font(font_PJSBOLD24, 0)
            # self.tips_top.set_style_text_color(lv_colors.WHITE, 0)
            # self.tips_top.set_text(_(i18n_keys.BUTTON__CLOSE))
            # self.tips_top.align(lv.ALIGN.TOP_MID, 0, 16)
            # self.down_img = lv.img(self.header)
            # self.down_img.set_src("A:/res/down-home.png")
            # self.down_img.align_to(self.tips_top, lv.ALIGN.OUT_BOTTOM_MID, 0, 8)
            # # content panel
            # self.panel = lv.obj(self)
            # self.panel.remove_style_all()
            # self.panel.set_size(lv.pct(100), 552)
            # self.panel.align_to(self.down_img, lv.ALIGN.OUT_BOTTOM_MID, 0, 0)
            # self.panel.set_style_bg_color(lv_colors.WHITE_3, 0)
            # self.panel.set_style_bg_opa(lv.OPA.COVER, 0)
            # self.panel.set_style_radius(30, 0)
            # self.panel.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            # # mask
            # self.mask = lv.obj(self)
            # self.mask.remove_style_all()
            # self.mask.set_size(lv.pct(100), 40)
            # self.mask.set_style_bg_color(lv_colors.WHITE_3, 0)
            # self.mask.set_style_bg_opa(lv.OPA.COVER, 0)
            # self.mask.set_align(lv.ALIGN.BOTTOM_MID)
            self.tips = lv.label(self)
            self.tips.set_long_mode(lv.label.LONG.WRAP)
            self.tips.add_style(
                StyleWrapper()
                .text_font(font_PJSREG24)
                .max_width(464)
                .text_color(lv_colors.ONEKEY_GRAY_4)
                .text_align_center(),
                0,
            )
            self.tips.set_text(_(i18n_keys.CONTENT__SWIPE_DOWN_TO_CLOSE))
            self.tips.align(lv.ALIGN.TOP_MID, 0, 52)
            # buttons
            click_style = (
                StyleWrapper()
                .bg_img_recolor_opa(lv.OPA._30)
                .bg_img_recolor(lv_colors.BLACK)
            )

            self.settings = lv.imgbtn(self)
            self.settings.set_pos(78, 134)
            self.settings.set_style_bg_img_src("A:/res/settings.png", 0)
            self.settings.add_style(click_style, lv.PART.MAIN | lv.STATE.PRESSED)
            self.settings.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

            self.guide = lv.imgbtn(self)
            self.guide.align_to(self.settings, lv.ALIGN.OUT_RIGHT_MID, 64, 0)
            self.guide.set_style_bg_img_src("A:/res/guide.png", 0)
            self.guide.add_style(click_style, lv.PART.MAIN | lv.STATE.PRESSED)
            self.guide.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

            self.nft = lv.imgbtn(self)
            self.nft.set_style_bg_img_src("A:/res/app_nft.png", 0)
            self.nft.add_style(click_style, lv.PART.MAIN | lv.STATE.PRESSED)
            self.nft.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            self.nft.align_to(self.settings, lv.ALIGN.OUT_BOTTOM_MID, 0, 65)

            self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
            self.add_event_cb(self.on_slide_down, lv.EVENT.GESTURE, None)
            self.show_anim = Anim(
                800, 0, self.set_pos, start_cb=self.anim_start_cb, delay=10
            )
            self.dismiss_anim = Anim(
                0, 800, self.set_pos, path_cb=lv.anim_t.path_ease_in, time=300
            )
            self.slide = False
            self.visible = False

        def anim_start_cb(self, _anim):
            self.parent.clear_state(lv.STATE.USER_1)

        def show(self):
            if self.visible:
                return
            self.parent.add_state(lv.STATE.USER_1)
            self.show_anim.start()
            # if self.header.has_flag(lv.obj.FLAG.HIDDEN):
            #     self.header.clear_flag(lv.obj.FLAG.HIDDEN)
            self.slide = False
            self.visible = True

        def dismiss(self):
            if not self.visible:
                return
            self.parent.hidden_titles(False)
            if hasattr(self.parent, "dev_state"):
                self.parent.dev_state.show()
            # self.header.add_flag(lv.obj.FLAG.HIDDEN)
            self.dismiss_anim.start()
            self.visible = False

        def on_click(self, event_obj):
            code = event_obj.code
            target = event_obj.get_target()
            if code == lv.EVENT.CLICKED:
                if utils.lcd_resume():
                    return
                if self.slide:
                    return
                if target == self.settings:
                    SettingsScreen(self.parent)
                elif target == self.guide:
                    UserGuide(self.parent)
                elif target == self.nft:
                    NftGallery(self.parent)

        def on_slide_down(self, event_obj):
            code = event_obj.code
            if code == lv.EVENT.GESTURE:
                _dir = lv.indev_get_act().get_gesture_dir()
                if _dir == lv.DIR.BOTTOM:
                    # lv.indev_get_act().wait_release()
                    self.slide = True
                    self.dismiss()


class NftGallery(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
            kwargs = {
                "prev_scr": prev_scr,
                "title": _(i18n_keys.TITLE__NFT_GALLERY),
                "nav_back": True,
            }
            super().__init__(**kwargs)
        else:
            self.overview.delete()
            self.container.delete()

        nft_counts = 0
        file_name_list = []
        if not utils.EMULATOR:
            for size, _attrs, name in io.fatfs.listdir("1:/res/nfts/zooms"):
                # if nft_counts >= 9:
                #     break
                if size > 0:
                    nft_counts += 1
                    file_name_list.append(name)
        if nft_counts == 0:
            self.empty()
        else:
            rows_num = math.ceil(nft_counts / 2)
            row_dsc = [238] * rows_num
            row_dsc.append(lv.GRID_TEMPLATE.LAST)
            # 3 columns
            col_dsc = [
                238,
                238,
                lv.GRID_TEMPLATE.LAST,
            ]

            self.overview = lv.label(self.content_area)
            self.overview.set_size(464, lv.SIZE.CONTENT)
            self.overview.add_style(
                StyleWrapper()
                .text_font(font_PJSREG30)
                .text_color(lv_colors.WHITE_2)
                .text_align_left()
                .text_letter_space(-1),
                0,
            )
            self.overview.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 32)
            self.overview.set_text(f"{nft_counts} items")
            self.container = ContainerGrid(
                self.content_area,
                row_dsc=row_dsc,
                col_dsc=col_dsc,
                align_base=self.title,
                pos=(-8, 74),
                pad_gap=4,
            )
            self.nfts = []
            if not utils.EMULATOR:
                file_name_list.sort(
                    key=lambda name: int(
                        name[5:].split("-")[-1][: -(len(name.split(".")[1]) + 1)]
                    )
                )
                for i, file_name in enumerate(file_name_list):
                    path_dir = "A:1:/res/nfts/zooms/"
                    current_nft = ImgGridItem(
                        self.container,
                        (i) % 2,
                        (i) // 2,
                        file_name,
                        path_dir,
                        is_internal=False,
                    )
                    self.nfts.append(current_nft)

            self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def empty(self):

        self.empty_tips = lv.label(self)
        self.empty_tips.set_text(_(i18n_keys.CONTENT__NO_ITEMS))
        self.empty_tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_color(lv_colors.WHITE_2)
            .text_letter_space(-1),
            0,
        )
        self.empty_tips.align(lv.ALIGN.TOP_MID, 0, 372)

        self.tips_bar = lv.obj(self)
        self.tips_bar.remove_style_all()
        self.tips_bar.add_style(
            StyleWrapper()
            .width(464)
            .height(lv.SIZE.CONTENT)
            .text_font(font_PJSREG30)
            .text_color(lv_colors.ONEKEY_GRAY_4)
            .text_letter_space(-1)
            .bg_color(lv_colors.ONEKEY_GRAY_3)
            .bg_opa()
            .radius(0)
            .border_width(0)
            .pad_hor(8)
            .pad_ver(16),
            0,
        )
        self.tips_bar.align(lv.ALIGN.BOTTOM_MID, 0, -8)
        self.tips_bar_img = lv.img(self.tips_bar)
        self.tips_bar_img.set_src("A:/res/notice.png")
        self.tips_bar_img.set_align(lv.ALIGN.TOP_LEFT)
        self.tips_bar_desc = lv.label(self.tips_bar)
        self.tips_bar_desc.set_size(408, lv.SIZE.CONTENT)
        self.tips_bar_desc.set_long_mode(lv.label.LONG.WRAP)
        self.tips_bar_desc.align_to(self.tips_bar_img, lv.ALIGN.OUT_RIGHT_TOP, 8, 0)
        self.tips_bar_desc.set_text(_(i18n_keys.CONTENT__HOW_TO_COLLECT_NFT__HINT))

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target not in self.nfts:
                return
            for nft in self.nfts:
                if target == nft:
                    file_name_without_ext = nft.file_name[5:-4]
                    desc_file_path = f"1:/res/nfts/desc/{file_name_without_ext}.json"
                    config = {
                        "header": "",
                        "subheader": "",
                        "network": "",
                        "owner": "",
                    }
                    with io.fatfs.open(desc_file_path, "r") as f:
                        description = bytearray(2048)
                        n = f.read(description)
                        if n > 0:
                            config = json.loads((description[:n]).decode("utf-8"))
                    NftManager(config, nft.file_name)

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)


class NftManager(FullSizeWindow):
    def __init__(self, nft_config, file_name):
        self.zoom_path = f"A:1:/res/nfts/zooms/{file_name}"
        self.file_name = file_name.replace("zoom-", "")
        self.img_path = f"A:1:/res/nfts/imgs/{self.file_name}"
        super().__init__(
            title=nft_config["header"],
            subtitle=nft_config["subheader"],
            icon_path=self.img_path,
            anim_dir=0,
        )
        self.nft_config = nft_config
        self.content_area.set_style_max_height(756, 0)
        self.nav_back = Navigation(self.content_area)
        self.nav_back.align(lv.ALIGN.TOP_MID, 0, 0)
        self.icon.align_to(self.nav_back, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 8)
        self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.btn_yes = NormalButton(self.content_area)
        self.btn_yes.set_size(464, 98)
        self.btn_yes.enable(lv_colors.ONEKEY_PURPLE, lv_colors.BLACK)
        self.btn_yes.label.set_text(_(i18n_keys.BUTTON__SET_AS_HOMESCREEN))
        self.btn_yes.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)

        self.btn_del = NormalButton(self.content_area, "")
        self.btn_del.set_size(464, 98)
        self.btn_del.align_to(self.btn_yes, lv.ALIGN.OUT_BOTTOM_MID, 0, 8)
        self.panel = lv.obj(self.btn_del)
        self.panel.remove_style_all()
        self.panel.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.panel.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.panel.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK)
            .text_color(lv_colors.ONEKEY_RED_1)
            .bg_opa(lv.OPA.TRANSP)
            .border_width(0)
            .align(lv.ALIGN.CENTER),
            0,
        )
        self.btn_del_img = lv.img(self.panel)
        self.btn_del_img.set_src("A:/res/btn-del.png")
        self.btn_label = lv.label(self.panel)
        self.btn_label.set_text(_(i18n_keys.BUTTON__DELETE))
        self.btn_label.align_to(self.btn_del_img, lv.ALIGN.OUT_RIGHT_MID, 4, 1)
        self.add_event_cb(self.on_nav_back, lv.EVENT.GESTURE, None)

    def on_nav_back(self, event_obj):
        code = event_obj.code
        if code == lv.EVENT.GESTURE:
            _dir = lv.indev_get_act().get_gesture_dir()
            if _dir == lv.DIR.RIGHT:
                lv.event_send(self.nav_back.nav_btn, lv.EVENT.CLICKED, None)

    def del_callback(self):
        io.fatfs.unlink(self.zoom_path[2:])
        io.fatfs.unlink(self.img_path[2:])
        io.fatfs.unlink("1:/res/nfts/desc/" + self.file_name[:-3] + "json")
        if device.get_homescreen() == self.img_path:
            device.set_homescreen("A:/res/wallpaper-1.jpg")
        self.delete()
        NftGallery()

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                NftManager.ConfirmSetHomeScreen(self.img_path)

            elif target == self.btn_del:
                from trezor.ui.layouts import confirm_remove_nft
                from trezor.wire import DUMMY_CONTEXT

                workflow.spawn(
                    confirm_remove_nft(
                        DUMMY_CONTEXT,
                        self.del_callback,
                        self.zoom_path,
                    )
                )
            elif target == self.nav_back.nav_btn:
                self.destroy(0)

    class ConfirmSetHomeScreen(FullSizeWindow):
        def __init__(self, homescreen):
            super().__init__(
                title=_(i18n_keys.TITLE__SET_AS_HOMESCREEN),
                subtitle=_(i18n_keys.SUBTITLE__SET_AS_HOMESCREEN),
                confirm_text=_(i18n_keys.BUTTON__CONFIRM),
                cancel_text=_(i18n_keys.BUTTON__CANCEL),
            )
            self.homescreen = homescreen

        def eventhandler(self, event_obj):
            code = event_obj.code
            target = event_obj.get_target()
            if code == lv.EVENT.CLICKED:
                if utils.lcd_resume():
                    return
                if target == self.btn_yes:
                    device.set_homescreen(self.homescreen)
                    self.destroy(0)
                    workflow.spawn(utils.internal_reloop())
                elif target == self.btn_no:
                    self.destroy()


class SettingsScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
            kwargs = {
                "prev_scr": prev_scr,
                "title": _(i18n_keys.TITLE__SETTINGS),
                "nav_back": True,
            }
            super().__init__(**kwargs)
        else:
            self.refresh_text()
            return
        # if __debug__:
        #     self.add_style(StyleWrapper().bg_color(lv_colors.ONEKEY_GREEN_1), 0)
        self.container = ContainerFlexCol(self.content_area, self.title, padding_row=2)
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
            _(i18n_keys.ITEM__HOMESCREEN),
            left_img_src="A:/res/homescreen.png",
        )
        self.security = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__SECURITY),
            left_img_src="A:/res/security.png",
        )
        self.wallet = ListItemBtn(
            self.container, _(i18n_keys.ITEM__WALLET), left_img_src="A:/res/wallet.png"
        )
        self.about = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__ABOUT_DEVICE),
            left_img_src="A:/res/about.png",
        )
        # self.boot_loader = ListItemBtn(
        #     self.container,
        #     _(i18n_keys.ITEM__UPDATE_MODE),
        #     left_img_src="A:/res/update_white.png",
        #     has_next=False,
        # )
        self.develop = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__DEVELOPER_OPTIONS),
            left_img_src="A:/res/develop.png",
        )
        self.power = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__POWER_OFF),
            left_img_src="A:/res/poweroff.png",
            has_next=False,
        )
        self.power.label_left.set_style_text_color(lv_colors.ONEKEY_RED_1, 0)
        # if __debug__:
        #     self.test = ListItemBtn(self.container, "UI test")
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def refresh_text(self):
        self.title.set_text(_(i18n_keys.TITLE__SETTINGS))
        self.general.label_left.set_text(_(i18n_keys.ITEM__GENERAL))
        self.connect.label_left.set_text(_(i18n_keys.ITEM__CONNECT))
        self.home_scr.label_left.set_text(_(i18n_keys.ITEM__HOME_SCREEN))
        self.security.label_left.set_text(_(i18n_keys.ITEM__SECURITY))
        self.wallet.label_left.set_text(_(i18n_keys.ITEM__WALLET))
        self.about.label_left.set_text(_(i18n_keys.ITEM__ABOUT_DEVICE))
        # self.boot_loader.label_left.set_text(_(i18n_keys.ITEM__UPDATE_MODE))
        self.develop.label_left.set_text(_(i18n_keys.ITEM__DEVELOPER_OPTIONS))
        self.power.label_left.set_text(_(i18n_keys.ITEM__POWER_OFF))

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.general:
                GeneralScreen(self)
            elif target == self.connect:
                ConnectSetting(self)
            elif target == self.home_scr:
                HomeScreenSetting(self)
            elif target == self.security:
                SecurityScreen(self)
            elif target == self.wallet:
                WalletScreen(self)
            elif target == self.about:
                AboutSetting(self)
            # elif target == self.boot_loader:
            #     Go2UpdateMode(self)
            elif target == self.develop:
                DevelopSettings(self)
            elif target == self.power:
                PowerOff(self)
            else:
                if __debug__:
                    if target == self.test:
                        UITest()

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)


if __debug__:

    class UITest(lv.obj):
        def __init__(self) -> None:
            super().__init__(lv.layer_sys())
            self.set_size(lv.pct(100), lv.pct(100))
            self.align(lv.ALIGN.TOP_LEFT, 0, 0)
            self.set_style_bg_color(lv_colors.BLACK, 0)
            self.set_style_pad_all(0, 0)
            self.set_style_border_width(0, 0)
            self.set_style_radius(0, 0)
            self.set_style_bg_img_src("A:/res/wallpaper-test.png", 0)
            self.add_flag(lv.obj.FLAG.CLICKABLE)
            self.clear_flag(lv.obj.FLAG.SCROLLABLE)
            self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

        def on_click(self, _event_obj):
            self.delete()


class GeneralScreen(Screen):
    cur_auto_lock = ""
    cur_auto_lock_ms = 0
    cur_auto_shutdown = ""
    cur_auto_shutdown_ms = 0
    cur_language = ""

    def __init__(self, prev_scr=None):
        GeneralScreen.cur_auto_lock_ms = device.get_autolock_delay_ms()
        GeneralScreen.cur_auto_shutdown_ms = device.get_autoshutdown_delay_ms()
        GeneralScreen.cur_auto_lock = self.get_str_from_ms(
            GeneralScreen.cur_auto_lock_ms
        )
        GeneralScreen.cur_auto_shutdown = self.get_str_from_ms(
            GeneralScreen.cur_auto_shutdown_ms
        )
        if not hasattr(self, "_init"):
            self._init = True
        else:
            if self.cur_auto_lock:
                self.auto_lock.label_right.set_text(GeneralScreen.cur_auto_lock)
            if self.cur_language:
                self.language.label_right.set_text(self.cur_language)
            self.backlight.label_right.set_text(
                brightness2_percent_str(device.get_brightness())
            )
            if self.cur_auto_shutdown:
                self.auto_shutdown.label_right.set_text(GeneralScreen.cur_auto_shutdown)
            self.refresh_text()
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__GENERAL), nav_back=True
        )

        self.container = ContainerFlexCol(self.content_area, self.title, padding_row=2)
        # self.container.set_height(580)
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
        self.keyboard_haptic = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__VIBRATION_AND_HAPTIC),
        )
        self.animation = ListItemBtn(self.container, _(i18n_keys.ITEM__ANIMATIONS))
        self.tap_awake = ListItemBtn(self.container, _(i18n_keys.ITEM__LOCK_SCREEN))
        self.auto_shutdown = ListItemBtn(
            self.container, _(i18n_keys.ITEM__SHUTDOWN), self.cur_auto_shutdown
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def refresh_text(self):
        self.title.set_text(_(i18n_keys.TITLE__GENERAL))
        self.auto_lock.label_left.set_text(_(i18n_keys.ITEM__AUTO_LOCK))
        self.language.label_left.set_text(_(i18n_keys.ITEM__LANGUAGE))
        self.backlight.label_left.set_text(_(i18n_keys.ITEM__BRIGHTNESS))
        self.keyboard_haptic.label_left.set_text(
            _(i18n_keys.ITEM__VIBRATION_AND_HAPTIC)
        )
        self.animation.label_left.set_text(_(i18n_keys.ITEM__ANIMATIONS))
        self.tap_awake.label_left.set_text(_(i18n_keys.ITEM__LOCK_SCREEN))
        self.auto_shutdown.label_left.set_text(_(i18n_keys.ITEM__SHUTDOWN))

    def get_str_from_ms(self, time_ms) -> str:
        if time_ms == device.AUTOLOCK_DELAY_MAXIMUM:
            return _(i18n_keys.ITEM__STATUS__NEVER)
        auto_lock_time = time_ms / 1000 // 60
        if auto_lock_time > 60:
            value = str(auto_lock_time // 60).split(".")[0]
            text = _(
                i18n_keys.OPTION__STR_HOUR
                if value == "1"
                else i18n_keys.OPTION__STR_HOURS
            ).format(value)
        else:
            value = str(auto_lock_time).split(".")[0]
            text = _(
                i18n_keys.OPTION__STR_MINUTE
                if value == "1"
                else i18n_keys.OPTION__STR_MINUTES
            ).format(value)
        return text

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.auto_lock:
                AutoLockSetting(self)
            elif target == self.language:
                LanguageSetting(self)
            elif target == self.backlight:
                BacklightSetting(self)
            elif target == self.keyboard_haptic:
                KeyboardHapticSetting(self)
            elif target == self.animation:
                AnimationSetting(self)
            elif target == self.tap_awake:
                TapAwakeSetting(self)
            elif target == self.auto_shutdown:
                AutoShutDownSetting(self)
            else:
                pass


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

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        self.setting_items = [1, 2, 5, 10, 30, "Never", None]
        has_custom = True
        self.checked_index = 0
        self.btns: [ListItemBtn] = [None] * (len(self.setting_items))
        for index, item in enumerate(self.setting_items):
            if item is None:
                break
            if not item == "Never":  # last item
                item = _(
                    i18n_keys.ITEM__STATUS__STR_MINUTES
                    if item != 1
                    else i18n_keys.OPTION__STR_MINUTE
                ).format(item)
            else:
                item = _(i18n_keys.ITEM__STATUS__NEVER)
            self.btns[index] = ListItemBtn(
                self.container, item, has_next=False, has_bgcolor=False
            )
            self.btns[index].label_left.add_style(
                StyleWrapper().text_font(font_PJSREG30), 0
            )
            self.btns[index].add_check_img()
            if item == GeneralScreen.cur_auto_lock:
                has_custom = False
                self.btns[index].set_checked()
                self.checked_index = index

        if has_custom:
            self.custom = device.get_autolock_delay_ms()
            self.btns[-1] = ListItemBtn(
                self.container,
                f"{GeneralScreen.cur_auto_lock}({_(i18n_keys.OPTION__CUSTOM__INSERT)})",
                has_next=False,
                has_bgcolor=False,
            )
            self.btns[-1].add_check_img()
            self.btns[-1].set_checked()
            self.btns[-1].label_left.add_style(
                StyleWrapper().text_font(font_PJSREG30), 0
            )
            self.checked_index = -1
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.tips = lv.label(self)
        self.tips.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 0)
        self.tips.set_long_mode(lv.label.LONG.WRAP)
        self.fresh_tips()
        self.tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .width(448)
            .text_color(lv_colors.WHITE_2)
            .text_align_left()
            .text_letter_space(-1)
            .pad_ver(16),
            0,
        )

    def fresh_tips(self):
        item_text = self.btns[self.checked_index].label_left.get_text()
        if self.setting_items[self.checked_index] is None:
            item_text = item_text.split("(")[0]
        if self.setting_items[self.checked_index] == "Never":
            self.tips.set_text(
                _(i18n_keys.CONTENT__SETTINGS_GENERAL_AUTO_LOCK_OFF_HINT)
            )
        else:
            self.tips.set_text(
                _(i18n_keys.CONTENT__SETTINGS_GENERAL_AUTO_LOCK_ON_HINT).format(
                    item_text or GeneralScreen.cur_auto_lock[:1]
                )
            )

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target in self.btns:
                for index, item in enumerate(self.btns):
                    if item == target and self.checked_index != index:
                        item.set_checked()
                        self.btns[self.checked_index].set_uncheck()
                        self.checked_index = index
                        if index == 5:
                            auto_lock_time = device.AUTOLOCK_DELAY_MAXIMUM
                        elif index == 6:
                            auto_lock_time = self.custom
                        else:
                            auto_lock_time = self.setting_items[index] * 60 * 1000
                        device.set_autolock_delay_ms(auto_lock_time)
                        GeneralScreen.cur_auto_lock_ms = auto_lock_time
                        self.fresh_tips()
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
        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.lang_buttons = []
        for idx, lang in enumerate(langs):
            lang_button = ListItemBtn(
                self.container, lang[1], has_next=False, has_bgcolor=False
            )
            lang_button.label_left.add_style(StyleWrapper().text_font(font_PJSREG30), 0)
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
            if utils.lcd_resume():
                return
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

        # self.container = ContainerFlexCol(self, self.title, padding_row=2)
        current_brightness = device.get_brightness()
        # self.item1 = ListItemBtn(
        #     self.container,
        #     _(i18n_keys.ITEM__BRIGHTNESS),
        #     brightness2_percent_str(current_brightness),
        #     has_next=False,
        # )
        self.slider = lv.slider(self)
        self.slider.set_size(464, 82)
        self.slider.set_ext_click_area(100)
        self.slider.set_range(5, style.BACKLIGHT_MAX)
        self.slider.set_value(current_brightness, lv.ANIM.OFF)
        self.slider.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 40)
        self.slider.add_style(
            StyleWrapper().border_width(0).radius(0).bg_color(lv_colors.GRAY_1), 0
        )
        self.slider.add_style(
            StyleWrapper().bg_color(lv_colors.WHITE).pad_all(-50), lv.PART.KNOB
        )
        self.slider.add_style(
            StyleWrapper().radius(0).bg_color(lv_colors.WHITE), lv.PART.INDICATOR
        )
        self.percent = lv.label(self)
        self.percent.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 16, 64)
        self.percent.add_style(
            StyleWrapper().text_font(font_PJSREG30).text_color(lv_colors.BLACK), 0
        )
        self.percent.set_text(brightness2_percent_str(current_brightness))
        self.slider.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        target = event_obj.get_target()
        if target == self.slider:
            value = target.get_value()
            display.backlight(value)
            self.percent.set_text(brightness2_percent_str(value))
            device.set_brightness(value)


class KeyboardHapticSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr,
            title=_(i18n_keys.TITLE__VIBRATION_AND_HAPTIC),
            nav_back=True,
        )

        self.container = ContainerFlexCol(self, self.title)
        self.keyboard = ListItemBtnWithSwitch(
            self.container, _(i18n_keys.ITEM__KEYBOARD_HAPTIC)
        )
        self.tips = lv.label(self)
        self.tips.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)
        self.tips.set_long_mode(lv.label.LONG.WRAP)
        self.tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .width(448)
            .text_color(lv_colors.WHITE_2)
            .text_align_left(),
            0,
        )
        self.tips.set_text(_(i18n_keys.CONTENT__VIBRATION_HAPTIC__HINT))
        if device.keyboard_haptic_enabled():
            self.keyboard.add_state()
        else:
            self.keyboard.clear_state()

        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.keyboard.switch:
                if target.has_state(lv.STATE.CHECKED):
                    device.toggle_keyboard_haptic(True)
                else:
                    device.toggle_keyboard_haptic(False)


class AnimationSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr,
            title=_(i18n_keys.TITLE__ANIMATIONS),
            nav_back=True,
        )

        self.container = ContainerFlexCol(self, self.title)
        self.item = ListItemBtnWithSwitch(self.container, _(i18n_keys.ITEM__ANIMATIONS))
        self.tips = lv.label(self)
        self.tips.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)
        self.tips.set_long_mode(lv.label.LONG.WRAP)
        self.tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .width(448)
            .text_color(lv_colors.WHITE_2)
            .text_align_left(),
            0,
        )
        if device.is_animation_enabled():
            self.item.add_state()
            self.tips.set_text(_(i18n_keys.CONTENT__ANIMATIONS__ENABLED_HINT))
        else:
            self.item.clear_state()
            self.tips.set_text(_(i18n_keys.CONTENT__ANIMATIONS__DISABLED_HINT))

        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.item.switch:
                if target.has_state(lv.STATE.CHECKED):
                    device.set_animation_enable(True)
                    self.tips.set_text(_(i18n_keys.CONTENT__ANIMATIONS__ENABLED_HINT))
                else:
                    device.set_animation_enable(False)
                    self.tips.set_text(_(i18n_keys.CONTENT__ANIMATIONS__DISABLED_HINT))


class TapAwakeSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__LOCK_SCREEN), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title)
        self.tap_awake = ListItemBtnWithSwitch(
            self.container, _(i18n_keys.ITEM__TAP_TO_WAKE)
        )
        self.description = lv.label(self)
        self.description.set_size(464, lv.SIZE.CONTENT)
        self.description.set_long_mode(lv.label.LONG.WRAP)
        self.description.set_style_text_color(lv_colors.ONEKEY_GRAY, lv.STATE.DEFAULT)
        self.description.set_style_text_font(font_PJSREG24, lv.STATE.DEFAULT)
        self.description.set_style_text_line_space(3, 0)
        self.description.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)

        if device.is_tap_awake_enabled():
            self.tap_awake.add_state()
            self.description.set_text(_(i18n_keys.CONTENT__TAP_TO_WAKE_ENABLED__HINT))
        else:
            self.tap_awake.clear_state()
            self.description.set_text(_(i18n_keys.CONTENT__TAP_TO_WAKE_DISABLED__HINT))
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.tap_awake.switch:
                if target.has_state(lv.STATE.CHECKED):
                    self.description.set_text(
                        _(i18n_keys.CONTENT__TAP_TO_WAKE_ENABLED__HINT)
                    )
                    device.set_tap_awake_enable(True)
                else:
                    self.description.set_text(
                        _(i18n_keys.CONTENT__TAP_TO_WAKE_DISABLED__HINT)
                    )
                    device.set_tap_awake_enable(False)


class AutoShutDownSetting(Screen):
    # TODO: i18n
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__SHUTDOWN), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        self.setting_items = [1, 2, 5, 10, "Never", None]
        has_custom = True
        self.checked_index = 0
        # pyright: off
        self.btns: [ListItemBtn] = [None] * (len(self.setting_items))
        for index, item in enumerate(self.setting_items):
            if item is None:
                break
            if not item == "Never":  # last item
                item = _(
                    i18n_keys.ITEM__STATUS__STR_MINUTES
                    if item != 1
                    else i18n_keys.OPTION__STR_MINUTE
                ).format(item)
            else:
                item = _(i18n_keys.ITEM__STATUS__NEVER)
            self.btns[index] = ListItemBtn(
                self.container, item, has_next=False, has_bgcolor=False
            )
            self.btns[index].label_left.add_style(
                StyleWrapper().text_font(font_PJSREG30), 0
            )
            self.btns[index].add_check_img()
            if item == GeneralScreen.cur_auto_shutdown:
                has_custom = False
                self.btns[index].set_checked()
                self.checked_index = index

        if has_custom:
            self.custom = device.get_autoshutdown_delay_ms()
            self.btns[-1] = ListItemBtn(
                self.container,
                f"{GeneralScreen.cur_auto_shutdown}({_(i18n_keys.OPTION__CUSTOM__INSERT)})",
                has_next=False,
                has_bgcolor=False,
            )
            self.btns[-1].add_check_img()
            self.btns[-1].set_checked()
            self.checked_index = -1
        # pyright: on
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.tips = lv.label(self)
        self.tips.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 0)
        self.tips.set_long_mode(lv.label.LONG.WRAP)
        self.fresh_tips()
        self.tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .width(448)
            .text_color(lv_colors.WHITE_2)
            .text_align_left()
            .text_letter_space(-1)
            .pad_ver(16),
            0,
        )

    def fresh_tips(self):
        item_text = self.btns[self.checked_index].label_left.get_text()
        if self.setting_items[self.checked_index] is None:
            item_text = item_text.split("(")[0]

        if self.setting_items[self.checked_index] == "Never":
            self.tips.set_text(_(i18n_keys.CONTENT__SETTINGS_GENERAL_SHUTDOWN_OFF_HINT))
        else:
            self.tips.set_text(
                _(i18n_keys.CONTENT__SETTINGS_GENERAL_SHUTDOWN_ON_HINT).format(
                    item_text or GeneralScreen.cur_auto_shutdown[:1]
                )
            )

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target in self.btns:
                for index, item in enumerate(self.btns):
                    if item == target and self.checked_index != index:
                        item.set_checked()
                        self.btns[self.checked_index].set_uncheck()
                        self.checked_index = index
                        if index == 4:
                            auto_shutdown_time = device.AUTOSHUTDOWN_DELAY_MAXIMUM
                        elif index == 5:
                            auto_shutdown_time = self.custom
                        else:
                            auto_shutdown_time = self.setting_items[index] * 60 * 1000
                        device.set_autoshutdown_delay_ms(auto_shutdown_time)
                        GeneralScreen.cur_auto_shutdown_ms = auto_shutdown_time
                        self.fresh_tips()
                        from apps.base import reload_settings_from_storage

                        reload_settings_from_storage()


class PinMapSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__PIN_KEYPAD), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title, padding_row=0)
        self.order = ListItemBtn(
            self.container,
            _(i18n_keys.OPTION__DEFAULT),
            has_next=False,
            has_bgcolor=False,
        )
        self.order.add_check_img()
        self.random = ListItemBtn(
            self.container,
            _(i18n_keys.OPTION__RANDOMIZED),
            has_next=False,
            has_bgcolor=False,
        )
        self.random.add_check_img()
        self.tips = lv.label(self)
        self.tips.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 0)
        self.tips.set_long_mode(lv.label.LONG.WRAP)
        self.fresh_tips()
        self.tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .width(448)
            .text_color(lv_colors.WHITE_2)
            .text_letter_space(-1)
            .text_align_left()
            .pad_ver(16),
            0,
        )

        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def fresh_tips(self):
        if device.is_random_pin_map_enabled():
            self.random.set_checked()
            self.tips.set_text(
                _(i18n_keys.CONTENT__SECURITY_PIN_KEYPAD_LAYOUT_RANDOMIZED__HINT)
            )
        else:
            self.order.set_checked()
            self.tips.set_text(
                _(i18n_keys.CONTENT__SECURITY_PIN_KEYPAD_LAYOUT_DEFAULT__HINT)
            )

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.random:
                self.random.set_checked()
                self.order.set_uncheck()
                if not device.is_random_pin_map_enabled():
                    device.set_random_pin_map_enable(True)
            elif target == self.order:
                self.random.set_uncheck()
                self.order.set_checked()
                if device.is_random_pin_map_enabled():
                    device.set_random_pin_map_enable(False)
            else:
                return
            self.fresh_tips()


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
        self.ble = ListItemBtnWithSwitch(self.container, _(i18n_keys.ITEM__BLUETOOTH))

        self.description = lv.label(self)
        self.description.set_size(464, lv.SIZE.CONTENT)
        self.description.set_long_mode(lv.label.LONG.WRAP)
        self.description.set_style_text_color(lv_colors.ONEKEY_GRAY, lv.STATE.DEFAULT)
        self.description.set_style_text_font(font_PJSREG24, lv.STATE.DEFAULT)
        self.description.set_style_text_line_space(3, 0)
        self.description.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)

        if uart.is_ble_opened():
            self.ble.add_state()
            self.description.set_text(
                _(i18n_keys.CONTENT__CONNECT_BLUETOOTH_ENABLED__HINT).format(
                    device.get_ble_name()
                )
            )
        else:
            self.ble.clear_state()
            self.description.set_text(
                _(i18n_keys.CONTENT__CONNECT_BLUETOOTH_DISABLED__HINT)
            )
        # self.usb = ListItemBtnWithSwitch(self.container, _(i18n_keys.ITEM__USB))
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:

            if target == self.ble.switch:
                if target.has_state(lv.STATE.CHECKED):
                    self.description.set_text(
                        self.description.set_text(
                            _(
                                i18n_keys.CONTENT__CONNECT_BLUETOOTH_ENABLED__HINT
                            ).format(device.get_ble_name())
                        )
                    )
                    uart.ctrl_ble(enable=True)
                else:
                    self.description.set_text(
                        _(i18n_keys.CONTENT__CONNECT_BLUETOOTH_DISABLED__HINT)
                    )
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

        ble_name = device.get_ble_name() or uart.get_ble_name()
        ble_version = uart.get_ble_version()
        storage = device.get_storage()
        boot_version = utils.boot_version()
        board_version = utils.board_version()
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__ABOUT_DEVICE), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title, padding_row=0)

        self.model = DisplayItem(self.container, _(i18n_keys.ITEM__MODEL), model)
        self.model.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.model.label_top.add_style(StyleWrapper().text_color(lv_colors.WHITE), 0)
        self.model.set_style_bg_color(lv_colors.BLACK, 0)

        self.ble_mac = DisplayItem(
            self.container,
            _(i18n_keys.ITEM__BLUETOOTH_NAME),
            ble_name,
        )
        self.ble_mac.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.ble_mac.label_top.add_style(StyleWrapper().text_color(lv_colors.WHITE), 0)
        self.ble_mac.set_style_bg_color(lv_colors.BLACK, 0)

        self.storage = DisplayItem(
            self.container,
            _(i18n_keys.ITEM__STORAGE),
            storage,
        )
        self.storage.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.storage.label_top.add_style(StyleWrapper().text_color(lv_colors.WHITE), 0)
        self.storage.set_style_bg_color(lv_colors.BLACK, 0)

        self.version = DisplayItem(
            self.container, _(i18n_keys.ITEM__SYSTEM_VERSION), version
        )
        self.version.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.version.label_top.add_style(StyleWrapper().text_color(lv_colors.WHITE), 0)
        self.version.set_style_bg_color(lv_colors.BLACK, 0)

        self.ble_version = DisplayItem(
            self.container,
            _(i18n_keys.ITEM__BLUETOOTH_VERSION),
            ble_version,
        )
        self.ble_version.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.ble_version.label_top.add_style(
            StyleWrapper().text_color(lv_colors.WHITE), 0
        )
        self.ble_version.set_style_bg_color(lv_colors.BLACK, 0)

        self.boot_version = DisplayItem(
            self.container, _(i18n_keys.ITEM__BOOTLOADER_VERSION), boot_version
        )
        self.boot_version.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.boot_version.label_top.add_style(
            StyleWrapper().text_color(lv_colors.WHITE), 0
        )
        self.boot_version.set_style_bg_color(lv_colors.BLACK, 0)

        self.board_version = DisplayItem(
            self.container,
            _(i18n_keys.ITEM__BOARDLOADER_VERSION),
            board_version,
        )
        self.board_version.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.board_version.label_top.add_style(
            StyleWrapper().text_color(lv_colors.WHITE), 0
        )
        self.board_version.set_style_bg_color(lv_colors.BLACK, 0)
        self.build_id = DisplayItem(
            self.container, _(i18n_keys.ITEM__BUILD_ID), utils.BUILD_ID[-7:]
        )
        self.build_id.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.build_id.label_top.add_style(StyleWrapper().text_color(lv_colors.WHITE), 0)
        self.build_id.set_style_bg_color(lv_colors.BLACK, 0)

        self.serial = DisplayItem(
            self.container, _(i18n_keys.ITEM__SERIAL_NUMBER), serial
        )
        self.serial.label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_color(lv_colors.LIGHT_GRAY), 0
        )
        self.serial.label_top.add_style(StyleWrapper().text_color(lv_colors.WHITE), 0)
        self.serial.set_style_bg_color(lv_colors.BLACK, 0)
        self.serial.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        self.board_loader = ListItemBtn(
            self.container, _(i18n_keys.ITEM__BOARDLOADER), has_next=False
        )
        self.board_loader.set_style_bg_color(lv_colors.BLACK, 0)
        self.board_loader.add_flag(lv.obj.FLAG.HIDDEN)
        self.firmware_update = NormalButton(self, _(i18n_keys.BUTTON__SYSTEM_UPDATE))
        self.firmware_update.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        self.serial.add_event_cb(self.on_long_pressed, lv.EVENT.LONG_PRESSED, None)
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.firmware_update.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        target = event_obj.get_target()
        if target == self.board_loader:
            GO2BoardLoader()
        if target == self.firmware_update:
            Go2UpdateMode(self)

    def on_long_pressed(self, event_obj):
        target = event_obj.get_target()
        if target == self.serial:
            if self.board_loader.has_flag(lv.obj.FLAG.HIDDEN):
                self.board_loader.clear_flag(lv.obj.FLAG.HIDDEN)
            else:
                self.board_loader.add_flag(lv.obj.FLAG.HIDDEN)


class GO2BoardLoader(FullSizeWindow):
    def __init__(self):
        super().__init__(
            title=_(i18n_keys.TITLE__ENTERING_BOARDLOADER),
            subtitle=_(i18n_keys.SUBTITLE__SWITCH_TO_BOARDLOADER_RECONFIRM),
            confirm_text=_(i18n_keys.BUTTON__RESTART),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            # icon_path="A:/res/warning.png",
        )

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                utils.reboot2boardloader()
            elif target == self.btn_no:
                self.destroy(100)


class Go2UpdateMode(Screen):
    def __init__(self, prev_scr):
        super().__init__(
            prev_scr=prev_scr,
            title=_(i18n_keys.TITLE__SYSTEM_UPDATE),
            subtitle=_(i18n_keys.SUBTITLE__SWITCH_TO_UPDATE_MODE_RECONFIRM),
            # icon_path="A:/res/update_green.png",
        )
        self.btn_yes = NormalButton(self, _(i18n_keys.BUTTON__RESTART))
        self.btn_yes.set_size(231, 98)
        self.btn_yes.align_to(self, lv.ALIGN.BOTTOM_RIGHT, -8, -8)
        self.btn_yes.enable(lv_colors.ONEKEY_GREEN, lv_colors.BLACK)
        self.btn_no = NormalButton(self, _(i18n_keys.BUTTON__CANCEL))
        self.btn_no.set_size(231, 98)
        self.btn_no.align(lv.ALIGN.BOTTOM_LEFT, 8, -8)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                utils.reboot_to_bootloader()
            elif target == self.btn_no:
                self.load_screen(self.prev_scr, destroy_self=True)


class PowerOff(Screen):
    def __init__(self, prev_scr=None, re_loop: bool = False):
        super().__init__(
            prev_scr=prev_scr,
            title=_(i18n_keys.TITLE__POWER_OFF),
        )
        self.btn_yes = NormalButton(self, _(i18n_keys.ITEM__POWER_OFF))
        self.btn_yes.set_size(231, 98)
        self.btn_yes.align_to(self, lv.ALIGN.BOTTOM_RIGHT, -8, -8)
        self.btn_yes.enable(lv_colors.ONEKEY_RED_1, text_color=lv_colors.BLACK)

        self.btn_no = NormalButton(self, _(i18n_keys.BUTTON__CANCEL))
        self.btn_no.set_size(231, 98)
        self.btn_no.align(lv.ALIGN.BOTTOM_LEFT, 8, -8)
        self.re_loop = re_loop
        from trezor import config

        self.has_pin = config.has_pin()
        if self.has_pin:
            config.lock()

    def back(self):
        self.load_screen(self.prev_scr, destroy_self=True)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                ShutingDown()
            elif target == self.btn_no:
                if self.has_pin:
                    from apps.common.request_pin import verify_user_pin

                    workflow.spawn(
                        verify_user_pin(
                            re_loop=self.re_loop,
                            allow_cancel=False,
                            callback=self.back,
                        )
                    )
                else:
                    self.back()

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)


class DevelopSettings(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            # self.safety_check.label_right.set_text(self.get_right_text())
            return
        super().__init__(
            prev_scr=prev_scr,
            title=_(i18n_keys.TITLE__DEVELOPER_OPTIONS),
            nav_back=True,
        )

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        self.safety_check = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__SAFETY_CHECKS),
            # right_text=self.get_right_text(),
        )
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def get_right_text(self) -> str:
        return (
            _(i18n_keys.ITEM__STATUS__STRICT)
            if safety_checks.is_strict()
            else _(i18n_keys.ITEM__STATUS__PROMPT)
        )

    def on_click(self, event_obj):
        target = event_obj.get_target()
        if target == self.safety_check:
            SafetyCheckSetting(self)


class ShutingDown(FullSizeWindow):
    def __init__(self):
        super().__init__(
            title=_(i18n_keys.TITLE__SHUTTING_DOWN), subtitle=None, anim_dir=0
        )

        async def shutdown_delay():
            await loop.sleep(3000)
            uart.ctrl_power_off()

        workflow.spawn(shutdown_delay())


class HomeScreenSetting(Screen):
    def __init__(self, prev_scr=None):
        homescreen = device.get_homescreen()
        if not hasattr(self, "_init"):
            self._init = True
            super().__init__(
                prev_scr=prev_scr, title=_(i18n_keys.TITLE__HOMESCREEN), nav_back=True
            )

        else:
            self.container.delete()

        internal_wp_nums = 4
        wp_nums = internal_wp_nums
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)
        file_name_list = []
        if not utils.EMULATOR:
            for size, _attrs, name in io.fatfs.listdir("1:/res/wallpapers"):
                if wp_nums >= 9:
                    break
                if size > 0 and name[:4] == "zoom":
                    wp_nums += 1
                    file_name_list.append(name)
        rows_num = math.ceil(wp_nums / 3)
        row_dsc = [GRID_CELL_SIZE_ROWS] * rows_num
        row_dsc.append(lv.GRID_TEMPLATE.LAST)
        # 3 columns
        col_dsc = [
            GRID_CELL_SIZE_COLS,
            GRID_CELL_SIZE_COLS,
            GRID_CELL_SIZE_COLS,
            lv.GRID_TEMPLATE.LAST,
        ]
        self.container = ContainerGrid(
            self, row_dsc=row_dsc, col_dsc=col_dsc, align_base=self.title
        )
        self.wps = []
        for i in range(wp_nums):
            path_dir = "A:/res/"
            if utils.EMULATOR:
                file_name = f"zoom-wallpaper-{i+1}.png"
            else:
                file_name = f"zoom-wallpaper-{i+1}.jpg"

            current_wp = ImgGridItem(
                self.container,
                i % 3,
                i // 3,
                file_name,
                path_dir,
                is_internal=True,
            )
            self.wps.append(current_wp)
            if homescreen == current_wp.img_path:
                current_wp.set_checked(True)

        if not utils.EMULATOR:
            file_name_list.sort(
                key=lambda name: int(
                    name[5:].split("-")[-1][: -(len(name.split(".")[1]) + 1)]
                )
            )
            for i, file_name in enumerate(file_name_list):
                path_dir = "A:1:/res/wallpapers/"
                current_wp = ImgGridItem(
                    self.container,
                    (i + internal_wp_nums) % 3,
                    (i + internal_wp_nums) // 3,
                    file_name,
                    path_dir,
                    is_internal=False,
                )
                self.wps.append(current_wp)
                if homescreen == current_wp.img_path:
                    current_wp.set_checked(True)
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target not in self.wps:
                return
            for wp in self.wps:
                if target == wp:
                    WallPaperManage(
                        self,
                        img_path=wp.img_path,
                        zoom_path=wp.zoom_path,
                        is_internal=wp.is_internal,
                    )


class WallPaperManage(Screen):
    def __init__(
        self,
        prev_scr=None,
        img_path: str = "",
        zoom_path: str = "",
        is_internal: bool = False,
    ):
        super().__init__(
            prev_scr,
            icon_path=zoom_path,
            title=_(i18n_keys.TITLE__MANAGE_WALLPAPER),
            subtitle=_(i18n_keys.SUBTITLE__MANAGE_WALLPAPER),
            nav_back=True,
        )
        self.img_path = img_path
        self.zoom_path = zoom_path

        self.btn_yes = NormalButton(self, _(i18n_keys.BUTTON__SET))
        self.btn_yes.add_style(
            StyleWrapper().bg_color(lv_colors.ONEKEY_GREEN).text_color(lv_colors.BLACK),
            0,
        )
        if not is_internal:
            self.btn_yes.set_size(231, 98)
            self.btn_yes.align_to(self, lv.ALIGN.BOTTOM_RIGHT, -8, -8)
            self.btn_del = NormalButton(self, "")
            self.btn_del.set_size(231, 98)
            self.btn_del.align(lv.ALIGN.BOTTOM_LEFT, 8, -8)
            # self.btn_del.add_style(
            #     StyleWrapper()
            #     .bg_color(lv_colors.ONEKEY_RED)
            #     .text_color(lv_colors.ONEKEY_RED_1),
            #     0,
            # )
            self.panel = lv.obj(self.btn_del)
            self.panel.remove_style_all()
            self.panel.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
            self.panel.clear_flag(lv.obj.FLAG.CLICKABLE)

            self.btn_del_img = lv.img(self.panel)
            self.btn_del_img.set_src("A:/res/btn-del.png")
            self.btn_label = lv.label(self.panel)
            self.btn_label.set_text(_(i18n_keys.BUTTON__DELETE))
            self.btn_label.align_to(self.btn_del_img, lv.ALIGN.OUT_RIGHT_MID, 4, 1)

            self.panel.add_style(
                StyleWrapper()
                .bg_color(lv_colors.ONEKEY_BLACK)
                .text_color(lv_colors.ONEKEY_RED_1)
                .bg_opa(lv.OPA.TRANSP)
                .border_width(0)
                .align(lv.ALIGN.CENTER),
                0,
            )

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)

    def del_callback(self):
        io.fatfs.unlink(self.img_path[2:])
        io.fatfs.unlink(self.zoom_path[2:])
        if device.get_homescreen() == self.img_path:
            device.set_homescreen("A:/res/wallpaper-1.jpg")
        self.load_screen(self.prev_scr, destroy_self=True)

    # def cancel_callback(self):
    #     self.btn_del.clear_flag(lv.obj.FLAG.HIDDEN)

    def eventhandler(self, event_obj):
        event = event_obj.code
        target = event_obj.get_target()
        if event == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if isinstance(target, lv.imgbtn):
                if target == self.nav_back.nav_btn:
                    if self.prev_scr is not None:
                        self.load_screen(self.prev_scr, destroy_self=True)
            else:
                if target == self.btn_yes:
                    device.set_homescreen(self.img_path)
                    self.load_screen(self.prev_scr, destroy_self=True)
                elif hasattr(self, "btn_del") and target == self.btn_del:
                    from trezor.ui.layouts import confirm_del_wallpaper
                    from trezor.wire import DUMMY_CONTEXT

                    workflow.spawn(
                        confirm_del_wallpaper(DUMMY_CONTEXT, self.del_callback)
                    )


class SecurityScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            # self.safety_check.label_right.set_text(self.get_right_text())
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__SECURITY), nav_back=True)
        self.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        self.pin_map_type = ListItemBtn(self.container, _(i18n_keys.ITEM__PIN_KEYPAD))
        self.usb_lock = ListItemBtn(self.container, _(i18n_keys.ITEM__USB_LOCK))
        # self.safety_check = ListItemBtn(
        #     self.container,
        #     _(i18n_keys.ITEM__SAFETY_CHECKS),
        #     right_text=self.get_right_text(),
        # )
        self.change_pin = ListItemBtn(self.container, _(i18n_keys.ITEM__CHANGE_PIN))
        # self.recovery_check = ListItemBtn(
        #     self.container, _(i18n_keys.ITEM__CHECK_RECOVERY_PHRASE)
        # )
        # self.passphrase = ListItemBtn(self.container, _(i18n_keys.ITEM__PASSPHRASE))
        self.rest_device = ListItemBtn(
            self.container, _(i18n_keys.ITEM__RESET_DEVICE), has_next=False
        )
        self.rest_device.label_left.set_style_text_color(lv_colors.ONEKEY_RED_1, 0)
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    # def get_right_text(self) -> str:
    #     return (
    #         _(i18n_keys.ITEM__STATUS__STRICT)
    #         if safety_checks.is_strict()
    #         else _(i18n_keys.ITEM__STATUS__PROMPT)
    #     )

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        # pyright: off
        if code == lv.EVENT.CLICKED:
            from trezor.wire import DUMMY_CONTEXT

            if utils.lcd_resume():
                return
            if target == self.change_pin:
                from apps.management.change_pin import change_pin
                from trezor.messages import ChangePin

                workflow.spawn(change_pin(DUMMY_CONTEXT, ChangePin(remove=False)))
            elif target == self.rest_device:
                from apps.management.wipe_device import wipe_device
                from trezor.messages import WipeDevice

                workflow.spawn(wipe_device(DUMMY_CONTEXT, WipeDevice()))
            elif target == self.pin_map_type:
                PinMapSetting(self)
            elif target == self.usb_lock:
                UsbLockSetting(self)
            # elif target == self.safety_check:
            #     SafetyCheckSetting(self)
            else:
                if __debug__:
                    print("unknown")
        # pyright: on


class UsbLockSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__USB_LOCK), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title)
        self.usb_lock = ListItemBtnWithSwitch(
            self.container, _(i18n_keys.ITEM__USB_LOCK)
        )

        self.description = lv.label(self)
        self.description.set_size(464, lv.SIZE.CONTENT)
        self.description.set_long_mode(lv.label.LONG.WRAP)
        self.description.set_style_text_color(lv_colors.ONEKEY_GRAY, lv.STATE.DEFAULT)
        self.description.set_style_text_font(font_PJSREG24, lv.STATE.DEFAULT)
        self.description.set_style_text_line_space(3, 0)
        self.description.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)

        if device.is_usb_lock_enabled():
            self.usb_lock.add_state()
            self.description.set_text(_(i18n_keys.CONTENT__USB_LOCK_ENABLED__HINT))
        else:
            self.usb_lock.clear_state()
            self.description.set_text(_(i18n_keys.CONTENT__USB_LOCK_DISABLED__HINT))
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.usb_lock.switch:
                if target.has_state(lv.STATE.CHECKED):
                    self.description.set_text(
                        _(i18n_keys.CONTENT__USB_LOCK_ENABLED__HINT)
                    )
                    device.set_usb_lock_enable(True)
                else:
                    self.description.set_text(
                        _(i18n_keys.CONTENT__USB_LOCK_DISABLED__HINT)
                    )
                    device.set_usb_lock_enable(False)


class SafetyCheckSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__SAFETY_CHECKS), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        # self.strict = ListItemBtn(
        #     self.container, _(i18n_keys.ITEM__STATUS__STRICT), has_next=False
        # )
        # self.strict.add_check_img()
        # self.prompt = ListItemBtn(
        #     self.container, _(i18n_keys.ITEM__STATUS__PROMPT), has_next=False
        # )
        self.safety_check = ListItemBtnWithSwitch(
            self.container, _(i18n_keys.ITEM__SAFETY_CHECKS)
        )
        # self.prompt.add_check_img()
        self.description = lv.label(self)
        self.description.set_size(464, lv.SIZE.CONTENT)
        self.description.set_long_mode(lv.label.LONG.WRAP)
        self.description.set_style_text_font(font_PJSREG24, lv.STATE.DEFAULT)
        self.description.set_style_text_line_space(3, 0)
        self.description.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)
        self.description.set_recolor(True)
        # self.set_checked()
        self.retrieval_state()

        self.container.add_event_cb(self.on_click, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.on_click, lv.EVENT.READY, None)

    def retrieval_state(self):
        if safety_checks.is_strict():
            self.safety_check.add_state()
            self.description.set_text(_(i18n_keys.CONTENT__SAFETY_CHECKS_STRICT__HINT))
            self.description.set_style_text_color(
                lv_colors.ONEKEY_GRAY, lv.STATE.DEFAULT
            )
            self.clear_warning_desc()
        else:
            self.safety_check.clear_state()
            if safety_checks.is_prompt_always():
                self.description.set_text(
                    _(i18n_keys.CONTENT__SAFETY_CHECKS_PERMANENTLY_PROMPT__HINT)
                )
                self.add_warning_desc(
                    lv_colors.ONEKEY_RED_2,
                    lv_colors.ONEKEY_RED_3,
                    "A:/res/alert-danger.png",
                    lv.color_hex(0x290700),
                )
            else:
                self.description.set_text(
                    _(i18n_keys.CONTENT__SAFETY_CHECKS_TEMPORARILY_PROMPT__HINT)
                )
                self.add_warning_desc(
                    lv_colors.ONEKEY_YELLOW,
                    lv.color_hex(0xC1A400),
                    "A:/res/alert-warning.png",
                    lv.color_hex(0x332C00),
                )

    def add_warning_desc(self, primary_color, border_color, icon_path, bg_color):
        if not hasattr(self, "warning_desc"):
            self.warning_desc = lv.obj(self)
            self.warning_desc.remove_style_all()
            self.warning_desc.set_size(464, lv.SIZE.CONTENT)
            self.warning_desc.align(lv.ALIGN.BOTTOM_MID, 0, -8)
            self.warning_desc.add_style(
                StyleWrapper()
                .bg_color(bg_color)
                .pad_all(8)
                .border_color(border_color)
                .border_width(1)
                .text_color(primary_color)
                .text_font(font_PJSREG24),
                0,
            )
            self.icon = lv.img(self.warning_desc)
            self.icon.set_src(icon_path)
            self.icon.set_align(lv.ALIGN.TOP_LEFT)
            self.desc = lv.label(self.warning_desc)
            self.desc.set_width(408)
            self.desc.set_long_mode(lv.label.LONG.WRAP)
            self.desc.align_to(self.icon, lv.ALIGN.OUT_RIGHT_TOP, 4, 0)
            self.desc.set_text(_(i18n_keys.MSG__SAFETY_CHECKS_PROMPT_WARNING))

    def clear_warning_desc(self):
        if hasattr(self, "warning_desc"):
            self.warning_desc.delete()
            del self.warning_desc

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.safety_check.switch:
                if target.has_state(lv.STATE.CHECKED):
                    SafetyCheckStrictConfirm(self)
                else:
                    SafetyCheckPromptConfirm(self)
        elif code == lv.EVENT.READY:
            self.retrieval_state()


class SafetyCheckStrictConfirm(FullSizeWindow):
    def __init__(self, callback_obj):
        super().__init__(
            _(i18n_keys.TITLE__ENABLE_SAFETY_CHECKS),
            _(i18n_keys.SUBTITLE__ENABLE_SAFETY_CHECKS),
            confirm_text=_(i18n_keys.BUTTON__CONFIRM),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            # icon_path="A:/res/warning.png",
        )
        self.callback = callback_obj

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                safety_checks.apply_setting(SafetyCheckLevel.Strict)
            elif target != self.btn_no:
                return
            lv.event_send(self.callback, lv.EVENT.READY, None)
            self.destroy(0)


class SafetyCheckPromptConfirm(FullSizeWindow):
    def __init__(self, callback_obj):
        super().__init__(
            _(i18n_keys.TITLE__DISABLE_SAFETY_CHECKS),
            _(i18n_keys.SUBTITLE__SET_SAFETY_CHECKS_TO_PROMPT),
            confirm_text=_(i18n_keys.BUTTON__SLIDE_TO_DISABLE),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            # icon_path="A:/res/warning.png",
            hold_confirm=True,
            anim_dir=0,
        )
        self.slider.change_knob_style(1)
        # self.status_bar = lv.obj(self)
        # self.status_bar.remove_style_all()
        # self.status_bar.set_size(lv.pct(100), 44)
        # self.status_bar.add_style(
        #     StyleWrapper()
        #     .bg_opa()
        #     .align(lv.ALIGN.TOP_LEFT)
        #     .bg_img_src("A:/res/warning_bar.png"),
        #     0,
        # )
        self.callback = callback_obj

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_no.click_mask:
                self.destroy(100)
        elif code == lv.EVENT.READY:
            if target == self.slider:
                safety_checks.apply_setting(SafetyCheckLevel.PromptTemporarily)
                self.destroy(0)
        lv.event_send(self.callback, lv.EVENT.READY, None)


class WalletScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__WALLET), nav_back=True)

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        self.check_mnemonic = ListItemBtn(
            self.container, _(i18n_keys.ITEM__CHECK_RECOVERY_PHRASE)
        )
        self.passphrase = ListItemBtn(self.container, _(i18n_keys.ITEM__PASSPHRASE))
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            from trezor.wire import DUMMY_CONTEXT

            if target == self.check_mnemonic:
                from apps.management.recovery_device import recovery_device
                from trezor.messages import RecoveryDevice

                # pyright: off
                workflow.spawn(
                    recovery_device(
                        DUMMY_CONTEXT,
                        RecoveryDevice(dry_run=True, enforce_wordlist=True),
                    )
                )
                # pyright: on
            elif target == self.passphrase:
                PassphraseScreen(self)


class PassphraseScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(
            prev_scr=prev_scr, title=_(i18n_keys.TITLE__PASSPHRASE), nav_back=True
        )

        self.container = ContainerFlexCol(self, self.title)
        self.passphrase = ListItemBtnWithSwitch(
            self.container, _(i18n_keys.ITEM__PASSPHRASE)
        )
        self.description = lv.label(self)
        self.description.set_size(464, lv.SIZE.CONTENT)
        self.description.set_long_mode(lv.label.LONG.WRAP)
        self.description.set_style_text_color(lv_colors.ONEKEY_GRAY, lv.STATE.DEFAULT)
        self.description.set_style_text_font(font_PJSREG24, lv.STATE.DEFAULT)
        self.description.set_style_text_line_space(3, 0)
        self.description.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 16)

        passphrase_enable = device.is_passphrase_enabled()
        if passphrase_enable:
            self.passphrase.add_state()
            self.description.set_text(_(i18n_keys.CONTENT__PASSPHRASE_ENABLED__HINT))
        else:
            self.passphrase.clear_state()
            self.description.set_text(_(i18n_keys.CONTENT__PASSPHRASE_DISABLED__HINT))
        self.container.add_event_cb(self.on_value_changed, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.on_value_changed, lv.EVENT.READY, None)
        self.add_event_cb(self.on_value_changed, lv.EVENT.CANCEL, None)

    def on_value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.passphrase.switch:
                if target.has_state(lv.STATE.CHECKED):
                    screen = PassphraseTipsConfirm(
                        _(i18n_keys.TITLE__ENABLE_PASSPHRASE),
                        _(i18n_keys.SUBTITLE__ENABLE_PASSPHRASE),
                        _(i18n_keys.BUTTON__ENABLE),
                        self,
                        primary_color=lv_colors.ONEKEY_YELLOW,
                    )
                    screen.btn_yes.enable(lv_colors.ONEKEY_YELLOW, lv_colors.BLACK)
                else:
                    PassphraseTipsConfirm(
                        _(i18n_keys.TITLE__DISABLE_PASSPHRASE),
                        _(i18n_keys.SUBTITLE__DISABLE_PASSPHRASE),
                        _(i18n_keys.BUTTON__DISABLE),
                        self,
                        icon_path="",
                    )
        elif code == lv.EVENT.READY:
            if self.passphrase.switch.has_state(lv.STATE.CHECKED):
                self.description.set_text(
                    _(i18n_keys.CONTENT__PASSPHRASE_ENABLED__HINT)
                )
                device.set_passphrase_enabled(True)
                device.set_passphrase_always_on_device(False)
            else:
                self.description.set_text(
                    _(i18n_keys.CONTENT__PASSPHRASE_DISABLED__HINT)
                )
                device.set_passphrase_enabled(False)
        elif code == lv.EVENT.CANCEL:
            if self.passphrase.switch.has_state(lv.STATE.CHECKED):
                self.passphrase.clear_state()
            else:
                self.passphrase.add_state()


class PassphraseTipsConfirm(FullSizeWindow):
    def __init__(
        self,
        title: str,
        subtitle: str,
        confirm_text: str,
        callback_obj,
        icon_path="A:/res/warning.png",
        primary_color=lv_colors.ONEKEY_GREEN,
    ):
        super().__init__(
            title,
            subtitle,
            confirm_text,
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            icon_path=icon_path,
            anim_dir=2,
            primary_color=primary_color,
        )
        self.callback_obj = callback_obj

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            elif target == self.btn_no:
                lv.event_send(self.callback_obj, lv.EVENT.CANCEL, None)
            elif target == self.btn_yes:
                lv.event_send(self.callback_obj, lv.EVENT.READY, None)
            else:
                return
            self.show_dismiss_anim()


class CryptoScreen(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__CRYPTO), nav_back=True)

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
        self.ethereum = ListItemBtn(self.container, _(i18n_keys.TITLE__ETHEREUM))
        self.solana = ListItemBtn(self.container, _(i18n_keys.TITLE__SOLANA))
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.ethereum:
                EthereumSetting(self)
            elif target == self.solana:
                SolanaSetting(self)


class EthereumSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__ETHEREUM), nav_back=True)

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
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
                BlindSign(self, coin_type=_(i18n_keys.TITLE__ETHEREUM))


class SolanaSetting(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        super().__init__(prev_scr, title=_(i18n_keys.TITLE__SOLANA), nav_back=True)

        self.container = ContainerFlexCol(self, self.title, padding_row=2)
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
                BlindSign(self, coin_type=_(i18n_keys.TITLE__SOLANA))


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
        self.container = ContainerFlexCol(self, self.title, padding_row=2)
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


class UserGuide(Screen):
    def __init__(self, prev_scr=None):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            return
        kwargs = {
            "prev_scr": prev_scr,
            "title": _(i18n_keys.APP__USER_GUIDE),
            "nav_back": True,
        }
        super().__init__(**kwargs)

        self.container = ContainerFlexCol(self.content_area, self.title, padding_row=2)
        self.app_tutorial = ListItemBtn(
            self.container, _(i18n_keys.ITEM__ONEKEY_APP_TUTORIAL)
        )
        self.power_off = ListItemBtn(
            self.container,
            _(i18n_keys.TITLE__POWER_ON_OFF__GUIDE),
        )
        self.recovery_phrase = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__WHAT_IS_RECOVERY_PHRASE),
        )
        self.pin_protection = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__ENABLE_PIN_PROTECTION),
        )
        self.hardware_wallet = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__HOW_HARDWARE_WALLET_WORKS),
        )
        self.passphrase = ListItemBtn(
            self.container,
            _(i18n_keys.ITEM__PASSPHRASE_ACCESS_HIDDEN_WALLETS),
        )
        self.need_help = ListItemBtn(self.container, _(i18n_keys.ITEM__NEED_HELP))
        self.container.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.app_tutorial:
                from trezor.lvglui.scrs import app_guide

                app_guide.GuideAppDownload()
            elif target == self.power_off:
                PowerOnOffDetails()
            elif target == self.recovery_phrase:
                RecoveryPhraseDetails()
            elif target == self.pin_protection:
                PinProtectionDetails()
            elif target == self.hardware_wallet:
                HardwareWalletDetails()
            elif target == self.passphrase:
                PassphraseDetails()
            elif target == self.need_help:
                HelpDetails()
            else:
                if __debug__:
                    print("Unknown")

    def _load_scr(self, scr: "Screen", back: bool = False) -> None:
        lv.scr_load(scr)


class PowerOnOffDetails(FullSizeWindow):
    def __init__(self):
        super().__init__(
            None,
            None,
            cancel_text=_(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/power-on-off.png",
        )
        self.container = ContainerFlexCol(self.content_area, self.icon, pos=(0, 24))
        self.item = DisplayItem(
            self.container,
            _(i18n_keys.TITLE__POWER_ON_OFF__GUIDE),
            _(i18n_keys.SUBTITLE__POWER_ON_OFF__GUIDE),
        )
        self.item.label_top.set_style_text_color(lv_colors.WHITE, 0)
        self.item.label.set_style_text_color(lv_colors.WHITE_2, 0)
        self.item.label.set_long_mode(lv.label.LONG.WRAP)

    # def destroy(self, _delay):
    #     return self.delete()


class RecoveryPhraseDetails(FullSizeWindow):
    def __init__(self):
        super().__init__(
            None,
            None,
            cancel_text=_(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/recovery-phrase.png",
        )
        self.container = ContainerFlexCol(self.content_area, self.icon, pos=(0, 24))
        self.item = DisplayItem(
            self.container,
            _(i18n_keys.TITLE__WHAT_IS_RECOVERY_PHRASE__GUIDE),
            _(i18n_keys.SUBTITLE__WHAT_IS_RECOVERY_PHRASE__GUIDE),
        )
        self.item.label_top.set_style_text_color(lv_colors.WHITE, 0)
        self.item.label.set_style_text_color(lv_colors.WHITE_2, 0)
        self.item.label.align_to(self.item.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.item.label.set_long_mode(lv.label.LONG.WRAP)

    # def destroy(self, _delay):
    #     return self.delete()


class PinProtectionDetails(FullSizeWindow):
    def __init__(self):
        super().__init__(
            None,
            None,
            cancel_text=_(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/pin-protection.png",
        )
        self.container = ContainerFlexCol(self.content_area, self.icon, pos=(0, 24))
        self.item = DisplayItem(
            self.container,
            _(i18n_keys.TITLE__ENABLE_PIN_PROTECTION__GUIDE),
            _(i18n_keys.SUBTITLE__ENABLE_PIN_PROTECTION__GUIDE),
        )
        self.item.label_top.set_style_text_color(lv_colors.WHITE, 0)
        self.item.label.set_style_text_color(lv_colors.WHITE_2, 0)
        self.item.label.align_to(self.item.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.item.label.set_long_mode(lv.label.LONG.WRAP)

    # def destroy(self, _delay):
    #     return self.delete()


class HardwareWalletDetails(FullSizeWindow):
    def __init__(self):
        super().__init__(
            None,
            None,
            cancel_text=_(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/hardware-wallet-works-way.png",
        )
        self.container = ContainerFlexCol(self.content_area, self.icon, pos=(0, 24))
        self.item = DisplayItem(
            self.container,
            _(i18n_keys.TITLE__HOW_HARDWARE_WALLET_WORKS__GUIDE),
            _(i18n_keys.SUBTITLE__HOW_HARDWARE_WALLET_WORKS__GUIDE),
        )
        self.item.label_top.set_style_text_color(lv_colors.WHITE, 0)
        self.item.label.set_style_text_color(lv_colors.WHITE_2, 0)
        self.item.label.align_to(self.item.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.item.label.set_long_mode(lv.label.LONG.WRAP)

    # def destroy(self, _delay):
    #     return self.delete()


class PassphraseDetails(FullSizeWindow):
    def __init__(self):
        super().__init__(
            None,
            None,
            cancel_text=_(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/hidden-wallet.png",
        )
        self.container = ContainerFlexCol(self.content_area, self.icon, pos=(0, 24))
        self.item = DisplayItem(
            self.container,
            _(i18n_keys.TITLE__ACCESS_HIDDEN_WALLET),
            _(i18n_keys.SUBTITLE__PASSPHRASE_ACCESS_HIDDEN_WALLETS__GUIDE),
        )
        self.item.label_top.set_style_text_color(lv_colors.WHITE, 0)
        self.item.label.set_style_text_color(lv_colors.WHITE_2, 0)
        self.item.label.align_to(self.item.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.item.label.set_long_mode(lv.label.LONG.WRAP)

    # def destroy(self, _delay):
    #     return self.delete()


class HelpDetails(FullSizeWindow):
    def __init__(self):
        super().__init__(
            None,
            None,
            cancel_text=_(i18n_keys.BUTTON__CLOSE),
            icon_path="A:/res/onekey-help.png",
        )
        self.container = ContainerFlexCol(self.content_area, self.icon, pos=(0, 24))
        self.item = DisplayItem(
            self.container,
            _(i18n_keys.TITLE__NEED_HELP__GUIDE),
            _(i18n_keys.SUBTITLE__NEED_HELP__GUIDE),
        )
        self.item.label_top.set_style_text_color(lv_colors.WHITE, 0)
        self.item.label.set_style_text_color(lv_colors.WHITE_2, 0)
        self.item.label.set_long_mode(lv.label.LONG.WRAP)
        self.item.label.align_to(self.item.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.website = lv.label(self.content_area)
        self.website.set_style_text_font(font_PJSREG30, 0)
        self.website.set_style_text_color(lv_colors.WHITE_2, 0)
        self.website.set_style_text_line_space(3, 0)
        self.website.set_style_text_letter_space(-1, 0)
        self.website.set_text("help.onekey.so/hc")
        self.website.align_to(self.container, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 0)
        self.underline = lv.line(self.content_area)
        self.underline.set_points(
            [
                {"x": 0, "y": 2},
                {"x": 232, "y": 2},
            ],
            2,
        )
        self.underline.set_style_line_color(lv_colors.WHITE_2, 0)
        self.underline.align_to(self.website, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

    # def destroy(self, _delay):
    #     return self.delete()
