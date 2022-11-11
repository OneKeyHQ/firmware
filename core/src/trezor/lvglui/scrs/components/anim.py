from .. import lv


class Anim(lv.anim_t):
    def __init__(
        self,
        stat_value,
        end_value,
        exec_cb,
        path_cb=lv.anim_t.path_ease_out,
        time=150,
        y_axis=True,
        delay=0,
        del_cb=None,
    ):
        super().__init__()
        self.init()
        self.set_time(time)
        self.set_values(stat_value, end_value)
        self.set_path_cb(path_cb)
        self.set_delay(delay)
        self.set_custom_exec_cb(
            lambda anim, val: (exec_cb(0, val) if y_axis else exec_cb(val, 0))
        )
        if del_cb:
            self.set_deleted_cb(del_cb)
