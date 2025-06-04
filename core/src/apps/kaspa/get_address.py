from typing import TYPE_CHECKING

from trezor import wire
from trezor.lvglui.scrs import lv
from trezor.messages import KaspaAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR
from .addresses import Prefix, encode_address
from .common import ALLOWED_SCHEMES

if TYPE_CHECKING:
    from trezor.messages import KaspaGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: KaspaGetAddress, keychain: Keychain
) -> KaspaAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    if not Prefix.is_valid(msg.prefix):
        raise wire.DataError("Invalid prefix provided.")
    if msg.scheme not in ALLOWED_SCHEMES:
        raise wire.DataError("Invalid address scheme provided.")
    node = keychain.derive(msg.address_n)
    address = encode_address(
        node,
        prefix=msg.prefix,
        scheme=msg.scheme,
        use_tweak=getattr(msg, "use_tweak", True),
    )

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="KAS",
        )

    return KaspaAddress(address=address)
