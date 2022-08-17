from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha3_256
from trezor.messages import StarcoinVerifyMessage, Success
from trezor.ui.layouts import show_success

from apps.common.keychain import Keychain, auto_keychain

from .helper import serialize_u32_as_uleb128


@auto_keychain(__name__)
async def verify_message(
    ctx: wire.Context, msg: StarcoinVerifyMessage, keychain: Keychain
) -> Success:

    prefix = sha3_256(b"STARCOIN::SigningMessage", keccak=False).digest()
    msg_data = msg.message if msg.message is not None else b""
    msg_len = len(msg_data)
    data = prefix + serialize_u32_as_uleb128(msg_len) + msg_data

    if (
        ed25519.verify(
            msg.public_key if msg.public_key is not None else b"",
            msg.signature if msg.signature is not None else b"",
            data,
        )
        is False
    ):
        raise wire.DataError("Invalid signature")

    await show_success(ctx, "verify_message", "The signature is valid.")
    return Success(message="Message verified")
