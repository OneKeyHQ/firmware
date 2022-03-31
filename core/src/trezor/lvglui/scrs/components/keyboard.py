from ..common import *  # noqa: F401,F403
from ...lv_colors import lv_colors
from trezor.crypto import random



class CharacterKeyboard(lv.keyboard):
    """character keyboard with textarea."""

    def __init__(self, parent):
        super().__init__(parent)
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_MID, 0, 200)
        self.ta.set_size(200, lv.SIZE.CONTENT)
        self.ta.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_max_length(11)
        self.ta.set_one_line(True)
        self.ta.set_accepted_chars("abcdefghijklmnopqrstuvwxyz")
        btnm_map = ["q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
                    " ", "a", "s", "d", "f", "g", "h", "j", "k", "l", " ", "\n",
                    lv.SYMBOL.BACKSPACE, "z", "x", "c", "v", "b", "n", "m", lv.SYMBOL.OK, ""]
        ctrl_map = [lv.btnmatrix.CTRL.NO_REPEAT] * 10
        ctrl_map.append(lv.btnmatrix.CTRL.HIDDEN)
        ctrl_map.extend([2 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 9)

        ctrl_map.append(lv.btnmatrix.CTRL.HIDDEN)
        ctrl_map.extend([4 | lv.btnmatrix.CTRL.NO_REPEAT])
        ctrl_map.extend([3 | lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 7)
        ctrl_map.extend([4 | lv.btnmatrix.CTRL.NO_REPEAT])
        self.set_map(lv.keyboard.MODE.TEXT_LOWER, btnm_map, ctrl_map)
        self.set_mode(lv.keyboard.MODE.TEXT_LOWER)
        self.set_width(lv.pct(100))
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x323232), lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_style_pad_row(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_column(6, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(8, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_height(208)
        self.set_y(-48)
        self.set_popovers(True)
        self.set_textarea(self.ta)
        self.add_event_cb(self.event_cb, lv.EVENT.ALL, None)

    def event_cb(self, event):
        if event.code == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.ta.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if len(txt_input) > 0:
                if dsc.id == 22:
                    dsc.rect_dsc.bg_color = lv.color_hex(0xAF2B0E)
                elif dsc.id == 30:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x1b7735)
            else:
                if dsc.id == 22:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x191919)
                elif dsc.id == 30:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x191919)



class NumberKeyboard(lv.keyboard):
    """number keyboard with textarea."""

    def __init__(self, parent) -> None:
        super().__init__(parent)
        self.ta = lv.textarea(parent)
        self.ta.align(lv.ALIGN.TOP_MID, 0, 200)
        self.ta.set_size(200, lv.SIZE.CONTENT)
        self.ta.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.ta.set_one_line(True)
        self.ta.set_accepted_chars("0123456789")
        self.ta.set_max_length(10)
        self.ta.set_password_mode(True)
        nums = [i for i in range(10)]
        random.shuffle(nums)
        btnm_map = [str(nums[0]), str(nums[1]), str(nums[2]), "\n",
                    str(nums[3]), str(nums[4]), str(nums[5]), "\n",
                    str(nums[6]), str(nums[7]), str(nums[8]), "\n",
                    lv.SYMBOL.BACKSPACE, str(nums[9]), lv.SYMBOL.OK, ""]
        ctrl_map = [lv.btnmatrix.CTRL.NO_REPEAT] * 12
        self.set_map(lv.keyboard.MODE.NUMBER, btnm_map, ctrl_map)
        self.set_mode(lv.keyboard.MODE.NUMBER)
        self.set_width(lv.pct(82))
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv.color_hex(0x323232), lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_style_pad_row(12, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_column(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_radius(30, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.set_height(292)
        self.set_y(-48)
        self.set_popovers(True)
        self.set_textarea(self.ta)
        self.add_event_cb(self.event_cb, lv.EVENT.ALL, None)

    def event_cb(self, event):
        if event.code == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.ta.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if len(txt_input) > 0:
                if dsc.id == 9:
                    dsc.rect_dsc.bg_color = lv.color_hex(0xAF2B0E)
                elif dsc.id == 11:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x1b7735)
            else:
                if dsc.id == 9:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x191919)
                elif dsc.id == 11:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x191919)
