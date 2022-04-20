import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


class QRCode(lv.qrcode):
    def __init__(self, parent, data: str, size: int = 240):
        bg_color = lv.color_hex(0xFFFFFF)
        fg_color = lv.color_hex(0x000000)
        super().__init__(parent, size, fg_color, bg_color)
        self.update(data, len(data))
        self.set_style_border_color(bg_color, 0)
        self.set_style_border_width(5, 0)
        self.align(lv.ALIGN.TOP_MID, 0, 181)
