from storage import device
from trezor.crypto import bip39, random

from .. import font_MONO24, font_PJSBOLD20, font_PJSBOLD32, lv, lv_colors


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
) -> None:
    if enabled:
        if dsc.id == id1:
            dsc.rect_dsc.bg_color = lv_colors.ONEKEY_RED_1
        elif dsc.id == id2:
            if all_enabled:
                dsc.rect_dsc.bg_color = lv_colors.ONEKEY_GREEN
            else:
                dsc.rect_dsc.bg_color = lv_colors.ONEKEY_BLACK_1
    else:
        if dsc.id in (id1, id2):
            dsc.rect_dsc.bg_color = lv_colors.ONEKEY_BLACK_1


class BIP39Keyboard(lv.keyboard):
    """character keyboard with textarea."""

    def __init__(self, parent):
        super().__init__(parent)
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_MID, 0, 220)
        self.ta.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.ta.set_style_max_width(300, lv.STATE.DEFAULT)
        self.ta.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.ta.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_font(font_PJSBOLD32, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_max_length(11)
        self.ta.set_one_line(True)
        self.ta.set_accepted_chars("abcdefghijklmnopqrstuvwxyz")
        self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
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
        self.ctrl_map = [lv.btnmatrix.CTRL.NO_REPEAT] * 10
        self.ctrl_map.append(lv.btnmatrix.CTRL.HIDDEN)
        self.ctrl_map.extend(
            [2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 9
        )

        self.ctrl_map.append(lv.btnmatrix.CTRL.HIDDEN)
        self.ctrl_map.extend(
            [4 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.DISABLED]
        )
        self.ctrl_map.extend(
            [3 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 7
        )
        self.ctrl_map.extend(
            [4 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.DISABLED]
        )
        self.dummy_ctl_map = []
        self.dummy_ctl_map.extend(self.ctrl_map)
        # delete button
        self.dummy_ctl_map[21] &= self.dummy_ctl_map[21] ^ lv.btnmatrix.CTRL.DISABLED
        self.set_map(lv.keyboard.MODE.TEXT_LOWER, self.btnm_map, self.ctrl_map)
        self.set_mode(lv.keyboard.MODE.TEXT_LOWER)
        self.set_width(lv.pct(100))
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK, lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_1, lv.PART.ITEMS | lv.STATE.DISABLED
        )
        self.set_style_text_color(
            lv_colors.ONEKEY_GRAY, lv.PART.ITEMS | lv.STATE.DISABLED
        )
        self.set_style_pad_row(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_column(6, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_font(font_MONO24, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_style_radius(8, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_height(168)
        self.align(lv.ALIGN.BOTTOM_MID, 0, -48)
        self.set_popovers(True)
        self.set_textarea(self.ta)
        self.add_event_cb(self.event_cb, lv.EVENT.ALL, None)
        self.mnemonic_prompt = lv.obj(parent)
        self.mnemonic_prompt.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.mnemonic_prompt.align_to(self, lv.ALIGN.OUT_TOP_LEFT, 0, -40)
        self.mnemonic_prompt.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.mnemonic_prompt.set_style_bg_color(
            lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.mnemonic_prompt.set_style_text_font(
            font_MONO24, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.mnemonic_prompt.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.mnemonic_prompt.set_flex_flow(lv.FLEX_FLOW.ROW)
        self.mnemonic_prompt.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

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

    def event_cb(self, event):
        if event.code == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.ta.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if len(txt_input) > 0:
                change_key_bg(dsc, 21, 29, True, self.completed)
            else:
                change_key_bg(dsc, 21, 29, False)
        elif event.code == lv.EVENT.VALUE_CHANGED:
            self.mnemonic_prompt.clean()
            txt_input = self.ta.get_text()
            if len(txt_input) > 0:
                words = bip39.complete_word(txt_input) or ""
                mask = bip39.word_completion_mask(txt_input)
                candidates = words.rstrip().split() if words else []
                for candidate in candidates:
                    btn = lv.btn(self.mnemonic_prompt)
                    btn.set_style_bg_color(
                        lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
                    )
                    btn.set_style_bg_color(
                        lv_colors.ONEKEY_BLACK, lv.PART.MAIN | lv.STATE.CHECKED
                    )
                    btn.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
                    label = lv.label(btn)
                    label.set_style_text_color(
                        lv_colors.WHITE_2, lv.PART.MAIN | lv.STATE.DEFAULT
                    )
                    label.set_style_text_color(
                        lv_colors.WHITE, lv.PART.MAIN | lv.STATE.CHECKED
                    )
                    label.set_style_text_font(
                        font_PJSBOLD20, lv.PART.MAIN | lv.STATE.DEFAULT
                    )
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
        self.ta.align(lv.ALIGN.TOP_MID, 0, 260)
        self.ta.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.ta.set_style_max_width(400, lv.STATE.DEFAULT)
        self.ta.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.ta.set_style_text_letter_space(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_font(font_PJSBOLD32, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_one_line(True)
        self.ta.set_accepted_chars("0123456789")
        self.ta.set_max_length(50)
        self.ta.set_password_mode(True)
        self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
        self.ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        self.nums = [i for i in range(10)]
        if not device.is_order_pin_map():
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
        self.ctrl_map = [lv.btnmatrix.CTRL.NO_REPEAT] * 12
        self.ctrl_map[-1] = lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.DISABLED
        self.set_map(lv.keyboard.MODE.NUMBER, self.dummy_btnm_map, self.ctrl_map)
        self.set_mode(lv.keyboard.MODE.NUMBER)
        self.set_width(lv.pct(96))
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK, lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK_1, lv.PART.ITEMS | lv.STATE.DISABLED
        )
        self.set_style_text_color(
            lv_colors.ONEKEY_GRAY, lv.PART.ITEMS | lv.STATE.DISABLED
        )
        self.set_style_pad_row(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_column(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(36, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_style_text_font(font_PJSBOLD32, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_height(324)
        self.align(lv.ALIGN.BOTTOM_MID, 0, -20)
        self.set_textarea(self.ta)
        self.add_event_cb(self.event_cb, lv.EVENT.DRAW_PART_BEGIN, None)
        self.add_event_cb(self.event_cb, lv.EVENT.VALUE_CHANGED, None)

    def toggle_number_input_keys(self, enable: bool):
        if enable:
            self.dummy_ctl_map = []
            self.dummy_ctl_map.extend(self.ctrl_map)
            if self.input_len > 3:
                self.dummy_ctl_map[-1] &= (
                    self.dummy_ctl_map[-1] ^ lv.btnmatrix.CTRL.DISABLED
                )
            self.set_map(lv.keyboard.MODE.NUMBER, self.btnm_map, self.dummy_ctl_map)

        else:
            self.dummy_ctl_map = []
            self.dummy_ctl_map.extend(self.ctrl_map)
            for i in range(12):
                if i not in (9, 11):
                    self.dummy_ctl_map[i] |= lv.btnmatrix.CTRL.DISABLED
            self.set_map(lv.keyboard.MODE.NUMBER, self.btnm_map, self.dummy_ctl_map)

    def event_cb(self, event):
        code = event.code
        input_len = len(self.ta.get_text())
        self.input_len = input_len
        if code == lv.EVENT.DRAW_PART_BEGIN:
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if input_len > 3:
                change_key_bg(dsc, 9, 11, True)
            elif input_len > 0:
                change_key_bg(dsc, 9, 11, True, False)
            else:
                change_key_bg(dsc, 9, 11, False)
                if dsc.id == 9:
                    dsc.rect_dsc.bg_img_src = "A:/res/keyboard-close.png"
        elif code == lv.EVENT.VALUE_CHANGED:
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


class PassphraseKeyboard(lv.btnmatrix):
    def __init__(self, parent, max_len) -> None:
        super().__init__(parent)
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_MID, 0, 200)
        self.ta.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.ta.set_style_max_width(300, lv.STATE.DEFAULT)
        self.ta.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.ta.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_font(font_PJSBOLD32, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_one_line(True)
        # include NBSP
        self.ta.set_accepted_chars(
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_<>.:@/|*\\!()+%&-[]?{},'`;\"~$^= "
        )
        self.ta.set_max_length(max_len)
        self.ta.set_password_mode(True)
        self.ta.clear_flag(lv.obj.FLAG.CLICKABLE)
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
            "123",
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
        self.ctrl_map.extend(
            [2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.HIDDEN]
        )
        self.ctrl_map.extend(
            [7 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 9
        )
        self.ctrl_map.extend(
            [2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.HIDDEN]
        )
        # line3
        self.ctrl_map.extend(
            [2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.HIDDEN]
        )
        self.ctrl_map.extend([6 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.ctrl_map.extend(
            [5 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 7
        )
        self.ctrl_map.extend(
            [2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.HIDDEN]
        )
        # line4
        self.ctrl_map.extend([3 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.ctrl_map.extend([2 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.ctrl_map.extend([7 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.ctrl_map.extend([3 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.set_map(self.btn_map_text_lower)
        self.set_ctrl_map(self.ctrl_map)

        self.set_size(lv.pct(100), 230)
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK, lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.set_style_pad_row(8, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_column(6, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(8, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_style_text_font(font_MONO24, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.align(lv.ALIGN.BOTTOM_MID, 0, -48)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_all(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.add_event_cb(self.event_cb, lv.EVENT.ALL, None)

    def event_cb(self, event):
        code = event.code
        if code == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.ta.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if len(txt_input) > 0:
                change_key_bg(dsc, 31, 34, True)
            else:
                change_key_bg(dsc, 31, 34, False)
            # if dsc.id in (22, 32):
            #     dsc.rect_dsc.bg_color = lv.color_hex(0x191919)
        elif code == lv.EVENT.VALUE_CHANGED:
            target = event.get_target()
            if isinstance(target, lv.btnmatrix):
                btn_id = target.get_selected_btn()
                text = target.get_btn_text(btn_id)
                if text == "":
                    return
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
                    return
                elif text == lv.SYMBOL.OK:
                    lv.event_send(self, lv.EVENT.READY, None)
                    return
                self.ta.add_text(text)
