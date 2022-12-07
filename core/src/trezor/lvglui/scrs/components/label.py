from .. import font_PJSBOLD72, font_PJSREG30, lv, lv_colors
from ..widgets.style import StyleWrapper


class Title(lv.label):
    def __init__(
        self, parent, align_base, relative_pos: tuple, text: str, pos_y: int = 52
    ) -> None:
        super().__init__(parent)
        self.set_long_mode(lv.label.LONG.WRAP)
        self.set_text(text)
        self.set_size(464, lv.SIZE.CONTENT)
        if align_base:
            self.align_to(
                align_base, lv.ALIGN.OUT_BOTTOM_MID, relative_pos[0], relative_pos[1]
            )
        else:
            self.align(lv.ALIGN.TOP_LEFT, 8, pos_y)
        self.add_style(
            StyleWrapper()
            .text_font(font_PJSBOLD72)
            .text_color(lv_colors.WHITE)
            .text_align_left()
            .text_letter_space(-4)
            .text_line_space(-8),
            0,
        )


class SubTitle(lv.label):
    def __init__(self, parent, align_base, relative_pos: tuple, text: str) -> None:
        super().__init__(parent)
        self.set_long_mode(lv.label.LONG.WRAP)
        self.set_text(text)
        self.set_size(464, lv.SIZE.CONTENT)

        self.align_to(
            align_base, lv.ALIGN.OUT_BOTTOM_MID, relative_pos[0], relative_pos[1]
        )
        self.add_style(
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_align(lv.TEXT_ALIGN.LEFT)
            .text_line_space(4)
            .text_color(lv_colors.LIGHT_GRAY)
            .text_letter_space(-1),
            0,
        )
