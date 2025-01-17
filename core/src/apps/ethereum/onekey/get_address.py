from typing import TYPE_CHECKING

from trezor.messages import EthereumAddressOneKey as EthereumAddress
from trezor.ui.layouts import show_address

from apps.common import paths

from .. import networks
from ..helpers import address_from_bytes, get_color_and_icon, get_display_network_name
from .keychain import PATTERNS_ADDRESS, with_keychain_from_path

if TYPE_CHECKING:
    from trezor.messages import EthereumGetAddressOneKey as EthereumGetAddress
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@with_keychain_from_path(*PATTERNS_ADDRESS)
async def get_address(
    ctx: Context, msg: EthereumGetAddress, keychain: Keychain
) -> EthereumAddress:
    await paths.validate_path(ctx, keychain, msg.address_n, force_strict=False)

    node = keychain.derive(msg.address_n, force_strict=False)

    if msg.chain_id:
        network = networks.by_chain_id(msg.chain_id)
    else:
        if len(msg.address_n) > 1:  # path has slip44 network identifier
            network = networks.by_slip44(msg.address_n[1] & 0x7FFF_FFFF)
        else:
            network = networks.UNKNOWN_NETWORK
    address = address_from_bytes(node.ethereum_pubkeyhash(), network)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = get_color_and_icon(
            network.chain_id if network else None
        )
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network=get_display_network_name(network),
            evm_chain_id=None
            if network is not networks.UNKNOWN_NETWORK
            else msg.chain_id,
        )

    return EthereumAddress(address=address)
