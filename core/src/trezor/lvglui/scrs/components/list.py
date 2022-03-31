import lvgl as lv


class CandidateWordList(lv.list):
    def __init__(self, parent, words):
        self.current_btn = None
        super().__init__(parent)

        self.set_style_pad_row(5, 0)
        self.set_size(lv.pct(100), 200)
        self.set_pos(0, 392)
        self.set_align(lv.ALIGN.TOP_LEFT)
        self.set_style_bg_opa(0, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.set_style_border_width(0, lv.PART.MAIN | lv.STATE.DEFAULT)

        for i in range(3):
            btn = lv.btn(self)
            btn.set_size(416, 48)
            btn.center()
            btn.set_style_bg_color(
                lv.color_hex(0x000000), lv.PART.MAIN | lv.STATE.DEFAULT
            )
            btn.set_style_radius(30, lv.PART.MAIN | lv.STATE.DEFAULT)
            btn.add_event_cb(self.eventhandler, lv.EVENT.CLICKED, None)
            label = lv.label(btn)
            label.set_text(words[i])
            label.center()
            label.set_style_text_font(
                lv.font_montserrat_16, lv.PART.MAIN | lv.STATE.DEFAULT
            )
            label.set_style_text_color(
                lv.color_hex(0xFFFFFF), lv.PART.MAIN | lv.STATE.DEFAULT
            )

    def eventhandler(self, evt):
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
