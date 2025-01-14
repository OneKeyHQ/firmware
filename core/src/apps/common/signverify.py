from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor import utils, wire
from trezor.crypto.hashlib import blake256, sha256

from apps.common.writers import write_compact_size

if TYPE_CHECKING:
    from apps.common.coininfo import CoinInfo


def message_digest(coin: CoinInfo, message: bytes) -> bytes:
    if not utils.BITCOIN_ONLY and coin.decred:
        h = utils.HashWriter(blake256())
    else:
        h = utils.HashWriter(sha256())
    if not coin.signed_message_header:
        raise wire.DataError("Empty message header not allowed.")
    write_compact_size(h, len(coin.signed_message_header))
    h.extend(coin.signed_message_header.encode())
    write_compact_size(h, len(message))
    h.extend(message)
    ret = h.get_digest()
    if coin.sign_hash_double:
        ret = sha256(ret).digest()
    return ret


def is_non_printable(message: str) -> bool:
    return any(ord(c) < 32 or ord(c) == 127 for c in message)


def decode_message(message: bytes) -> str:
    try:
        decoded_message = bytes(message).decode()
        if is_non_printable(decoded_message):
            return f"0x{hexlify(message).decode()}"
        return decoded_message
    except UnicodeError:
        return f"0x{hexlify(message).decode()}"
