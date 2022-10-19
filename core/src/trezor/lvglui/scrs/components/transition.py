from .. import lv


class DefaultTransition(lv.style_transition_dsc_t):
    def __init__(self) -> None:
        super().__init__()
        props = [
            lv.STYLE.BG_COLOR,
            lv.STYLE.TRANSFORM_WIDTH,
            lv.STYLE.TRANSFORM_HEIGHT,
            # lv.STYLE.TEXT_COLOR,
            0,
        ]
        self.init(props, lv.anim_t.path_linear, 100, 0, None)


class BtnClickTransition(lv.style_transition_dsc_t):
    def __init__(self) -> None:
        super().__init__()
        props = [
            lv.STYLE.BG_OPA,
            lv.STYLE.TRANSFORM_WIDTH,
            lv.STYLE.TRANSFORM_HEIGHT,
            # lv.STYLE.TEXT_COLOR,
            0,
        ]
        self.init(props, lv.anim_t.path_linear, 100, 0, None)
