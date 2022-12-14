from .. import lv


class ImgWrapper(lv.img):
    def __init__(self, parent) -> None:
        super().__init__(parent)

    def src(self, src) -> "ImgWrapper":
        self.set_src(src)
        return self
