import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


class ImgButton(lv.imgbtn):
    pass


class ImgBottonGridItem(lv.imgbtn):
    """Home Screen setting display"""

    def __init__(
        self,
        parent,
        col_num,
        row_num,
        bg_img_path: str,
        img_path_other: str = "A:/res/checked_48.png",
    ):
        super().__init__(parent)
        self.set_grid_cell(
            lv.GRID_ALIGN.STRETCH, col_num, 1, lv.GRID_ALIGN.STRETCH, row_num, 1
        )
        self.set_style_bg_img_src(bg_img_path, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.check = lv.img(self)
        self.check.set_src(img_path_other)
        self.check.align(lv.ALIGN.BOTTOM_RIGHT, -30, -20)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def set_checked(self, checked: bool):
        if checked:
            self.check.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.check.add_flag(lv.obj.FLAG.HIDDEN)
