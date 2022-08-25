from ubinascii import hexlify

from trezor.crypto.hashlib import sha3_256

# Prefix_bytes with SHA3_256 hash bytes of string `STARCOIN::RawUserTransaction`
TRANSACTION_PREFIX = b"\xb2\t\xf5\xdb0\xe7\x8b3\n\xd5\xec\xe3\xfco\xc1>}a\x05\xc6\x8dY\xf3\xe4\xffb0V|\xfeU\xf5"


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
