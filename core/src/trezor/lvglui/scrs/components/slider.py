from trezor import motor
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

from .. import font_PJSBOLD24, lv, lv_colors

MAX_VISIBLE_VALUE = 177
MIN_VISIBLE_VALUE = 23


class Slider(lv.slider):
    SLIDER_DISABLE_ARROW_IMG_SRC = "A:/res/slide-arrow-disable.png"
    SLIDER_DEFAULT_ARROW_IMG_SRC = "A:/res/slide-arrow-green.png"
    SLIDER_DEFAULT_DONE_IMG_SRC = "A:/res/slide-done-green.png"
    SLIDER_ARROW_WHITE_IMG_SRC = "A:/res/slide-arrow-white.png"
    SLIDER_DONE_WHITE_IMG_SRC = "A:/res/slide-done-white.png"

    def __init__(self, parent, text, relative_y=-140) -> None:
        super().__init__(parent)
        self.disable = False
        self.arrow_img_src = Slider.SLIDER_DEFAULT_ARROW_IMG_SRC
        self.done_img_src = Slider.SLIDER_DEFAULT_DONE_IMG_SRC
        self.set_size(448, 96)
        self.add_flag(lv.obj.FLAG.ADV_HITTEST)
        self.align(lv.ALIGN.BOTTOM_MID, 0, relative_y)
        self.set_range(0, 200)
        self.set_value(MIN_VISIBLE_VALUE, lv.ANIM.OFF)
        self.set_style_anim_time(100, lv.PART.MAIN)

        self.set_style_pad_hor(0, lv.PART.KNOB | lv.STATE.DEFAULT)
        self.set_style_pad_ver(-3, lv.PART.KNOB | lv.STATE.DEFAULT)

        self.set_style_bg_opa(lv.OPA.COVER, lv.PART.KNOB | lv.STATE.DEFAULT)
        self.set_style_bg_opa(lv.OPA.COVER, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_opa(lv.OPA.COVER, lv.PART.INDICATOR | lv.STATE.DEFAULT)

        self.set_style_bg_color(lv_colors.ONEKEY_BLACK, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_bg_color(
            lv_colors.ONEKEY_BLACK, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.set_style_bg_color(lv_colors.WHITE, lv.PART.KNOB | lv.STATE.DEFAULT)
        self.text = text

        self.tips = lv.label(self)
        self.tips.set_style_text_font(font_PJSBOLD24, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.tips.set_style_text_color(
            lv_colors.WHITE_2, lv.PART.MAIN | lv.STATE.DEFAULT
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
                lv_colors.ONEKEY_BLACK, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK, lv.PART.INDICATOR | lv.STATE.DEFAULT
            )
            self.arrow_img_src = Slider.SLIDER_ARROW_WHITE_IMG_SRC
            self.done_img_src = Slider.SLIDER_DONE_WHITE_IMG_SRC
            self.set_style_bg_color(
                lv_colors.ONEKEY_RED_1, lv.PART.KNOB | lv.STATE.DEFAULT
            )
        else:
            self.disable = True
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK_1, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK_1, lv.PART.INDICATOR | lv.STATE.DEFAULT
            )
            self.set_style_bg_color(
                lv_colors.ONEKEY_BLACK, lv.PART.KNOB | lv.STATE.DEFAULT
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
                        motor.vibrate()
                        lv.event_send(self, lv.EVENT.READY, None)
        elif code == lv.EVENT.DRAW_PART_END:
            dsc = lv.obj_draw_part_dsc_t.__cast__(event.get_param())
            if dsc.part == lv.PART.MAIN:
                label_text = self.text
                label_size = lv.point_t()
                lv.txt_get_size(
                    label_size, label_text, font_PJSBOLD24, 0, 0, lv.COORD.MAX, 0
                )
                label_area = lv.area_t()
                label_area.x1 = (
                    dsc.draw_area.x1
                    + dsc.draw_area.get_width() // 2
                    - label_size.x // 2
                )
                label_area.x2 = label_area.x1 + label_size.x
                label_area.y2 = dsc.draw_area.y1 + 60
                label_area.y1 = label_area.y2 - label_size.y
                label_draw_dsc = lv.draw_label_dsc_t()
                label_draw_dsc.init()
                label_draw_dsc.color = (
                    lv_colors.ONEKEY_GRAY if self.disable else lv_colors.WHITE_2
                )
                label_draw_dsc.font = font_PJSBOLD24
                dsc.draw_ctx.label(label_draw_dsc, label_area, label_text, None)
