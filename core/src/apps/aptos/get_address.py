from typing import TYPE_CHECKING

from trezor.lvglui.scrs.common import lv
from trezor.messages import AptosAddress
from trezor.ui.layouts import show_address

from apps.common import paths, seed
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR
from .helper import aptos_address_from_pubkey

if TYPE_CHECKING:
    from trezor.messages import AptosGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: AptosGetAddress, keychain: Keychain
) -> AptosAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = aptos_address_from_pubkey(pub_key_bytes)
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="APTOS",
        )

    return AptosAddress(address=address)
