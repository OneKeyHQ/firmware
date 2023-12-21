from ubinascii import a2b_base64, unhexlify

from trezor import wire
from trezor.crypto import aes, bech32
from trezor.crypto.curve import secp256k1
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import NostrDecryptedMessage, NostrDecryptMessage
from trezor.ui.layouts import confirm_nostrmessage

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR


@auto_keychain(__name__)
async def decrypt(
    ctx: wire.Context, msg: NostrDecryptMessage, keychain: Keychain
) -> NostrDecryptedMessage:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = unhexlify(f"02{msg.pubkey}")
    key = secp256k1.multiply(node.private_key(), pubkey)
    normalizedKey = key[1:33]

    convertedbits = bech32.convertbits(node.public_key()[1:33], 8, 5)
    npub = bech32.bech32_encode("npub", convertedbits, bech32.Encoding.BECH32)
    if msg.show_display:
        ctx.primary_color, ctx.icon_path = (
            lv.color_hex(PRIMARY_COLOR),
            ICON,
        )
        await confirm_nostrmessage(
            ctx,
            npub,
            msg.msg,
            True,
            title=_(i18n_keys.TITLE__DECRYPT_STR_MESSAGE).format("Nostr"),
        )

    m = msg.msg.split("?iv=")
    ct = a2b_base64(m[0].encode())
    iv = a2b_base64(m[1].encode())

    # pyright: off
    ctx = aes(aes.CBC, normalizedKey, iv)
    decrypted = ctx.decrypt(ct).rstrip(b"\0")
    pad_size = decrypted[len(decrypted) - 1]
    if pad_size <= 0xF:
        decrypted = decrypted
        decryptedtexto = decrypted[0 : len(decrypted) - pad_size].decode("utf-8")
    else:
        decryptedtexto = decrypted.decode("utf-8")
    return NostrDecryptedMessage(msg=decryptedtexto)
    # pyright: on
