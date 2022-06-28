import ustruct
from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import config, io, loop
from trezor.lvglui import StatusBar
from trezor.ui import display

import apps.base

if TYPE_CHECKING:
    from trezor.lvglui.scrs.ble import PairCodeDisplay


_PREFIX = const(42330)  # 0xA55A
_FORMAT = ">HHB"
_HEADER_LEN = const(5)
# fmt: off
_CMD_BLE_NAME = _PRESS_SHORT = _USB_STATUS_PLUG_IN = _BLE_STATUS_CONNECTED = _BLE_PAIR_SUCCESS = const(1)
_PRESS_LONG = _USB_STATUS_PLUG_OUT = _BLE_STATUS_DISCONNECTED = _BLE_PAIR_FAILED = _CMD_BLE_STATUS = const(2)
# fmt: on
_BLE_STATUS_OPENED = _POWER_STATUS_CHARGING = _CMD_BLE_PAIR_CODE = const(3)
_BLE_STATUS_CLOSED = _CMD_BLE_PAIR_RES = _POWER_STATUS_CHARGING_FINISHED = const(4)
_CMD_NRF_VERSION = const(5)  # ble firmware version
_CMD_DEVICE_CHARGING_STATUS = const(8)
_CMD_BATTERY_STATUS = const(9)
_CMD_SIDE_BUTTON_PRESS = const(10)
CHARGING = False
SCREEN: PairCodeDisplay | None = None
BLE_NAME: str | None = None
BLE_ENABLED: bool | None = None
NRF_VERSION: str | None = None
BLE_CTRL = io.BLE()
BATTERY_CAP = 255


async def handle_usb_state():
    while True:
        usb_state = loop.wait(io.USB_STATE)
        state = await usb_state
        if state:
            StatusBar.get_instance().show_usb(True)
        else:
            StatusBar.get_instance().show_usb(False)
            apps.base.lock_device()
            loop.clear()
            return


async def handle_uart():
    fetch_all()
    while True:
        try:
            await process_push()
        except Exception:
            pass


async def process_push() -> None:

    uart = loop.wait(io.UART | io.POLL_READ)

    response = await uart
    header = response[:_HEADER_LEN]
    prefix, length, cmd = ustruct.unpack(_FORMAT, header)
    if prefix != _PREFIX:
        # unexpected prefix, ignore directly
        return
    value = response[_HEADER_LEN:][: length - 2]
    if __debug__:
        print(f"cmd == {cmd} with value {value} ")
    if cmd == _CMD_BLE_STATUS:
        # 1 connected 2 disconnected 3 opened 4 closed
        _deal_ble_status(value)
    elif cmd == _CMD_BLE_PAIR_CODE:
        # show six bytes pair code as string
        _deal_ble_pair(value)
    elif cmd == _CMD_BLE_PAIR_RES:
        # paring result 1 success 2 failed
        await _deal_pair_res(value)
    elif cmd == _CMD_DEVICE_CHARGING_STATUS:
        # 1 usb plug in 2 usb plug out 3 charging
        _deal_charging_state(value)
    elif cmd == _CMD_BATTERY_STATUS:
        global BATTERY_CAP
        # current battery level, 0-100 only effective when not charging
        res = ustruct.unpack(">B", value)[0]
        BATTERY_CAP = res
        StatusBar.get_instance().set_battery_img(res, CHARGING)

    elif cmd == _CMD_SIDE_BUTTON_PRESS:
        # 1 short press 2 long press
        _deal_button_press(value)
    elif cmd == _CMD_BLE_NAME:
        # retrieve ble name has format: ^T[0-9]{4}$
        _retrieve_ble_name(value)
    elif cmd == _CMD_NRF_VERSION:
        # retrieve nrf version
        _retrieve_nrf_version(value)
    else:
        if __debug__:
            print("unknown or not care command:", cmd)


def _deal_ble_pair(value):
    global SCREEN
    pair_codes = value.decode("utf-8")
    # pair_codes = "".join(list(map(lambda c: chr(c), ustruct.unpack(">6B", value))))
    from trezor.lvglui.scrs.ble import PairCodeDisplay

    SCREEN = PairCodeDisplay(pair_codes)


