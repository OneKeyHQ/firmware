from typing import TYPE_CHECKING

from apps.common import paths

from . import networks

if TYPE_CHECKING:
    from trezor.messages import EthereumGetPublicKey, EthereumPublicKey
    from trezor.wire import Context


async def get_public_key(ctx: Context, msg: EthereumGetPublicKey) -> EthereumPublicKey:
    from ubinascii import hexlify
    from trezor.messages import EthereumPublicKey, GetPublicKey
    from trezor.ui.layouts import show_pubkey
    from apps.bitcoin import get_public_key as bitcoin_get_public_key

    # we use the Bitcoin format for Ethereum xpubs
    btc_pubkey_msg = GetPublicKey(address_n=msg.address_n)
    resp = await bitcoin_get_public_key.get_public_key(ctx, btc_pubkey_msg)

    if msg.show_display:
        from .helpers import get_color_and_icon, get_display_network_name

        if len(msg.address_n) > 1:  # path has slip44 network identifier
            network = networks.by_slip44(msg.address_n[1] & 0x7FFF_FFFF)
        else:
            network = None
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = get_color_and_icon(
            network.chain_id if network else None
        )
        await show_pubkey(
            ctx,
            hexlify(resp.node.public_key).decode(),
            path=path,
            network=get_display_network_name(network),
        )

    return EthereumPublicKey(node=resp.node, xpub=resp.xpub)
