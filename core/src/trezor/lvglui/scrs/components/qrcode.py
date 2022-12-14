import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


class QRCode(lv.qrcode):
    def __init__(self, parent, data: str, icon_path=None, size: int = 440):
        bg_color = lv.color_hex(0xFFFFFF)
        fg_color = lv.color_hex(0x000000)
        super().__init__(parent, size, fg_color, bg_color)
        self.update(data, len(data))
        self.set_style_border_color(bg_color, 0)
        self.set_style_border_width(12, 0)
        self.set_style_bg_opa(0, 0)
        if icon_path:
            self.icon = lv.img(self)
            self.icon.set_src(icon_path)
            self.icon.set_align(lv.ALIGN.CENTER)
