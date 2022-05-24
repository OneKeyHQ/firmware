from trezor import io, log, loop, utils

import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


async def lvgl_tick():
    while True:
        lv.tick_inc(10)
        await loop.sleep(10)
        lv.timer_handler()


def init_lvgl() -> None:
    import lvgldrv as lcd  # type: ignore[Import "lvgldrv" could not be resolved]

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


def init_theme() -> None:
    dispp = lv.disp_get_default()
    theme = lv.theme_default_init(
        dispp,
        lv.palette_main(lv.PALETTE.BLUE),
        lv.palette_main(lv.PALETTE.RED),
        True,
        lv.font_default(),
    )
    dispp.set_theme(theme)


def init_file_system() -> None:
    if not utils.EMULATOR:
        io.fatfs.mount()


def get_elapsed() -> int:
    """Get elapsed time since last user activity(e.g. click).
    @return: elapsed time in milliseconds
    """
    disp = lv.disp_get_default()
    return disp.get_inactive_time()


try:
    init_file_system()
    init_lvgl()
    init_theme()
    if __debug__:
        log.info("init", "initialized successfully")
except BaseException:
    if __debug__:
        log.error("init", "failed to initialize emulator")


class StatusBar:
    _instance = None

    @classmethod
    def get_instance(cls) -> "StatusBar":
        if cls._instance is None:
            cls._instance = StatusBar()
        return cls._instance

    def __init__(self):
        self.ble = lv.img(lv.layer_top())
        self.ble.set_src("A:/res/Bluetooth.png")
        self.ble.align(lv.ALIGN.TOP_RIGHT, -30, 5)
        self.ble.add_flag(lv.obj.FLAG.HIDDEN)
        self.battery = lv.img(lv.layer_top())
        self.battery.set_src("A:/res/Status=50%.png")
        self.battery.align(lv.ALIGN.TOP_RIGHT, -5, 5)
        self.usb = lv.img(lv.layer_top())
        self.usb.set_src("A:/res/usb.png")
        self.usb.add_flag(lv.obj.FLAG.HIDDEN)
        self.usb.align(lv.ALIGN.TOP_RIGHT, -55, 5)

    def show_ble(self, show: bool = False):
        if show:
            if self.ble.has_flag(lv.obj.FLAG.HIDDEN):
                self.ble.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            if not self.ble.has_flag(lv.obj.FLAG.HIDDEN):
                self.ble.add_flag(lv.obj.FLAG.HIDDEN)

    def show_usb(self, show: bool = False):
        if show:
            if self.usb.has_flag(lv.obj.FLAG.HIDDEN):
                self.usb.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            if not self.usb.has_flag(lv.obj.FLAG.HIDDEN):
                self.usb.add_flag(lv.obj.FLAG.HIDDEN)

    def set_battery_img(self, value: int):
        icon_path = retrieve_icon_path(value)
        self.battery.set_src(icon_path)


def retrieve_icon_path(value: int) -> str:
    if value == 101:
        return "A:/res/Status=charging.png"
    elif value >= 85:
        return "A:/res/Status=100%.png"
    elif value >= 75:
        return "A:/res/Status=75%.png"
    elif value >= 50:
        return "A:/res/Status=50%.png"
    elif value >= 25:
        return "A:/res/Status=25%.png"
    else:
        return "A:/res/Status=0%.png"
