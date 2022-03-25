import storage
import storage.device
from trezor import config, log, loop, ui, utils, wire
from trezor.pin import show_pin_timeout

from apps.common.request_pin import can_lock_device, verify_user_pin
from apps.homescreen.lockscreen import Lockscreen

import lvgl as lv
from lvglui import lv_ui
from lvglui import globalvar as gl

async def lvgl_tick():
    while True:
        lv.tick_inc(10)        
        await loop.sleep(10)
        lv.timer_handler()

async def bootscreen() -> None:
    # lockscreen = Lockscreen(bootscreen=True)     
    # ui.display.orientation(storage.device.get_rotation())
    ui_boot = lv_ui.Screen_Home("Not connected","Tap to connect")
    gl.set_value('ui_boot',ui_boot)
    while True:
        try:
            if can_lock_device():
                await ui_boot.screen_response()
            await verify_user_pin()
            storage.init_unlocked()
            loop.close(lvgl_task)
            return
        except wire.PinCancelled:
            # verify_user_pin will convert a SdCardUnavailable (in case of sd salt)
            # to PinCancelled exception.
            # Ignore exception, retry loop.
            pass
        except BaseException as e:
            # other exceptions here are unexpected and should halt the device
            if __debug__:
                log.exception(__name__, e)
            utils.halt(e.__class__.__name__)


ui.display.backlight(ui.BACKLIGHT_NONE)
ui.backlight_fade(ui.BACKLIGHT_NORMAL)
config.init(show_pin_timeout)

if __debug__ and not utils.EMULATOR:
    config.wipe()

loop.schedule(bootscreen())
lvgl_task = lvgl_tick()
loop.schedule(lvgl_task)

loop.run()

lv_ui = gl.get_dictionary()
for key, value in lv_ui.items():
    value.delete()
gl.del_all()
