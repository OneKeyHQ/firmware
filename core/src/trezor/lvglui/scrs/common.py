from typing import TYPE_CHECKING

from trezor import loop, utils

import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]

from ..lv_colors import lv_colors
from .components import slider
from .components.button import NormalButton
from .components.label import SubTitle, Title
from .components.roller import Roller

if TYPE_CHECKING:
    from typing import Any

    pass


class Screen(lv.obj):
    """Singleton screen object."""

    def __init__(self, prev_scr=None, **kwargs):
        super().__init__()
        self.prev_scr = prev_scr or lv.scr_act()
        self.channel = loop.chan()
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        # icon
        if "icon_path" in kwargs:
            self.icon = lv.img(self)
            self.icon.set_src(kwargs["icon_path"])
            self.icon.align(lv.ALIGN.TOP_MID, 0, 68)
        # title
        if "title" in kwargs:
            self.title = Title(self, None, 452, (), kwargs["title"])
            if kwargs.get("icon_path"):
                self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
        # subtitle
        if "subtitle" in kwargs:
            self.subtitle = SubTitle(self, self.title, 452, (0, 32), kwargs["subtitle"])
        # roller
        if "options" in kwargs:
            self.roller = Roller(self, kwargs["options"])
            self.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
            self.select_option = kwargs["options"].split()[1]
        # btn
        if "btn_text" in kwargs:
            self.btn = NormalButton(self, kwargs["btn_text"])
            self.btn.enable(lv_colors.ONEKEY_GREEN)
            # self.add_event_cb(
            #     self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.PRESSED, None
            # )
        # nav_back
        if kwargs.get("nav_back", False):
            self.nav_back = lv.imgbtn(self)
            self.nav_back.set_size(48, 48)
            self.nav_back.set_pos(24, 92)
            self.nav_back.set_ext_click_area(100)
            self.nav_back.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
            self.nav_back.set_style_bg_img_src(
                "A:/res/nav-back.png", lv.PART.MAIN | lv.STATE.DEFAULT
            )
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

        self.load_screen(self)

    # event callback
    def eventhandler(self, event_obj):
        event = event_obj.code
        target = event_obj.get_target()
        if event == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if isinstance(target, lv.imgbtn):
                if target == self.nav_back:
                    if self.prev_scr is not None:
                        self.load_screen(self.prev_scr, destroy_self=True)
            else:
                if hasattr(self, "btn") and target == self.btn:
                    self.on_click(target)
        elif event == lv.EVENT.VALUE_CHANGED:
            self.on_value_changed(target)

    # click event callback
    def on_click(self, event_obj):
        pass

    # value changed callback
    def on_value_changed(self, event_obj):
        pass

    async def request(self) -> Any:
        return await self.channel.take()

    # NOTE:====================Functional Code Don't Edit========================

    def __new__(cls, pre_scr=None, *args, **kwargs):
        if not hasattr(cls, "_instance"):
            cls._instance = super(lv.obj, cls).__new__(cls)
            utils.SCREENS.append(cls._instance)
        return cls._instance

    def load_screen(self, scr, destroy_self: bool = False):
        if destroy_self:
            load_scr_with_animation(scr.__class__(), back=True)
            utils.SCREENS.remove(self)
            self.del_delayed(1000)
            if hasattr(self, "_init"):
                del self._init
            del self.__class__._instance
        else:
            load_scr_with_animation(scr)

    def __del__(self):
        """Micropython doesn't support user defined __del__ now, so this not work at all."""
        try:
            self.delete()
        except BaseException:
            pass

    # NOTE:====================Functional Code Don't Edit========================


