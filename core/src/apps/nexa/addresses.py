from typing import TYPE_CHECKING

from trezor.crypto import cashaddr
from trezor.crypto.scripts import sha256_ripemd160

if TYPE_CHECKING:
    from trezor.crypto import bip32

else:
    pass


class Prefix:
    Mainnet = "nexa"
    Testnet = "nexatest"
    Regnet = "nexreg"

    @classmethod
    def is_valid(cls, prefix: str) -> bool:
        return prefix in (cls.Mainnet, cls.Testnet, cls.Regnet)


def encode_address(
    node: bip32.HDNode,
    prefix: str = Prefix.Mainnet,
) -> str:
    pubkey = node.public_key()
    script = []
    script.append(0x17)  # script len
    script.append(0x00)  # OP_FALSE
    script.append(0x51)  # OP_PUSHDATA1
    script.append(0x14)  # script len
    script.extend(sha256_ripemd160(bytes([33]) + pubkey).digest())
    address = cashaddr.encode(prefix, 152, bytes(script))
    return address
