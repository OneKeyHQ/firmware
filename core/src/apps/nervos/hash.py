from trezor.crypto import hashlib

from .utils import bytes_to_hex_str


def ckb_hasher():
    return hashlib.blake2b(outlen=32, personal=b"ckb-default-hash")


def ckb_hash(message: bytes) -> str:
    hasher = ckb_hasher()
    hasher.update(message)
    hash_bytes = hasher.digest()
    hex_str = bytes_to_hex_str(hash_bytes)
    hasher_hex = "0x" + hex_str
    return hasher_hex


def ckb_blake160(message: bytes) -> str:
    return ckb_hash(message)[0:42]
