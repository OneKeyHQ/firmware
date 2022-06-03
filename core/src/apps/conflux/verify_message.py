from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.messages import Success
from trezor.ui.layouts import confirm_signverify, show_success

from apps.common.signverify import decode_message

from .helpers import bytes_from_address, decode_hex_address
from .sign_message import message_digest

if TYPE_CHECKING:
    from trezor.messages import ConfluxVerifyMessage
    from trezor.wire import Context


async def verify_message(ctx: Context, msg: ConfluxVerifyMessage) -> Success:
    digest = message_digest(msg.message)
    if len(msg.signature) != 65:
        raise wire.DataError("Invalid signature")
    sig = bytearray([msg.signature[64] + 27]) + msg.signature[:64]

    pubkey = secp256k1.verify_recover(sig, digest)
    hex_addr = decode_hex_address(msg.address)

    if not pubkey:
        raise wire.DataError("Invalid signature")

    pkh = sha3_256(pubkey[1:], keccak=True).digest()[-20:]

    address_bytes = bytes_from_address(hex_addr)
    if address_bytes != pkh:
        raise wire.DataError("Invalid signature")

    await confirm_signverify(
        ctx, "CFX", decode_message(msg.message), address=msg.address, verify=True
    )

    await show_success(ctx, "verify_message", "The signature is valid.")
    return Success(message="Message verified")
