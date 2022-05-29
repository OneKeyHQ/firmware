import ustruct
from micropython import const
from typing import TYPE_CHECKING

from trezor import io, loop
from trezor.lvglui import StatusBar

if TYPE_CHECKING:
    from trezor.lvglui.scrs.ble import PairCodeDisplay


_PREFIX = const(42330)  # 0xA55A
_FORMAT = ">HHB"
_HEADER_LEN = const(5)
# fmt: off
_CMD_BLE_NAME = _PRESS_SHORT = _USB_STATUS_PLUG_IN = _BLE_STATUS_CONNECTED = _BLE_PAIR_SUCCESS = const(1)
_PRESS_LONG = _USB_STATUS_PLUG_OUT = _BLE_STATUS_DISCONNECTED = _BLE_PAIR_FAILED = _CMD_BLE_STATUS = const(2)
# fmt: on
_POWER_STATUS_CHARGING = _CMD_BLE_PAIR_CODE = const(3)
_CMD_BLE_PAIR_RES = const(4)
_CMD_NRF_VERSION = const(5)
_CMD_DEVICE_CHARGING_STATUS = const(8)
_CMD_BATTEARY_STATUS = const(9)
_CMD_SIDE_BUTTON_PRESS = const(10)
CHARGING = False
SCREEN: PairCodeDisplay | None = None
BLE_NAME: str | None = None
NRF_VERSION: str | None = None


async def handle_uart():
    while True:
        await process_push()


async def process_push() -> None:
    uart = loop.wait(io.UART | io.POLL_READ)

    response = await uart
    header = response[:_HEADER_LEN]
    prefix, length, cmd = ustruct.unpack(_FORMAT, header)
    if prefix != _PREFIX:
        # unexpected prefix, ignore directly
        return
    value = response[_HEADER_LEN:][: length - 2]
    if cmd == _CMD_BLE_STATUS:
        # 1 connected 2 disconnected 3 opened 4 closed
        deal_ble_status(value)
    elif cmd == _CMD_BLE_PAIR_CODE:
        # show six bytes pair code as string
        deal_ble_pair(value)
    elif cmd == _CMD_BLE_PAIR_RES:
        # paring result 1 success 2 failed
        await deal_pair_res(value)
    elif cmd == _CMD_DEVICE_CHARGING_STATUS:
        # 1 usb plug in 2 usb plug out 3 charging
        deal_charging_state(value)
    elif cmd == _CMD_BATTEARY_STATUS:
        # current battery level, 0-100 only effective when not charging
        if not CHARGING:
            res = ustruct.unpack(">B", value)[0]
            StatusBar.get_instance().set_battery_img(res)
    elif cmd == _CMD_SIDE_BUTTON_PRESS:
        # 1 short press 2 long press
        deal_button_press(value)
    elif cmd == _CMD_BLE_NAME:
        # retrieve ble name has format: ^T[0-9]{4}$
        retrieve_ble_name(value)
    elif cmd == _CMD_NRF_VERSION:
        # retrieve nrf version
        retrieve_nrf_version(value)
    else:
        if __debug__:
            print("unknown or not care command:", cmd)


def deal_ble_pair(value):
    global SCREEN
    pair_codes = value.decode("utf-8")
    # pair_codes = "".join(list(map(lambda c: chr(c), ustruct.unpack(">6B", value))))
    from trezor.lvglui.scrs.ble import PairCodeDisplay

    SCREEN = PairCodeDisplay(pair_codes)


def deal_button_press(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res == _PRESS_SHORT:
        if __debug__:
            print("short press")
    elif res == _PRESS_LONG:
        if __debug__:
            print("long press")


def deal_charging_state(value: bytes) -> None:
    global CHARGING
    res = ustruct.unpack(">B", value)[0]
    if res == _USB_STATUS_PLUG_IN:
        StatusBar.get_instance().show_usb(True)
    elif res == _USB_STATUS_PLUG_OUT:
        CHARGING = False
        StatusBar.get_instance().show_usb(False)
    elif res == _POWER_STATUS_CHARGING:
        CHARGING = True
        StatusBar.get_instance().set_battery_img(101)


async def deal_pair_res(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res in [_BLE_PAIR_SUCCESS, _BLE_PAIR_FAILED]:
        if SCREEN is not None and not SCREEN.destoried:
            SCREEN.destory()
    if res == _BLE_PAIR_FAILED:
        from trezor.ui.layouts import show_pairing_error

        await show_pairing_error()


def deal_ble_status(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res == _BLE_STATUS_CONNECTED:
        # show icon in status bar
        StatusBar.get_instance().show_ble(True)
    elif res == _BLE_STATUS_DISCONNECTED:
        # hidden icon in status bar
        StatusBar.get_instance().show_ble(False)
    else:
        # ignore other status
        if __debug__:
            print("BLE status:", res)


def retrieve_ble_name(value: bytes) -> None:
    global BLE_NAME
    if value != b"":
        BLE_NAME = value.decode("utf-8")


def retrieve_nrf_version(value: bytes) -> None:
    global NRF_VERSION
    if value != b"":
        NRF_VERSION = value.decode("utf-8")
