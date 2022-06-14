from typing import TYPE_CHECKING

from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs.components.container import ContainerFlexCol
from trezor.lvglui.scrs.components.listitem import ListItemWithLeadingCheckbox

from . import font_MONO28, lv
from .common import FullSizeWindow
from .components.button import NormalButton

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
        )
        self.content_area.set_style_bg_color(
            lv.color_hex(0xFFFFFF), lv.PART.SCROLLBAR | lv.STATE.DEFAULT
        )
        self.panel = lv.obj(self.content_area)
        self.panel.set_size(460, lv.SIZE.CONTENT)
        self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        self.panel.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.panel.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_pad_ver(24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_pad_hor(5, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_radius(12, lv.PART.MAIN | lv.STATE.DEFAULT)
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
            text_col += f"#666666 {index+1:>2}.# {mnemonics[index]}\n"
            text_col2 += f"#666666 {int(index+int(word_count/2)+1):>2}.# {mnemonics[int(index+int(word_count/2))]}\n"
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
                    self.btn_yes.enable(
                        bg_color=lv.color_hex(0x1BAC44),
                        text_color=lv.color_hex(0xFFFFFF),
                    )
                else:
                    self.item.enable_bg_color(enable=False)
                    self.btn_yes.disable()


class BackupTips(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__BACK_UP_RECOVERY_PHRASE),
            _(i18n_keys.SUBTITLE__DEVICE_BACKUP_BACK_UP_RECOVERY_PHRASE),
        )
        self.container = ContainerFlexCol(
            self, self.subtitle, pos=(0, 10), padding_row=10
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
        self.btn = NormalButton(self, _(i18n_keys.BUTTON__CONTINUE), False)
        self.container.add_event_cb(self.eventhandler, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
        self.cb_cnt = 0

    def eventhandler(self, event_obj: lv.event_t):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn:
                self.channel.publish(1)
                self.destroy()
        elif code == lv.EVENT.VALUE_CHANGED:
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
                self.btn.enable(
                    bg_color=lv.color_hex(0x1BAC44), text_color=lv.color_hex(0xFFFFFF)
                )
            elif self.cb_cnt < 3:
                self.btn.disable()
