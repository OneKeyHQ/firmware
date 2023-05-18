from storage import device
from trezor import motor, utils
from trezor.crypto import bip39, random
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

from .. import (
    font_MONO24,
    font_PJSBOLD30,
    font_PJSBOLD36,
    font_PJSBOLD48,
    font_PJSREG24,
    font_STATUS_BAR,
    lv,
    lv_colors,
)
from ..widgets.style import StyleWrapper
from .transition import BtnClickTransition

# from .transition import DefaultTransition


def compute_mask(text: str) -> int:
    mask = 0
    for c in text:
        shift = ord(c) - 97  # ord('a') == 97
        if shift < 0:
            continue
        mask |= 1 << shift
    return mask


def change_key_bg(
    dsc: lv.obj_draw_part_dsc_t,
    id1: int,
    id2: int,
    enabled: bool,
    all_enabled: bool = True,
    allow_empty: bool = False,
) -> None:
    if dsc.id in (id1, id2):
        dsc.label_dsc.font = font_PJSBOLD48
    if enabled:
        if dsc.id == id1:
            dsc.rect_dsc.bg_color = lv_colors.ONEKEY_RED_1
        elif dsc.id == id2:
            if all_enabled:
                dsc.rect_dsc.bg_color = lv_colors.ONEKEY_GREEN
                dsc.label_dsc.color = lv_colors.BLACK
            else:
                dsc.rect_dsc.bg_color = lv_colors.ONEKEY_BLACK_1
                dsc.label_dsc.color = lv_colors.ONEKEY_GRAY_1
    else:
        if dsc.id in (id1, id2):
            dsc.rect_dsc.bg_color = lv_colors.ONEKEY_BLACK_1
            if dsc.id == id2:
                dsc.label_dsc.color = (
                    lv_colors.ONEKEY_GRAY_1 if not allow_empty else lv_colors.BLACK
                )


