from trezor import motor
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

from .. import font_PJSREG30, lv, lv_colors
from ..widgets.style import StyleWrapper

MAX_VISIBLE_VALUE = 175
MIN_VISIBLE_VALUE = 20


class Slider(lv.slider):
    SLIDER_DISABLE_ARROW_IMG_SRC = "A:/res/slide-arrow-disable.png"
    SLIDER_DEFAULT_ARROW_IMG_SRC = "A:/res/slide-arrow-black.png"
    SLIDER_DEFAULT_DONE_IMG_SRC = "A:/res/slide-done-black.png"
    SLIDER_ARROW_WHITE_IMG_SRC = "A:/res/slide-arrow-white.png"
    SLIDER_DONE_WHITE_IMG_SRC = "A:/res/slide-done-white.png"

    def __init__(self, parent, text, relative_y=-122) -> None:
        super().__init__(parent)
        self.remove_style_all()
        self.disable = False
        self.arrow_img_src = Slider.SLIDER_DEFAULT_ARROW_IMG_SRC
        self.done_img_src = Slider.SLIDER_DEFAULT_DONE_IMG_SRC
        self.set_size(464, 98)
        self.add_flag(lv.obj.FLAG.ADV_HITTEST)
        self.align(lv.ALIGN.BOTTOM_MID, 0, relative_y)
        self.set_range(0, 200)
        self.set_value(MIN_VISIBLE_VALUE, lv.ANIM.OFF)
        self.set_style_anim_time(100, lv.PART.MAIN)

        self.add_style(
            StyleWrapper()
            .border_width(1)
            .border_color(lv_colors.WHITE)
            .bg_color(lv_colors.ONEKEY_BLACK_4)
            .bg_opa(lv.OPA.COVER)
            .pad_all(3)
            .radius(2),
            lv.PART.MAIN,
        )
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.WHITE)
            .bg_opa(lv.OPA.COVER)
            .width(104)
            .height(90)
            .pad_ver(-4)
            .pad_left(-5)
            .pad_right(8)
            .radius(2),
            lv.PART.KNOB,
        )
        self.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK_4)
            .bg_opa(lv.OPA.COVER)
            .radius(0),
            lv.PART.INDICATOR,
        )
        self.text = text

        self.tips = lv.label(self)
        self.tips.add_style(
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_letter_space(-1)
            .text_color(lv_colors.ONEKEY_WHITE_4),
            0,
        )
        self.tips.set_text(_(i18n_keys.BUTTON__PROCESSING))
        self.tips.center()
        self.tips.add_flag(lv.obj.FLAG.HIDDEN)

        self.add_event_cb(self.on_event, lv.EVENT.PRESSING, None)
        self.add_event_cb(self.on_event, lv.EVENT.RELEASED, None)
        self.add_event_cb(self.on_event, lv.EVENT.DRAW_PART_BEGIN, None)
        self.add_event_cb(self.on_event, lv.EVENT.DRAW_PART_END, None)

    def enable(self, enable: bool = True):
        if enable:
            self.disable = False
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK_4, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK_4, lv.PART.INDICATOR | lv.STATE.DEFAULT
            )
            self.arrow_img_src = Slider.SLIDER_ARROW_WHITE_IMG_SRC
            self.done_img_src = Slider.SLIDER_DONE_WHITE_IMG_SRC
            self.set_style_bg_color(
                lv_colors.ONEKEY_RED_1, lv.PART.KNOB | lv.STATE.DEFAULT
            )
            self.set_style_border_color(lv_colors.ONEKEY_GRAY, 0)
            self.tips.set_style_text_color(lv_colors.ONEKEY_WHITE_4, 0)
        else:
            self.disable = True
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK_3, lv.PART.KNOB | lv.STATE.DEFAULT
            )
            self.set_style_border_color(lv_colors.ONEKEY_GRAY_1, 0)
            self.tips.set_style_text_color(lv_colors.WHITE_2, 0)

    def change_knob_style(self, level):
        if level == 1:
            self.add_style(
                StyleWrapper().bg_color(lv_colors.ONEKEY_YELLOW),
                lv.PART.KNOB | lv.STATE.DEFAULT,
            )
            # self.arrow_img_src = "A:/res/slide-arrow-black.png"
            # self.done_img_src = "A:/res/slider-done-black.png"
        elif level == 2:
            self.slider.add_style(
                StyleWrapper().bg_color(lv_colors.ONEKEY_RED_1),
                lv.PART.KNOB | lv.STATE.DEFAULT,
            )

    def on_event(self, event):
        code = event.code
        target = event.get_target()
        current_value = target.get_value()
        if code == lv.EVENT.PRESSING:
            if current_value > MAX_VISIBLE_VALUE:
                self.set_value(MAX_VISIBLE_VALUE, lv.ANIM.OFF)
            elif current_value < MIN_VISIBLE_VALUE:
                self.set_value(MIN_VISIBLE_VALUE, lv.ANIM.OFF)
        elif code == lv.EVENT.RELEASED:
            if current_value < MAX_VISIBLE_VALUE:
                self.set_value(MIN_VISIBLE_VALUE, lv.ANIM.ON)
        elif code == lv.EVENT.DRAW_PART_BEGIN:
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if dsc.part == lv.PART.KNOB:
                if dsc.id == 0:
                    if current_value < MAX_VISIBLE_VALUE:
                        if self.disable:
                            dsc.rect_dsc.bg_img_src = (
                                Slider.SLIDER_DISABLE_ARROW_IMG_SRC
                            )
                        else:
                            dsc.rect_dsc.bg_img_src = self.arrow_img_src
                    else:
                        self.tips.clear_flag(lv.obj.FLAG.HIDDEN)
                        dsc.rect_dsc.bg_img_src = self.done_img_src
                        if self.has_flag(lv.obj.FLAG.CLICKABLE):
                            self.clear_flag(lv.obj.FLAG.CLICKABLE)
                        else:
                            return
                        motor.vibrate()
                        lv.event_send(self, lv.EVENT.READY, None)
        elif code == lv.EVENT.DRAW_PART_END:
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if dsc.part == lv.PART.MAIN:
                label_text = self.text
                label_size = lv.point_t()
                lv.txt_get_size(
                    label_size, label_text, font_PJSREG30, -1, 0, lv.COORD.MAX, 0
                )
                label_area = lv.area_t()
                label_area.x1 = (
                    dsc.draw_area.x1
                    + dsc.draw_area.get_width() // 2
                    - min(98, label_size.x // 2)
                )
                label_area.x2 = dsc.draw_area.x2 - 8
                label_area.y1 = (
                    dsc.draw_area.y1
                    + dsc.draw_area.get_height() // 2
                    - (label_size.y if label_size.x > 300 else label_size.y // 2)
                )
                label_area.y2 = dsc.draw_area.y2 - 8
                label_draw_dsc = lv.draw_label_dsc_t()
                label_draw_dsc.init()
                label_draw_dsc.color = (
                    lv_colors.WHITE_2 if self.disable else lv_colors.ONEKEY_WHITE_4
                )
                label_draw_dsc.font = font_PJSREG30
                dsc.draw_ctx.label(label_draw_dsc, label_area, label_text, None)
