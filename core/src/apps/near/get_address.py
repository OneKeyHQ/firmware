from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor.messages import NearAddress, NearGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import FORBIDDEN_KEY_PATH, Keychain, auto_keychain

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def get_address(
    ctx: Context, msg: NearGetAddress, keychain: Keychain
) -> NearAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)
    if (
        len(msg.address_n) != 3
        or msg.address_n[0] != 0x8000002C
        or msg.address_n[1] != 0x8000018D
    ):
        raise FORBIDDEN_KEY_PATH

    node = keychain.derive(msg.address_n)
    public_key = node.public_key()[1:]
    address = "0x" + hexlify(public_key).decode()
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="NEAR",
        )

    return NearAddress(address=address)
