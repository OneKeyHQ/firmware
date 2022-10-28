from .common import FullSizeWindow, lv
from .components.keyboard import BIP39Keyboard


class WordEnter(FullSizeWindow):
    def __init__(self, title):
        super().__init__(title, None, anim_dir=0)
        self.keyboard = BIP39Keyboard(self)
        self.keyboard.add_event_cb(self.on_ready, lv.EVENT.READY, None)

    def on_ready(self, event_obj):
        input = self.keyboard.ta.get_text()
        if input == "":
            return
        self.channel.publish(input)
        self.keyboard.ta.set_text("")
        self.destroy()
