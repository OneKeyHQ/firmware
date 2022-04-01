from typing import TYPE_CHECKING

from trezor import log, loop, utils

import lvgl as lv

from .lv_styles import *

if TYPE_CHECKING:
    from typing import Any

# if utils.EMULATOR:
#     font_siyuan32 = lv.font_load("A:/res/ui_font_siyuan32.bin")


class Screen(lv.obj):

    # def __new__(cls, *args, **kwargs):
    #     if not hasattr(cls, '_instance'):
    #         log.debug(__name__, "+++++++++create scteen+++++++++")
    #         cls._instance = super().__new__(cls)
    #     return cls._instance

    def __init__(self):
        super().__init__()
        log.debug(__name__, "+++++++++create scteen+++++++++")
        prev_screen = lv.scr_act()
        self.lv_chan = loop.chan()
        self.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_font(lv.font_montserrat_32, lv.PART.MAIN | lv.STATE.DEFAULT)
        lv.scr_load(self)
        prev_screen.delete()

    async def response(self) -> Any:
        return await self.lv_chan.take()

    def btn_cancel_eventhandler(self, evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.lv_chan.publish("cancel")

    def btn_confirm_eventhandler(self, evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.lv_chan.publish("confirm")


class Screen_Generic(Screen):
    def __init__(
        self,
        return_btn: bool = False,
        cancel_btn: bool = False,
        icon: str | None = None,
        title: str | None = None,
        description: str | None = None,
        confirm_text: str | None = None,
        cancel_text: str | None = None,
    ):
        super().__init__()
        icon_offset_y = 0
        if return_btn:
            self.btn_return = lv.btn(self)
            self.btn_return.set_size(64, 64)
            self.btn_return.set_pos(0, 72)
            self.btn_return.set_align(lv.ALIGN.TOP_LEFT)
            self.btn_return.add_event_cb(
                self.btn_cancel_eventhandler, lv.EVENT.CLICKED, None
            )
            self.btn_return.set_style_bg_opa(
                lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.btn_return.set_style_shadow_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

            self.img_return = lv.img(self.btn_return)
            if utils.EMULATOR:
                self.img_return.set_src("A:/res/nav-arrow-left.png")
            else:
                self.img_return.set_src(lv.SYMBOL.LEFT)

            self.img_return.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
            self.img_return.set_align(lv.ALIGN.TOP_LEFT)

        if icon:
            icon_offset_y = 120
            self.img_icon = lv.img(self)
            self.img_icon.set_src(icon)
            self.img_icon.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
            self.img_icon.set_pos(0, 140)
            self.img_icon.set_align(lv.ALIGN.TOP_MID)

        if title:
            self.label_title = lv.label(self)
            self.label_title.set_long_mode(lv.label.LONG.WRAP)
            self.label_title.set_text(title)
            self.label_title.set_size(416, lv.SIZE.CONTENT)
            self.label_title.set_pos(32, 132 + icon_offset_y)
            self.label_title.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.label_title.set_style_text_opa(
                lv.OPA.COVER, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.label_title.set_style_text_align(
                lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
            )

        if description:
            self.label_description = lv.label(self)
            self.label_description.set_long_mode(lv.label.LONG.WRAP)
            self.label_description.set_text(description)
            self.label_description.set_size(416, lv.SIZE.CONTENT)
            self.label_description.set_pos(32, 200 + icon_offset_y)
            self.label_description.set_align(lv.ALIGN.TOP_LEFT)
            self.label_description.set_style_text_color(
                lv.color_hex(0xCCCCCC), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.label_description.set_style_text_align(
                lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.label_description.set_style_text_font(
                lv.font_montserrat_20, lv.PART.MAIN | lv.STATE.DEFAULT
            )

        if cancel_btn:
            self.btn_cancel = lv.btn(self)
            self.btn_cancel.set_size(192, 62)
            self.btn_cancel.set_pos(32, -64)
            self.btn_cancel.set_align(lv.ALIGN.BOTTOM_LEFT)
            self.btn_cancel.add_event_cb(
                self.btn_cancel_eventhandler, lv.EVENT.CLICKED, None
            )
            self.btn_cancel.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.btn_cancel.set_style_bg_color(
                lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.btn_cancel.set_style_shadow_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

            if not cancel_text:
                self.img_cancel = lv.img(self.btn_cancel)
                if utils.EMULATOR:
                    self.img_cancel.set_src("A:/res/close.png")
                else:
                    self.img_cancel.set_src(lv.SYMBOL.CLOSE)

                self.img_cancel.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
                self.img_cancel.set_pos(0, 0)
                self.img_cancel.set_align(lv.ALIGN.CENTER)
            else:
                self.label_cancel = lv.label(self.btn_cancel)
                self.label_cancel.set_long_mode(lv.label.LONG.WRAP)
                self.label_cancel.set_text(cancel_text)
                self.label_cancel.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
                self.label_cancel.set_pos(0, 0)
                self.label_cancel.set_align(lv.ALIGN.CENTER)
                self.label_cancel.set_style_text_color(
                    lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
                )
                self.label_cancel.set_style_text_font(
                    lv.font_montserrat_20, lv.PART.MAIN | lv.STATE.DEFAULT
                )

        if confirm_text:
            self.btn_confirm = lv.btn(self)

            if cancel_btn:
                self.btn_confirm.set_size(192, 62)
            else:
                self.btn_confirm.set_size(320, 62)

            if cancel_btn:
                self.btn_confirm.set_pos(-32, -64)
                self.btn_confirm.set_align(lv.ALIGN.BOTTOM_RIGHT)
            else:
                self.btn_confirm.set_pos(0, -64)
                self.btn_confirm.set_align(lv.ALIGN.BOTTOM_MID)

            self.btn_confirm.add_event_cb(
                self.btn_confirm_eventhandler, lv.EVENT.CLICKED, None
            )
            self.btn_confirm.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.btn_confirm.set_style_bg_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.btn_confirm.set_style_shadow_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

            self.label_confirm = lv.label(self.btn_confirm)
            self.label_confirm.set_long_mode(lv.label.LONG.WRAP)
            self.label_confirm.set_text(confirm_text)
            self.label_confirm.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
            self.label_confirm.set_pos(0, 0)
            self.label_confirm.set_align(lv.ALIGN.CENTER)
            self.label_confirm.set_style_text_color(
                lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.label_confirm.set_style_text_font(
                lv.font_montserrat_20, lv.PART.MAIN | lv.STATE.DEFAULT
            )


class WordsGrid:
    def __init__(self, parent, words):
        col_dsc = [130, 130, lv.GRID_TEMPLATE.LAST]
        row_dsc = [32, 32, 32, 32, 32, 32, lv.GRID_TEMPLATE.LAST]

        # Create a container with grid
        self.cont = lv.obj(parent)
        self.cont.set_style_grid_column_dsc_array(col_dsc, 0)
        self.cont.set_style_grid_row_dsc_array(row_dsc, 0)
        self.cont.set_size(368, 280)
        self.cont.set_align(lv.ALIGN.TOP_LEFT)
        self.cont.set_pos(56, 258)
        self.cont.set_layout(lv.LAYOUT_GRID.value)
        self.cont.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.cont.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.MAIN | lv.STATE.DEFAULT
        )

        i = 0
        for index, word in words:
            col = i % 2
            row = i // 2
            i += 1

            label = lv.label(self.cont)
            # Stretch the cell horizontally and vertically too
            # Set span to 1 to make the cell 1 column/row sized
            label.set_grid_cell(
                lv.GRID_ALIGN.STRETCH, col, 1, lv.GRID_ALIGN.STRETCH, row, 1
            )
            label.set_recolor(True)

            label.set_text(str(index + 1) + "#666666  : #" + word)
            label.center()
            label.set_style_text_font(
                lv.font_montserrat_20, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            label.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )


class CandidateWordList:
    def __init__(self, parent, words):
        self.current_btn = None
        self.parent = parent
        self.list_candidate = lv.list(parent)
        self.list_candidate.set_style_pad_row(5, 0)
        self.list_candidate.set_size(lv.pct(100), 200)
        self.list_candidate.set_pos(0, 392)
        self.list_candidate.set_align(lv.ALIGN.TOP_LEFT)
        self.list_candidate.set_style_bg_opa(
            lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.list_candidate.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

        for i in range(3):
            btn = lv.btn(self.list_candidate)
            btn.set_size(416, 48)
            btn.center()
            btn.set_style_bg_color(
                lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            btn.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
            btn.add_event_cb(self.btn_event_handler, lv.EVENT.CLICKED, None)
            label = lv.label(btn)
            label.set_text(words[i])
            label.center()
            label.set_style_text_font(
                lv.font_montserrat_20, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            label.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )

    def btn_event_handler(self, evt):
        event = evt.code
        btn = evt.get_target()
        if event == lv.EVENT.CLICKED:
            self.current_btn = btn

        parent = btn.get_parent()
        for i in range(parent.get_child_cnt()):
            child = parent.get_child(i)
            if child == self.current_btn:
                child.set_style_bg_color(
                    lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
                )
            else:
                child.set_style_bg_color(
                    lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
                )


class Screen_ShowWords(Screen_Generic):
    def __init__(
        self,
        share_words,
        title: str | None = None,
        description: str | None = None,
        confirm_text: str | None = None,
    ):
        super().__init__(
            cancel_btn=False,
            title=title,
            description=description,
            confirm_text=confirm_text,
        )

        self.grid = WordsGrid(self, share_words)


class Screen_CandidateWords(Screen_Generic):
    def __init__(
        self,
        share_words,
        title: str | None = None,
        description: str | None = None,
        confirm_text: str | None = None,
    ):
        super().__init__(
            cancel_btn=False,
            title=title,
            description=description,
            confirm_text=confirm_text,
        )
        self.current_btn = None
        self.list_candidate = lv.list(self)
        self.list_candidate.set_style_pad_row(5, 0)
        self.list_candidate.set_size(lv.pct(100), 200)
        self.list_candidate.set_pos(0, 392)
        self.list_candidate.set_align(lv.ALIGN.TOP_LEFT)
        self.list_candidate.set_style_bg_opa(
            lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.list_candidate.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

        for i in range(3):
            btn = lv.btn(self.list_candidate)
            btn.set_size(416, 48)
            btn.center()
            btn.set_style_bg_color(
                lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            btn.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
            btn.add_event_cb(self.btn_event_handler, lv.EVENT.CLICKED, None)
            btn.set_style_shadow_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

            label = lv.label(btn)
            label.set_text(share_words[i])
            label.center()
            label.set_style_text_font(
                lv.font_montserrat_20, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            label.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )

    def btn_event_handler(self, evt):
        event = evt.code
        btn = evt.get_target()
        if event == lv.EVENT.CLICKED:
            self.current_btn = btn

        parent = btn.get_parent()
        for i in range(parent.get_child_cnt()):
            child = parent.get_child(i)
            if child == self.current_btn:
                child.set_style_bg_color(
                    lv.color_hex(0x191919), lv.PART.MAIN | lv.STATE.DEFAULT
                )
            else:
                child.set_style_bg_color(
                    lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
                )

    def btn_confirm_eventhandler(self, evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            if self.current_btn:
                txt = self.list_candidate.get_btn_text(self.current_btn)
                self.lv_chan.publish(txt)


class Screen_WordCount(Screen_Generic):
    def __init__(
        self,
        title: str | None = None,
        description: str | None = None,
        confirm_text: str | None = None,
    ):
        super().__init__(
            cancel_btn=False,
            title=title,
            description=description,
            confirm_text=confirm_text,
        )
        btnm_map = [
            "12",
            "18",
            "20",
            "\n",
            "24",
            "33",
            "",
        ]

        # self.button_style = ButtonStyle()

        self.btnm = lv.btnmatrix(self)
        self.btnm.set_map(btnm_map)
        self.btnm.set_size(lv.pct(80), 120)
        self.btnm.align(lv.ALIGN.BOTTOM_MID, 0, -30)
        self.btnm.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btnm.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btnm.set_style_pad_column(60, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.btnm.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.btnm.set_style_text_font(
            lv.font_montserrat_20, lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.btnm.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.btnm.set_style_shadow_width(0, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.btnm.add_event_cb(self.btnm_event_handler, lv.EVENT.VALUE_CHANGED, None)

    def btnm_event_handler(self, evt):
        event = evt.code
        obj = evt.get_target()
        if event == lv.EVENT.VALUE_CHANGED:
            btn = evt.get_target()
            txt = btn.get_btn_text(btn.get_selected_btn())
            self.lv_chan.publish(txt)


class Screen_WordInput(Screen_Generic):
    def __init__(
        self,
        title: str | None = None,
        description: str | None = None,
        confirm_text: str | None = None,
    ):
        super().__init__(
            cancel_btn=False,
            title=title,
            description=description,
            confirm_text=confirm_text,
        )
        self.textarea_word = lv.textarea(self)

        self.textarea_word.set_accepted_chars(None)

        self.textarea_word.set_max_length(11)
        self.textarea_word.set_text("")
        self.textarea_word.set_placeholder_text("")
        self.textarea_word.set_size(416, 64)
        self.textarea_word.align(lv.ALIGN.TOP_MID, 0, 217)

        self.textarea_word.add_event_cb(
            self.textarea_input_eventhandler, lv.EVENT.ALL, None
        )
        self.textarea_word.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.textarea_word.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.textarea_word.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.textarea_word.set_style_bg_opa(
            lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT
        )

        btnm_map = [
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
            "a",
            "s",
            "d",
            "f",
            "g",
            "h",
            "j",
            "k",
            "l",
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

        ctrl_map = [lv.btnmatrix.CTRL.NO_REPEAT | lv.btnmatrix.CTRL.POPOVER] * 28

        self.keyboard = lv.keyboard(self)
        self.keyboard.set_map(lv.keyboard.MODE.TEXT_LOWER, btnm_map, ctrl_map)

        self.keyboard.set_mode(self.keyboard.MODE.TEXT_LOWER)
        self.keyboard.set_height(240)
        self.keyboard.set_width(lv.pct(100))
        self.keyboard.set_popovers(True)

        self.keyboard.align(lv.ALIGN.BOTTOM_MID, 0, 0)

        self.keyboard.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.keyboard.set_style_pad_row(5, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.keyboard.set_style_pad_column(1, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.keyboard.set_style_radius(5, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.keyboard.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.keyboard.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.keyboard.set_style_text_font(
            lv.font_montserrat_20, lv.PART.ITEMS | lv.STATE.DEFAULT
        )

        self.keyboard.set_textarea(self.textarea_word)

    def textarea_input_eventhandler(self, evt):
        event = evt.code
        text_area = evt.get_target()
        if event == lv.EVENT.CLICKED:
            pass
        elif event == lv.EVENT.READY:
            word = text_area.get_text()
            if len(word) > 0:
                self.lv_chan.publish(word)


class Screen_InputPIN(Screen_Generic):
    def __init__(
        self,
        title: str | None = None,
        description: str | None = None,
        allow_cancel: bool = True,
    ):
        super().__init__(return_btn=allow_cancel, title=title, description=description)

        self.textarea_pin = lv.textarea(self)

        self.textarea_pin.set_accepted_chars("0123456789:")

        self.textarea_pin.set_max_length(9)
        self.textarea_pin.set_text("")
        self.textarea_pin.set_placeholder_text("")
        self.textarea_pin.set_one_line(True)
        self.textarea_pin.set_password_mode(True)

        self.textarea_pin.set_size(416, lv.SIZE.CONTENT)
        self.textarea_pin.set_pos(32, 217)
        self.textarea_pin.set_align(lv.ALIGN.TOP_LEFT)

        self.textarea_pin.add_event_cb(
            self.textarea_pin_eventhandler, lv.EVENT.ALL, None
        )
        self.textarea_pin.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.textarea_pin.set_style_text_opa(
            lv.OPA.COVER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.textarea_pin.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.textarea_pin.set_style_text_align(
            lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.textarea_pin.set_style_bg_color(
            lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
        )
        self.textarea_pin.set_style_bg_opa(
            lv.OPA.COVER, lv.PART.MAIN | lv.STATE.DEFAULT
        )

        self.btnm_map = [
            "1",
            "2",
            "3",
            "\n",
            "4",
            "5",
            "6",
            "\n",
            "7",
            "8",
            "9",
            "\n",
            lv.SYMBOL.CLOSE,
            "0",
            lv.SYMBOL.OK,
            "",
        ]

        self.btnm_map1 = [
            "1",
            "2",
            "3",
            "\n",
            "4",
            "5",
            "6",
            "\n",
            "7",
            "8",
            "9",
            "\n",
            lv.SYMBOL.BACKSPACE,
            "0",
            lv.SYMBOL.OK,
            "",
        ]

        self.btnm = lv.btnmatrix(self)
        self.btnm.set_size(400, 320)
        self.btnm.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        self.btnm.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btnm.set_style_bg_opa(lv.OPA.TRANSP, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.btnm.add_event_cb(self.btnm_event_handler, lv.EVENT.ALL, None)
        self.btnm.set_style_radius(30, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.btnm.set_style_bg_color(
            lv.color_hex(0x323232), lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.btnm.set_style_bg_opa(lv.OPA.COVER, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.btnm.set_style_text_color(
            lv.color_hex(0xFFFFFF), lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.btnm.set_style_text_font(
            lv.font_montserrat_20, lv.PART.ITEMS | lv.STATE.DEFAULT
        )
        self.btnm.set_style_shadow_width(0, lv.PART.ITEMS | lv.STATE.DEFAULT)
        self.btnm.clear_flag(
            lv.obj.FLAG.CLICK_FOCUSABLE
        )  # To keep the text area focused on button clicks
        self.btnm.set_map(self.btnm_map)

    def btnm_event_handler(self, evt):
        event = evt.code
        obj = evt.get_target()
        if event == lv.EVENT.VALUE_CHANGED:
            btn = evt.get_target()
            txt = btn.get_btn_text(btn.get_selected_btn())
            if txt == lv.SYMBOL.BACKSPACE:
                self.textarea_pin.del_char()
            elif txt == lv.SYMBOL.OK:
                lv.event_send(self.textarea_pin, lv.EVENT.READY, None)
            elif txt:
                self.textarea_pin.add_text(txt)

        if event == lv.EVENT.DRAW_PART_BEGIN:
            txt_input = self.textarea_pin.get_text()
            dsc = lv.obj_draw_part_dsc_t.__cast__(evt.get_param())
            if len(txt_input) > 0:
                if dsc.id == 9:
                    dsc.rect_dsc.bg_color = lv.color_hex(0xAF2B0E)
                elif dsc.id == 11:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x1B7735)

                self.btnm.set_map(self.btnm_map1)

            else:
                if dsc.id == 9:
                    dsc.rect_dsc.bg_color = lv.color_hex(0xAF2B0E)
                elif dsc.id == 11:
                    dsc.rect_dsc.bg_color = lv.color_hex(0x191919)

                self.btnm.set_map(self.btnm_map)

    def textarea_pin_eventhandler(self, evt):
        event = evt.code
        text_area = evt.get_target()
        if event == lv.EVENT.CLICKED:
            pass
        elif event == lv.EVENT.READY:
            pin = text_area.get_text()
            self.lv_chan.publish(pin)


class Screen_Home(Screen):
    def __init__(
        self,
        title: str | None = None,
        description: str | None = None,
    ):
        super().__init__()

        self.add_event_cb(self.screen_eventhandler, lv.EVENT.CLICKED, None)

        img_logo = lv.img(self)
        if utils.EMULATOR:
            img_logo.set_src("A:/res/logo.png")
        else:
            img_logo.set_src(lv.SYMBOL.IMAGE)

        img_logo.set_pos(0, 0)
        img_logo.set_align(lv.ALIGN.CENTER)

        if title:
            lable_title = lv.label(self)
            lable_title.set_long_mode(lv.label.LONG.WRAP)
            lable_title.set_text(title)
            lable_title.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
            lable_title.set_pos(lv.pct(0), lv.pct(10))
            lable_title.set_align(lv.ALIGN.CENTER)
            lable_title.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )
        if description:
            lable_description = lv.label(self)
            lable_description.set_long_mode(lv.label.LONG.WRAP)
            lable_description.set_text(description)
            lable_description.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)  # 1
            lable_description.set_pos(lv.pct(0), lv.pct(15))
            lable_description.set_align(lv.ALIGN.CENTER)
            lable_description.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )

    def screen_eventhandler(self, evt):
        event = evt.code
        if event == lv.EVENT.CLICKED:
            self.lv_chan.publish("clicked")
