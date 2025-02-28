from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.lvglui.scrs import lv
from trezor.messages import SolanaMessageSignature
from trezor.ui.layouts.lvgl import confirm_sol_message

from apps.common import paths, seed
from apps.common.helpers import validate_message
from apps.common.keychain import auto_keychain
from apps.common.signverify import decode_message

from . import ICON, PRIMARY_COLOR
from .message import Message
from .publickey import PublicKey

if TYPE_CHECKING:
    from trezor.messages import SolanaSignUnsafeMessage
    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def sign_unsafe_message(
    ctx: wire.Context, msg: SolanaSignUnsafeMessage, keychain: Keychain
) -> SolanaMessageSignature:
    # sanitize message
    message = msg.message
    sanitize_message(message)

    # path validation
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    signer_pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    decoded_message = decode_message(message)
    address = str(PublicKey(signer_pub_key_bytes))

    # display the decoded message to confirm
    await confirm_sol_message(ctx, address, None, decoded_message, is_unsafe=True)

    signature = ed25519.sign(node.private_key(), message)
    return SolanaMessageSignature(signature=signature)


def sanitize_message(message: bytes) -> None:
    validate_message(message)
    try:
        _ = Message.deserialize(message)
        raise wire.DataError("Valid transaction message format is not allowed")
    except BaseException as e:
        if __debug__:
            import sys

            sys.print_exception(e)  # type: ignore["print_exception" is not a known member of module]
        return
