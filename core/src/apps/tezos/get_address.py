from typing import TYPE_CHECKING

from trezor.crypto import hashlib
from trezor.lvglui.scrs import lv
from trezor.messages import TezosAddress, TezosGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths, seed
from apps.common.keychain import with_slip44_keychain

from . import CURVE, ICON, PATTERNS, PRIMARY_COLOR, SLIP44_ID, helpers

if TYPE_CHECKING:
    from apps.common.keychain import Keychain
    from trezor.wire import Context


@with_slip44_keychain(*PATTERNS, slip44_id=SLIP44_ID, curve=CURVE)
async def get_address(
    ctx: Context, msg: TezosGetAddress, keychain: Keychain
) -> TezosAddress:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    pk = seed.remove_ed25519_prefix(node.public_key())
    pkh = hashlib.blake2b(pk, outlen=helpers.PUBLIC_KEY_HASH_SIZE).digest()
    address = helpers.base58_encode_check(
        pkh, prefix=helpers.TEZOS_ED25519_ADDRESS_PREFIX
    )
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="XTZ",
        )

    return TezosAddress(address=address)
