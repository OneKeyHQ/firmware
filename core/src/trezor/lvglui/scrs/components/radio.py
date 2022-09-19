from .. import font_PJSBOLD24, font_PJSREG24, lv, lv_colors
from .container import ContainerFlexCol


class Radio:
    def __init__(self, parent, options) -> None:
        self.change_color_only = False
        self.container = ContainerFlexCol(parent, None, padding_row=0)
        self.items: list[ButtonCell] = []
        self.check_index = 0
        self.choices = options.split("\n")
        for idx, choice in enumerate(self.choices):
            item = ButtonCell(self.container, choice)
            if idx == 0:
                item.set_checked()
            self.items.append(item)
        self.container.add_event_cb(self.on_selected_changed, lv.EVENT.CLICKED, None)

    def on_selected_changed(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            from trezor import utils

            if utils.lcd_resume():
                return
            last_checked = self.check_index
            for idx, item in enumerate(self.items):

                if target != item and idx == last_checked:
                    item.set_uncheck(change_color_only=self.change_color_only)
                if target == item and idx != last_checked:
                    self.check_index = idx
                    item.set_checked(change_color_only=self.change_color_only)

    def set_flag(self):
        self.change_color_only = True

    def get_selected_index(self):
        return self.check_index

    def get_selected_str(self):
        return self.items[self.check_index].get_text()

    def set_style_text_font(self, font, _other):
        for item in self.items:
            item.set_style_text_font(font, lv.PART.MAIN | lv.STATE.DEFAULT)


class ButtonCell(lv.btn):
    def __init__(
        self,
        parent,
        text: str,
    ) -> None:
        super().__init__(parent)
        self.content = text
        self.set_size(lv.pct(100), 78)
        self.set_style_pad_ver(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_pad_hor(16, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(255, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_font(font_PJSREG24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_color(lv_colors.WHITE_2, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN | lv.STATE.DEFAULT)

        self.label = lv.label(self)
        self.label.set_long_mode(lv.label.LONG.WRAP)
        self.label.set_text(text)
        self.label.set_align(lv.ALIGN.CENTER)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.checked = False

    def set_checked(self, change_color_only: bool = False):
        if not self.checked:
            self.checked = True
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK_1, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.set_style_text_color(lv_colors.WHITE, lv.PART.MAIN | lv.STATE.DEFAULT)
            if not change_color_only:
                self.set_style_text_font(
                    font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT
                )

    def set_uncheck(self, change_color_only: bool = False):
        if self.checked:
            self.checked = False
            self.set_style_bg_color(lv_colors.BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
            self.set_style_text_color(
                lv_colors.WHITE_2, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            if not change_color_only:
                self.set_style_text_font(font_PJSREG24, lv.PART.MAIN | lv.STATE.DEFAULT)

    def get_text(self) -> str:
        return self.content
