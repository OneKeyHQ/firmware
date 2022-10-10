from .. import font_PJSBOLD24, font_PJSMID20, font_PJSREG24, lv, lv_colors


class ListItemWithLeadingCheckbox(lv.obj):
    def __init__(self, parent, text):
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_font(font_PJSMID20, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.checkbox = lv.checkbox(self)
        self.checkbox.set_size(40, 40)
        self.checkbox.set_align(lv.ALIGN.TOP_LEFT)
        self.checkbox.set_text("")
        self.checkbox.set_style_radius(8, lv.PART.INDICATOR | lv.STATE.DEFAULT)
        self.checkbox.set_style_border_color(
            lv_colors.ONEKEY_GRAY, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.checkbox.set_style_border_opa(255, lv.PART.INDICATOR | lv.STATE.DEFAULT)
        self.checkbox.set_style_bg_color(
            lv_colors.ONEKEY_GREEN, lv.PART.INDICATOR | lv.STATE.CHECKED
        )
        self.checkbox.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_1, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.checkbox.set_style_bg_opa(255, lv.PART.INDICATOR | lv.STATE.CHECKED)
        self.checkbox.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.label = lv.label(self)
        self.label.set_size(340, lv.SIZE.CONTENT)
        self.label.align_to(self.checkbox, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)
        self.label.set_text(text)
        self.label.set_style_text_line_space(4, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_color(lv_colors.WHITE_1, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
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
            self.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        else:
            self.set_style_text_color(
                lv_colors.WHITE_1, lv.PART.MAIN | lv.STATE.DEFAULT
            )


class DisplayItem(lv.obj):
    def __init__(self, parent, title, content):
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_hor(24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_ver(10, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label_top = lv.label(self)
        self.label_top.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.label_top.set_style_text_font(
            font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label_top.set_long_mode(lv.label.LONG.WRAP)
        self.label_top.set_style_text_color(
            lv_colors.WHITE_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label_top.set_text(title)
        self.label_top.set_align(lv.ALIGN.TOP_LEFT)

        self.label = lv.label(self)
        self.label.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.label.set_style_text_font(font_PJSREG24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label.set_long_mode(lv.label.LONG.DOT)
        self.label.set_style_max_height(400, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label.set_style_text_color(
            lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.label.set_style_text_line_space(6, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.label.set_text(content)
        self.label.align_to(self.label_top, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 6)


class ImgGridItem(lv.img):
    """Home Screen setting display"""

    def __init__(
        self,
        parent,
        col_num,
        row_num,
        img_path: str,
        img_path_other: str = "A:/res/checked-solid.png",
        is_internal: bool = False,
    ):
        super().__init__(parent)
        self.set_grid_cell(
            lv.GRID_ALIGN.CENTER, col_num, 1, lv.GRID_ALIGN.CENTER, row_num, 1
        )
        self.is_internal = is_internal

        self.set_src(img_path)
        # if img_path == "A:/res/wallpaper-4.png":
        #     self.set_style_border_width(1, lv.PART.MAIN | lv.STATE.DEFAULT)
        #     self.set_style_border_color(
        #         lv_colors.ONEKEY_BLACK_1, lv.PART.MAIN | lv.STATE.DEFAULT
        #     )
        # self.set_size_mode(lv.img.SIZE_MODE.REAL)
        # self.set_style_radius(4, lv.PART.MAIN | lv.STATE.DEFAULT)
        # self.set_style_clip_corner(True, lv.PART.MAIN | lv.STATE.DEFAULT)
        # self.set_antialias(True)
        # self.set_zoom(76)
        self.zoom_path = img_path
        self.wp_path = img_path.replace("zoom-", "")
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
