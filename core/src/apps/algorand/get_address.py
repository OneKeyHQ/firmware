from typing import TYPE_CHECKING

from trezor.messages import AlgorandAddress, AlgorandGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import encoding

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: AlgorandGetAddress, keychain: Keychain
) -> AlgorandAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    public_key = node.public_key()[1:]
    address = encoding.encode_address(public_key)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="ALGO",
        )

    return AlgorandAddress(address=address)
