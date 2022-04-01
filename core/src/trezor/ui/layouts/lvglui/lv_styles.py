import lvgl as lv


class ButtonStyle(lv.style_t):
    def __init__(self, bg_color, text_color):
        super().__init__()
        self.set_bg_color(bg_color)
        self.set_border_width(0)
        self.set_shadow_width(0)
        self.set_text_color(text_color)
