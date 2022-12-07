from .. import lv


class LabelWrapper(lv.label):
    def __init__(self, parent) -> None:
        super().__init__(parent)

    def text_long_mode(self, mode) -> "LabelWrapper":
        self.set_long_mode(mode)
        return self

    def text(self, text) -> "LabelWrapper":
        self.set_text(text)
        return self

    def size(self, width, height=lv.SIZE.CONTENT) -> "LabelWrapper":
        self.set_size(width, height)
        return self

    def align(self, align_type, base=None, rel_x=0, rel_y=0) -> "LabelWrapper":
        super().align_to(base or self.parent, align_type, rel_x, rel_y)
        return self
