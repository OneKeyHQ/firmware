from typing import TYPE_CHECKING

from trezor.lvglui.scrs import lv
from trezor.messages import TezosGetPublicKey, TezosPublicKey
from trezor.ui.layouts import show_pubkey

from apps.common import paths, seed
from apps.common.keychain import with_slip44_keychain

from . import CURVE, ICON, PATTERNS, PRIMARY_COLOR, SLIP44_ID, helpers

if TYPE_CHECKING:
    from apps.common.keychain import Keychain
    from trezor.wire import Context


@with_slip44_keychain(*PATTERNS, slip44_id=SLIP44_ID, curve=CURVE)
async def get_public_key(
    ctx: Context, msg: TezosGetPublicKey, keychain: Keychain
) -> TezosPublicKey:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pk = seed.remove_ed25519_prefix(node.public_key())
    pk_prefixed = helpers.base58_encode_check(pk, prefix=helpers.TEZOS_PUBLICKEY_PREFIX)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_pubkey(
            ctx,
            pk_prefixed,
            path=path,
            network="XTZ",
        )

    return TezosPublicKey(public_key=pk_prefixed)
