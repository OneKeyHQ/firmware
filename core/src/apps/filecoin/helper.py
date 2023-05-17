from trezor import wire
from trezor.crypto import base32
from trezor.crypto.hashlib import blake2b


class Protocol:
    SECP256K1 = 1  # SECP256K1


def pubkey_to_address(pubkey: bytes, network_prefix: str):
    buf = bytes([1]) + blake2b(data=pubkey, outlen=20).digest()
    check_payload = buf[1:] + blake2b(data=buf, outlen=4).digest()
    b = base32.encode(check_payload)
    addr_str = network_prefix + "1" + b
    return addr_str.lower().rstrip("=")


def decompressLEB128(b: bytearray) -> int:
    """Decode the unsigned leb128 encoded bytearray"""
    r = 0
    for i, e in enumerate(b):
        r = r + ((e & 0x7F) << (i * 7))
        if (e & 0x80) == 0:
            return r
    return r


def format_protocol(addr: bytes, network_prefix: str):
    protocol = addr[0]
    buf = addr
    if protocol == 1:  # ADDRESS_PROTOCOL_SECP256K1
        check_payload = buf[1:] + blake2b(data=buf, outlen=4).digest()
        b = base32.encode(check_payload)
        addr_str = network_prefix + "1" + b
    elif protocol == 4:  # ADDRESS_PROTOCOL_DELEGATED
        check_payload = buf[2:] + blake2b(data=buf, outlen=4).digest()
        b = base32.encode(check_payload)
        id = decompressLEB128(bytearray(addr[1:]))
        id_str = f"{id}f"
        addr_str = network_prefix + "4" + id_str + b
    else:
        return wire.DataError("unexpected protocol")

    return addr_str.lower().rstrip("=")
