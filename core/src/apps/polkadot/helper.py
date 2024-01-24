from typing import TYPE_CHECKING

from trezor.crypto import base58
from trezor.crypto.hashlib import blake2b
from trezor.lvglui.scrs import lv

from . import (
    ICON,
    ICON_ASTAR,
    ICON_JOY,
    ICON_KSM,
    ICON_MANTA,
    ICON_WESTEND,
    PRIMARY_COLOR,
    PRIMARY_COLOR_ASTAR,
    PRIMARY_COLOR_JOY,
    PRIMARY_COLOR_KSM,
    PRIMARY_COLOR_MAMTA,
    PRIMARY_COLOR_WESTEND,
)

if TYPE_CHECKING:
    from trezor.wire import Context


POLKADOT_ADDRESS_TYPES = [
    ["polkadot", 0],
    ["kusama", 2],
    ["westend", 42],
    ["astar", 5],
    ["joystream", 126],
    ["manta", 77],
]

COIN_AMOUNT_DECIMAL_PLACES = 10
COIN_AMOUNT_DECIMAL_PLACES_12 = 12
COIN_AMOUNT_DECIMAL_PLACES_18 = 18
POLKADOT_TICKER = "DOT"
KUSAMA_COIN_TICKER = "KSM"
WESTEND_COIN_TICKER = "WND"
ASTAR_COIN_TICKER = "ASTR"
JOY_COIN_TICKER = "JOY"
MANTA_COIN_TICKER = "MANTA"


def ss58_encode(address: bytes, ss58_format: int = 42) -> str:
    """
    Encodes an account ID to an Substrate address according to provided address_type
    Parameters
    ----------
    address
    ss58_format
    Returns
    -------
    """
    checksum_prefix = b"SS58PRE"

    if ss58_format < 0 or ss58_format > 16383 or ss58_format in [46, 47]:
        raise ValueError("Invalid value for ss58_format")

    address_bytes = address
    if len(address_bytes) in [32, 33]:
        # Checksum size is 2 bytes for public key
        checksum_length = 2
    elif len(address_bytes) in [1, 2, 4, 8]:
        # Checksum size is 1 byte for account index
        checksum_length = 1
    else:
        raise ValueError("Invalid length for address")

    if ss58_format < 64:
        ss58_format_bytes = bytes([ss58_format])
    else:
        ss58_format_bytes = bytes(
            [
                ((ss58_format & 0b0000_0000_1111_1100) >> 2) | 0b0100_0000,
                (ss58_format >> 8) | ((ss58_format & 0b0000_0000_0000_0011) << 6),
            ]
        )

    input_bytes = ss58_format_bytes + address_bytes
    checksum = blake2b(checksum_prefix + input_bytes).digest()

    return base58.encode(input_bytes + checksum[:checksum_length])


def update_chain_res(ctx: Context, network: str) -> tuple[str, str, int]:
    chain_name = "Polkadot"
    symbol = POLKADOT_TICKER
    decimal = COIN_AMOUNT_DECIMAL_PLACES
    if network == "polkadot":
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    elif network == "kusama":
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR_KSM), ICON_KSM
        chain_name = "kusama"
        symbol = KUSAMA_COIN_TICKER
        decimal = COIN_AMOUNT_DECIMAL_PLACES_12
    elif network == "westend":
        ctx.primary_color, ctx.icon_path = (
            lv.color_hex(PRIMARY_COLOR_WESTEND),
            ICON_WESTEND,
        )
        chain_name = "Westend"
        symbol = WESTEND_COIN_TICKER
        decimal = COIN_AMOUNT_DECIMAL_PLACES_12
    elif network == "astar":
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR_ASTAR), ICON_ASTAR
        chain_name = "Astar"
        symbol = ASTAR_COIN_TICKER
        decimal = COIN_AMOUNT_DECIMAL_PLACES_18
    elif network == "joystream":
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR_JOY), ICON_JOY
        chain_name = "Joystream"
        symbol = JOY_COIN_TICKER
        decimal = COIN_AMOUNT_DECIMAL_PLACES
    elif network == "manta":
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR_MAMTA), ICON_MANTA
        chain_name = "Manta"
        symbol = MANTA_COIN_TICKER
        decimal = COIN_AMOUNT_DECIMAL_PLACES_18
    else:
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        chain_name = "UNKN Chain"
        symbol = " UNKN"
        decimal = 0
    return chain_name, symbol, decimal


def get_address_type(network: str) -> int:
    address_type = 42
    for element in POLKADOT_ADDRESS_TYPES:
        if network == element[0]:
            address_type = element[1]

    return address_type
