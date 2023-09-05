from .. import font_MONO28, font_PJSBOLD30, font_PJSMID30, font_PJSREG30, lv, lv_colors
from ..widgets.style import StyleWrapper


class ListItemWithLeadingCheckbox(lv.obj):
    def __init__(self, parent, text):
        super().__init__(parent)
        self.remove_style_all()
        self.set_size(464, lv.SIZE.CONTENT)
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK_4)
            .bg_opa(lv.OPA.COVER)
            .min_height(94)
            .radius(0)
            .border_width(1)
            .border_color(lv_colors.ONEKEY_GRAY_2)
            .pad_all(8)
            .text_color(lv_colors.WHITE_1)
            .text_font(font_PJSMID30)
            .text_letter_space(-1),
            0,
        )
        self.checkbox = lv.checkbox(self)
        self.checkbox.set_align(lv.ALIGN.TOP_LEFT)
        self.checkbox.set_text("")
        self.checkbox.add_style(
            StyleWrapper()
            .pad_all(0)
            .text_align(lv.TEXT_ALIGN.LEFT)
            .text_color(lv_colors.WHITE_1)
            .text_line_space(4),
            0,
        )
        self.checkbox.add_style(
            StyleWrapper()
            .radius(0)
            .pad_all(0)
            .bg_color(lv_colors.ONEKEY_BLACK_4)
            .border_color(lv_colors.ONEKEY_GRAY)
            .border_width(2)
            .border_opa(lv.OPA.COVER),
            lv.PART.INDICATOR | lv.STATE.DEFAULT,
        )
        self.checkbox.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_GREEN)
            .text_color(lv_colors.BLACK)
            .text_font(font_MONO28)
            .text_align(lv.TEXT_ALIGN.CENTER)
            .border_width(0)
            .bg_opa(),
            lv.PART.INDICATOR | lv.STATE.CHECKED,
        )
        self.checkbox.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.label = lv.label(self)
        self.label.remove_style_all()
        self.label.set_long_mode(lv.label.LONG.WRAP)
        self.label.set_size(396, lv.SIZE.CONTENT)
        self.label.align_to(self.checkbox, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)
        self.label.set_text(text)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE | lv.obj.FLAG.CLICKABLE)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event):
        code = event.code
        target = event.get_target()
        # if target == self.checkbox ignore instead. because value_change event is also triggered which needless to deal with
        if code == lv.EVENT.CLICKED and target != self.checkbox:
            if self.checkbox.get_state() & lv.STATE.CHECKED:
                self.checkbox.clear_state(lv.STATE.CHECKED)
            else:
                self.checkbox.add_state(lv.STATE.CHECKED)
            lv.event_send(self.checkbox, lv.EVENT.VALUE_CHANGED, None)

    def get_checkbox(self):
        return self.checkbox

    def get_label(self):
        return self.label

    def enable_bg_color(self, enable: bool = True):
        if enable:
            self.add_style(
                StyleWrapper()
                .text_color(lv_colors.WHITE)
                .bg_color(lv_colors.ONEKEY_BLACK_3),
                0,
            )
        else:
            self.add_style(
                StyleWrapper()
                .text_color(lv_colors.WHITE_1)
                .bg_color(lv_colors.ONEKEY_BLACK_4),
                0,
            )


class DisplayItem(lv.obj):
    def __init__(self, parent, title, content, bg_color=lv_colors.ONEKEY_GRAY_3):
        super().__init__(parent)
        self.set_size(464, lv.SIZE.CONTENT)
        self.add_style(
            StyleWrapper()
            .bg_color(bg_color)
            .bg_opa(lv.OPA.COVER)
            .min_height(88)
            .border_width(0)
            .pad_all(8)
            .radius(0)
            .text_align_left(),
            0,
        )
        self.label_top = lv.label(self)
        self.label_top.set_recolor(True)
        self.label_top.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.label_top.set_long_mode(lv.label.LONG.WRAP)
        self.label_top.set_text(title)
        self.label_top.set_align(lv.ALIGN.TOP_LEFT)
        self.label_top.add_style(
            StyleWrapper()
            .text_font(font_PJSBOLD30)
            .text_color(lv_colors.WHITE)
            .text_letter_space(-1),
            0,
        )

        self.label = lv.label(self)
        self.label.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.label.set_long_mode(lv.label.LONG.DOT)
        self.label.set_text(content)
        self.label.add_style(
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_color(lv_colors.ONEKEY_WHITE_4)
            .text_line_space(6)
            .text_letter_space(-1)
            .max_height(400),
            0,
        )
        self.label.align_to(self.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 4)


class DisplayItemNoBgc(DisplayItem):
    def __init__(self, parent, title, content):
        super().__init__(parent, title, content, bg_color=lv_colors.BLACK)
        self.add_style(
            StyleWrapper().min_height(0).pad_hor(0),
            0,
        )


class ImgGridItem(lv.img):
    """Home Screen setting display"""

    def __init__(
        self,
        parent,
        col_num,
        row_num,
        file_name: str,
        path_dir: str,
        img_path_other: str = "A:/res/checked-solid.png",
        is_internal: bool = False,
    ):
        super().__init__(parent)
        self.set_grid_cell(
            lv.GRID_ALIGN.CENTER, col_num, 1, lv.GRID_ALIGN.CENTER, row_num, 1
        )
        self.is_internal = is_internal
        self.file_name = file_name
        self.zoom_path = path_dir + file_name
        self.set_src(self.zoom_path)
        self.img_path = self.zoom_path.replace("zoom-", "")
        self.check = lv.img(self)
        self.check.set_src(img_path_other)
        self.check.center()
        self.set_checked(False)
        self.add_flag(lv.obj.FLAG.CLICKABLE)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def set_checked(self, checked: bool):
        if checked:
            self.check.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.check.add_flag(lv.obj.FLAG.HIDDEN)
