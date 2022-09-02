from ubinascii import hexlify

from trezor.crypto.hashlib import sha3_256

TRANSACTION_PREFIX = b"STARCOIN::RawUserTransaction"

MESSAGE_PREFIX = b"STARCOIN::SigningMessage"


def get_address_from_public_key(pubkey):
    msg = pubkey + b"\x00"
    address = sha3_256(msg, keccak=False).digest()[-16:]
    return "0x" + hexlify(address).decode()


def serialize_u32_as_uleb128(value: int):
    res = bytearray()
    while value >= 0x80:
        b = (value & 0x7F) | 0x80
        res.append(b)
        value >>= 7
    res.append(value)
    return res
