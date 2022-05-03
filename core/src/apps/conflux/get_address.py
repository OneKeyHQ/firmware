from typing import TYPE_CHECKING

from trezor.messages import ConfluxAddress, ConfluxGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths

from .helpers import address_from_bytes, address_from_hex
from apps.common.keychain import Keychain, auto_keychain

if TYPE_CHECKING:
    from trezor.wire import Context

@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: ConfluxGetAddress, keychain: Keychain
) -> ConfluxAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    address = address_from_bytes(node.ethereum_pubkeyhash(), None)
    cfx_address = address_from_hex(address,msg.chain_id)
    
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=cfx_address,
            address_n=path,
            network="CFX",
        )

    return ConfluxAddress(address=cfx_address)
