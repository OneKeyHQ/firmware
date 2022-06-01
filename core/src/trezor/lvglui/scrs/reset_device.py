from typing import TYPE_CHECKING

from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs.components.container import ContainerFlexCol
from trezor.lvglui.scrs.components.listitem import ListItemWithLeadingCheckbox

from . import font_MONO24, lv
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
            _(i18n_keys.BUTTON__HOLD_TO_CONFIRM),
            hold_confirm=True,
        )
        self.panel = lv.obj(self)
        self.panel.set_size(440, lv.SIZE.CONTENT)
        self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 24)
        self.panel.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.panel.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.panel.set_style_radius(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col1 = lv.label(self.panel)
        self.word_col1.set_size(lv.pct(50), lv.SIZE.CONTENT)
        self.word_col1.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.word_col1.set_style_text_font(font_MONO24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col1.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.word_col1.set_style_pad_all(10, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col1.set_style_text_line_space(8, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col2 = lv.label(self.panel)
        self.word_col2.set_size(lv.pct(50), lv.SIZE.CONTENT)
        self.word_col2.align(lv.ALIGN.TOP_RIGHT, 0, 0)
        self.word_col2.set_style_text_font(font_MONO24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col2.set_style_text_align(
            lv.TEXT_ALIGN.LEFT, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.word_col2.set_style_pad_all(10, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col2.set_style_pad_left(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.word_col2.set_style_text_line_space(8, lv.PART.MAIN | lv.STATE.DEFAULT)

        text_col = ""
        text_col2 = ""
        for index in range(0, int(word_count / 2)):
            text_col += f"{index+1:>2}. {mnemonics[index]}\n"
            text_col2 += f"{int(index+int(word_count/2)+1):>2}. {mnemonics[int(index+int(word_count/2))]}\n"
            self.word_col1.set_text(text_col.rstrip())
            self.word_col2.set_text(text_col2.rstrip())
        self.btn_yes.align_to(self.panel, lv.ALIGN.OUT_BOTTOM_MID, 0, 48)


class BackupTips(FullSizeWindow):
    def __init__(self):
        super().__init__(
            _(i18n_keys.TITLE__BACK_UP_RECOVERY_PHRASE),
            _(i18n_keys.SUBTITLE__DEVICE_BACKUP_BACK_UP_RECOVERY_PHRASE),
        )
        self.container = ContainerFlexCol(self, self.subtitle, pos=(0, 10))
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
                    bg_color=lv.color_hex(0x1B7735), text_color=lv.color_hex(0xFFFFFF)
                )
            elif self.cb_cnt < 3:
                self.btn.disable()
