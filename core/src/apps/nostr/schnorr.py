from ubinascii import unhexlify

from trezor import wire
from trezor.crypto import bech32
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import NostrSignedSchnorr, NostrSignSchnorr
from trezor.ui.layouts import confirm_signverify

from apps.bitcoin.common import bip340_sign_internal
from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR


@auto_keychain(__name__)
async def schnorr(
    ctx: wire.Context, msg: NostrSignSchnorr, keychain: Keychain
) -> NostrSignedSchnorr:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    convertedbits = bech32.convertbits(node.public_key()[1:33], 8, 5)
    npub = bech32.bech32_encode("npub", convertedbits, bech32.Encoding.BECH32)
    ctx.primary_color, ctx.icon_path = (
        lv.color_hex(PRIMARY_COLOR),
        ICON,
    )
    await confirm_signverify(
        ctx,
        "Nostr",
        msg.hash,
        npub,
        verify=False,
        title=_(i18n_keys.TITLE__STR_MESSAGE).format("Nostr"),
    )

    hash = unhexlify(msg.hash)
    node = keychain.derive(msg.address_n)
    signature = bip340_sign_internal(node, hash)

    return NostrSignedSchnorr(signature=signature)
