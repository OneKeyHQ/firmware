from ubinascii import hexlify

from trezor.crypto.hashlib import sha3_256

TRANSACTION_PREFIX = b"APTOS::RawTransaction"


def aptos_address_from_pubkey(pub_key_bytes: bytes) -> str:
    # append single-signature scheme identifier
    payloads = pub_key_bytes + b"\x00"
    address = f"0x{hexlify(sha3_256(payloads).digest()).decode()}"
    return address
