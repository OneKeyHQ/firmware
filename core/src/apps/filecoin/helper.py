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


def format_protocol(addr: bytes, network_prefix: str):
    buf = addr
    check_payload = buf[1:] + blake2b(data=buf, outlen=4).digest()
    b = base32.encode(check_payload)
    addr_str = network_prefix + "1" + b
    return addr_str.lower().rstrip("=")
