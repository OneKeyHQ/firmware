from typing import TYPE_CHECKING
from ubinascii import unhexlify

from trezor import wire
from trezor.crypto import bip32
from trezor.lvglui.scrs import lv
from trezor.messages import NervosAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import auto_keychain
from apps.nervos.hash import ckb_blake160

from . import ICON, PRIMARY_COLOR

if TYPE_CHECKING:
    from trezor.messages import NervosGetAddress

CODE_INDEX_SECP256K1_SINGLE = 0x00
FORMAT_TYPE_SHORT = 0x01


class Prefix:
    Mainnet = "ckb"
    Testnet = "ckt"


CHARSET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l"


def convertbits(data, frombits, tobits, pad=True):
    """General power-of-2 base conversion."""
    acc = 0
    bits = 0
    ret = []
    maxv = (1 << tobits) - 1
    max_acc = (1 << (frombits + tobits - 1)) - 1
    for value in data:
        if value < 0 or (value >> frombits):
            return None
        acc = ((acc << frombits) | value) & max_acc
        bits += frombits
        while bits >= tobits:
            bits -= tobits
            ret.append((acc >> bits) & maxv)
    if pad:
        if bits:
            ret.append((acc << (tobits - bits)) & maxv)
    elif bits >= frombits or ((acc << (tobits - bits)) & maxv):
        return None
    return ret


def generate_ckb_short_address(node: bip32.HDNode, network="mainnet") -> str:
    hrp = {"ckb": Prefix.Mainnet, "ckt": Prefix.Testnet}[network]
    public_key = node.public_key()
    args = ckb_blake160(public_key)[2:]
    hrpexp = bech32_hrp_expand(hrp)
    format_type = FORMAT_TYPE_SHORT
    payload = bytes([format_type, CODE_INDEX_SECP256K1_SINGLE]) + unhexlify(args)
    data_part = convertbits(payload, 8, 5)
    if data_part is None:
        data_part = []
    values = hrpexp + data_part
    polymod = bech32_polymod(values + [0, 0, 0, 0, 0, 0]) ^ 1
    checksum = [(polymod >> 5 * (5 - i)) & 31 for i in range(6)]
    combined = data_part + checksum
    addr = hrp + "1" + "".join([CHARSET[d] for d in combined])
    return addr


def bech32_hrp_expand(hrp):
    """Expand the HRP into values for checksum computation."""
    return [ord(x) >> 5 for x in hrp] + [0] + [ord(x) & 31 for x in hrp]


def bech32_polymod(values):
    """Internal function that computes the Bech32 checksum."""
    generator = [0x3B6A57B2, 0x26508E6D, 0x1EA119FA, 0x3D4233DD, 0x2A1462B3]
    chk = 1
    for value in values:
        top = chk >> 25
        chk = (chk & 0x1FFFFFF) << 5 ^ value
        for i in range(5):
            chk ^= generator[i] if ((top >> i) & 1) else 0
    return chk


def bech32_create_checksum(hrp, data):
    """Compute the checksum values given HRP and data."""
    values = bech32_hrp_expand(hrp) + data
    polymod = bech32_polymod(values + [0, 0, 0, 0, 0, 0]) ^ 1
    return [(polymod >> 5 * (5 - i)) & 31 for i in range(6)]


@auto_keychain(__name__)
async def get_address(
    ctx: wire.Context, msg: NervosGetAddress, keychain
) -> NervosAddress:

    if msg.network not in ["ckb", "ckt"]:
        raise ValueError(f"Invalid network: {msg.network}")
    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    address = generate_ckb_short_address(node, network=msg.network)
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="CKB",
        )

    return NervosAddress(address=address)
