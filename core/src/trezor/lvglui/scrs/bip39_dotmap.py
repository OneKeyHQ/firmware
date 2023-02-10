from trezor import utils
from trezor.crypto import bip39
from trezor.errors import MnemonicError
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import font_PJSREG24, font_PJSREG30
from trezor.lvglui.scrs.components.button import NormalButton

from . import lv, lv_colors
from .common import FullSizeWindow
from .components.container import ContainerFlexCol, ContainerFlexRow
from .widgets.style import StyleWrapper

ALLOWED_WORD_COUNTER = (12, 18, 24)


class Bip39DotMap(FullSizeWindow):
    def __init__(self, word_count: int) -> None:
        super().__init__(
            _(i18n_keys.TITLE__KEYTAG),
            _(i18n_keys.SUBTITLE__STR_WORDS).format(word_count),
            _(i18n_keys.BUTTON__VIEW_BACKSIDE)
            if word_count > 12
            else _(i18n_keys.BUTTON__DONE),
            anim_dir=0,
        )
        assert (
            word_count in ALLOWED_WORD_COUNTER
        ), "Allowed word count include 12, 18 or 24"
        self.word_cnt = word_count
        self.front = True
        self.panel = lv.obj(self.content_area)
        self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_LEFT, 4, 40)
        self.panel.add_style(
            StyleWrapper()
            .width(456)
            .height(472)
            .border_width(0)
            .radius(0)
            .pad_all(0)
            .bg_img_src("A:/res/dotmap_front.png")
            .bg_color(lv_colors.BLACK),
            0,
        )

        self.tag_label = lv.label(self.content_area)
        self.tag_label.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_align_center(), 0
        )
        self.tag_label.set_text(_(i18n_keys.CONTENT__FRONT_STR).format(1, 12))
        self.tag_label.align_to(self.panel, lv.ALIGN.OUT_BOTTOM_MID, 0, 8)

        self.tips_bar = lv.obj(self.content_area)
        self.tips_bar.remove_style_all()
        self.tips_bar.add_style(
            StyleWrapper()
            .width(464)
            .height(lv.SIZE.CONTENT)
            .text_font(font_PJSREG30)
            .text_color(lv_colors.ONEKEY_GRAY_4)
            .text_letter_space(-1)
            .bg_color(lv_colors.ONEKEY_GRAY_3)
            .bg_opa()
            .radius(0)
            .border_width(0)
            .pad_hor(8)
            .pad_ver(16),
            0,
        )
        self.tips_bar.align_to(self.panel, lv.ALIGN.OUT_BOTTOM_MID, 0, 61)
        self.tips_bar_img = lv.img(self.tips_bar)
        self.tips_bar_img.set_src("A:/res/notice.png")
        self.tips_bar_img.set_align(lv.ALIGN.TOP_LEFT)
        self.tips_bar_desc = lv.label(self.tips_bar)
        self.tips_bar_desc.set_size(408, lv.SIZE.CONTENT)
        self.tips_bar_desc.set_long_mode(lv.label.LONG.WRAP)
        self.tips_bar_desc.align_to(self.tips_bar_img, lv.ALIGN.OUT_RIGHT_TOP, 8, 0)
        self.tips_bar_desc.set_text(
            _(i18n_keys.CONTENT__FOLLOW_DOTMAP_TO_BACKUP_WITH_KEYTAG)
        )
        self.content_area.set_style_max_height(756, 0)
        self.btn_yes.set_parent(self.content_area)
        self.btn_yes.align_to(self.tips_bar, lv.ALIGN.OUT_BOTTOM_MID, 0, 16)

        self.matrix = ContainerFlexCol(self.panel, None, padding_row=0)
        self.matrix.align(lv.ALIGN.TOP_LEFT, 59, 75)
        self.matrix.set_size(373, 373)
        self.matrix.add_style(StyleWrapper().pad_all(0), 0)
        self.matrix.set_flex_align(
            lv.FLEX_ALIGN.SPACE_EVENLY, lv.FLEX_ALIGN.SPACE_EVENLY, lv.FLEX_ALIGN.START
        )
        self.indexs = []
        for i in range(12):
            index = self.BinaryWordIndex(self.matrix)
            if i != 11:
                Line(self.matrix, 2 if i % 4 == 3 else 1)
            self.indexs.append(index)

    def show(self, mnemonics: str, front=True):
        if not bip39.check(mnemonics):
            raise MnemonicError()
        self.mnemonic = mnemonics
        words = mnemonics.split()
        assert len(words) == self.word_cnt
        for i, word in enumerate(words[:12] if front else words[12:]):
            if not front:
                i += 12
            index = bip39.find(word) + 1
            self.indexs[i].show_binary_index(index)

    # def to_words(self):
    #     words: list[str] = []
    #     for index in self.indexs:
    #         words.append(index.to_word())
    #     return " ".join(words[:self.word_cnt])

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if utils.lcd_resume():
                return
            if target == self.btn_yes:
                if self.word_cnt != 12:
                    if self.front:
                        self.show_back()
                        return
                self.show_unload_anim()
                self.channel.publish(1)

            elif hasattr(self, "btn_no") and (target == self.btn_no):
                self.show_dismiss_anim()
                self.channel.publish(0)

    def show_back(self):
        self.front = False
        self.btn_yes.delete()
        self.btn_yes = NormalButton(self.content_area, _(i18n_keys.BUTTON__DONE))
        self.btn_yes.set_size(231, 98)
        self.btn_yes.enable(lv_colors.ONEKEY_GREEN, text_color=lv_colors.BLACK)
        self.btn_yes.align_to(self.tips_bar, lv.ALIGN.OUT_BOTTOM_RIGHT, 0, 16)
        self.btn_no = NormalButton(self.content_area, _(i18n_keys.BUTTON__BACK))
        self.btn_no.set_size(231, 98)
        self.btn_no.align_to(self.tips_bar, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.tag_label.set_text(
            _(i18n_keys.CONTENT__BACK_STR).format(13, self.word_cnt)
        )
        self.panel.set_style_bg_img_src("A:/res/dotmap_back.png", 0)
        self.matrix.clean()
        for i in range(12):
            index = self.BinaryWordIndex(self.matrix)
            if i != 11:
                Line(self.matrix, 2 if i % 4 == 3 else 1)
            self.indexs.append(index)
        self.show(self.mnemonic, False)

    class BinaryWordIndex(lv.obj):
        def __init__(self, parent) -> None:
            super().__init__(parent)
            self.set_size(373, 30)
            self.add_style(
                StyleWrapper()
                .bg_opa(lv.OPA.TRANSP)
                .border_width(0)
                .radius(0)
                .pad_all(7),
                0,
            )
            self.container = ContainerFlexRow(self, None, padding_col=7)
            self.container.add_style(StyleWrapper().pad_all(0), 0)
            self.container.set_flex_align(
                lv.FLEX_ALIGN.SPACE_BETWEEN, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER
            )
            self.point_style = (
                StyleWrapper()
                .width(16)
                .height(16)
                .bg_color(lv_colors.ONEKEY_WHITE_4)
                .border_width(0)
                .radius(lv.RADIUS.CIRCLE)
                .pad_all(0)
            )
            self.points = []
            for i in range(12):
                point = self.Point(self.container, self.point_style)
                if i != 11:
                    Line(self.container, 2 if i % 4 == 3 else 1)
                self.points.append(point)

        def show_binary_index(self, word_index: int) -> None:
            for i, point in enumerate(self.points):
                point.clear_flag(lv.obj.FLAG.CLICKABLE)
                self.pow_value = pow(2, 11 - i)
                if word_index & self.pow_value == self.pow_value:
                    point.set_checked()

        # def to_word(self) -> str:
        #     summary = 1
        #     for i, point in self.points:
        #         summary += pow(2, 11 - i) if point.checked else 0
        #     return bip39.get_word(summary - 1)

        class Point(lv.obj):
            def __init__(self, parent, style) -> None:
                super().__init__(parent)
                self.add_style(style, 0)
                self.checked = False
                # self.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

            def set_checked(self):
                self.checked = True
                self.set_style_bg_color(lv_colors.ONEKEY_GREEN_2, 0)

            def set_uncheck(self):
                self.checked = False
                self.set_style_bg_color(lv_colors.ONEKEY_WHITE_4, 0)

            # def on_click(self, _event_obj):
            #     self.set_uncheck() if self.checked else self.set_checked()


class Line(lv.line):
    def __init__(self, parent, line_width) -> None:
        super().__init__(parent)
        line_points = [{"x": 0, "y": 0}, {"x": 0, "y": 0}]
        style_line = lv.style_t()
        style_line.init()
        style_line.set_line_color(lv_colors.ONEKEY_GRAY_2)
        style_line.set_line_width(line_width)
        self.remove_style_all()
        self.set_points(line_points, 2)
        self.add_style(style_line, 0)
