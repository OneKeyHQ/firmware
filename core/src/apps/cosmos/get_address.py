from typing import TYPE_CHECKING

from trezor.crypto import bech32
from trezor.crypto.scripts import sha256_ripemd160
from trezor.lvglui.scrs import lv
from trezor.messages import CosmosAddress, CosmosGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from .networks import retrieve_theme_by_hrp

if TYPE_CHECKING:
    from trezor.wire import Context

DEFAULT_BECH32_HRP = "cosmos"


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: CosmosGetAddress, keychain: Keychain
) -> CosmosAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    public_key = node.public_key()
    hrp = msg.hrp if msg.hrp is not None else DEFAULT_BECH32_HRP
    h = sha256_ripemd160(public_key).digest()
    convertedbits = bech32.convertbits(h, 8, 5)
    assert convertedbits is not None, "Unsuccessful bech32.convertbits call"
    address = bech32.bech32_encode(hrp, convertedbits, bech32.Encoding.BECH32)
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        chain_name, primary_color, ctx.icon_path = retrieve_theme_by_hrp(hrp)
        ctx.primary_color = lv.color_hex(primary_color)
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network=chain_name,
        )

    return CosmosAddress(address=address)
