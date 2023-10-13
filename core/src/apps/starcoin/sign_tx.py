from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha3_256
from trezor.lvglui.scrs import lv
from trezor.messages import StarcoinSignedTx, StarcoinSignTx

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .helper import TRANSACTION_PREFIX, get_address_from_public_key


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: StarcoinSignTx, keychain: Keychain
) -> StarcoinSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = node.public_key()[1:]
    address = get_address_from_public_key(pubkey)

    from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    await confirm_blind_sign_common(ctx, address, msg.raw_tx)
    await confirm_final(ctx, "STC")
    prefix_bytes = sha3_256(TRANSACTION_PREFIX).digest()
    raw_tx = prefix_bytes + msg.raw_tx
    signature = ed25519.sign(node.private_key(), raw_tx)

    return StarcoinSignedTx(public_key=pubkey, signature=signature)
