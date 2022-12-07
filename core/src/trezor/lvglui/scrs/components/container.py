from trezor.lvglui.lv_colors import lv_colors

import lvgl as lv  # type: ignore[Import "lvgl" could not be resolved]

from ..widgets.style import StyleWrapper


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
        self.remove_style_all()
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        if align_base is None:
            self.set_align(lv.ALIGN.BOTTOM_MID)
        else:
            self.align_to(align_base, align, 0, pos[1])
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.BLACK)
            .bg_opa(lv.OPA.COVER)
            .radius(0)
            .border_width(0)
            .pad_hor(0)
            .pad_row(padding_row),
            0,
        )
        self.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        self.set_flex_align(
            lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER
        )


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
        self.remove_style_all()
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        if align_base:
            self.align_to(align_base, align, pos[0], pos[1])
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.BLACK)
            .bg_opa()
            .radius(0)
            .border_width(0)
            .pad_column(padding_col),
            0,
        )
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
        align_type=lv.ALIGN.OUT_BOTTOM_LEFT,
        pos: tuple = (0, 40),
    ) -> None:
        super().__init__(parent)
        self.set_size(lv.pct(100), lv.SIZE.CONTENT)
        if align_base:
            self.align_to(align_base, align_type, pos[0], pos[1])
        else:
            self.align(lv.ALIGN.TOP_MID, 0, 48)

        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.BLACK)
            .radius(0)
            .bg_opa()
            .pad_all(0)
            .pad_gap(16)
            .border_width(0)
            .grid_column_dsc_array(col_dsc)
            .grid_row_dsc_array(row_dsc),
            0,
        )
        self.set_grid_align(lv.GRID_ALIGN.START, lv.GRID_ALIGN.END)
        self.set_layout(lv.LAYOUT_GRID.value)
