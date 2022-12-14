from .. import lv


class DefaultTransition(lv.style_transition_dsc_t):
    def __init__(self) -> None:
        super().__init__()
        props = [
            # lv.STYLE.TRANSFORM_WIDTH,
            lv.STYLE.TRANSFORM_HEIGHT,
            lv.STYLE.BG_COLOR,
            # lv.STYLE.TEXT_COLOR,
            0,
        ]
        self.init(props, lv.anim_t.path_ease_in, 80, 10, None)


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
        self.init(props, lv.anim_t.path_ease_in, 80, 10, None)
