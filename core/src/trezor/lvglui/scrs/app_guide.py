import storage.device
from trezor import loop, uart, utils
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs.components.button import NormalButton

from . import font_PJSBOLD72, font_PJSREG30, lv, lv_colors
from .widgets.style import StyleWrapper

CHANNEL = loop.chan()


async def request():
    await CHANNEL.take()


def signal():
    CHANNEL.publish(1)


class GuideAppDownload(lv.obj):
    def __init__(self):
        super().__init__(lv.scr_act())
        self.set_size(lv.pct(100), lv.pct(100))
        self.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.set_style_bg_color(lv_colors.BLACK, 0)
        self.set_style_pad_all(0, 0)
        self.set_style_border_width(0, 0)
        self.set_style_radius(0, 0)

        self.process_bar = lv.bar(self)
        self.process_bar.set_size(464, 4)
        self.process_bar.align(lv.ALIGN.TOP_MID, 0, 44)
        self.process_bar.set_style_radius(0, 0)
        self.process_bar.set_style_bg_color(lv_colors.ONEKEY_BLACK, 0)
        self.process_bar.set_style_bg_color(
            lv_colors.WHITE, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.process_bar.set_value(33, lv.ANIM.OFF)

        self.content_area = lv.obj(self)
        self.content_area.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.content_area.align_to(self.process_bar, lv.ALIGN.OUT_BOTTOM_MID, 0, 0)
        self.content_area.set_style_bg_color(lv_colors.BLACK, 0)
        self.content_area.set_style_bg_color(
            lv_colors.WHITE_3, lv.PART.SCROLLBAR | lv.STATE.DEFAULT
        )
        self.content_area.set_style_pad_all(0, 0)
        self.content_area.set_style_border_width(0, 0)
        self.content_area.set_style_radius(0, 0)
        self.content_area.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        self.content_area.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        self.content_area.set_style_max_height(622, 0)
        self.content_area.set_style_min_height(400, 0)

        self.label1 = lv.label(self.content_area)
        self.label1.add_style(
            StyleWrapper()
            .text_font(font_PJSBOLD72)
            .text_color(lv_colors.WHITE)
            .text_letter_space(-4)
            .text_line_space(-8),
            0,
        )
        self.label1.set_text(_(i18n_keys.TITLE__DOWNLOAD__TUTORIAL_AFTER_INIT))
        self.label1.align(lv.ALIGN.TOP_LEFT, 8, 10)

        self.label2 = lv.label(self.content_area)
        self.label2.add_style(
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_color(lv_colors.LIGHT_GRAY)
            .text_letter_space(-1)
            .text_line_space(6),
            0,
        )
        self.label2.set_text(
            f"{_(i18n_keys.SUBTITLE__DOWNLOAD__TUTORIAL_AFTER_INIT)}:\nonekey.so/download"
        )
        self.label2.align_to(self.label1, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.images = lv.img(self.content_area)
        self.images.set_src("A:/res/download.png")
        self.images.align_to(self.label2, lv.ALIGN.OUT_BOTTOM_LEFT, 32, 40)

        # self.website = lv.label(self.content_area)
        # self.website.set_style_text_font(font_PJSREG30, 0)
        # self.website.set_style_text_color(
        #     lv_colors.WHITE_2, 0
        # )
        # self.website.set_text("onekey.so/download")
        # self.website.align_to(self.label2, lv.ALIGN.OUT_BOTTOM_MID, 0, 6)
        # self.underline = lv.line(self.content_area)
        # self.underline.set_points(
        #     [
        #         {"x": 0, "y": 2},
        #         {"x": 263, "y": 2},
        #     ],
        #     2,
        # )
        # self.underline.set_style_line_color(
        #     lv_colors.WHITE_2, 0
        # )
        # self.underline.align_to(self.website, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

        self.next_btn = NormalButton(self, "")
        self.next_btn.set_size(231, 98)
        self.next_btn.align(lv.ALIGN.BOTTOM_RIGHT, -8, -8)
        self.next_btn.set_style_bg_img_src("A:/res/arrow-right_2.png", 0)

        self.placeholder = NormalButton(self, "", False)
        self.placeholder.set_size(231, 98)
        self.placeholder.align(lv.ALIGN.BOTTOM_LEFT, 8, -8)
        self.placeholder.set_style_bg_img_src("A:/res/arrow-left_gray.png", 0)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            utils.lcd_resume()
            if target == self.next_btn:
                self.del_delayed(100)
                ConnectWallet()


class ConnectWallet(lv.obj):
    def __init__(self):
        super().__init__(lv.scr_act())
        self.set_size(lv.pct(100), lv.pct(100))
        self.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.set_style_bg_color(lv_colors.BLACK, 0)
        self.set_style_pad_all(0, 0)
        self.set_style_border_width(0, 0)
        self.set_style_radius(0, 0)

        self.process_bar = lv.bar(self)
        self.process_bar.set_size(464, 4)
        self.process_bar.align(lv.ALIGN.TOP_MID, 0, 44)
        self.process_bar.set_style_radius(0, 0)
        self.process_bar.set_style_bg_color(lv_colors.ONEKEY_BLACK, 0)
        self.process_bar.set_style_bg_color(
            lv_colors.WHITE, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.process_bar.set_value(66, lv.ANIM.OFF)

        self.content_area = lv.obj(self)
        self.content_area.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.content_area.align_to(self.process_bar, lv.ALIGN.OUT_BOTTOM_MID, 0, 0)
        self.content_area.set_style_bg_color(lv_colors.BLACK, 0)
        self.content_area.set_style_bg_color(
            lv_colors.WHITE_3, lv.PART.SCROLLBAR | lv.STATE.DEFAULT
        )
        self.content_area.set_style_pad_all(0, 0)
        self.content_area.set_style_border_width(0, 0)
        self.content_area.set_style_radius(0, 0)
        self.content_area.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        self.content_area.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        self.content_area.set_style_max_height(622, 0)
        self.content_area.set_style_min_height(400, 0)

        # self.images = lv.img(self.content_area)
        # self.images.set_src("A:/res/connect-wallet.png")
        # self.images.align(lv.ALIGN.TOP_MID, 0, 36)

        self.label1 = lv.label(self.content_area)
        self.label1.set_width(432)
        self.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
        self.label1.add_style(
            StyleWrapper()
            .text_font(font_PJSBOLD72)
            .text_color(lv_colors.WHITE)
            .text_letter_space(-4)
            .text_line_space(-8),
            0,
        )
        self.label1.set_text(_(i18n_keys.TITLE__CONNECT__TUTORIAL_AFTER_INIT))
        self.label1.align(lv.ALIGN.TOP_LEFT, 8, 10)

        text_style = (
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_color(lv_colors.LIGHT_GRAY)
            .text_align_left()
            .text_letter_space(-1)
            .text_line_space(6)
            .width(392)
        )

        self.label2 = lv.label(self.content_area)
        self.label2.add_style(text_style, 0)
        self.label2.set_long_mode(lv.label.LONG.WRAP)
        self.label2.set_text(
            f"1. {_(i18n_keys.SUBTITLE__CONNECT_1__TUTORIAL_AFTER_INIT)}"
        )
        self.label2.align_to(self.label1, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 28)

        self.label3 = lv.label(self.content_area)
        self.label3.add_style(text_style, 0)
        self.label3.set_long_mode(lv.label.LONG.WRAP)
        self.label3.set_text(
            f"2. {_(i18n_keys.SUBTITLE__CONNECT_2__TUTORIAL_AFTER_INIT)}"
        )
        self.label3.align_to(self.label2, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.label4 = lv.label(self.content_area)
        self.label4.add_style(text_style, 0)
        self.label4.set_long_mode(lv.label.LONG.WRAP)
        self.label4.set_text(
            f"3. {_(i18n_keys.SUBTITLE__CONNECT_3__TUTORIAL_AFTER_INIT).format(storage.device.get_ble_name() or uart.get_ble_name())}"
        )
        self.label4.align_to(self.label3, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.label5 = lv.label(self.content_area)
        self.label5.add_style(text_style, 0)
        self.label5.set_long_mode(lv.label.LONG.WRAP)
        self.label5.set_text(
            f"4. {_(i18n_keys.SUBTITLE__CONNECT_4__TUTORIAL_AFTER_INIT)}"
        )
        self.label5.align_to(self.label4, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.next_btn = NormalButton(self, "")
        self.next_btn.set_size(231, 98)
        self.next_btn.align(lv.ALIGN.BOTTOM_RIGHT, -8, -8)
        self.next_btn.set_style_bg_img_src("A:/res/arrow-right_2.png", 0)

        self.back_btn = NormalButton(self, "")
        self.back_btn.set_size(231, 98)
        self.back_btn.align(lv.ALIGN.BOTTOM_LEFT, 8, -8)
        self.back_btn.set_style_bg_img_src("A:/res/arrow-left_2.png", 0)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            utils.lcd_resume()
            if target == self.next_btn:
                self.del_delayed(100)
                AddAccount()
            elif target == self.back_btn:
                self.del_delayed(100)
                GuideAppDownload()


class AddAccount(lv.obj):
    def __init__(self):
        super().__init__(lv.scr_act())
        self.set_size(lv.pct(100), lv.pct(100))
        self.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.set_style_bg_color(lv_colors.BLACK, 0)
        self.set_style_pad_all(0, 0)
        self.set_style_border_width(0, 0)
        self.set_style_radius(0, 0)

        self.process_bar = lv.bar(self)
        self.process_bar.set_size(464, 4)
        self.process_bar.align(lv.ALIGN.TOP_MID, 0, 44)
        self.process_bar.set_style_radius(0, 0)
        self.process_bar.set_style_bg_color(lv_colors.ONEKEY_BLACK, 0)
        self.process_bar.set_style_bg_color(
            lv_colors.WHITE, lv.PART.INDICATOR | lv.STATE.DEFAULT
        )
        self.process_bar.set_value(100, lv.ANIM.OFF)

        self.content_area = lv.obj(self)
        self.content_area.set_size(lv.pct(100), lv.SIZE.CONTENT)
        self.content_area.align_to(self.process_bar, lv.ALIGN.OUT_BOTTOM_MID, 0, 0)
        self.content_area.set_style_bg_color(lv_colors.BLACK, 0)
        self.content_area.set_style_bg_color(
            lv_colors.WHITE_3, lv.PART.SCROLLBAR | lv.STATE.DEFAULT
        )
        self.content_area.set_style_pad_all(0, 0)
        self.content_area.set_style_border_width(0, 0)
        self.content_area.set_style_radius(0, 0)
        self.content_area.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
        self.content_area.add_flag(lv.obj.FLAG.EVENT_BUBBLE)

        self.content_area.set_style_max_height(622, 0)
        self.content_area.set_style_min_height(400, 0)

        # self.images = lv.img(self.content_area)
        # self.images.set_src("A:/res/add-account.png")
        # self.images.align(lv.ALIGN.TOP_MID, 0, 36)

        self.label1 = lv.label(self.content_area)
        self.label1.set_width(432)
        self.set_style_text_align(lv.TEXT_ALIGN.LEFT, 0)
        self.label1.add_style(
            StyleWrapper()
            .text_font(font_PJSBOLD72)
            .text_color(lv_colors.WHITE)
            .text_letter_space(-4)
            .text_line_space(-8),
            0,
        )
        self.label1.set_text(_(i18n_keys.TITLE__ADD_ACCOUNT__TUTORIAL_AFTER_INIT))
        self.label1.align(lv.ALIGN.TOP_LEFT, 8, 10)

        text_style = (
            StyleWrapper()
            .text_font(font_PJSREG30)
            .text_color(lv_colors.LIGHT_GRAY)
            .text_align_left()
            .text_letter_space(-1)
            .text_line_space(6)
            .width(392)
        )
        self.label2 = lv.label(self.content_area)
        self.label2.add_style(text_style, 0)
        self.label2.set_long_mode(lv.label.LONG.WRAP)
        self.label2.set_text(
            f"1. {_(i18n_keys.SUBTITLE__ADD_ACCOUNT_1__TUTORIAL_AFTER_INIT)}"
        )
        self.label2.align_to(self.label1, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 28)

        self.label3 = lv.label(self.content_area)
        self.label3.add_style(text_style, 0)
        self.label3.set_long_mode(lv.label.LONG.WRAP)
        self.label3.set_text(
            f"2. {_(i18n_keys.SUBTITLE__ADD_ACCOUNT_2__TUTORIAL_AFTER_INIT)}"
        )
        self.label3.align_to(self.label2, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

        self.next_btn = NormalButton(self, "")
        self.next_btn.set_size(231, 98)
        self.next_btn.align(lv.ALIGN.BOTTOM_RIGHT, -8, -8)
        self.next_btn.set_style_bg_img_src("A:/res/arrow-right_2.png", 0)

        self.back_btn = NormalButton(self, "")
        self.back_btn.set_size(231, 98)
        self.back_btn.align(lv.ALIGN.BOTTOM_LEFT, 8, -8)
        self.back_btn.set_style_bg_img_src("A:/res/arrow-left_2.png", 0)
        self.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            global INIT_TUTORIAL
            utils.lcd_resume()
            if target == self.next_btn:
                self.del_delayed(100)
                signal()
            elif target == self.back_btn:
                self.del_delayed(100)
                ConnectWallet()
