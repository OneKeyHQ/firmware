from .. import lv
from ..widgets.style import StyleWrapper


class Navigation(lv.obj):
    def __init__(self, parent) -> None:
        super().__init__(parent)
        self.remove_style_all()
        self.set_size(lv.pct(100), 72)
        self.align(lv.ALIGN.TOP_MID, 0, 44)
        self.nav_btn = lv.imgbtn(self)
        self.nav_btn.set_size(48, 48)
        self.nav_btn.align(lv.ALIGN.LEFT_MID, 8, 0)
        self.nav_btn.set_ext_click_area(100)
        self.nav_btn.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
        self.nav_btn.add_style(StyleWrapper().bg_img_src("A:/res/nav-back.png"), 0)
        self.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
