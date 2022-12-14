from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha3_256
from trezor.lvglui.scrs import lv
from trezor.messages import StarcoinMessageSignature, StarcoinSignMessage
from trezor.ui.layouts import confirm_signverify

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain
from apps.common.signverify import decode_message

from . import ICON, PRIMARY_COLOR
from .helper import (
    MESSAGE_PREFIX,
    get_address_from_public_key,
    serialize_u32_as_uleb128,
)


@auto_keychain(__name__)
async def sign_message(
    ctx: wire.Context, msg: StarcoinSignMessage, keychain: Keychain
) -> StarcoinMessageSignature:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = node.public_key()[1:]
    address = get_address_from_public_key(pubkey)
    if msg.message is None:
        raise wire.DataError("Message can't be None")
    data = decode_message(msg.message)
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    await confirm_signverify(
        ctx,
        "STC",
        data,
        address,
        verify=False,
    )
    prefix_bytes = sha3_256(MESSAGE_PREFIX).digest()
    msg_len = len(msg.message)
    data = prefix_bytes + serialize_u32_as_uleb128(msg_len) + msg.message
    signature = ed25519.sign(node.private_key(), data)

    return StarcoinMessageSignature(public_key=pubkey, signature=signature)
