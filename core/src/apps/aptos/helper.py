from ubinascii import hexlify

from trezor.crypto.hashlib import sha3_256

TRANSACTION_PREFIX = b"APTOS::RawTransaction"
TRANSACTION_WITH_DATA_PREFIX = b"APTOS::RawTransactionWithData"


def aptos_address_from_pubkey(pub_key_bytes: bytes) -> str:
    # append single-signature scheme identifier
    payloads = pub_key_bytes + b"\x00"
    address = f"0x{hexlify(sha3_256(payloads).digest()).decode()}"
    return address


def aptos_transaction_prefix(tx_type: int) -> bytes:
    if tx_type == 0:
        return TRANSACTION_PREFIX
    elif tx_type == 1:
        return TRANSACTION_WITH_DATA_PREFIX
    else:
        raise ValueError("Invalid transaction type")
