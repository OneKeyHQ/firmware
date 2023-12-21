from ubinascii import b2a_base64, unhexlify

from trezor import wire
from trezor.crypto import aes, bech32, random
from trezor.crypto.curve import secp256k1
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import NostrEncryptedMessage, NostrEncryptMessage
from trezor.ui.layouts import confirm_nostrmessage

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR


@auto_keychain(__name__)
async def encrypt(
    ctx: wire.Context, msg: NostrEncryptMessage, keychain: Keychain
) -> NostrEncryptedMessage:
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
            False,
            title=_(i18n_keys.TITLE__ENCRYPT_STR_MESSAGE).format("Nostr"),
        )

    iv = random.bytes(16)
    raw_msg = msg.msg.encode()

    if len(raw_msg) % 16:
        pad_size = 16 - len(raw_msg) % 16
    else:
        pad_size = 0
    pad = bytearray(pad_size.to_bytes(1, "big") * pad_size)
    raw_msg += bytes(pad)

    # pyright: off
    ctx = aes(aes.CBC, normalizedKey, iv)
    encrypted = ctx.encrypt(raw_msg)
    # pyright: on

    ctb64 = b2a_base64(encrypted)[:-1].decode("utf-8")
    ivb64 = b2a_base64(iv)[:-1].decode("utf-8")
    b64 = f"{ctb64}?iv={ivb64}"
    return NostrEncryptedMessage(msg=b64)