class FullSizeWindow(lv.obj):
    """Generic screen contains a title, a subtitle, and one or two button and a optional icon."""

    def __init__(
        self,
        title: str | None,
        subtitle: str | None,
        confirm_text: str = "",
        cancel_text: str = "",
        icon_path: str | None = None,
        options: str | None = None,
        hold_confirm: bool = False,
        top_layer: bool = False,
        auto_close: bool = False,
    ):
        if top_layer:
            super().__init__(lv.layer_top())
        else:
            super().__init__(lv.scr_act())

        self.channel = loop.chan()
        self.set_size(lv.pct(100), lv.pct(100))
        self.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_all(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.hold_confirm = hold_confirm  # and not utils.EMULATOR
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
        if icon_path:
            self.icon = lv.img(self.content_area)
            self.icon.set_src(icon_path)
            self.icon.align(lv.ALIGN.TOP_MID, 0, 24)
        if title:
            self.title = Title(self.content_area, None, 452, (), title, pos_y=48)
            if icon_path:
                self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
            if subtitle is not None:
                self.subtitle = SubTitle(
                    self.content_area, self.title, 452, (0, 24), subtitle
                )
        else:
            self.icon.align(lv.ALIGN.TOP_MID, 0, 0)

        if options:
            self.roller = Roller(self, options)
            self.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
            self.select_option = options.split()[1]

        self.content_area.set_style_max_height(646, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.content_area.set_style_min_height(400, lv.PART.MAIN | lv.STATE.DEFAULT)
        if cancel_text:
            self.btn_no = NormalButton(self, cancel_text)
            if confirm_text:
                if not self.hold_confirm:
                    self.btn_no.set_size(192, 76)
                    self.btn_no.align(lv.ALIGN.BOTTOM_LEFT, 32, -18)
                else:
                    self.btn_no.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
                    self.btn_no.align(lv.ALIGN.BOTTOM_LEFT, 32, -8)
                self.btn_no.enable()
            # self.btn_no.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        if confirm_text:
            if cancel_text:
                if self.hold_confirm:
                    self.content_area.set_style_max_height(
                        550, lv.PART.MAIN | lv.STATE.DEFAULT
                    )
                    self.slider = slider.Slider(self, confirm_text, relative_y=-88)
                else:
                    self.btn_yes = NormalButton(self, confirm_text)
                    self.btn_yes.set_size(192, 76)
                    self.btn_yes.align_to(self, lv.ALIGN.BOTTOM_RIGHT, -32, -18)
            else:
                self.btn_yes = NormalButton(self, confirm_text)
            if self.hold_confirm:
                self.slider.add_event_cb(self.eventhandler, lv.EVENT.READY, None)
            else:
                self.btn_yes.enable(lv_colors.ONEKEY_GREEN)
                # self.btn_yes.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        if auto_close:
            self.destroy(delay_ms=10 * 1000)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.roller:
                selected_str = " " * 11
                target.get_selected_str(selected_str, len(selected_str))
                self.select_option = selected_str.strip()[:-1]
            return
        elif code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if hasattr(self, "btn_no") and target == self.btn_no:
                self.channel.publish(0)
                self.destroy(100)
                return
            elif hasattr(self, "btn_yes") and target == self.btn_yes:
                if hasattr(self, "roller"):
                    self.channel.publish(self.select_option)
                else:
                    if not self.hold_confirm:
                        # delete the confirm button manually to fix a display bug in reset process with desktop
                        # self.btn_yes.del_delayed(10)
                        self.channel.publish(1)
                    else:
                        return
            else:
                return
        elif code == lv.EVENT.READY and self.hold_confirm:
            if target == self.slider:
                self.channel.publish(1)
            else:
                return
        self.destroy()

    async def request(self) -> Any:
        return await self.channel.take()

    def destroy(self, delay_ms=400):
        self.del_delayed(delay_ms)


def load_scr_with_animation(scr: Screen, back: bool = False) -> None:
    """Load a screen with animation."""
    # TODO: FIX ANIMATION LOAD
    # lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.OVER_LEFT if back else lv.SCR_LOAD_ANIM.OVER_RIGHT, 100, 0, False)
    lv.scr_load(scr)
