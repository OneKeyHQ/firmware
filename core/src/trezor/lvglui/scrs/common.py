
from typing import Any
from trezor import loop, utils
from . import *

class Screen(lv.obj):
    """Singleton screen object."""

    def __init__(self, prev_scr=None, **kwargs):
        super().__init__()
        self.prev_scr = prev_scr
        self.channel = loop.chan()
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        # title
        if 'title' in kwargs:
            self.title = lv.label(self)
            self.title.set_long_mode(lv.label.LONG.WRAP)
            self.title.set_text(kwargs['title'])
            self.title.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
            title_pos = kwargs.get('title_pos', (lv.pct(0), lv.pct(17)))
            self.title.set_pos(title_pos[0], title_pos[1])
            title_align = kwargs.get('title_align', lv.ALIGN.TOP_MID)
            self.title.set_align(title_align)
            self.title.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT)
            self.title.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)

        # subtitle
        if 'subtitle' in kwargs:
            pass
        # roller
        if 'roller' in kwargs:
            pass
        # btn
        if 'btn' in kwargs:
            pass
        # nav_back
        if 'nav_back' in kwargs:
            self.nav_back = lv.imgbtn(self)
            self.nav_back.set_src(lv.imgbtn.STATE.RELEASED, None, None, None)
            self.nav_back.set_size(48, 48)
            self.nav_back.set_pos(lv.pct(1), lv.pct(7))
            self.nav_back.add_event_cb(self.eventhandler, lv.EVENT.CLICKED | lv.EVENT.PRESSED , None)
            self.img_back = lv.img(self.nav_back)
            self.img_back.set_src('A:/res/nav-arrow-left.png')
            self.img_back.set_align(lv.ALIGN.CENTER)
    # event callback
    def eventhandler(self, event_obj):
        event = event_obj.code
        target = event_obj.get_target()
        if event == lv.EVENT.CLICKED | lv.EVENT.PRESSED:
            if isinstance(target, lv.imgbtn):
                if target == self.nav_back:
                    if self.prev_scr is not None:
                        self.load_screen(self.prev_scr, destory_self=True)
            else:
                if target == self.btn:
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

    def set_subtitle(self, text: str):
        self.subtitle.set_text(text)
    #NOTE:====================Functional Code Don't Edit========================

    def __new__(cls, pre_scr=None, **kwargs):
        if not hasattr(cls, '_instance'):
            cls._instance = super(lv.obj, cls).__new__(cls)
            utils.SCREENS.append(cls._instance)
        return cls._instance

    def load_screen(self, scr, destory_self: bool=False):
        load_scr_with_animation(scr)
        if destory_self:
            utils.SCREENS.remove(self)
            self.del_delayed(1000)
            del self.__class__._instance
            del self._init

    def __del__(self):
        """Micropython doesn't support user defined __del__ now, so this not work at all."""
        try:
            self.delete()
        except BaseException as e:
            pass
    #NOTE:====================Functional Code Don't Edit========================


class ScreenGeneric(Screen):
    """Generic screen contains a title, a subtitle, and one or two button and a optional icon."""
    def __init__(
        self,
        title: str,
        description: str,
        confirm_text: str,
        cancel_btn: bool =False,
        icon_path: str | None =None,
    ):
        super().__init__()
        if icon_path:
            self.icon = lv.img(self)
            self.icon.set_src(icon_path)
            self.icon.align(lv.ALIGN.TOP_MID, 0, -140)
        self.title = lv.label(self)
        self.title.set_long_mode(lv.label.LONG.WRAP)
        self.title.set_text(title)
        self.title.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.title.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.title.set_style_text_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)
        if icon_path:
            self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_MID, 0, -32)
        else:
            self.title.align(lv.ALIGN.TOP_MID, 0, -140)
        self.subtitle = lv.label(self)
        self.subtitle.set_long_mode(lv.label.LONG.WRAP)
        self.subtitle.set_text(description)
        self.subtitle.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_MID, 0, -24)
        self.subtitle.set_style_text_color(lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.subtitle.set_style_text_font(font_PJSBOLD16, lv.PART.MAIN | lv.STATE.DEFAULT)

        if cancel_btn:
            self.btn_cancel = lv.btn(self)
            self.btn_cancel.set_size(192, 62)
            self.btn_cancel.set_pos(32, -64)
            self.btn_cancel.set_align(lv.ALIGN.BOTTOM_LEFT)
            self.btn_cancel.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.btn_cancel.set_style_bg_color(lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT)

            self.btn_cancel_label = lv.label(self.btn_cancel)
            self.btn_cancel_label.set_text("Cancel")
            self.btn_cancel_label.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
            self.btn_cancel_label.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT)
            self.btn_cancel_label.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.btn_cancel_label.set_align(lv.ALIGN.CENTER)

            self.btn_cancel.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

        self.btn_confirm = lv.btn(self)
        if cancel_btn:
            self.btn_confirm.set_size(192, 62)
            self.btn_confirm.align_to(self, lv.ALIGN.BOTTOM_RIGHT, -32, -64)
        else:
            self.btn_confirm.set_size(320, 62)
            self.btn_confirm.align_to(self, lv.ALIGN.BOTTOM_MID, 0, -64)

        self.btn_confirm.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btn_confirm.set_style_bg_color(lv.color_hex(0x1B7735), lv.PART.MAIN | lv.STATE.DEFAULT)

        self.label_confirm = lv.label(self.btn_confirm)
        self.label_confirm.set_long_mode(lv.label.LONG.WRAP)
        self.label_confirm.set_text(confirm_text)
        self.label_confirm.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
        self.label_confirm.set_style_text_color(lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label_confirm.set_style_text_font(lv.font_montserrat_32, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label_confirm.set_align(lv.ALIGN.CENTER)

        self.btn_confirm.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

        load_scr_with_animation(self)

    def eventhandler(self, obj, event):
        if event == lv.EVENT.CLICKED:
            if obj == self.btn_cancel:
                self.channel.publish(0)
            elif obj == self.btn_confirm:
                self.channel.publish(1)
            else:
                pass
def load_scr_with_animation(scr: Screen) -> None:
    """Load a screen with animation."""
    #TODO: FIX ANIMATION LOAD
    # lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.OVER_LEFT, 100, 0, False)
    lv.scr_load(scr)
