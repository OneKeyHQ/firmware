from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor.lvglui.scrs import lv
from trezor.messages import BinanceGetPublicKey, BinancePublicKey

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def get_public_key(
    ctx: Context, msg: BinanceGetPublicKey, keychain: Keychain
) -> BinancePublicKey:
    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    pubkey = node.public_key()

    if msg.show_display:
        from trezor.ui.layouts import show_pubkey

        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        path = paths.address_n_to_str(msg.address_n)
        await show_pubkey(
            ctx,
            hexlify(pubkey).decode(),
            path=path,
            network="BNB",
        )

    return BinancePublicKey(public_key=pubkey)
