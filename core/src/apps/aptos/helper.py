from ubinascii import hexlify

from trezor.crypto.hashlib import sha256

# Prefix_bytes with SHA3_256 hash bytes of string `APTOS::RawTransaction`
TRANSACTION_PREFIX = (
    b"\x81\xe9\n\x02Bx\x80G\xae\xa5=B;\x9b\t\r1\xea/\xcc\xf8\xf7.Q\x9e/\xcd\xa7\x97+&H"
)


def aptos_address_from_pubkey(pub_key_bytes: bytes) -> str:
    # append single-signature scheme identifier
    payloads = pub_key_bytes + b"\x00"
    address = f"0x{hexlify(sha256(payloads).digest()).decode()}"
    return address
