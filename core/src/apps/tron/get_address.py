from typing import TYPE_CHECKING

from trezor import utils
from trezor.crypto.curve import secp256k1
from trezor.lvglui.scrs import lv
from trezor.messages import TronAddress, TronGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain
from apps.tron.address import get_address_from_public_key

from . import ICON, PRIMARY_COLOR

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: TronGetAddress, keychain: Keychain
) -> TronAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    if utils.USE_THD89:
        from trezor.crypto import se_thd89

        public_key = se_thd89.uncompress_pubkey("secp256k1", node.public_key())
    else:
        seckey = node.private_key()
        public_key = secp256k1.publickey(seckey, False)
    address = get_address_from_public_key(public_key[:65])

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="TRON",
        )

    return TronAddress(address=address)
