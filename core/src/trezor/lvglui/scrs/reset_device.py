from typing import TYPE_CHECKING

from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs.components.container import ContainerFlexCol, ContainerGrid
from trezor.lvglui.scrs.components.listitem import ListItemWithLeadingCheckbox

from . import font_MONO28, font_PJSREG24, lv, lv_colors
from .common import FullSizeWindow
from .components.radio import RadioTrigger
from .widgets.style import StyleWrapper

if TYPE_CHECKING:
    from typing import Sequence

    pass


class MnemonicDisplay(FullSizeWindow):
    def __init__(self, title: str, mnemonics: Sequence[str]):
        word_count = len(mnemonics)
        super().__init__(
            title,
            _(i18n_keys.SUBTITLE__DEVICE_BACKUP_MANUAL_BACKUP).format(word_count),
            _(i18n_keys.BUTTON__CONTINUE),
            anim_dir=0,
        )
        # self.content_area.set_style_bg_color(
        #     lv_colors.WHITE_3, lv.PART.SCROLLBAR | lv.STATE.DEFAULT
        # )
        row_dsc = [58] * (int(word_count / 2))
        row_dsc.append(lv.GRID_TEMPLATE.LAST)
        # 3 columns
        col_dsc = [
            231,
            231,
            lv.GRID_TEMPLATE.LAST,
        ]
        self.container = ContainerGrid(
            self.content_area,
            row_dsc=row_dsc,
            col_dsc=col_dsc,
            align_base=self.subtitle,
        )
        word_style = (
            StyleWrapper()
            .width(231)
            .height(58)
            .pad_left(8)
            .bg_color(lv_colors.ONEKEY_GRAY_3)
            .bg_opa(lv.OPA.COVER)
            .text_align_left()
        )
        self.container.add_style(
            StyleWrapper()
            .pad_gap(2)
            .width(464)
            .text_font(font_MONO28)
            .text_color(lv_colors.WHITE),
            0,
        )
        self.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.content_area.set_scroll_dir(lv.DIR.VER)
        self.set_grid_align(lv.GRID_ALIGN.CENTER, lv.GRID_ALIGN.CENTER)
        half = word_count // 2
        for i in range(word_count):
            col = 0 if i < half else 1
            row = i % half
            word = lv.obj(self.container)
            word.remove_style_all()
            word.add_style(word_style, 0)
            word_label = lv.label(word)
            word_label.set_align(lv.ALIGN.LEFT_MID)
            word_label.set_text(f"{i+1:>2}. {mnemonics[i]}")
            word.set_grid_cell(
                lv.GRID_ALIGN.STRETCH, col, 1, lv.GRID_ALIGN.STRETCH, row, 1
            )
        # self.panel = lv.obj(self.content_area)
        # self.panel.set_size(464, lv.SIZE.CONTENT)
        # self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        # self.panel.set_style_border_width(0, 0)
        # self.panel.set_style_bg_color(lv_colors.ONEKEY_GRAY_3, 0)
        # self.panel.set_style_bg_opa(255, 0)
        # self.panel.set_style_pad_ver(24, 0)
        # self.panel.set_style_pad_hor(5, 0)
        # self.panel.set_style_radius(2, 0)
        # self.panel.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        # self.word_col1 = lv.label(self.panel)
        # self.word_col1.set_size(lv.pct(50), lv.SIZE.CONTENT)
        # self.word_col1.set_recolor(True)
        # self.word_col1.align(lv.ALIGN.TOP_LEFT, 0, 0)
        # self.word_col1.set_style_text_font(font_MONO28, 0)
        # self.word_col1.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
        # self.word_col1.set_style_pad_hor(5, 0)
        # self.word_col1.set_style_text_line_space(18, 0)
        # # set_style_text_letter_space
        # self.word_col2 = lv.label(self.panel)
        # self.word_col2.set_size(lv.pct(50), lv.SIZE.CONTENT)
        # self.word_col2.set_recolor(True)
        # self.word_col2.align(lv.ALIGN.TOP_RIGHT, 0, 0)
        # self.word_col2.set_style_text_font(font_MONO28, 0)
        # self.word_col2.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
        # self.word_col2.set_style_pad_hor(5, 0)
        # self.word_col2.set_style_text_line_space(18, 0)

        # text_col = ""
        # text_col2 = ""
        # for index in range(0, int(word_count / 2)):
        #     text_col += f"#999999 {index+1:>2}.#{mnemonics[index]}\n"
        #     text_col2 += f"#999999 {int(index+int(word_count/2)+1):>2}.#{mnemonics[int(index+int(word_count/2))]}\n"
        #     self.word_col1.set_text(text_col.rstrip())
        #     self.word_col2.set_text(text_col2.rstrip())
        # self.item = ListItemWithLeadingCheckbox(
        #     self.content_area,
        #     _(i18n_keys.CHECK__I_HAVE_WRITE_DOWN_THE_WORDS),
        # )
        # self.item.set_size(460, lv.SIZE.CONTENT)
        # self.item.align_to(self.panel, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

    #     self.btn_yes.disable()
    #     self.content_area.add_event_cb(self.value_changed, lv.EVENT.VALUE_CHANGED, None)

    # def value_changed(self, event_obj):
    #     code = event_obj.code
    #     target = event_obj.get_target()
    #     if code == lv.EVENT.VALUE_CHANGED:
    #         if target == self.item.checkbox:
    #             if target.get_state() & lv.STATE.CHECKED:
    #                 self.item.enable_bg_color()
    #                 self.btn_yes.enable(bg_color=lv_colors.ONEKEY_GREEN)
    #             else:
    #                 self.item.enable_bg_color(enable=False)
    #                 self.btn_yes.disable()


