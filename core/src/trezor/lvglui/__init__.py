from trezor import log, loop

import lvgl as lv


async def lvgl_tick():
    while True:
        lv.tick_inc(10)
        await loop.sleep(10)
        lv.timer_handler()


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
    indev_drv.long_press_time = 2000
    indev_drv.register()


try:
    init()
    if __debug__:
        log.info("init", "initialized successfully")
except:
    if __debug__:
        log.error("init", "failed to initialize emulator")


class StatusBar:
    _instance = None

    @classmethod
    def get_instance(cls):
        if cls._instance is None:
            cls._instance = StatusBar()
        return cls._instance

    def __init__(self):
        self.ble = lv.img(lv.layer_top())
        self.ble.set_src("A:/res/Bluetooth.png")
        self.ble.align(lv.ALIGN.TOP_RIGHT, -30, 5)
        # ble.set_zoom(255)
        self.battery = lv.img(lv.layer_top())
        self.battery.set_src("A:/res/Status=50%.png")
        self.battery.align(lv.ALIGN.TOP_RIGHT, -5, 5)
        # battery.set_zoom(255)

    def set_ble_img(self, path):
        self.ble.set_src(path)

    def set_battery_img(self, path):
        self.battery.set_src(path)
