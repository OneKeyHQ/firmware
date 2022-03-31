# isort:skip_file
# fmt: off

# We are tightly controlling the memory layout. Order of imports is important.
# Modules imported directly from here also must take care to have as few dependencies
# as possible.

# === Import always-active modules
# trezor imports only C modules
import trezor
# trezor.utils import only C modules
from trezor import utils
from trezor import log
# we need space for 30 items in the trezor module
utils.presize_module("trezor", 30)

# storage imports storage.common, storage.cache and storage.device.
# These import trezor, trezor.config (which is a C module), trezor.utils, and each other.
import storage
# we will need space for 12 items in the storage module
utils.presize_module("storage", 12)

if not utils.BITCOIN_ONLY:
    # storage.fido2 only imports C modules
    import storage.fido2  # noqa: F401

if __debug__:
    # storage.debug only imports C modules
    import storage.debug

# trezor.pin imports trezor.utils
# We need it as an always-active module because trezor.pin.show_pin_timeout is used
# as a UI callback for storage, which can be invoked at any time
import trezor.pin  # noqa: F401

# === Prepare the USB interfaces first. Do not connect to the host yet.
# usb imports trezor.utils and trezor.io which is a C module
import usb

import lvgl as lv
import lvgldrv as lcd
lv.init()

disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = lcd.framebuffer(1)
# buf1_2 = lcd.framebuffer(2)
disp_buf1.init(buf1_1, None, len(buf1_1) // lv.color_t.__SIZE__)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = lcd.flush
disp_drv.hor_res = 480
disp_drv.ver_res = 800
disp_drv.register()

indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = lcd.ts_read
indev_drv.register()

# create an unimport manager that will be reused in the main loop
unimport_manager = utils.unimport()

# unlock the device, unload the boot module afterwards
with unimport_manager:
    import boot
    del boot

# start the USB
import storage.device

usb.bus.open(storage.device.get_device_id())

import micropython

# run the endless loop
while True:
    print('++++++++++++++++++++++++')
    micropython.mem_info()
    with unimport_manager:        
        import session  # noqa: F401         
        del session
        print('-------------------------') 
        micropython.mem_info()
