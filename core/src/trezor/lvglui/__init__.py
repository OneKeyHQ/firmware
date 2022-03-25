import lvgl as lv
from trezor import loop

async def lvgl_tick():
    while True:
        lv.tick_inc(10)
        await loop.sleep(10)
        lv.timer_handler()
