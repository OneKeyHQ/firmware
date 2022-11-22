from ubinascii import hexlify

from trezor.crypto.hashlib import sha3_256

TRANSACTION_PREFIX = b"TransactionData::"


def sui_address_from_pubkey(pub_key_bytes: bytes) -> str:
    # set signature scheme identifier
    payloads = b"\x00" + pub_key_bytes
    h = sha3_256(payloads).digest()[0:20]
    return f"0x{hexlify(h).decode()}"
