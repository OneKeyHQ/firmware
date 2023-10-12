from micropython import const
from typing import TYPE_CHECKING
from ubinascii import hexlify

import storage.cache
from storage import common
from trezor import config, utils

if TYPE_CHECKING:
    from trezor.enums import BackupType
    from typing_extensions import Literal

# Namespace:
_NAMESPACE = common.APP_DEVICE
_BRIGHTNESS_VALUE: int | None = None
_LANGUAGE_VALUE: str | None = None
_LABEL_VALUE: str | None = None
_USE_PASSPHRASE_VALUE: bool | None = None
_PASSPHRASE_ALWAYS_ON_DEVICE_VALUE: bool | None = None
_AUTOLOCK_DELAY_MS_VALUE: int | None = None
_HOMESCREEN_VALUE: str | None = None
_BLE_NAME_VALUE: str | None = None
_BLE_VERSION_VALUE: str | None = None
_BLE_ENABLED_VALUE: bool | None = None
_AUTOSHUTDOWN_DELAY_MS_VALUE: int | None = None
_WALLPAPER_COUNTS_VALUE: int | None = None
_USE_USB_PROTECT_VALUE: bool | None = None
_USE_RANDOM_PIN_MAP_VALUE: bool | None = None
_TAP_AWAKE_VALUE: bool | None = None
_KEYBOARD_HAPTIC_VALUE: bool | None = None
_ANIMATION_VALUE: bool | None = None
_EXPERIMENTAL_FEATURES_VALUE: bool | None = None
_ROTATION_VALUE: int | None = None

_INITIALIZED_VALUE: bool | None = None
_FLAGS_VALUE: int | None = None
_UNFINISHED_BACKUP_VALUE: bool | None = None
_NO_BACKUP_VALUE: bool | None = None
_BACKUP_TYPE_VALUE: int | None = None
_SAFETY_CHECK_LEVEL_VALUE: int | None = None

