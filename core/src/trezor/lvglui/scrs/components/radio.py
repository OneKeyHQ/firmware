from .. import font_MONO28, font_PJSBOLD30, font_PJSREG30, lv, lv_colors
from ..widgets.style import StyleWrapper
from .container import ContainerFlexCol
from .transition import DefaultTransition


class Radio:
    def __init__(self, parent, options) -> None:
        self.change_color_only = False
        self.container = ContainerFlexCol(parent, None, padding_row=2)
        self.items: list[Radio.RadioItem] = []
        self.check_index = 0
        self.choices = options.split("\n")
        for idx, choice in enumerate(self.choices):
            item = Radio.RadioItem(self.container, choice)
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
            item.set_style_text_font(font, 0)

    class RadioItem(lv.btn):
        def __init__(
            self,
            parent,
            text: str,
        ) -> None:
            super().__init__(parent)
            self.content = text
            self.set_size(464, 94)
            self.add_style(
                StyleWrapper()
                .bg_color(lv_colors.BLACK)
                .bg_opa()
                .radius(0)
                .pad_hor(8)
                .text_font(font_PJSREG30)
                .text_color(lv_colors.WHITE_2)
                .text_align_left(),
                0,
            )
            self.label = lv.label(self)
            self.label.set_long_mode(lv.label.LONG.WRAP)
            self.label.set_text(text)
            self.label.set_align(lv.ALIGN.CENTER)
            self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
            self.checked = False

        def set_checked(self, change_color_only: bool = False):
            if not self.checked:
                self.checked = True
                self.set_style_bg_color(lv_colors.ONEKEY_BLACK_1, 0)
                self.set_style_text_color(lv_colors.WHITE, 0)
                if not change_color_only:
                    self.set_style_text_font(font_PJSBOLD30, 0)

        def set_uncheck(self, change_color_only: bool = False):
            if self.checked:
                self.checked = False
                self.set_style_bg_color(lv_colors.BLACK, 0)
                self.set_style_text_color(lv_colors.WHITE_2, 0)
                if not change_color_only:
                    self.set_style_text_font(font_PJSREG30, 0)

        def get_text(self) -> str:
            return self.content


class RadioTrigger:
    def __init__(self, parent, options, font=font_MONO28) -> None:
        self.parent = parent
        self.container = ContainerFlexCol(parent, None, padding_row=2)
        self.items: list[RadioTrigger.RadioItem] = []
        self.check_index = 0
        self.choices = options.split("\n")
        for _idx, choice in enumerate(self.choices):
            item = RadioTrigger.RadioItem(self.container, choice, font=font)
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
        def __init__(self, parent, text: str, font=font_MONO28) -> None:
            super().__init__(parent)
            self.content = text
            self.set_size(464, 94)
            self.add_style(
                StyleWrapper()
                .bg_color(lv_colors.ONEKEY_BLACK_1)
                .bg_opa()
                .radius(0)
                .pad_hor(8)
                .text_font(font)
                .text_color(lv_colors.WHITE)
                .text_align_left(),
                0,
            )
            self.add_style(
                StyleWrapper().bg_color(lv_colors.ONEKEY_BLACK_2).transform_height(-2)
                # .transform_width(-4)
                .transition(DefaultTransition()),
                lv.PART.MAIN | lv.STATE.PRESSED,
            )
            self.label = lv.label(self)
            self.label.set_long_mode(lv.label.LONG.WRAP)
            self.label.set_text(text)
            self.label.set_align(lv.ALIGN.LEFT_MID)
            self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        def get_text(self) -> str:
            return self.content
