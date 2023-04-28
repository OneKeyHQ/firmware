from ubinascii import hexlify

from trezor.crypto.hashlib import blake2b

# This is currently hardcoded with [IntentScope::TransactionData = 0, Version::V0 = 0, AppId::Sui = 0]
# https://github.com/MystenLabs/sui/pull/6445
INTENT_BYTES = b"\x00\x00\x00"


def sui_address_from_pubkey(pub_key_bytes: bytes) -> str:
    # set signature scheme identifier
    payloads = b"\x00" + pub_key_bytes
    h = blake2b(data=payloads, outlen=32).digest()
    return f"0x{hexlify(h).decode()}"
