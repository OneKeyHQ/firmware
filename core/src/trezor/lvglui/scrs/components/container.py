import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]


class ContainerFlexCol(lv.obj):
    def __init__(
        self,
        parent,
        align_base,
        align=lv.ALIGN.OUT_BOTTOM_MID,
        pos: tuple = (0, 30),
        padding_row: int = 32,
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.align_to(align_base, align, pos[0], pos[1])
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_row(padding_row, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_flex_flow(lv.FLEX_FLOW.COLUMN)


class ContanierGrid(lv.obj):
    def __init__(
        self,
        parent,
        align_base,
        align=lv.ALIGN.OUT_BOTTOM_MID,
        pos: tuple = (0, 30),
        cell_size: tuple = (213, 192),
    ) -> None:
        super().__init__(parent)
        col_dsc = [cell_size[0], cell_size[0], lv.GRID_TEMPLATE.LAST]
        row_dsc = [cell_size[1], cell_size[1], lv.GRID_TEMPLATE.LAST]
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.align_to(align_base, align, pos[0], pos[1])
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_grid_column_dsc_array(col_dsc, 0)
        self.set_style_grid_row_dsc_array(row_dsc, 0)
        self.set_layout(lv.LAYOUT_GRID.value)