def _deal_button_press(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res == _PRESS_SHORT:
        if display.backlight():
            apps.base.lock_device()
            display.backlight(0)
        else:
            display.backlight(device.get_brightness())
    elif res == _PRESS_LONG:
        from trezor.lvglui.scrs.homescreen import PowerOff

        PowerOff()


def _deal_charging_state(value: bytes) -> None:
    global BATTERY_CAP, CHARGING
    res = ustruct.unpack(">B", value)[0]
    if res in (
        _USB_STATUS_PLUG_IN,
        _POWER_STATUS_CHARGING,
    ):
        CHARGING = True
        StatusBar.get_instance().show_charging(True)
        StatusBar.get_instance().set_battery_img(BATTERY_CAP, CHARGING)
        # StatusBar.get_instance().show_usb(show=True) # use sub enumerate to achieve this
    elif res in (_USB_STATUS_PLUG_OUT, _POWER_STATUS_CHARGING_FINISHED):
        CHARGING = False
        StatusBar.get_instance().show_charging(False)
        StatusBar.get_instance().set_battery_img(BATTERY_CAP, CHARGING)
        # if res == _USB_STATUS_PLUG_OUT: # use sub enumerate to achieve this
        #     StatusBar.get_instance().show_usb()
    # if res in (_USB_STATUS_PLUG_IN, _USB_STATUS_PLUG_OUT):
    #     apps.base.lock_device()


async def _deal_pair_res(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res in [_BLE_PAIR_SUCCESS, _BLE_PAIR_FAILED]:
        if SCREEN is not None and not SCREEN.destroyed:
            SCREEN.destroy()
    if res == _BLE_PAIR_FAILED:
        from trezor.ui.layouts import show_pairing_error

        await show_pairing_error()


def _deal_ble_status(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res == _BLE_STATUS_CONNECTED:
        # show icon in status bar
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_CONNECTED)
    elif res == _BLE_STATUS_DISCONNECTED:
        # hidden icon in status bar
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_ENABLED)
    elif res == _BLE_STATUS_OPENED:
        global BLE_ENABLED
        BLE_ENABLED = True
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_ENABLED)
        if config.is_unlocked():
            device.set_ble_status(enable=True)
    elif res == _BLE_STATUS_CLOSED:
        global BLE_ENABLED
        BLE_ENABLED = False
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_DISABLED)
        if config.is_unlocked():
            device.set_ble_status(enable=False)


def _retrieve_ble_name(value: bytes) -> None:
    global BLE_NAME
    if value != b"":
        BLE_NAME = value.decode("utf-8")
        # device.set_ble_name(BLE_NAME)


def _retrieve_nrf_version(value: bytes) -> None:
    global NRF_VERSION
    if value != b"":
        NRF_VERSION = value.decode("utf-8")
        # device.set_ble_version(NRF_VERSION)


def _request_ble_name():
    """Request ble name."""
    BLE_CTRL.ctrl(0x83, 0x01)


def _request_ble_version():
    """Request ble version."""
    BLE_CTRL.ctrl(0x83, 0x02)


def _request_battery_level():
    """Request battery level."""
    BLE_CTRL.ctrl(0x82, 0x04)


def _request_ble_status():
    """Request current ble status."""
    BLE_CTRL.ctrl(0x81, 0x04)


def _request_charging_status():
    """Request charging status."""
    BLE_CTRL.ctrl(0x82, 0x05)


def fetch_all():
    """Request some important data."""
    _request_ble_name()
    _request_ble_version()
    _request_ble_status()
    _request_battery_level()
    _request_charging_status()


def ctrl_ble(enable: bool) -> None:
    """Request to open or close ble.
    @param enable: True to open, False to close
    """
    if not device.ble_enabled() and enable:
        BLE_CTRL.ctrl(0x81, 0x01)
    elif device.ble_enabled() and not enable:
        BLE_CTRL.ctrl(0x81, 0x02)


def ctrl_power_off() -> None:
    """Request to power off the device."""
    BLE_CTRL.ctrl(0x82, 0x01)


def get_ble_name() -> str:
    """Get ble name."""
    return BLE_NAME if BLE_NAME else ""


def get_ble_version() -> str:
    """Get ble version."""
    return NRF_VERSION if NRF_VERSION else ""
