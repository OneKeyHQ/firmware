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
        self.offset_x = 0
        self.ble = lv.img(lv.layer_top())
        self.ble.set_src("A:/res/Bluetooth.png")
        self.ble.align(lv.ALIGN.TOP_RIGHT, -32, 0)
        self.ble.add_flag(lv.obj.FLAG.HIDDEN)
        self.battery = lv.img(lv.layer_top())
        self.battery.add_flag(lv.obj.FLAG.HIDDEN)
        self.battery.set_src("A:/res/Status=100%.png")
        self.battery.align(lv.ALIGN.TOP_RIGHT, -2, 0)
        self.usb = lv.img(lv.layer_top())
        self.usb.set_src("A:/res/usb.png")
        self.usb.add_flag(lv.obj.FLAG.HIDDEN)
        self.usb.align(lv.ALIGN.TOP_RIGHT, -56, 0)
        self.charging = lv.img(lv.layer_top())
        self.charging.set_src("A:/res/Status=charging.png")
        self.charging.add_flag(lv.obj.FLAG.HIDDEN)
        self.charging.align(lv.ALIGN.TOP_RIGHT, 0, 0)

        from .scrs import font_STATUS_BAR

        self.battery_cap = lv.label(lv.layer_top())
        self.battery_cap.set_style_text_font(
            font_STATUS_BAR, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.battery_cap.add_flag(lv.obj.FLAG.HIDDEN)
        self.battery_cap.align(lv.ALIGN.TOP_RIGHT, -56, 8)
        self.battery_cap.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )

    def show_ble(self, status: int):
        if self.ble.has_flag(lv.obj.FLAG.HIDDEN):
            self.ble.clear_flag(lv.obj.FLAG.HIDDEN)
        if status == 1:
            self.ble.set_src("A:/res/Bluetooth_connected.png")
        elif status in (2, 3):
            self.ble.set_src("A:/res/Bluetooth.png")
        elif status == 4:
            self.ble.set_src("A:/res/Bluetooth_closed.png")

        if self.charging.has_flag(lv.obj.FLAG.HIDDEN):
            self.ble.align(lv.ALIGN.TOP_RIGHT, -32, 0)
        else:
            self.ble.align(lv.ALIGN.TOP_RIGHT, -56 - self.offset_x, 0)

    def show_usb(self, show: bool = False):
        if show:
            if self.usb.has_flag(lv.obj.FLAG.HIDDEN):
                self.usb.clear_flag(lv.obj.FLAG.HIDDEN)
                if self.charging.has_flag(lv.obj.FLAG.HIDDEN):
                    self.usb.align(lv.ALIGN.TOP_RIGHT, -56, 0)
                else:
                    self.usb.align(lv.ALIGN.TOP_RIGHT, -80 - self.offset_x, 0)
        else:
            if not self.usb.has_flag(lv.obj.FLAG.HIDDEN):
                self.usb.add_flag(lv.obj.FLAG.HIDDEN)

    def set_battery_img(self, value: int):
        if self.battery.has_flag(lv.obj.FLAG.HIDDEN):
                self.battery.clear_flag(lv.obj.FLAG.HIDDEN)
        if self.charging.has_flag(lv.obj.FLAG.HIDDEN):
            icon_path = retrieve_icon_path(False, value)
            self.battery.align(lv.ALIGN.TOP_RIGHT, -2, 0)
            self.battery_cap.add_flag(lv.obj.FLAG.HIDDEN)
            self.offset_x = 0
        else:
            icon_path = retrieve_icon_path(True, value)
            self.battery.align(lv.ALIGN.TOP_RIGHT, -24, 0)
            self.battery_cap.clear_flag(lv.obj.FLAG.HIDDEN)
            self.battery_cap.set_text(str(value) + "%")
            pos = lv.point_t()
            self.battery_cap.get_letter_pos(len(str(value) + "%"), pos)
            self.offset_x = pos.x
            if not self.ble.has_flag(lv.obj.FLAG.HIDDEN):
                self.ble.align(lv.ALIGN.TOP_RIGHT, -56 - self.offset_x, 0)
            if not self.usb.has_flag(lv.obj.FLAG.HIDDEN):
                self.usb.align(lv.ALIGN.TOP_RIGHT, -80 - self.offset_x, 0)
        self.battery.set_src(icon_path)

    def show_charging(self, show: bool = False):
        if show:
            if self.charging.has_flag(lv.obj.FLAG.HIDDEN):
                self.charging.clear_flag(lv.obj.FLAG.HIDDEN)

        else:
            if not self.charging.has_flag(lv.obj.FLAG.HIDDEN):
                self.charging.add_flag(lv.obj.FLAG.HIDDEN)
            if not self.ble.has_flag(lv.obj.FLAG.HIDDEN):
                self.ble.align(lv.ALIGN.TOP_RIGHT, -32, 0)
            if not self.usb.has_flag(lv.obj.FLAG.HIDDEN):
                self.usb.align(lv.ALIGN.TOP_RIGHT, -56, 0)
            self.battery.align(lv.ALIGN.TOP_RIGHT, -2, 0)


def retrieve_icon_path(charging: bool, value: int) -> str:

    if charging:
        if value >= 80:
            return "A:/res/Charging=100%.png"
        elif value >= 60:
            return "A:/res/Charging=80%.png"
        elif value >= 40:
            return "A:/res/Charging=60%.png"
        elif value >= 20:
            return "A:/res/Charging=40%.png"
        elif value >= 5:
            return "A:/res/Charging=20%.png"
        else:
            return "A:/res/Charging=5%.png"
    else:
        if value >= 80:
            return "A:/res/Status=100%.png"
        elif value >= 60:
            return "A:/res/Status=80%.png"
        elif value >= 40:
            return "A:/res/Status=60%.png"
        elif value >= 20:
            return "A:/res/Status=40%.png"
        elif value >= 5:
            return "A:/res/Status=20%.png"
        else:
            return "A:/res/Status=5%.png"
