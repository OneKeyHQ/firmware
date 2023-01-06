from typing import TYPE_CHECKING

from trezor.crypto.curve import secp256k1
from trezor.lvglui.scrs import lv
from trezor.messages import FilecoinAddress, FilecoinGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR, helper

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: FilecoinGetAddress, keychain: Keychain
) -> FilecoinAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    public_key = secp256k1.publickey(node.private_key(), False)
    address = helper.pubkey_to_address(public_key, "t" if msg.testnet else "f")
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="Filecoin",
        )

    return FilecoinAddress(address=address)
