import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


class ContainerFlexCol(lv.obj):
    def __init__(
        self,
        parent,
        align_base,
        align=lv.ALIGN.OUT_BOTTOM_MID,
        pos: tuple = (0, 30),
        padding_row: int = 12,
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        if align_base is None:
            self.align(lv.ALIGN.BOTTOM_MID, 0, -30)
        else:
            self.align_to(align_base, align, pos[0], pos[1])
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_ver(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_row(padding_row, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_flex_flow(lv.FLEX_FLOW.COLUMN)


class ContainerFlexRow(lv.obj):
    def __init__(
        self,
        parent,
        align_base,
        align=lv.ALIGN.OUT_TOP_MID,
        pos: tuple = (0, -48),
        padding_col: int = 8,
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.align_to(align_base, align, pos[0], pos[1])
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_column(padding_col, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_flex_flow(lv.FLEX_FLOW.ROW)
        # align style of the items in the container
        self.set_flex_align(
            lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER
        )


class ContainerGrid(lv.obj):
    def __init__(
        self,
        parent,
        row_dsc,
        col_dsc,
        align_base=None,
        align_type=lv.ALIGN.OUT_BOTTOM_MID,
        pos: tuple = (0, 30),
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        if align_base:
            self.align_to(align_base, align_type, pos[0], pos[1])
        else:
            self.align(lv.ALIGN.TOP_MID, 0, 48)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_grid_column_dsc_array(col_dsc, 0)
        self.set_style_grid_row_dsc_array(row_dsc, 0)
        self.set_layout(lv.LAYOUT_GRID.value)
