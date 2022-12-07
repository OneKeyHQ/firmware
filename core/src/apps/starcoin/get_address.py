from typing import TYPE_CHECKING

from trezor.lvglui.scrs import lv
from trezor.messages import StarcoinAddress, StarcoinGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .helper import get_address_from_public_key

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: StarcoinGetAddress, keychain: Keychain
) -> StarcoinAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    public_key = node.public_key()[1:]
    address = get_address_from_public_key(public_key)
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="STC",
        )

    return StarcoinAddress(address=address)
