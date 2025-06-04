from typing import TYPE_CHECKING

from trezor.crypto import cashaddr
from trezor.crypto.curve import bip340

if TYPE_CHECKING:
    from enum import IntEnum
    from trezor.crypto import bip32

else:
    IntEnum = object


class Version(IntEnum):
    PubKey = 0
    PubKeyECDSA = 1
    ScriptHash = 8

    @classmethod
    def public_key_len(cls, version: int) -> int:
        if version == cls.PubKeyECDSA:
            return 33
        elif version in (cls.PubKey, cls.ScriptHash):
            return 32
        else:
            raise ValueError("Unknown version")


class Prefix:
    Mainnet = "kaspa"
    Testnet = "kaspatest"
    Simnet = "kaspasim"
    Devnet = "kaspadev"

    @classmethod
    def is_valid(cls, prefix: str) -> bool:
        return prefix in (cls.Mainnet, cls.Testnet, cls.Simnet, cls.Devnet)


def encode_address(
    node: bip32.HDNode,
    scheme: str,
    prefix: str = Prefix.Mainnet,
    use_tweak: bool = True,
) -> str:
    raw_pubkey = node.public_key()
    if scheme == "schnorr":
        pubkey = (
            bip340.tweak_public_key(raw_pubkey[1:]) if use_tweak else raw_pubkey[1:]
        )
        assert len(pubkey) == Version.public_key_len(Version.PubKey)
        version = Version.PubKey
    else:
        pubkey = raw_pubkey
        assert len(pubkey) == Version.public_key_len(Version.PubKeyECDSA)
        version = Version.PubKeyECDSA

    return cashaddr.encode(prefix, version, pubkey)
