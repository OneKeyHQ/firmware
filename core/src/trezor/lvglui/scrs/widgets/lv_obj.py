from .. import lv


class LvObjectWrapper(lv.obj):
    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self.parent = parent

    def size(self, width=lv.pct(100), height=lv.SIZE.CONTENT) -> "LvObjectWrapper":
        self.set_size(width, height)
        return self

    def pos(self, x=0, y=0) -> "LvObjectWrapper":
        self.set_pos(x, y)
        return self

    def align(self, align_type, base=None, rel_x=0, rel_y=0) -> "LvObjectWrapper":
        super().align_to(base or self.parent, align_type, rel_x, rel_y)
        return self

    def add_flag(self, flag) -> "LvObjectWrapper":
        super().add_flag(flag)
        return self

    def clear_flag(self, flag) -> "LvObjectWrapper":
        super().clear_flag(flag)
        return self

    def clickable(self, clickable: bool) -> "LvObjectWrapper":
        if clickable:
            self.add_flag(lv.obj.FLAG.CLICKABLE)
        else:
            self.clear_flag(lv.obj.FLAG.CLICKABLE)
        return self

    def hidden(self, hidden: bool) -> "LvObjectWrapper":
        if hidden:
            self.add_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.clear_flag(lv.obj.FLAG.HIDDEN)
        return self

    def style(self, style, selector) -> "LvObjectWrapper":
        self.add_style(style, selector)
        return self

    def remove_style_all(self) -> "LvObjectWrapper":
        super().remove_style_all()
        return self
