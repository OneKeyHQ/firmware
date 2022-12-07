from .. import lv


class SwitchWrapper(lv.switch):
    def __init__(self, parent) -> None:
        super().__init__(parent)
