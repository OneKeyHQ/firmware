from .. import lv


class StyleWrapper(lv.style_t):
    def __init__(self):
        super().__init__()

    def radius(self, radius) -> "StyleWrapper":
        self.set_radius(radius)
        return self

    def align(self, align) -> "StyleWrapper":
        self.set_align(align)
        return self

    def text_color(self, text_color) -> "StyleWrapper":
        self.set_text_color(text_color)
        return self

    def text_opa(self, text_opa) -> "StyleWrapper":
        self.set_text_opa(text_opa)
        return self

    def text_align(self, text_align) -> "StyleWrapper":
        self.set_text_align(text_align)
        return self

    def text_align_right(self) -> "StyleWrapper":
        self.set_text_align(lv.TEXT_ALIGN.RIGHT)
        return self

    def text_align_left(self) -> "StyleWrapper":
        self.set_text_align(lv.TEXT_ALIGN.LEFT)
        return self

    def text_align_center(self) -> "StyleWrapper":
        self.set_text_align(lv.TEXT_ALIGN.CENTER)
        return self

    def text_font(self, text_font) -> "StyleWrapper":
        self.set_text_font(text_font)
        return self

    def bg_color(self, bg_color) -> "StyleWrapper":
        self.set_bg_color(bg_color)
        return self

    def bg_opa(self, bg_opa=lv.OPA.COVER) -> "StyleWrapper":
        self.set_bg_opa(bg_opa)
        return self

    def text_line_space(self, line_space) -> "StyleWrapper":
        self.set_text_line_space(line_space)
        return self

    def text_letter_space(self, letter_space) -> "StyleWrapper":
        self.set_text_letter_space(letter_space)
        return self

    def pad_all(self, pad) -> "StyleWrapper":
        self.set_pad_all(pad)
        return self

    def pad_hor(self, pad) -> "StyleWrapper":
        self.set_pad_hor(pad)
        return self

    def pad_ver(self, pad) -> "StyleWrapper":
        self.set_pad_ver(pad)
        return self

    def pad_gap(self, gap) -> "StyleWrapper":
        """pad row + pad column"""
        self.set_pad_gap(gap)
        return self

    def pad_row(self, pad) -> "StyleWrapper":
        self.set_pad_row(pad)
        return self

    def pad_column(self, pad) -> "StyleWrapper":
        self.set_pad_column(pad)
        return self

    def pad_top(self, pad) -> "StyleWrapper":
        self.set_pad_top(pad)
        return self

    def pad_bottom(self, pad) -> "StyleWrapper":
        self.set_pad_bottom(pad)
        return self

    def pad_left(self, pad) -> "StyleWrapper":
        self.set_pad_left(pad)
        return self

    def pad_right(self, pad) -> "StyleWrapper":
        self.set_pad_right(pad)
        return self

    def transform_height(self, height) -> "StyleWrapper":
        self.set_transform_height(height)
        return self

    def transform_width(self, width) -> "StyleWrapper":
        self.set_transform_width(width)
        return self

    def transform_zoom(self, zoom) -> "StyleWrapper":
        self.set_transform_zoom(zoom)
        return self

    def translate_y(self, y) -> "StyleWrapper":
        self.set_translate_y(y)
        return self

    def translate_x(self, x) -> "StyleWrapper":
        self.set_translate_x(x)
        return self

    def bg_img_src(self, src) -> "StyleWrapper":
        self.set_bg_img_src(src)
        return self

    def bg_img_opa(self, opa) -> "StyleWrapper":
        self.set_bg_img_opa(opa)
        return self

    def bg_img_recolor(self, color) -> "StyleWrapper":
        self.set_bg_img_recolor(color)
        return self

    def bg_img_recolor_opa(self, opa) -> "StyleWrapper":
        self.set_bg_img_recolor_opa(opa)
        return self

    def border_width(self, width) -> "StyleWrapper":
        self.set_border_width(width)
        return self

    def max_height(self, height) -> "StyleWrapper":
        self.set_max_height(height)
        return self

    def max_width(self, width) -> "StyleWrapper":
        self.set_max_width(width)
        return self

    def min_width(self, width) -> "StyleWrapper":
        self.set_min_width(width)
        return self

    def min_height(self, height) -> "StyleWrapper":
        self.set_min_height(height)
        return self

    def transition(self, transition) -> "StyleWrapper":
        self.set_transition(transition)
        return self

    def bg_grad_color(self, color) -> "StyleWrapper":
        self.set_bg_grad_color(color)
        return self

    def bg_grad_dir(self, dir) -> "StyleWrapper":
        self.set_bg_grad_dir(dir)
        return self

    def bg_main_stop(self, value) -> "StyleWrapper":
        self.set_bg_main_stop(value)
        return self

    def bg_grad_stop(self, value) -> "StyleWrapper":
        self.set_bg_grad_stop(value)
        return self

    def border_color(self, color) -> "StyleWrapper":
        self.set_border_color(color)
        return self

    def border_opa(self, opa=lv.OPA.COVER) -> "StyleWrapper":
        self.set_border_opa(opa)
        return self

    def width(self, width) -> "StyleWrapper":
        self.set_width(width)
        return self

    def height(self, height) -> "StyleWrapper":
        self.set_height(height)
        return self

    def grid_column_dsc_array(self, arr) -> "StyleWrapper":
        self.set_grid_column_dsc_array(arr)
        return self

    def grid_row_dsc_array(self, arr) -> "StyleWrapper":
        self.set_grid_row_dsc_array(arr)
        return self

    # def size(self, width=lv.pct(100), height=lv.SIZE.CONTENT) -> "StyleWrapper":
    #     self.set_size(width, height)
    #     return self
