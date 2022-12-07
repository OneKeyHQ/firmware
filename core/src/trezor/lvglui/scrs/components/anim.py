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
        start_cb=None,
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
        if start_cb:
            self.set_start_cb(start_cb)
        if del_cb:
            self.del_cb = del_cb
        self.set_deleted_cb(self.default_delete_cb)

    def start_anim(self):
        top_layer = lv.layer_top()
        top_layer.add_flag(lv.obj.FLAG.CLICKABLE)
        self.start()

    def default_delete_cb(self, _amin):
        if hasattr(self, "del_cb"):
            self.del_cb(_amin)
        top_layer = lv.layer_top()
        top_layer.clear_flag(lv.obj.FLAG.CLICKABLE)
