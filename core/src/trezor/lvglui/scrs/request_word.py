from .common import FullSizeWindow, lv
from .components.keyboard import BIP39Keyboard


class WordEnter(FullSizeWindow):
    def __init__(self, title):
        super().__init__(title, None)
        self.keyboard = BIP39Keyboard(self)
        self.keyboard.add_event_cb(self.eventhandler, lv.EVENT.READY, None)

    def eventhandler(self, event_obj):
        input = self.keyboard.ta.get_text()
        if input == "":
            return
        self.channel.publish(input)
        self.keyboard.ta.set_text("")
        self.destroy()
        if __debug__:
            print(f"Enter word: == {input}")
