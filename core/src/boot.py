import storage
import storage.device
from trezor import config, log, loop, ui, utils, wire
from trezor.lvglui import lvgl_tick
from trezor.lvglui.scrs import *
from trezor.lvglui.scrs.bootscreen import BootScreen
from trezor.lvglui.scrs.lockscreen import LockScreen
from trezor.pin import show_pin_timeout

from apps.common.request_pin import can_lock_device, verify_user_pin

lvgl_task = lvgl_tick()


async def bootscreen() -> None:
    bootscreen = BootScreen()
    # wait for bootscreen animation to finish
    await loop.sleep(1500)
    # await bootscreen.request()
    lockscreen = LockScreen()
    while True:
        try:
            if can_lock_device():
                await lockscreen.request()
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

loop.schedule(lvgl_task)

loop.run()
