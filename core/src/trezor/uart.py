import ustruct
from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import config, io, log, loop, utils
from trezor.lvglui import StatusBar
from trezor.ui import display

from apps import base

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
BLE_ENABLED: bool | None = None
NRF_VERSION: str | None = None
BLE_CTRL = io.BLE()


async def handle_usb_state():
    global CHARGING
    while True:
        try:
            usb_state = loop.wait(io.USB_STATE)
            state = await usb_state
            utils.lcd_resume()
            if state:
                StatusBar.get_instance().show_usb(True)
                # deal with charging state
                CHARGING = True
                StatusBar.get_instance().show_charging(True)
                if utils.BATTERY_CAP:
                    StatusBar.get_instance().set_battery_img(
                        utils.BATTERY_CAP, CHARGING
                    )
            else:
                StatusBar.get_instance().show_usb(False)
                # deal with charging state
                CHARGING = False
                StatusBar.get_instance().show_charging()
                if utils.BATTERY_CAP:
                    StatusBar.get_instance().set_battery_img(
                        utils.BATTERY_CAP, CHARGING
                    )
            if (
                device.is_usb_lock_enabled()
                and device.is_initialized()
                and config.has_pin()
            ):
                if config.is_unlocked():
                    config.lock()
                    # single to restart the main loop
                    raise loop.TASK_CLOSED
            base.reload_settings_from_storage()
        except Exception as exec:
            if __debug__:
                log.exception(__name__, exec)
            loop.clear()
            return  # pylint: disable=lost-exception


async def handle_uart():
    fetch_all()
    while True:
        try:
            await process_push()
        except Exception as exec:
            if __debug__:
                log.exception(__name__, exec)
            loop.clear()
            return  # pylint: disable=lost-exception


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
        await _deal_ble_status(value)
    elif cmd == _CMD_BLE_PAIR_CODE:
        # show six bytes pair code as string
        await _deal_ble_pair(value)
    elif cmd == _CMD_BLE_PAIR_RES:
        # paring result 1 success 2 failed
        await _deal_pair_res(value)
    elif cmd == _CMD_DEVICE_CHARGING_STATUS:
        # 1 usb plug in 2 usb plug out 3 charging
        await _deal_charging_state(value)
    elif cmd == _CMD_BATTERY_STATUS:
        # current battery level, 0-100 only effective when not charging
        res = ustruct.unpack(">B", value)[0]
        utils.BATTERY_CAP = res
        StatusBar.get_instance().set_battery_img(res, CHARGING)

    elif cmd == _CMD_SIDE_BUTTON_PRESS:
        # 1 short press 2 long press
        await _deal_button_press(value)
    elif cmd == _CMD_BLE_NAME:
        # retrieve ble name has format: ^T[0-9]{4}$
        _retrieve_ble_name(value)
    elif cmd == _CMD_NRF_VERSION:
        # retrieve nrf version
        _retrieve_nrf_version(value)
    else:
        if __debug__:
            print("unknown or not care command:", cmd)


async def _deal_ble_pair(value):
    global SCREEN
    pair_codes = value.decode("utf-8")
    # pair_codes = "".join(list(map(lambda c: chr(c), ustruct.unpack(">6B", value))))
    utils.turn_on_lcd_if_possible()
    from trezor.lvglui.scrs.ble import PairCodeDisplay

    SCREEN = PairCodeDisplay(pair_codes)


async def _deal_button_press(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res == _PRESS_SHORT:
        if display.backlight():
            display.backlight(0)
            if device.is_initialized():
                utils.AUTO_POWER_OFF = True
                if config.has_pin() and config.is_unlocked():
                    config.lock()
                # single to restart the main loop
                raise loop.TASK_CLOSED
        else:
            utils.turn_on_lcd_if_possible()
    elif res == _PRESS_LONG:
        from trezor.lvglui.scrs.homescreen import PowerOff

        PowerOff(set_home=True)


async def _deal_charging_state(value: bytes) -> None:
    """THIS DOESN'T WORK CORRECT DUE TO THE PUSHED STATE, ONLY USED AS A FALLBACK WHEN
    CHARGING WITH A CHARGER NOW.

    """
    global CHARGING
    res = ustruct.unpack(">B", value)[0]
    if res in (
        _USB_STATUS_PLUG_IN,
        _POWER_STATUS_CHARGING,
    ):
        if res != _POWER_STATUS_CHARGING:
            utils.turn_on_lcd_if_possible()
        if CHARGING:
            return
        CHARGING = True
        StatusBar.get_instance().show_charging(True)
        if utils.BATTERY_CAP:
            StatusBar.get_instance().set_battery_img(utils.BATTERY_CAP, CHARGING)
    elif res in (_USB_STATUS_PLUG_OUT, _POWER_STATUS_CHARGING_FINISHED):
        if not CHARGING:
            return
        CHARGING = False
        StatusBar.get_instance().show_charging()
        if utils.BATTERY_CAP:
            StatusBar.get_instance().set_battery_img(utils.BATTERY_CAP, CHARGING)


async def _deal_pair_res(value: bytes) -> None:
    res = ustruct.unpack(">B", value)[0]
    if res in [_BLE_PAIR_SUCCESS, _BLE_PAIR_FAILED]:
        if SCREEN is not None and not SCREEN.destroyed:
            SCREEN.destroy()
    if res == _BLE_PAIR_FAILED:
        from trezor.ui.layouts import show_pairing_error

        await show_pairing_error()


async def _deal_ble_status(value: bytes) -> None:
    global BLE_ENABLED
    res = ustruct.unpack(">B", value)[0]
    if res == _BLE_STATUS_CONNECTED:
        utils.BLE_CONNECTED = True
        # show icon in status bar
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_CONNECTED)
    elif res == _BLE_STATUS_DISCONNECTED:
        utils.BLE_CONNECTED = False
        if not BLE_ENABLED:
            return
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_ENABLED)

    elif res == _BLE_STATUS_OPENED:
        if utils.BLE_CONNECTED:
            return
        BLE_ENABLED = True
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_ENABLED)
        if config.is_unlocked():
            device.set_ble_status(enable=True)
    elif res == _BLE_STATUS_CLOSED:
        BLE_ENABLED = False
        StatusBar.get_instance().show_ble(StatusBar.BLE_STATE_DISABLED)
        if config.is_unlocked():
            device.set_ble_status(enable=False)


def _retrieve_ble_name(value: bytes) -> None:
    if value != b"":
        utils.BLE_NAME = value.decode("utf-8")
        # if config.is_unlocked():
        #     device.set_ble_name(BLE_NAME)


def _retrieve_nrf_version(value: bytes) -> None:
    global NRF_VERSION
    if value != b"":
        NRF_VERSION = value.decode("utf-8")
        # if config.is_unlocked():
        #     device.set_ble_version(NRF_VERSION)


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
    return utils.BLE_NAME if utils.BLE_NAME else ""


def get_ble_version() -> str:
    """Get ble version."""
    return NRF_VERSION if NRF_VERSION else ""


def is_ble_opened() -> bool:
    return BLE_ENABLED if BLE_ENABLED is not None else True
