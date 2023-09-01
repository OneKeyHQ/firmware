from typing import TYPE_CHECKING

from trezor import wire
from trezor.lvglui.scrs import lv
from trezor.messages import NexaAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR
from .addresses import Prefix, encode_address

if TYPE_CHECKING:
    from trezor.messages import NexaGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: NexaGetAddress, keychain: Keychain
) -> NexaAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    if not Prefix.is_valid(msg.prefix):
        raise wire.DataError("Invalid prefix provided.")
    node = keychain.derive(msg.address_n)
    address = encode_address(node, prefix=msg.prefix)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="NEXA",
        )

    return NexaAddress(address=address, public_key=node.public_key())
