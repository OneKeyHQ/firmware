from .. import lv


class ButtonWrapper(lv.btn):
    def __init__(self, parent) -> None:
        super().__init__(parent)
        self.remove_style_all()
