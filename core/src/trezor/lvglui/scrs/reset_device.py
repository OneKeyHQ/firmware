from typing import TYPE_CHECKING

from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs.components.anim import Anim
from trezor.lvglui.scrs.components.container import ContainerFlexCol
from trezor.lvglui.scrs.components.listitem import ListItemWithLeadingCheckbox
from trezor.lvglui.scrs.components.transition import DefaultTransition

from . import font_MONO28, lv, lv_colors
from .common import FullSizeWindow

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
        self.panel = lv.obj(self.content_area)
        self.panel.set_size(460, lv.SIZE.CONTENT)
        self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        self.panel.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_bg_color(
            lv_colors.ONEKEY_BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.panel.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_pad_ver(24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_pad_hor(5, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_radius(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.word_col1 = lv.label(self.panel)
        self.word_col1.set_size(lv.pct(50), lv.SIZE.CONTENT)
        self.word_col1.set_recolor(True)
        self.word_col1.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.word_col1.set_style_text_font(font_MONO28, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col1.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.word_col1.set_style_pad_hor(5, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col1.set_style_text_line_space(18, lv.PART.MAIN | lv.STATE.DEFAULT)
        # set_style_text_letter_space
        self.word_col2 = lv.label(self.panel)
        self.word_col2.set_size(lv.pct(50), lv.SIZE.CONTENT)
        self.word_col2.set_recolor(True)
        self.word_col2.align(lv.ALIGN.TOP_RIGHT, 0, 0)
        self.word_col2.set_style_text_font(font_MONO28, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col2.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.word_col2.set_style_pad_hor(5, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col2.set_style_text_line_space(18, lv.PART.MAIN | lv.STATE.DEFAULT)

        text_col = ""
        text_col2 = ""
        for index in range(0, int(word_count / 2)):
            text_col += f"#999999 {index+1:>2}.#{mnemonics[index]}\n"
            text_col2 += f"#999999 {int(index+int(word_count/2)+1):>2}.#{mnemonics[int(index+int(word_count/2))]}\n"
            self.word_col1.set_text(text_col.rstrip())
            self.word_col2.set_text(text_col2.rstrip())
        self.item = ListItemWithLeadingCheckbox(
            self.content_area,
            _(i18n_keys.CHECK__I_HAVE_WRITE_DOWN_THE_WORDS),
        )
        self.item.set_size(460, lv.SIZE.CONTENT)
        self.item.align_to(self.panel, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)
        self.btn_yes.disable()
        self.content_area.add_event_cb(self.value_changed, lv.EVENT.VALUE_CHANGED, None)

    def value_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.VALUE_CHANGED:
            if target == self.item.checkbox:
                if target.get_state() & lv.STATE.CHECKED:
                    self.item.enable_bg_color()
                    self.btn_yes.enable(bg_color=lv_colors.ONEKEY_GREEN)
                else:
                    self.item.enable_bg_color(enable=False)
                    self.btn_yes.disable()


class BackupTips(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__BACK_UP_RECOVERY_PHRASE),
            _(i18n_keys.SUBTITLE__DEVICE_BACKUP_BACK_UP_RECOVERY_PHRASE),
            _(i18n_keys.BUTTON__CONTINUE),
        )
        self.container = ContainerFlexCol(
            self.content_area, self.subtitle, pos=(0, 10), padding_row=10
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
                self.btn_yes.enable(bg_color=lv_colors.ONEKEY_GREEN)
            elif self.cb_cnt < 3:
                self.btn_yes.disable()


class CheckWord(FullSizeWindow):
    def __init__(self, title: str, subtitle: str, options: str):
        super().__init__(title, subtitle)

        self.choices = Radio(self, options)
        self.tip = lv.label(self.content_area)
        self.tip.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.tip.set_recolor(True)
        self.tip.set_text("")
        self.tip.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.tip.align_to(self.choices.container, lv.ALIGN.OUT_TOP_MID, 0, -14)
        self.add_event_cb(self.on_ready, lv.EVENT.READY, None)

    def on_ready(self, event_obj):
        # self.show_unload_anim()
        self.channel.publish(self.choices.get_selected_str())
        self.destroy()

    def tip_correct(self):
        self.tip.set_text(
            f"#00CC36 {lv.SYMBOL.OK}# {_(i18n_keys.MSG__CORRECT__EXCLAMATION)}"
        )
        # self.tip.clear_flag(lv.obj.FLAG.HIDDEN)

    def tip_incorrect(self):
        self.tip.set_text(
            f"#DF320C {lv.SYMBOL.CLOSE}# {_(i18n_keys.MSG__INCORRECT__EXCLAMATION)}"
        )
        # self.tip.clear_flag(lv.obj.FLAG.HIDDEN)

    def show_unload_anim(self):
        Anim(0, -480, self.set_pos, time=200, y_axis=False, delay=400).start()


class Radio:
    def __init__(self, parent, options) -> None:
        self.parent = parent
        self.container = ContainerFlexCol(parent, None, padding_row=2)
        self.container.set_style_pad_all(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.items: list[RadioItem] = []
        self.check_index = 0
        self.choices = options.split("\n")
        for _idx, choice in enumerate(self.choices):
            item = RadioItem(self.container, choice)
            self.items.append(item)
        self.container.add_event_cb(self.on_selected_changed, lv.EVENT.CLICKED, None)

    def on_selected_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            from trezor import utils

            if utils.lcd_resume():
                return
            for idx, item in enumerate(self.items):
                if target == item:
                    self.check_index = idx
                    lv.event_send(self.parent, lv.EVENT.READY, None)

    def get_selected_index(self):
        return self.check_index

    def get_selected_str(self):
        return self.items[self.check_index].get_text()


class RadioItem(lv.btn):
    def __init__(
        self,
        parent,
        text: str,
    ) -> None:
        super().__init__(parent)
        self.content = text
        self.remove_style_all()
        self.set_style_height(84, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_width(464, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_hor(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(4, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_1, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_font(font_MONO28, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT)

        transition = DefaultTransition()

        self.set_style_transform_height(-4, lv.PART.MAIN | lv.STATE.PRESSED)
        self.set_style_transform_width(-4, lv.PART.MAIN | lv.STATE.PRESSED)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_2, lv.PART.MAIN | lv.STATE.PRESSED
        )
        # self.set_style_text_color(
        #     lv_colors.ONEKEY_GRAY, lv.PART.MAIN | lv.STATE.PRESSED
        # )
        self.set_style_transition(transition, lv.PART.MAIN | lv.STATE.PRESSED)

        self.label = lv.label(self)
        self.label.set_long_mode(lv.label.LONG.WRAP)
        self.label.set_text(text)
        self.label.set_align(lv.ALIGN.CENTER)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

    def get_text(self) -> str:
        return self.content
