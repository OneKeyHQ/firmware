from trezor import log

import lvgl as lv


def init():
    import lvgldrv as lcd

    lv.init()
    disp_buf1 = lv.disp_draw_buf_t()
    buf1_1 = lcd.framebuffer(1)
    disp_buf1.init(buf1_1, None, len(buf1_1) // lv.color_t.__SIZE__)
    disp_drv = lv.disp_drv_t()
    disp_drv.init()
    disp_drv.draw_buf = disp_buf1
    disp_drv.flush_cb = lcd.flush
    disp_drv.hor_res = 480
    disp_drv.ver_res = 800
    disp_drv.register()

    indev_drv = lv.indev_drv_t()
    indev_drv.init()
    indev_drv.type = lv.INDEV_TYPE.POINTER
    indev_drv.read_cb = lcd.ts_read
    indev_drv.register()
    log.info("init", "initialized successfully")


try:
    init()
except:
    log.error("init", "failed to initialize emulator")

dispp = lv.disp_get_default()
theme = lv.theme_default_init(
    dispp,
    lv.palette_main(lv.PALETTE.BLUE),
    lv.palette_main(lv.PALETTE.RED),
    True,
    lv.font_default(),
)
dispp.set_theme(theme)
font_PJSBOLD36 = lv.font_load("A:/res/ui_font_PJSBOLD36.bin")
font_PJSBOLD24 = lv.font_load("A:/res/ui_font_PJSBOLD24.bin")
font_PJSMID20 = lv.font_load("A:/res/ui_font_PJSMID20.bin")
font_MONO20 = lv.font_load("A:/res/ui_font_MONO20.bin")
font_PJSBOLD32 = lv.font_load("A:/res/ui_font_PJSBOLD32.bin")
font_PJSBOLD16 = lv.font_load("A:/res/ui_font_PJSBOLD16.bin")
font_MONO24 = lv.font_load("A:/res/ui_font_MONO24.bin")


class StatusBar:
    _instance = None

    @classmethod
    def get_instance(cls):
        if cls._instance is None:
            cls._instance = StatusBar()
        return cls._instance

    def __init__(self):
        ble = lv.img(lv.layer_top())
        ble.set_src("A:/res/Bluetooth.png")
        ble.set_width(lv.SIZE.CONTENT)  # 0
        ble.set_height(lv.SIZE.CONTENT)  # 0
        ble.set_x(-30)
        ble.set_y(5)
        ble.set_align(lv.ALIGN.TOP_RIGHT)
        ble.set_pivot(0, 0)
        ble.set_angle(0)
        ble.set_zoom(255)
        battery = lv.img(lv.layer_top())
        battery.set_src("A:/res/Status=50%.png")
        battery.set_width(lv.SIZE.CONTENT)  # 96
        battery.set_height(lv.SIZE.CONTENT)  # 96
        battery.set_x(-5)
        battery.set_y(5)
        battery.set_align(lv.ALIGN.TOP_RIGHT)
        battery.set_pivot(0, 0)
        battery.set_angle(0)
        battery.set_zoom(255)

    def set_ble_img(self, path):
        self.ble.set_src(path)

    def set_battery_img(self, path):
        self.battery.set_src(path)