if utils.USE_THD89:
    import uctypes

    _PUBLIC_REGION_SIZE = const(0x600)
    _PRIVATE_REGION_SIZE = const(0x200)

    struct_bool: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "value": 3 | uctypes.UINT8,
    }

    struct_uint32: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "value": 3 | uctypes.UINT32,
    }

    struct_uuid: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "uuid": (3 | uctypes.ARRAY, 25 | uctypes.UINT8),
    }

    struct_language: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "language": (3 | uctypes.ARRAY, 17 | uctypes.UINT8),
    }

    struct_label: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "uuid": (3 | uctypes.ARRAY, 13 | uctypes.UINT8),
    }

    struct_homescreen: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "uuid": (3 | uctypes.ARRAY, 128 | uctypes.UINT8),
    }

    struct_sessionkey: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "uuid": (3 | uctypes.ARRAY, 16 | uctypes.UINT8),
    }

    struct_ble_name: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "ble_name": (3 | uctypes.ARRAY, 16 | uctypes.UINT8),
    }

    struct_ble_version: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "ble_version": (3 | uctypes.ARRAY, 8 | uctypes.UINT8),
    }

    struct_SD_auth_key: uctypes.StructDict = {
        "has_value": 0 | uctypes.UINT8,
        "size": 1 | uctypes.UINT16,
        "ble_version": (3 | uctypes.ARRAY, 12 | uctypes.UINT8),
    }

    struct_public = {}

    offset = 0

    struct_public["version"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["device_id"] = (offset, struct_uuid)
    offset += uctypes.sizeof(struct_uuid, uctypes.LITTLE_ENDIAN)
    struct_public["language"] = (offset, struct_language)
    offset += uctypes.sizeof(struct_language, uctypes.LITTLE_ENDIAN)
    struct_public["label"] = (offset, struct_label)
    offset += uctypes.sizeof(struct_label, uctypes.LITTLE_ENDIAN)
    struct_public["use_passphrase"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["passphrase_always_on_device"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["autolock_delay_ms"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["homescreen"] = (offset, struct_homescreen)
    offset += uctypes.sizeof(struct_homescreen, uctypes.LITTLE_ENDIAN)
    struct_public["session_key"] = (offset, struct_sessionkey)
    offset += uctypes.sizeof(struct_sessionkey, uctypes.LITTLE_ENDIAN)
    struct_public["ble_name"] = (offset, struct_ble_name)
    offset += uctypes.sizeof(struct_ble_name, uctypes.LITTLE_ENDIAN)
    struct_public["ble_version"] = (offset, struct_ble_version)
    offset += uctypes.sizeof(struct_ble_version, uctypes.LITTLE_ENDIAN)
    struct_public["ble_enabled"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["brightness"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["autoshutdown_delay_ms"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["wallpaper_counts"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["use_usb_protect"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["use_random_pin_map"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["keyboard_haptic"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["tap_awake"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["animation"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["trezor_compatible"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["experimental_features"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["rotation"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["slip39_identifier_device"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["slip39_iteration_e_device"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["in_progress"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["dry_run"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_public["slip39_identifier_recover"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["slip39_threshold"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["remaining"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["slip39_iteration_e_recover"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_public["slip39_group_count"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    # public_field = uctypes.struct(0, struct_public, uctypes.LITTLE_ENDIAN)
    assert (
        uctypes.sizeof(struct_public, uctypes.LITTLE_ENDIAN) < _PUBLIC_REGION_SIZE
    ), "public region size too large"

    struct_private = {}
    offset = 0
    struct_private["need_backup"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_private["flags"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_private["unfinished_backup"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_private["no_backup"] = (offset, struct_bool)
    offset += uctypes.sizeof(struct_bool, uctypes.LITTLE_ENDIAN)
    struct_private["backup_type"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_private["safety_check"] = (offset, struct_uint32)
    offset += uctypes.sizeof(struct_uint32, uctypes.LITTLE_ENDIAN)
    struct_private["sd_auth_key"] = (offset, struct_SD_auth_key)
    offset += uctypes.sizeof(struct_SD_auth_key, uctypes.LITTLE_ENDIAN)
    # private_field = uctypes.struct(0, struct_private, uctypes.LITTLE_ENDIAN)
    assert (
        uctypes.sizeof(struct_private, uctypes.LITTLE_ENDIAN) < _PRIVATE_REGION_SIZE
    ), "private region size too large"

    _PRIVATE_FLAG = const(1 << 31)

    DEVICE_ID = struct_public["device_id"][0]
    _VERSION = struct_public["version"][0]
    _LANGUAGE = struct_public["language"][0]
    _LABEL = struct_public["label"][0]
    _USE_PASSPHRASE = struct_public["use_passphrase"][0]
    _PASSPHRASE_ALWAYS_ON_DEVICE = struct_public["passphrase_always_on_device"][0]
    _AUTOLOCK_DELAY_MS = struct_public["autolock_delay_ms"][0]
    _HOMESCREEN = struct_public["homescreen"][0]
    _BLE_NAME = struct_public["ble_name"][0]
    _BLE_VERSION = struct_public["ble_version"][0]
    _BLE_ENABLED = struct_public["ble_enabled"][0]
    _BRIGHTNESS = struct_public["brightness"][0]
    _AUTOSHUTDOWN_DELAY_MS = struct_public["autoshutdown_delay_ms"][0]
    _WALLPAPER_COUNTS = struct_public["wallpaper_counts"][0]
    _USE_USB_PROTECT = struct_public["use_usb_protect"][0]
    _USE_RANDOM_PIN_MAP = struct_public["use_random_pin_map"][0]
    _KEYBOARD_HAPTIC = struct_public["keyboard_haptic"][0]
    _TAP_AWAKE = struct_public["tap_awake"][0]
    _ANIMATION = struct_public["animation"][0]
    _TREZOR_COMPATIBLE = struct_public["trezor_compatible"][0]
    _EXPERIMENTAL_FEATURES = struct_public["experimental_features"][0]
    _ROTATION = struct_public["rotation"][0]
    _SLIP39_IDENTIFIER_DEVICE = struct_public["slip39_identifier_device"][0]
    _SLIP39_ITERATION_E_DEVICE = struct_public["slip39_iteration_e_device"][0]

    U2F_COUNTER = 0x00  # u2f counter

    # recovery key
    _IN_PROGRESS = struct_public["in_progress"][0]
    _DRY_RUN = struct_public["dry_run"][0]
    _SLIP39_IDENTIFIER_RECOVER = struct_public["slip39_identifier_recover"][0]
    _SLIP39_THRESHOLD = struct_public["slip39_threshold"][0]
    _REMAINING = struct_public["remaining"][0]
    _SLIP39_ITERATION_E_RECOVER = struct_public["slip39_iteration_e_recover"][0]
    _SLIP39_GROUP_COUNT = struct_public["slip39_group_count"][0]

    _NEEDS_BACKUP = _PRIVATE_FLAG | struct_private["need_backup"][0]
    _FLAGS = _PRIVATE_FLAG | struct_private["flags"][0]
    _UNFINISHED_BACKUP = _PRIVATE_FLAG | struct_private["unfinished_backup"][0]
    _NO_BACKUP = _PRIVATE_FLAG | struct_private["no_backup"][0]
    _BACKUP_TYPE = _PRIVATE_FLAG | struct_private["backup_type"][0]
    _SAFETY_CHECK_LEVEL = _PRIVATE_FLAG | struct_private["safety_check"][0]
    _SD_SALT_AUTH_KEY = _PRIVATE_FLAG | struct_private["sd_auth_key"][0]

else:
    # fmt: off
    # Keys:
    DEVICE_ID                  = (0x00)  # bytes
    _VERSION                   = (0x01)  # int
    _MNEMONIC_SECRET           = (0x02)  # bytes
    _LANGUAGE                  = (0x03)  # str
    _LABEL                     = (0x04)  # str
    _USE_PASSPHRASE            = (0x05)  # bool (0x01 or empty)
    _HOMESCREEN                = (0x06)  # int
    _NEEDS_BACKUP              = (0x07)  # bool (0x01 or empty)
    _FLAGS                     = (0x08)  # int
    U2F_COUNTER                = (0x09)  # int
    _PASSPHRASE_ALWAYS_ON_DEVICE = (0x0A)  # bool (0x01 or empty)
    _UNFINISHED_BACKUP         = (0x0B)  # bool (0x01 or empty)
    _AUTOLOCK_DELAY_MS         = (0x0C)  # int
    _NO_BACKUP                 = (0x0D)  # bool (0x01 or empty)
    _BACKUP_TYPE               = (0x0E)  # int
    _ROTATION                  = (0x0F)  # int
    _SLIP39_IDENTIFIER_DEVICE  = (0x10)  # int
    _SLIP39_ITERATION_E_DEVICE = (0x11)  # int
    _SD_SALT_AUTH_KEY          = (0x12)  # bytes
    INITIALIZED                = (0x13)  # bool (0x01 or empty)
    _SAFETY_CHECK_LEVEL        = (0x14)  # int
    _EXPERIMENTAL_FEATURES     = (0x15)  # bool (0x01 or empty)

    _BLE_NAME = (0x80)  # bytes
    _BLE_VERSION = (0x81)  # bytes
    _BLE_ENABLED = (0x82)  # bool (0x01 or empty)
    _BRIGHTNESS = (0x83)   # int
    _AUTOSHUTDOWN_DELAY_MS = (0x84)  # int
    # deprecated
    _PIN_MAP_TYPES = (0x84)  # int

    _WALLPAPER_COUNTS = (0x85)  # int
    _USE_USB_PROTECT = (0x86)  # bool (0x01 or empty)
    _USE_RANDOM_PIN_MAP = (0x87)  # bool (0x01 or empty)
    _KEYBOARD_HAPTIC = (0x88)   # bool
    _TAP_AWAKE = (0x89)  # bool
    _ANIMATION = (0x8A)  # bool
    _TREZOR_COMPATIBLE = (0x8B)
    # fmt: on
SAFETY_CHECK_LEVEL_STRICT: Literal[0] = const(0)
SAFETY_CHECK_LEVEL_PROMPT: Literal[1] = const(1)
_DEFAULT_SAFETY_CHECK_LEVEL = SAFETY_CHECK_LEVEL_STRICT

if TYPE_CHECKING:
    StorageSafetyCheckLevel = Literal[0, 1]

HOMESCREEN_MAXSIZE = 16384
LABEL_MAXLENGTH = 32

if __debug__:
    AUTOLOCK_DELAY_MINIMUM = AUTOSHUTDOWN_DELAY_MINIMUM = 10 * 1000  # 10 seconds
else:
    AUTOLOCK_DELAY_MINIMUM = AUTOSHUTDOWN_DELAY_MINIMUM = 60 * 1000  # 1 minute

AUTOLOCK_DELAY_DEFAULT = AUTOSHUTDOWN_DELAY_DEFAULT = 10 * 60 * 1000  # 10 minutes
# autolock intervals larger than AUTOLOCK_DELAY_MAXIMUM cause issues in the scheduler
if __debug__:
    AUTOSHUTDOWN_DELAY_MAXIMUM = AUTOLOCK_DELAY_MAXIMUM = 0x2000_0000  # ~6 days
else:
    AUTOSHUTDOWN_DELAY_MAXIMUM = AUTOLOCK_DELAY_MAXIMUM = 0x1000_0000  # ~3 days

# Length of SD salt auth tag.
# Other SD-salt-related constants are in sd_salt.py
SD_SALT_AUTH_KEY_LEN_BYTES = const(16)

PIN_MAX_ATTEMPTS = 10


def is_version_stored() -> bool:
    return bool(common.get(_NAMESPACE, _VERSION))


def get_version() -> bytes | None:
    return common.get(_NAMESPACE, _VERSION)


def set_version(version: bytes) -> None:
    common.set(_NAMESPACE, _VERSION, version)


def get_firmware_version() -> str:
    return utils.ONEKEY_VERSION


def get_storage() -> str:
    if utils.EMULATOR:
        return "14 GB"
    return config.get_capacity()


def set_ble_name(name: str) -> None:
    global _BLE_NAME_VALUE
    common.set(_NAMESPACE, _BLE_NAME, name.encode(), True)
    _BLE_NAME_VALUE = name


def get_ble_name() -> str:
    global _BLE_NAME_VALUE
    if _BLE_NAME_VALUE is None:
        ble_name = common.get(_NAMESPACE, _BLE_NAME, public=True)
        if ble_name:
            _BLE_NAME_VALUE = ble_name.decode()
        else:
            return "T1122" if utils.EMULATOR else ""
    return _BLE_NAME_VALUE


def ble_enabled() -> bool:
    global _BLE_ENABLED_VALUE
    if _BLE_ENABLED_VALUE is None:
        _BLE_ENABLED_VALUE = common.get_bool(_NAMESPACE, _BLE_ENABLED, public=True)
    return _BLE_ENABLED_VALUE


def set_ble_status(enable: bool) -> None:
    global _BLE_ENABLED_VALUE
    if _BLE_ENABLED_VALUE == enable:
        return
    common.set_bool(
        _NAMESPACE,
        _BLE_ENABLED,
        enable,
        public=True,
    )
    _BLE_ENABLED_VALUE = enable


def set_ble_version(version: str) -> None:
    """Set ble firmware version."""
    global _BLE_VERSION_VALUE
    common.set(_NAMESPACE, _BLE_VERSION, version.encode(), True)
    _BLE_VERSION_VALUE = version


def get_ble_version() -> str:
    global _BLE_VERSION_VALUE
    if _BLE_VERSION_VALUE is None:
        ble_version = common.get(_NAMESPACE, _BLE_VERSION, public=True)
        _BLE_VERSION_VALUE = ble_version.decode() if ble_version else ""
    return _BLE_VERSION_VALUE


def get_model() -> str:
    return "OneKey Touch"


def get_serial() -> str:
    if utils.EMULATOR:
        return "TC01WBD202206030544190000099"
    return config.get_serial()


def set_brightness(brightness: int) -> None:
    # valid value range  0-255
    common.set(_NAMESPACE, _BRIGHTNESS, brightness.to_bytes(2, "big"), public=True)


def get_brightness() -> int:
    global _BRIGHTNESS_VALUE
    if _BRIGHTNESS_VALUE is None:
        brightness = common.get(_NAMESPACE, _BRIGHTNESS, public=True)
        if brightness is None:
            # default brightness is 150
            _BRIGHTNESS_VALUE = 150
        else:
            _BRIGHTNESS_VALUE = int.from_bytes(brightness, "big")
        return _BRIGHTNESS_VALUE
    else:
        return _BRIGHTNESS_VALUE


def set_random_pin_map_enable(enable: bool):
    global _USE_RANDOM_PIN_MAP_VALUE
    common.set_bool(_NAMESPACE, _USE_RANDOM_PIN_MAP, enable, public=True)
    _USE_RANDOM_PIN_MAP_VALUE = enable


def is_random_pin_map_enabled() -> bool:
    global _USE_RANDOM_PIN_MAP_VALUE
    if _USE_RANDOM_PIN_MAP_VALUE is None:
        _USE_RANDOM_PIN_MAP_VALUE = common.get_bool(
            _NAMESPACE, _USE_RANDOM_PIN_MAP, public=True
        )
    return _USE_RANDOM_PIN_MAP_VALUE


def is_usb_lock_enabled() -> bool:
    global _USE_USB_PROTECT_VALUE
    if _USE_USB_PROTECT_VALUE is None:
        _USE_USB_PROTECT_VALUE = common.get_bool(
            _NAMESPACE, _USE_USB_PROTECT, public=True
        )
    return _USE_USB_PROTECT_VALUE


def set_usb_lock_enable(enable: bool) -> None:
    global _USE_USB_PROTECT_VALUE
    common.set_bool(_NAMESPACE, _USE_USB_PROTECT, enable, public=True)
    _USE_USB_PROTECT_VALUE = enable


def is_tap_awake_enabled() -> bool:
    global _TAP_AWAKE_VALUE
    if _TAP_AWAKE_VALUE is None:
        _TAP_AWAKE_VALUE = common.get_bool(_NAMESPACE, _TAP_AWAKE, public=True)
    return _TAP_AWAKE_VALUE


def set_tap_awake_enable(enable: bool) -> None:
    global _TAP_AWAKE_VALUE
    common.set_bool(
        _NAMESPACE,
        _TAP_AWAKE,
        enable,
        public=True,
    )
    _TAP_AWAKE_VALUE = enable


def is_animation_enabled() -> bool:
    global _ANIMATION_VALUE
    if _ANIMATION_VALUE is None:
        _ANIMATION_VALUE = common.get_bool(_NAMESPACE, _ANIMATION, public=True)
    return _ANIMATION_VALUE


def set_animation_enable(enable: bool) -> None:
    global _ANIMATION_VALUE
    common.set(
        _NAMESPACE,
        _ANIMATION,
        common._TRUE_BYTE if enable else common._FALSE_BYTE,
        public=True,
    )
    _ANIMATION_VALUE = enable


def keyboard_haptic_enabled() -> bool:
    global _KEYBOARD_HAPTIC_VALUE
    if _KEYBOARD_HAPTIC_VALUE is None:
        enabled = common.get(_NAMESPACE, _KEYBOARD_HAPTIC, public=True)
        if enabled == common._FALSE_BYTE:
            _KEYBOARD_HAPTIC_VALUE = False
        else:
            _KEYBOARD_HAPTIC_VALUE = True
        return _KEYBOARD_HAPTIC_VALUE
    else:
        return _KEYBOARD_HAPTIC_VALUE


def toggle_keyboard_haptic(enable: bool) -> None:
    global _KEYBOARD_HAPTIC_VALUE
    common.set(
        _NAMESPACE,
        _KEYBOARD_HAPTIC,
        common._TRUE_BYTE if enable else common._FALSE_BYTE,
        public=True,
    )
    _KEYBOARD_HAPTIC_VALUE = enable


def increase_wp_cnts() -> None:
    global _WALLPAPER_COUNTS_VALUE
    cur_cnt = get_wp_cnts()
    cnts = cur_cnt + 1
    common.set(_NAMESPACE, _WALLPAPER_COUNTS, cnts.to_bytes(2, "big"), public=True)
    _WALLPAPER_COUNTS_VALUE = cnts


def get_wp_cnts() -> int:
    global _WALLPAPER_COUNTS_VALUE
    if _WALLPAPER_COUNTS_VALUE is None:
        cnts = common.get(_NAMESPACE, _WALLPAPER_COUNTS, public=True)
        if cnts is None:
            _WALLPAPER_COUNTS_VALUE = 0
        else:
            _WALLPAPER_COUNTS_VALUE = int.from_bytes(cnts, "big")
    return _WALLPAPER_COUNTS_VALUE


def is_initialized() -> bool:
    global _INITIALIZED_VALUE
    if utils.EMULATOR:
        return common.get_bool(_NAMESPACE, INITIALIZED, public=True)
    else:
        if _INITIALIZED_VALUE is None:
            _INITIALIZED_VALUE = config.is_initialized()
        return _INITIALIZED_VALUE


def _new_device_id() -> str:
    from trezorcrypto import random  # avoid pulling in trezor.crypto

    return hexlify(random.bytes(12)).decode().upper()


def get_device_id() -> str:
    dev_id = common.get(_NAMESPACE, DEVICE_ID, public=True)
    if not dev_id:
        dev_id = _new_device_id().encode()
        common.set(_NAMESPACE, DEVICE_ID, dev_id, public=True)
    return dev_id.decode()


def get_rotation() -> int:
    global _ROTATION_VALUE
    if _ROTATION_VALUE is None:
        rotation = common.get(_NAMESPACE, _ROTATION, public=True)
        _ROTATION_VALUE = int.from_bytes(rotation, "big") if rotation else 0
    return _ROTATION_VALUE


def set_rotation(value: int) -> None:
    if value not in (0, 90, 180, 270):
        raise ValueError  # unsupported display rotation
    global _ROTATION_VALUE
    common.set(_NAMESPACE, _ROTATION, value.to_bytes(2, "big"), True)  # public
    _ROTATION_VALUE = value


def get_label() -> str:
    """Get device label.

    Returns:
        str: if label == "", return default label "OneKey Touch" instead
    """
    global _LABEL_VALUE
    if _LABEL_VALUE is None:
        label = common.get(_NAMESPACE, _LABEL, True)  # public
        _LABEL_VALUE = label.decode() if label else utils.DEFAULT_LABEL
    return _LABEL_VALUE


def set_label(label: str) -> None:
    global _LABEL_VALUE
    if len(label) > LABEL_MAXLENGTH:
        raise ValueError  # label too long
    common.set(_NAMESPACE, _LABEL, label.encode(), True)  # public
    _LABEL_VALUE = label


def get_language() -> str:
    global _LANGUAGE_VALUE
    if _LANGUAGE_VALUE is None:
        lang = common.get(_NAMESPACE, _LANGUAGE, True)  # public
        if lang is None:
            _LANGUAGE_VALUE = "en"
        else:
            _LANGUAGE_VALUE = lang.decode()

    return _LANGUAGE_VALUE


def set_language(lang: str) -> None:
    global _LANGUAGE_VALUE
    from trezor.langs import langs_keys

    if lang == "en-US":
        lang = "en"
    if lang not in langs_keys:
        raise ValueError(
            f"all support ISO_639-1 language keys include {' '.join(langs_keys)})"
        )
    _LANGUAGE_VALUE = lang
    common.set(_NAMESPACE, _LANGUAGE, lang.encode(), True)  # public


def get_mnemonic_secret() -> bytes | None:
    if utils.EMULATOR:
        return common.get(_NAMESPACE, _MNEMONIC_SECRET)
    else:
        return config.se_export_mnemonic()


def get_backup_type() -> BackupType:
    from trezor.enums import BackupType

    global _BACKUP_TYPE_VALUE
    if _BACKUP_TYPE_VALUE is None:
        backup_type = common.get_uint8(_NAMESPACE, _BACKUP_TYPE)
        if backup_type is None:
            backup_type = BackupType.Bip39

        if backup_type not in (
            BackupType.Bip39,
            BackupType.Slip39_Basic,
            BackupType.Slip39_Advanced,
        ):
            # Invalid backup type
            raise RuntimeError
        _BACKUP_TYPE_VALUE = backup_type
    return _BACKUP_TYPE_VALUE  # type: ignore [int-into-enum]


def is_passphrase_enabled() -> bool:
    global _USE_PASSPHRASE_VALUE
    if _USE_PASSPHRASE_VALUE is None:
        _USE_PASSPHRASE_VALUE = common.get_bool(_NAMESPACE, _USE_PASSPHRASE)
    return _USE_PASSPHRASE_VALUE


def set_passphrase_enabled(enable: bool) -> None:
    global _USE_PASSPHRASE_VALUE
    global _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE
    common.set_bool(_NAMESPACE, _USE_PASSPHRASE, enable)
    if not enable:
        set_passphrase_always_on_device(False)
        _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE = False
    _USE_PASSPHRASE_VALUE = enable


def get_homescreen() -> str | None:
    global _HOMESCREEN_VALUE

    if _HOMESCREEN_VALUE is None:
        homescreen = common.get(_NAMESPACE, _HOMESCREEN, public=True)
        _HOMESCREEN_VALUE = (
            homescreen.decode() if homescreen else "A:/res/wallpaper-1.png"
        )
    return _HOMESCREEN_VALUE


def set_homescreen(full_path: str) -> None:
    # if len(homescreen) > HOMESCREEN_MAXSIZE:
    #     raise ValueError  # homescreen too large
    global _HOMESCREEN_VALUE
    common.set(_NAMESPACE, _HOMESCREEN, full_path.encode(), public=True)
    _HOMESCREEN_VALUE = full_path


def store_mnemonic_secret(
    secret: bytes,
    backup_type: BackupType,
    needs_backup: bool = False,
    no_backup: bool = False,
) -> None:
    global _NO_BACKUP_VALUE
    global _NEEDS_BACKUP_VALUE
    global _INITIALIZED_VALUE
    set_version(common.STORAGE_VERSION_CURRENT)
    if utils.EMULATOR:
        common.set(_NAMESPACE, _MNEMONIC_SECRET, secret)
        common.set_bool(_NAMESPACE, INITIALIZED, True, public=True)
    else:
        config.se_import_mnemonic(secret)  # type: ignore["se_import_mnemonic" is not a known member of module]
    common.set_uint8(_NAMESPACE, _BACKUP_TYPE, backup_type)
    common.set_true_or_delete(_NAMESPACE, _NO_BACKUP, no_backup)
    if not no_backup:
        set_backed_up(needs_backup)
        _NEEDS_BACKUP_VALUE = needs_backup
    _NO_BACKUP_VALUE = no_backup
    _INITIALIZED_VALUE = True


def needs_backup() -> bool:
    if utils.EMULATOR:
        return common.get_bool(_NAMESPACE, _NEEDS_BACKUP)
    else:
        return config.get_needs_backup()


def set_backed_up(stat: bool) -> None:
    if utils.EMULATOR:
        common.delete(_NAMESPACE, _NEEDS_BACKUP)
    else:
        config.set_needs_backup(stat)


def unfinished_backup() -> bool:
    global _UNFINISHED_BACKUP_VALUE
    if _UNFINISHED_BACKUP_VALUE is None:
        _UNFINISHED_BACKUP_VALUE = common.get_bool(_NAMESPACE, _UNFINISHED_BACKUP)
    return _UNFINISHED_BACKUP_VALUE


def set_unfinished_backup(state: bool) -> None:
    global _UNFINISHED_BACKUP_VALUE
    common.set_bool(_NAMESPACE, _UNFINISHED_BACKUP, state)
    _UNFINISHED_BACKUP_VALUE = state


def no_backup() -> bool:
    global _NO_BACKUP_VALUE
    if _NO_BACKUP_VALUE is None:
        _NO_BACKUP_VALUE = common.get_bool(_NAMESPACE, _NO_BACKUP)
    return _NO_BACKUP_VALUE


def get_passphrase_always_on_device() -> bool:
    """
    This is backwards compatible with _PASSPHRASE_SOURCE:
    - If ASK(0) => returns False, the check against b"\x01" in get_bool fails.
    - If DEVICE(1) => returns True, the check against b"\x01" in get_bool succeeds.
    - If HOST(2) => returns False, the check against b"\x01" in get_bool fails.
    Only support input on device.
    """
    # Some models do not support passphrase input on device
    if utils.MODEL in ("1", "R"):
        return False
    global _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE
    if _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE is None:
        _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE = common.get_bool(
            _NAMESPACE, _PASSPHRASE_ALWAYS_ON_DEVICE
        )
    return _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE
    # return is_passphrase_enabled()


def set_passphrase_always_on_device(enable: bool) -> None:
    global _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE
    common.set_bool(_NAMESPACE, _PASSPHRASE_ALWAYS_ON_DEVICE, enable)
    _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE = enable


def get_flags() -> int:
    global _FLAGS_VALUE
    if _FLAGS_VALUE is None:
        b = common.get(_NAMESPACE, _FLAGS)
        _FLAGS_VALUE = int.from_bytes(b, "big") if b else 0
    return _FLAGS_VALUE


def set_flags(flags: int) -> None:
    global _FLAGS_VALUE
    b = common.get(_NAMESPACE, _FLAGS)
    if b is None:
        i = 0
    else:
        i = int.from_bytes(b, "big")
    flags = (flags | i) & 0xFFFF_FFFF
    if flags != i:
        common.set(_NAMESPACE, _FLAGS, flags.to_bytes(4, "big"))
        _FLAGS_VALUE = flags


def _normalize_autolock_delay(delay_ms: int) -> int:
    delay_ms = max(delay_ms, AUTOLOCK_DELAY_MINIMUM)
    delay_ms = min(delay_ms, AUTOLOCK_DELAY_MAXIMUM)
    return delay_ms


def get_autolock_delay_ms() -> int:
    global _AUTOLOCK_DELAY_MS_VALUE
    if _AUTOLOCK_DELAY_MS_VALUE is None:
        b = common.get(_NAMESPACE, _AUTOLOCK_DELAY_MS)
        _AUTOLOCK_DELAY_MS_VALUE = (
            _normalize_autolock_delay(int.from_bytes(b, "big"))
            if b
            else AUTOLOCK_DELAY_DEFAULT
        )
    return _AUTOLOCK_DELAY_MS_VALUE


def set_autolock_delay_ms(delay_ms: int) -> None:
    global _AUTOLOCK_DELAY_MS_VALUE
    delay_ms = _normalize_autolock_delay(delay_ms)
    common.set(_NAMESPACE, _AUTOLOCK_DELAY_MS, delay_ms.to_bytes(4, "big"))
    utils.AUTO_POWER_OFF = False
    _AUTOLOCK_DELAY_MS_VALUE = delay_ms


def _normalize_autoshutdown_delay(delay_ms: int) -> int:
    delay_ms = max(delay_ms, AUTOSHUTDOWN_DELAY_MINIMUM)
    delay_ms = min(delay_ms, AUTOSHUTDOWN_DELAY_MAXIMUM)
    return delay_ms


def get_autoshutdown_delay_ms() -> int:
    global _AUTOSHUTDOWN_DELAY_MS_VALUE
    if _AUTOSHUTDOWN_DELAY_MS_VALUE is None:
        b = common.get(_NAMESPACE, _AUTOSHUTDOWN_DELAY_MS, public=True)
        _AUTOSHUTDOWN_DELAY_MS_VALUE = (
            _normalize_autoshutdown_delay(int.from_bytes(b, "big"))
            if b
            else AUTOSHUTDOWN_DELAY_DEFAULT
        )
    return _AUTOSHUTDOWN_DELAY_MS_VALUE


def set_autoshutdown_delay_ms(delay_ms: int) -> None:
    global _AUTOSHUTDOWN_DELAY_MS_VALUE
    _AUTOSHUTDOWN_DELAY_MS_VALUE = delay_ms
    delay_ms = _normalize_autoshutdown_delay(delay_ms)
    common.set(
        _NAMESPACE, _AUTOSHUTDOWN_DELAY_MS, delay_ms.to_bytes(4, "big"), public=True
    )


def next_u2f_counter() -> int:
    return common.next_counter(_NAMESPACE, U2F_COUNTER, writable_locked=True)


def set_u2f_counter(count: int) -> None:
    common.set_counter(_NAMESPACE, U2F_COUNTER, count, writable_locked=True)


def set_slip39_identifier(identifier: int) -> None:
    """
    The device's actual SLIP-39 identifier used in passphrase derivation.
    Not to be confused with recovery.identifier, which is stored only during
    the recovery process and it is copied here upon success.
    """
    common.set_uint16(_NAMESPACE, _SLIP39_IDENTIFIER_DEVICE, identifier)


def get_slip39_identifier() -> int | None:
    """The device's actual SLIP-39 identifier used in passphrase derivation."""
    return common.get_uint16(_NAMESPACE, _SLIP39_IDENTIFIER_DEVICE)


def set_slip39_iteration_exponent(exponent: int) -> None:
    """
    The device's actual SLIP-39 iteration exponent used in passphrase derivation.
    Not to be confused with recovery.iteration_exponent, which is stored only during
    the recovery process and it is copied here upon success.
    """
    common.set_uint8(_NAMESPACE, _SLIP39_ITERATION_E_DEVICE, exponent)


def get_slip39_iteration_exponent() -> int | None:
    """
    The device's actual SLIP-39 iteration exponent used in passphrase derivation.
    """
    return common.get_uint8(_NAMESPACE, _SLIP39_ITERATION_E_DEVICE)


def get_sd_salt_auth_key() -> bytes | None:
    """
    The key used to check the authenticity of the SD card salt.
    """
    auth_key = common.get(_NAMESPACE, _SD_SALT_AUTH_KEY, public=True)
    if auth_key is not None and len(auth_key) != SD_SALT_AUTH_KEY_LEN_BYTES:
        raise ValueError
    return auth_key


def set_sd_salt_auth_key(auth_key: bytes | None) -> None:
    """
    The key used to check the authenticity of the SD card salt.
    """
    if auth_key is not None:
        if len(auth_key) != SD_SALT_AUTH_KEY_LEN_BYTES:
            raise ValueError
        return common.set(_NAMESPACE, _SD_SALT_AUTH_KEY, auth_key, public=True)
    else:
        return common.delete(_NAMESPACE, _SD_SALT_AUTH_KEY, public=True)


# do not use this function directly, see apps.common.safety_checks instead
def safety_check_level() -> StorageSafetyCheckLevel:
    global _SAFETY_CHECK_LEVEL_VALUE
    if _SAFETY_CHECK_LEVEL_VALUE is None:
        level = common.get_uint8(_NAMESPACE, _SAFETY_CHECK_LEVEL)
        if level not in (SAFETY_CHECK_LEVEL_STRICT, SAFETY_CHECK_LEVEL_PROMPT):
            _SAFETY_CHECK_LEVEL_VALUE = _DEFAULT_SAFETY_CHECK_LEVEL
        else:
            _SAFETY_CHECK_LEVEL_VALUE = level
    assert _SAFETY_CHECK_LEVEL_VALUE is not None
    return _SAFETY_CHECK_LEVEL_VALUE  # type: ignore [int-into-enum]


# do not use this function directly, see apps.common.safety_checks instead
def set_safety_check_level(level: StorageSafetyCheckLevel) -> None:
    global _SAFETY_CHECK_LEVEL_VALUE
    if level not in (SAFETY_CHECK_LEVEL_STRICT, SAFETY_CHECK_LEVEL_PROMPT):
        raise ValueError
    common.set_uint8(_NAMESPACE, _SAFETY_CHECK_LEVEL, level)
    _SAFETY_CHECK_LEVEL_VALUE = level


@storage.cache.stored(storage.cache.STORAGE_DEVICE_EXPERIMENTAL_FEATURES)
def _get_experimental_features() -> bytes:
    if common.get_bool(_NAMESPACE, _EXPERIMENTAL_FEATURES):
        return b"\x01"
    else:
        return b""


def get_experimental_features() -> bool:
    return bool(_get_experimental_features())


def set_experimental_features(enabled: bool) -> None:
    cached_bytes = b"\x01" if enabled else b""
    storage.cache.set(storage.cache.STORAGE_DEVICE_EXPERIMENTAL_FEATURES, cached_bytes)
    common.set_true_or_delete(_NAMESPACE, _EXPERIMENTAL_FEATURES, enabled)


def is_trezor_compatible() -> bool:
    if utils.EMULATOR:  # in order to work with hwi
        return False
    enabled = common.get(_NAMESPACE, _TREZOR_COMPATIBLE, public=True)
    if enabled == common._FALSE_BYTE:
        return False
    return True


def enable_trezor_compatible(enable: bool) -> None:
    common.set(
        _NAMESPACE,
        _TREZOR_COMPATIBLE,
        common._TRUE_BYTE if enable else common._FALSE_BYTE,
        public=True,
    )


def clear_global_cache() -> None:
    global _LANGUAGE_VALUE
    global _LABEL_VALUE
    global _USE_PASSPHRASE_VALUE
    global _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE
    global _AUTOLOCK_DELAY_MS_VALUE
    global _HOMESCREEN_VALUE
    global _BLE_NAME_VALUE
    global _BLE_VERSION_VALUE
    global _BLE_ENABLED_VALUE
    global _AUTOSHUTDOWN_DELAY_MS_VALUE
    global _WALLPAPER_COUNTS_VALUE
    global _USE_USB_PROTECT_VALUE
    global _USE_RANDOM_PIN_MAP_VALUE
    global _TAP_AWAKE_VALUE
    global _KEYBOARD_HAPTIC_VALUE
    global _ANIMATION_VALUE
    global _EXPERIMENTAL_FEATURES_VALUE
    global _ROTATION_VALUE

    global _INITIALIZED_VALUE
    global _FLAGS_VALUE
    global _UNFINISHED_BACKUP_VALUE
    global _NO_BACKUP_VALUE
    global _BACKUP_TYPE_VALUE
    global _SAFETY_CHECK_LEVEL_VALUE

    _LANGUAGE_VALUE = None
    _LABEL_VALUE = None
    _USE_PASSPHRASE_VALUE = None
    _PASSPHRASE_ALWAYS_ON_DEVICE_VALUE = None
    _AUTOLOCK_DELAY_MS_VALUE = None
    _HOMESCREEN_VALUE = None
    _BLE_NAME_VALUE = None
    _BLE_VERSION_VALUE = None
    _BLE_ENABLED_VALUE = None
    _AUTOSHUTDOWN_DELAY_MS_VALUE = None
    _WALLPAPER_COUNTS_VALUE = None
    _USE_USB_PROTECT_VALUE = None
    _USE_RANDOM_PIN_MAP_VALUE = None
    _TAP_AWAKE_VALUE = None
    _KEYBOARD_HAPTIC_VALUE = None
    _ANIMATION_VALUE = None
    _EXPERIMENTAL_FEATURES_VALUE = None
    _ROTATION_VALUE = None

    _INITIALIZED_VALUE = None
    _FLAGS_VALUE = None
    _UNFINISHED_BACKUP_VALUE = None
    _NO_BACKUP_VALUE = None
    _BACKUP_TYPE_VALUE = None
    _SAFETY_CHECK_LEVEL_VALUE = None
