from typing import TYPE_CHECKING

from trezor.lvglui.scrs import lv
from trezor.messages import StellarAddress, StellarGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths, seed
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR, helpers

if TYPE_CHECKING:
    from trezor.wire import Context
    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: StellarGetAddress, keychain: Keychain
) -> StellarAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = seed.remove_ed25519_prefix(node.public_key())
    address = helpers.address_from_public_key(pubkey)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="XLM",
        )

    return StellarAddress(address=address)
