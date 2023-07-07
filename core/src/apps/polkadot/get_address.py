from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor import utils
from trezor.crypto.curve import ed25519
from trezor.messages import PolkadotAddress, PolkadotGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths

from . import helper, seed

if TYPE_CHECKING:
    from trezor.wire import Context


@seed.with_keychain
async def get_address(
    ctx: Context, msg: PolkadotGetAddress, keychain: seed.Keychain
) -> PolkadotAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    if utils.USE_THD89:
        public_key = node.public_key()[1:]
    else:
        public_key = ed25519.publickey(node.private_key())

    address = helper.ss58_encode(public_key, msg.prefix)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        chain_name, _, _ = helper.update_chain_res(ctx, msg.network)
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network=chain_name,
        )

    return PolkadotAddress(
        address=address, public_key=f"0x{hexlify(public_key).decode()}"
    )
