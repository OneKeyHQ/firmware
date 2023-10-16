from typing import TYPE_CHECKING

from trezor.messages import EthereumAddress
from trezor.ui.layouts import show_address

from apps.common import paths

from . import networks
from .helpers import address_from_bytes, get_color_and_icon, get_display_network_name
from .keychain import PATTERNS_ADDRESS, with_keychain_from_path

if TYPE_CHECKING:
    from trezor.messages import EthereumGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain
    from .definitions import Definitions


@with_keychain_from_path(*PATTERNS_ADDRESS)
async def get_address(
    ctx: Context, msg: EthereumGetAddress, keychain: Keychain, defs: Definitions
) -> EthereumAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    address = address_from_bytes(node.ethereum_pubkeyhash(), defs.network)
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = get_color_and_icon(
            defs.network.chain_id if defs.network else None
        )
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network=get_display_network_name(defs.network),
            evm_chain_id=defs.chain_id
            if defs.network is networks.UNKNOWN_NETWORK
            else None,
        )

    return EthereumAddress(address=address)