class BIP39Keyboard(lv.keyboard):
    """character keyboard with textarea."""

    def __init__(self, parent):
        super().__init__(parent)
        self.parent = parent
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_LEFT, 0, 188)
        self.ta.set_size(448, lv.SIZE.CONTENT)
        self.ta.add_style(
            StyleWrapper()
            .border_width(0)
            .radius(0)
            .bg_color(lv_colors.BLACK)
            .text_font(font_PJSBOLD36)
            .text_color(lv_colors.WHITE)
            .text_align_left(),
            0,
        )
        self.ta.set_max_length(11)
        self.ta.set_one_line(True)
        self.ta.set_accepted_chars("abcdefghijklmnopqrstuvwxyz")
        self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

        self.remove_style_all()
        self.btnm_map = [
            "q",
            "w",
            "e",
            "r",
            "t",
            "y",
            "u",
            "i",
            "o",
            "p",
            "\n",
            " ",
            "a",
            "s",
            "d",
            "f",
            "g",
            "h",
            "j",
            "k",
            "l",
            " ",
            "\n",
            lv.SYMBOL.BACKSPACE,
            "z",
            "x",
            "c",
            "v",
            "b",
            "n",
            "m",
            lv.SYMBOL.OK,
            "",
        ]
        self.keys = [
            "q",
            "w",
            "e",
            "r",
            "t",
            "y",
            "u",
            "i",
            "o",
            "p",
            "",  # ignore placeholder
            "a",
            "s",
            "d",
            "f",
            "g",
            "h",
            "j",
            "k",
            "l",
            "",  # ignore placeholder
            "",  # ignore backspace
            "z",
            "x",
            "c",
            "v",
            "b",
            "n",
            "m",
            "READY",
        ]
        self.ctrl_map = [
            lv.btnmatrix.CTRL.NO_REPEAT
            | lv.btnmatrix.CTRL.CLICK_TRIG
            | lv.btnmatrix.CTRL.POPOVER
        ] * 10
        self.ctrl_map.append(2 | lv.btnmatrix.CTRL.HIDDEN)
        self.ctrl_map.extend(
            [
                7
                | lv.btnmatrix.CTRL.NO_REPEAT
                | lv.btnmatrix.CTRL.POPOVER
                | lv.btnmatrix.CTRL.CLICK_TRIG
                | lv.btnmatrix.CTRL.POPOVER
            ]
            * 9
        )

        self.ctrl_map.append(2 | lv.btnmatrix.CTRL.HIDDEN)
        self.ctrl_map.extend(
            [4 | lv.btnmatrix.CTRL.DISABLED | lv.btnmatrix.CTRL.CLICK_TRIG]
        )
        self.ctrl_map.extend(
            [
                3
                | lv.btnmatrix.CTRL.NO_REPEAT
                | lv.btnmatrix.CTRL.POPOVER
                | lv.btnmatrix.CTRL.CLICK_TRIG
                | lv.btnmatrix.CTRL.POPOVER
            ]
            * 7
        )
        self.ctrl_map.extend(
            [
                4
                | lv.btnmatrix.CTRL.NO_REPEAT
                | lv.btnmatrix.CTRL.DISABLED
                | lv.btnmatrix.CTRL.CLICK_TRIG
            ]
        )
        self.dummy_ctl_map = []
        self.dummy_ctl_map.extend(self.ctrl_map)
        # delete button
        self.dummy_ctl_map[21] &= self.dummy_ctl_map[21] ^ lv.btnmatrix.CTRL.DISABLED
        self.set_map(lv.keyboard.MODE.TEXT_LOWER, self.btnm_map, self.ctrl_map)
        self.set_mode(lv.keyboard.MODE.TEXT_LOWER)
        self.set_width(lv.pct(100))

        self.add_style(StyleWrapper().bg_color(lv_colors.BLACK).pad_gap(2), 0)
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK)
            .bg_opa()
            .text_font(font_MONO24)
            .radius(2),
            lv.PART.ITEMS | lv.STATE.DEFAULT,
        )
        self.add_style(
            StyleWrapper().bg_color(lv_colors.ONEKEY_GRAY_3),
            lv.PART.ITEMS | lv.STATE.PRESSED,
        )
        self.add_style(
            StyleWrapper()
            .bg_grad_color(lv_colors.ONEKEY_BLACK_1)
            .text_color(lv_colors.GRAY_1),
            lv.PART.ITEMS | lv.STATE.DISABLED,
        )
        self.set_height(220)
        self.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        self.set_popovers(True)
        self.set_textarea(self.ta)
        self.add_event_cb(self.event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
        self.add_event_cb(self.event_cb, lv.EVENT.VALUE_CHANGED, None)
        self.mnemonic_prompt = lv.obj(parent)
        self.mnemonic_prompt.set_size(lv.pct(100), 66)
        self.mnemonic_prompt.align_to(self, lv.ALIGN.OUT_TOP_LEFT, 0, -4)
        self.mnemonic_prompt.add_style(
            StyleWrapper()
            .border_width(0)
            .bg_color(lv_colors.BLACK)
            .pad_all(0)
            .bg_opa()
            .radius(0)
            .pad_column(2),
            0,
        )
        self.mnemonic_prompt.set_flex_flow(lv.FLEX_FLOW.ROW)
        self.mnemonic_prompt.set_flex_align(
            lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.END
        )
        self.mnemonic_prompt.set_scrollbar_mode(lv.SCROLLBAR_MODE.ACTIVE)
        self.mnemonic_prompt.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.move_foreground()

    def tip_submitted(self):
        self.tip_panel = lv.obj(self.parent)
        self.tip_panel.remove_style_all()
        self.tip_panel.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.tip_img = lv.img(self.tip_panel)
        self.tip_img.set_align(lv.ALIGN.LEFT_MID)
        self.tip_img.set_src("A:/res/feedback_correct.png")
        self.tip = lv.label(self.tip_panel)
        self.tip.set_recolor(True)
        self.tip.align_to(self.tip_img, lv.ALIGN.OUT_RIGHT_MID, 4, 0)
        self.tip_panel.add_style(
            StyleWrapper()
            .text_font(font_PJSREG24)
            .text_color(lv_colors.ONEKEY_GREEN)
            .text_align_left(),
            0,
        )
        self.tip_panel.align_to(self.ta, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 24)
        self.tip.set_text(f"{_(i18n_keys.MSG__SUBMITTED)}")

    def on_click(self, event_obj):
        target = event_obj.get_target()
        child = target.get_child(0)
        if isinstance(child, lv.label):
            text = child.get_text()
            if text:
                self.ta.set_text(text)
            self.mnemonic_prompt.clean()
            for i, key in enumerate(self.keys):
                if key:
                    self.dummy_ctl_map[i] |= lv.btnmatrix.CTRL.DISABLED
            self.dummy_ctl_map[-1] &= (
                self.dummy_ctl_map[-1] ^ lv.btnmatrix.CTRL.DISABLED
            )
            self.completed = True
            self.set_map(lv.keyboard.MODE.TEXT_LOWER, self.btnm_map, self.dummy_ctl_map)
            lv.event_send(self, lv.EVENT.READY, None)

    def event_cb(self, event):
        if event.code == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.ta.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if len(txt_input) > 0:
                change_key_bg(dsc, 21, 29, True, self.completed)
            else:
                change_key_bg(dsc, 21, 29, False)
            # if dsc.id in (10, 20):
            #     dsc.rect_dsc.bg_color = lv_colors.BLACK
        elif event.code == lv.EVENT.VALUE_CHANGED:
            utils.lcd_resume()
            # btn_id = event.target.get_selected_btn()
            # text = event.target.get_btn_text(btn_id)
            # if text == " ":
            #     if btn_id in (10, 21):
            #         event.target.set_selected_btn(btn_id + 1)
            #     return
            motor.vibrate()
            self.mnemonic_prompt.clean()
            txt_input = self.ta.get_text()
            if len(txt_input) > 0:
                words = bip39.complete_word(txt_input) or ""
                mask = bip39.word_completion_mask(txt_input)
                candidates = words.rstrip().split() if words else []
                btn_style_default = (
                    StyleWrapper()
                    .bg_color(lv_colors.ONEKEY_BLACK)
                    .bg_opa()
                    .pad_all(16)
                    .radius(2)
                    .text_font(font_PJSBOLD30)
                    .text_color(lv_colors.WHITE)
                )
                btn_style_pressed = (
                    StyleWrapper()
                    .bg_color(lv_colors.ONEKEY_GRAY_3)
                    .bg_opa()
                    .text_color(lv_colors.WHITE_2)
                    .transform_height(-2)
                    .transform_width(-2)
                    .transition(BtnClickTransition())
                )
                for candidate in candidates:
                    btn = lv.btn(self.mnemonic_prompt)
                    btn.remove_style_all()
                    btn.add_style(btn_style_default, 0)
                    btn.add_style(btn_style_pressed, lv.PART.MAIN | lv.STATE.PRESSED)
                    btn.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
                    label = lv.label(btn)
                    label.set_text(candidate)
                for i, key in enumerate(self.keys):
                    if key and compute_mask(key) & mask:
                        self.dummy_ctl_map[i] &= (
                            self.dummy_ctl_map[i] ^ lv.btnmatrix.CTRL.DISABLED
                        )
                    else:
                        if key:
                            self.dummy_ctl_map[i] |= lv.btnmatrix.CTRL.DISABLED
                if txt_input in candidates:
                    self.dummy_ctl_map[-1] &= (
                        self.dummy_ctl_map[-1] ^ lv.btnmatrix.CTRL.DISABLED
                    )
                    self.completed = True
                else:
                    self.completed = False
                self.set_map(
                    lv.keyboard.MODE.TEXT_LOWER, self.btnm_map, self.dummy_ctl_map
                )
            else:
                self.set_map(lv.keyboard.MODE.TEXT_LOWER, self.btnm_map, self.ctrl_map)


class NumberKeyboard(lv.keyboard):
    """number keyboard with textarea."""

    def __init__(self, parent) -> None:
        super().__init__(parent)
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_MID, 0, 188)

        self.ta.add_style(
            StyleWrapper()
            .bg_color(lv_colors.BLACK)
            .border_width(0)
            .width(lv.SIZE.CONTENT)
            .max_width(432)
            .text_font(font_PJSBOLD48)
            .text_color(lv_colors.WHITE)
            .text_letter_space(12)
            .text_align_center(),
            0,
        )
        self.ta.set_one_line(True)
        self.ta.set_accepted_chars("0123456789")
        self.ta.set_max_length(50)
        self.ta.set_password_mode(True)
        self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        self.nums = [i for i in range(10)]
        if device.is_random_pin_map_enabled():
            random.shuffle(self.nums)
        self.btnm_map = [
            str(self.nums[1]),
            str(self.nums[2]),
            str(self.nums[3]),
            "\n",
            str(self.nums[4]),
            str(self.nums[5]),
            str(self.nums[6]),
            "\n",
            str(self.nums[7]),
            str(self.nums[8]),
            str(self.nums[9]),
            "\n",
            lv.SYMBOL.BACKSPACE,
            str(self.nums[0]),
            lv.SYMBOL.OK,
            "",
        ]
        self.dummy_btnm_map = [
            str(self.nums[1]),
            str(self.nums[2]),
            str(self.nums[3]),
            "\n",
            str(self.nums[4]),
            str(self.nums[5]),
            str(self.nums[6]),
            "\n",
            str(self.nums[7]),
            str(self.nums[8]),
            str(self.nums[9]),
            "\n",
            lv.SYMBOL.CLOSE,
            str(self.nums[0]),
            lv.SYMBOL.OK,
            "",
        ]
        self.ctrl_map = [
            lv.btnmatrix.CTRL.NO_REPEAT
            | lv.btnmatrix.CTRL.CLICK_TRIG
            | lv.btnmatrix.CTRL.POPOVER
        ] * 12
        self.ctrl_map[-1] = (
            lv.btnmatrix.CTRL.NO_REPEAT
            | lv.btnmatrix.CTRL.DISABLED
            | lv.btnmatrix.CTRL.CLICK_TRIG
            | lv.btnmatrix.CTRL.POPOVER
        )
        self.set_map(lv.keyboard.MODE.NUMBER, self.dummy_btnm_map, self.ctrl_map)
        self.set_mode(lv.keyboard.MODE.NUMBER)
        self.set_size(lv.pct(100), 362)

        self.add_style(
            StyleWrapper().bg_color(lv_colors.BLACK).pad_hor(4).pad_gap(2), 0
        )
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK)
            .radius(2)
            .text_font(font_PJSBOLD48),
            lv.PART.ITEMS | lv.STATE.DEFAULT,
        )
        self.add_style(StyleWrapper(), 0)
        self.add_style(
            StyleWrapper().bg_color(lv_colors.ONEKEY_GRAY_3)
            # .transform_height(-2)
            # .transform_width(-2)
            # .transition(DefaultTransition())
            ,
            lv.PART.ITEMS | lv.STATE.PRESSED,
        )
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK_1)
            .text_color(lv_colors.ONEKEY_GRAY),
            lv.PART.ITEMS | lv.STATE.DISABLED,
        )

        self.set_popovers(True)
        self.align(lv.ALIGN.BOTTOM_MID, 0, -4)
        self.set_textarea(self.ta)

        self.input_count_tips = lv.label(parent)
        self.input_count_tips.align(lv.ALIGN.BOTTOM_MID, 0, -378)
        self.input_count_tips.add_style(
            StyleWrapper()
            .text_font(font_STATUS_BAR)
            .text_letter_space(1)
            .text_color(lv_colors.LIGHT_GRAY),
            0,
        )
        self.input_count_tips.add_flag(lv.obj.FLAG.HIDDEN)

        self.add_event_cb(self.event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
        self.add_event_cb(self.event_cb, lv.EVENT.VALUE_CHANGED, None)
        self.add_event_cb(self.event_cb, lv.EVENT.READY, None)
        self.add_event_cb(self.event_cb, lv.EVENT.CANCEL, None)
        self.previous_input_len = 0

    def update_count_tips(self):
        """Update/show tips only when input length larger than 10"""
        input_len = len(self.ta.get_text())
        if input_len >= 10:
            self.input_count_tips.set_text(f"{len(self.ta.get_text())}/50")
            if self.input_count_tips.has_flag(lv.obj.FLAG.HIDDEN):
                self.input_count_tips.clear_flag(lv.obj.FLAG.HIDDEN)
        else:
            if not self.input_count_tips.has_flag(lv.obj.FLAG.HIDDEN):
                self.input_count_tips.add_flag(lv.obj.FLAG.HIDDEN)

    def toggle_number_input_keys(self, enable: bool):
        if enable:
            self.dummy_ctl_map = []
            self.dummy_ctl_map.extend(self.ctrl_map)
            if self.input_len > 3:
                self.dummy_ctl_map[-1] &= (
                    self.dummy_ctl_map[-1] ^ lv.btnmatrix.CTRL.DISABLED
                )
            if self.input_len > 1:
                self.dummy_ctl_map[-3] = (
                    lv.btnmatrix.CTRL.CLICK_TRIG | lv.btnmatrix.CTRL.POPOVER
                )
            else:
                if self.previous_input_len > self.input_len:
                    self.ta.add_flag(lv.obj.FLAG.HIDDEN)
            self.set_map(lv.keyboard.MODE.NUMBER, self.btnm_map, self.dummy_ctl_map)

        else:
            self.dummy_ctl_map = []
            self.dummy_ctl_map.extend(self.ctrl_map)
            for i in range(12):
                if i not in (9, 11):
                    self.dummy_ctl_map[i] |= lv.btnmatrix.CTRL.DISABLED
                elif i == 11:
                    self.dummy_ctl_map[-1] &= (
                        self.dummy_ctl_map[-1] ^ lv.btnmatrix.CTRL.DISABLED
                    )
            self.set_map(lv.keyboard.MODE.NUMBER, self.btnm_map, self.dummy_ctl_map)

    def event_cb(self, event):
        code = event.code
        input_len = len(self.ta.get_text())
        self.input_len = input_len
        self.ta.clear_flag(lv.obj.FLAG.HIDDEN)
        if code == lv.EVENT.DRAW_PART_BEGIN:
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if input_len > 3:
                change_key_bg(dsc, 9, 11, True)
            elif input_len > 0:
                change_key_bg(dsc, 9, 11, True, False)
            else:
                change_key_bg(dsc, 9, 11, False)
                if dsc.id == 9:
                    dsc.rect_dsc.bg_color = lv_colors.ONEKEY_RED_1
                    # dsc.rect_dsc.bg_img_src = "A:/res/keyboard-close.png"
        elif code == lv.EVENT.VALUE_CHANGED:
            utils.lcd_resume()
            motor.vibrate()
            # if input_len > 10:
            #     self.ta.set_cursor_pos(lv.TEXTAREA_CURSOR.LAST)
            if input_len >= 50:
                # disable number keys
                self.toggle_number_input_keys(False)
            elif input_len > 0:
                # enable number keys
                self.toggle_number_input_keys(True)
            else:
                self.set_map(
                    lv.keyboard.MODE.NUMBER, self.dummy_btnm_map, self.ctrl_map
                )
            self.update_count_tips()
            self.previous_input_len = input_len
        elif code in (lv.EVENT.READY, lv.EVENT.CANCEL):
            motor.vibrate()


class PassphraseKeyboard(lv.btnmatrix):
    def __init__(self, parent, max_len) -> None:
        super().__init__(parent)
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_MID, 0, 180)
        self.ta.set_size(464, 264)
        self.ta.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK_3)
            .bg_opa()
            .border_width(1)
            .border_color(lv_colors.ONEKEY_GRAY_2)
            .text_font(font_PJSBOLD36)
            .text_color(lv_colors.WHITE)
            .text_align_left()
            .radius(0)
            .pad_ver(8),
            0,
        )
        # self.ta.set_one_line(True)
        # include NBSP
        self.ta.set_accepted_chars(
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_<>.:@/|*#\\!()+%&-[]?{},'`;\"~$^= "
        )
        self.ta.set_max_length(max_len)
        # self.ta.set_password_mode(True)
        # self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.ta.set_cursor_click_pos(True)
        self.ta.add_state(lv.STATE.FOCUSED)  # make the cursor visible
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        self.btn_map_text_lower = [
            "q",
            "w",
            "e",
            "r",
            "t",
            "y",
            "u",
            "i",
            "o",
            "p",
            "\n",
            " ",
            "a",
            "s",
            "d",
            "f",
            "g",
            "h",
            "j",
            "k",
            "l",
            " ",
            "\n",
            " ",
            "ABC",
            "z",
            "x",
            "c",
            "v",
            "b",
            "n",
            "m",
            " ",
            "\n",
            lv.SYMBOL.BACKSPACE,
            "123",
            " ",
            lv.SYMBOL.OK,
            "",
        ]
        self.btn_map_text_upper = [
            "Q",
            "W",
            "E",
            "R",
            "T",
            "Y",
            "U",
            "I",
            "O",
            "P",
            "\n",
            " ",
            "A",
            "S",
            "D",
            "F",
            "G",
            "H",
            "J",
            "K",
            "L",
            " ",
            "\n",
            " ",
            "abc",
            "Z",
            "X",
            "C",
            "V",
            "B",
            "N",
            "M",
            " ",
            "\n",
            lv.SYMBOL.BACKSPACE,
            "123",
            " ",
            lv.SYMBOL.OK,
            "",
        ]
        self.btn_map_text_special = [
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "0",
            "\n",
            " ",
            "^",
            "_",
            "[",
            "]",
            "@",
            "$",
            "%",
            "{",
            "}",
            " ",
            "\n",
            " ",
            "#*<",
            "`",
            "-",
            "/",
            ",",
            ".",
            ":",
            ";",
            " ",
            "\n",
            lv.SYMBOL.BACKSPACE,
            "abc",
            " ",
            lv.SYMBOL.OK,
            "",
        ]
        self.btn_map_text_special1 = [
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "0",
            "\n",
            " ",
            "!",
            "?",
            "#",
            "~",
            "&",
            '"',
            "'",
            "(",
            ")",
            " ",
            "\n",
            " ",
            "123",
            "+",
            "=",
            "<",
            ">",
            "\\",
            "|",
            "*",
            " ",
            "\n",
            lv.SYMBOL.BACKSPACE,
            "abc",
            " ",
            lv.SYMBOL.OK,
            "",
        ]
        # line1
        self.ctrl_map = [lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 10
        # line2
        self.ctrl_map.extend([3 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.ctrl_map.extend(
            [7 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 9
        )
        self.ctrl_map.extend([3 | lv.btnmatrix.CTRL.NO_REPEAT])
        # line3
        self.ctrl_map.extend([2 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.ctrl_map.extend(
            [7 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.CLICK_TRIG]
        )
        self.ctrl_map.extend(
            [5 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 7
        )
        self.ctrl_map.extend([4 | lv.btnmatrix.CTRL.NO_REPEAT])
        # line4
        self.ctrl_map.extend([3])
        self.ctrl_map.extend(
            [2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.CLICK_TRIG]
        )
        self.ctrl_map.extend(
            [7 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.CLICK_TRIG]
        )
        self.ctrl_map.extend(
            [3 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.CLICK_TRIG]
        )
        self.set_map(self.btn_map_text_lower)
        self.set_ctrl_map(self.ctrl_map)
        self.set_size(lv.pct(100), 294)
        self.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.BLACK)
            .border_width(0)
            .pad_all(0)
            .pad_gap(2),
            0,
        )
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK)
            .radius(2)
            .text_font(font_MONO24)
            .text_letter_space(-1),
            lv.PART.ITEMS | lv.STATE.DEFAULT,
        )
        self.add_style(
            StyleWrapper().bg_color(lv_colors.ONEKEY_GRAY_3),
            lv.PART.ITEMS | lv.STATE.PRESSED,
        )

        self.input_count_tips = lv.label(parent)
        self.input_count_tips.align_to(self, lv.ALIGN.OUT_TOP_MID, 0, -8)
        self.input_count_tips.add_style(
            StyleWrapper()
            .text_font(font_STATUS_BAR)
            .text_letter_space(1)
            .text_color(lv_colors.LIGHT_GRAY),
            0,
        )

        self.update_count_tips()

        self.add_event_cb(self.event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
        self.add_event_cb(self.event_cb, lv.EVENT.VALUE_CHANGED, None)
        self.ta.add_event_cb(self.event_cb, lv.EVENT.FOCUSED, None)
        self.move_foreground()

    def update_count_tips(self):
        self.input_count_tips.set_text(f"{len(self.ta.get_text())}/50")

    def event_cb(self, event):
        code = event.code
        target = event.get_target()
        if code == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.ta.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if len(txt_input) > 0:
                change_key_bg(dsc, 31, 34, True)
            else:
                change_key_bg(dsc, 31, 34, False, allow_empty=True)
            # if dsc.id in (22, 32):
            #     dsc.rect_dsc.bg_color = lv.color_hex(0x191919)
            if dsc.id == 34:
                dsc.rect_dsc.bg_color = lv_colors.ONEKEY_GREEN
            elif dsc.id in (10, 20, 21, 30):
                dsc.rect_dsc.bg_color = lv_colors.BLACK
        elif code == lv.EVENT.VALUE_CHANGED:
            if isinstance(target, lv.btnmatrix):
                utils.lcd_resume()
                btn_id = target.get_selected_btn()
                text = target.get_btn_text(btn_id)
                if text == "":
                    return
                if text == " ":
                    if btn_id in (10, 21):
                        # text = target.get_btn_text(btn_id + 1)
                        target.set_selected_btn(btn_id + 1)
                        return
                    elif btn_id in (20, 30):
                        # text = target.get_btn_text(btn_id - 1)
                        target.set_selected_btn(btn_id - 1)
                        return
                motor.vibrate()
                if text == "ABC":
                    self.set_map(self.btn_map_text_upper)
                    self.set_ctrl_map(self.ctrl_map)
                    return
                elif text == "123":
                    self.set_map(self.btn_map_text_special)
                    self.set_ctrl_map(self.ctrl_map)
                    return
                elif text == "abc":
                    self.set_map(self.btn_map_text_lower)
                    self.set_ctrl_map(self.ctrl_map)
                    return
                elif text == "#*<":
                    self.set_map(self.btn_map_text_special1)
                    self.set_ctrl_map(self.ctrl_map)
                    return
                elif text == lv.SYMBOL.BACKSPACE:
                    self.ta.del_char()
                    self.update_count_tips()
                    return
                elif text == lv.SYMBOL.OK:
                    lv.event_send(self, lv.EVENT.READY, None)
                    return
                self.ta.add_text(text)
                self.update_count_tips()
        elif code == lv.EVENT.FOCUSED and target == self.ta:
            utils.lcd_resume()
