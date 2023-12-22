from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor.crypto import bech32
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import NostrPublicKey
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR

if TYPE_CHECKING:
    from trezor.messages import NostrGetPublicKey
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def get_public_key(
    ctx: Context, msg: NostrGetPublicKey, keychain: Keychain
) -> NostrPublicKey:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = hexlify(node.public_key()[1:33]).decode()
    convertedbits = bech32.convertbits(node.public_key()[1:33], 8, 5)
    npub = bech32.bech32_encode("npub", convertedbits, bech32.Encoding.BECH32)
    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=npub,
            address_n=path,
            network="Nostr",
            title=_(i18n_keys.TITLE__STR_PUBLIC_KEY).format("Nostr"),
        )

    return NostrPublicKey(publickey=pubkey, npub=npub)
