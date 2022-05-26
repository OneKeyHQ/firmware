import storage
from trezor import config, log, loop, ui, utils, wire
from trezor.lvglui import lvgl_tick
from trezor.pin import show_pin_timeout

lvgl_task = lvgl_tick()


def clear() -> None:
    """if device is not initialized, pin is needless, so clear it"""
    if not storage.device.is_initialized() and config.has_pin():
        storage.wipe()


async def bootscreen() -> None:
    from trezor.lvglui.scrs.bootscreen import BootScreen
    from trezor.lvglui.scrs.lockscreen import LockScreen
    from apps.common.request_pin import can_lock_device, verify_user_pin

    bootscreen = BootScreen()
    # wait for bootscreen animation to finish
    await loop.sleep(1500)
    bootscreen.del_delayed(100)
    # await bootscreen.request()
    lockscreen = LockScreen(storage.device.get_label())
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
clear()

if __debug__ and not utils.EMULATOR:
    config.wipe()


loop.schedule(bootscreen())

loop.schedule(lvgl_task)

loop.run()
