from trezor import workflow

from apps.base import unlock_device

from .common import *


class LockScreen(Screen):
    def __init__(self, device_name="OneKey Touch"):
        if not hasattr(self, "_init"):
            self._init = True
        else:
            load_scr_with_animation(self)
            return
        super().__init__()
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_img_src(
            "A:/res/wallpaper_dark.png", lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_bg_img_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.tap_tip = lv.label(self)
        self.tap_tip.set_long_mode(lv.label.LONG.WRAP)
        self.tap_tip.set_text("Tap to Unlock")
        self.tap_tip.set_width(lv.SIZE.CONTENT)  # 1
        self.tap_tip.set_height(lv.SIZE.CONTENT)  # 1
        self.tap_tip.set_x(0)
        self.tap_tip.set_y(-100)
        self.tap_tip.set_align(lv.ALIGN.BOTTOM_MID)
        self.tap_tip.set_style_text_font(
            font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
        )

        self.title = lv.label(self)
        self.title.set_long_mode(lv.label.LONG.WRAP)
        self.title.set_text(device_name)
        self.title.set_width(lv.SIZE.CONTENT)  # 1
        self.title.set_height(lv.SIZE.CONTENT)  # 1
        self.title.set_x(0)
        self.title.set_y(-200)
        self.title.set_align(lv.ALIGN.CENTER)
        self.title.set_style_text_font(font_PJSBOLD36, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        load_scr_with_animation(self)

    def eventhandler(self, event_obj: lv.event_t):
        # target = event_obj.get_target()
        code = event_obj.code
        # dir = lv.indev_t.get_gesture_dir(lv.indev_get_act())
        # if dir == lv.DIR.TOP:
        #     print('top')
        if code == lv.EVENT.CLICKED:
            if self.channel.takers:
                self.channel.publish("clicked")
            else:
                workflow.spawn(unlock_device())