class CheckWordTips(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__SETUP_CREATE_ALMOST_DONE),
            _(i18n_keys.SUBTITLE__SETUP_CREATE_ALMOST_DOWN),
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__BACK),
        )


class BackupTips(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__READY_TO_BACK_UP),
            _(i18n_keys.SUBTITLE__DEVICE_BACKUP_BACK_UP_RECOVERY_PHRASE),
            _(i18n_keys.BUTTON__BACK_UP),
        )
        self.container = ContainerFlexCol(
            self.content_area, self.subtitle, pos=(0, 40), padding_row=10
        )
        self.container.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.item1 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_BACK_UP_RECOVERY_PHRASE_1),
        )
        self.item2 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_BACK_UP_RECOVERY_PHRASE_2),
        )
        self.item3 = ListItemWithLeadingCheckbox(
            self.container,
            _(i18n_keys.CHECK__DEVICE_BACK_UP_RECOVERY_PHRASE_3),
        )
        self.btn_yes.disable()
        self.container.add_event_cb(self.on_event, lv.EVENT.VALUE_CHANGED, None)
        self.cb_cnt = 0

    def on_event(self, event_obj: lv.event_t):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.item1.checkbox:
                if target.get_state() & lv.STATE.CHECKED:
                    self.item1.enable_bg_color()
                    self.cb_cnt += 1
                else:
                    self.item1.enable_bg_color(False)
                    self.cb_cnt -= 1
            elif target == self.item2.checkbox:
                if target.get_state() & lv.STATE.CHECKED:
                    self.item2.enable_bg_color()
                    self.cb_cnt += 1
                else:
                    self.item2.enable_bg_color(False)
                    self.cb_cnt -= 1
            elif target == self.item3.checkbox:
                if target.get_state() & lv.STATE.CHECKED:
                    self.item3.enable_bg_color()
                    self.cb_cnt += 1
                else:
                    self.item3.enable_bg_color(False)
                    self.cb_cnt -= 1
            if self.cb_cnt == 3:
                self.btn_yes.enable(
                    bg_color=lv_colors.ONEKEY_GREEN, text_color=lv_colors.BLACK
                )
            elif self.cb_cnt < 3:
                self.btn_yes.disable()


class CheckWord(FullSizeWindow):
    def __init__(self, title: str, options: str):
        super().__init__(title, _(i18n_keys.SUBTITLE__DEVICE_BACKUP_CHECK_WORD))

        self.choices = RadioTrigger(self, options)

        self.tip_panel = lv.obj(self.content_area)
        self.tip_panel.remove_style_all()
        self.tip_panel.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.tip_img = lv.img(self.tip_panel)
        self.tip_img.set_align(lv.ALIGN.LEFT_MID)
        self.tip = lv.label(self.tip_panel)
        self.tip.set_recolor(True)
        self.tip.set_text("")
        self.tip_panel.add_style(
            StyleWrapper().text_font(font_PJSREG24).text_align_center(),
            0,
        )
        self.tip_panel.align_to(self.choices.container, lv.ALIGN.OUT_TOP_MID, 8, -14)

        self.add_event_cb(self.on_ready, lv.EVENT.READY, None)

    def on_ready(self, event_obj):
        self.show_unload_anim()
        self.channel.publish(self.choices.get_selected_str())
        # self.destroy()

    def tip_correct(self):
        self.tip_img.set_src("A:/res/feedback_correct.png")
        self.tip.set_text(f"#00FF33 {_(i18n_keys.MSG__CORRECT__EXCLAMATION)}#")
        self.tip.align_to(self.tip_img, lv.ALIGN.OUT_RIGHT_MID, 4, 0)
        # self.tip.clear_flag(lv.obj.FLAG.HIDDEN)

    def tip_incorrect(self):
        self.tip_img.set_src("A:/res/feedback_incorrect.png")
        self.tip.set_text(f"#FF1100 {_(i18n_keys.MSG__INCORRECT__EXCLAMATION)}#")
        self.tip.align_to(self.tip_img, lv.ALIGN.OUT_RIGHT_MID, 4, 0)
        # self.tip.clear_flag(lv.obj.FLAG.HIDDEN)
