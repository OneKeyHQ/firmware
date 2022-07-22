from .common import Screen, lv, lv_colors  # noqa: F401,F403, F405


class BootScreen(Screen):
    def __init__(self):
        super().__init__()
        self.img = lv.img(self)
        self.img.set_src("A:/res/logo-white.png")
        self.img.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 72
        self.img.align(lv.ALIGN.CENTER, 0, -96)

        self.bar = lv.bar(self)
        self.bar.set_size(288, 8)
        self.bar.align(lv.ALIGN.CENTER, 0, 236)
        self.bar.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.bar.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.bar.set_style_border_color(
            lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.bar.set_style_border_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.bar.set_style_border_width(1, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.bar.set_style_bg_color(
            lv_colors.WHITE, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.bar.set_style_bg_opa(255, lv.PART.INDICATOR | lv.STATE.DEFAULT)
        self.bar.set_style_anim_time(200, lv.PART.MAIN)
        self.bar.set_value(100, lv.ANIM.ON)
        # self.add_event_cb(self.eventhandler, lv.EVENT.SCREEN_LOADED, None)

    # def eventhandler(self, event_obj):
    #     code = event_obj.code
    #     if code == lv.EVENT.SCREEN_LOADED:
    #         print(f'draw end')
    #         self.channel.publish("boot_screen_done")
    #     else:
    #         print(f'clicked boot')
