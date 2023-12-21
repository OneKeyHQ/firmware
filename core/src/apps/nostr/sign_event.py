from ubinascii import hexlify

from trezor import wire
from trezor.crypto import bech32
from trezor.crypto.hashlib import sha256
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import NostrSignedEvent, NostrSignEvent
from trezor.ui.layouts import confirm_signverify

import ujson as json
from apps.bitcoin.common import bip340_sign_internal
from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR


@auto_keychain(__name__)
async def sign_event(
    ctx: wire.Context, msg: NostrSignEvent, keychain: Keychain
) -> NostrSignedEvent:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = hexlify(node.public_key()[1:33])

    convertedbits = bech32.convertbits(node.public_key()[1:33], 8, 5)
    npub = bech32.bech32_encode("npub", convertedbits, bech32.Encoding.BECH32)
    ctx.primary_color, ctx.icon_path = (
        lv.color_hex(PRIMARY_COLOR),
        ICON,
    )
    await confirm_signverify(
        ctx,
        "Nostr",
        msg.event.decode("utf-8"),
        npub,
        verify=False,
        title=_(i18n_keys.TITLE__STR_MESSAGE).format("Nostr"),
    )

    j = json.loads(msg.event.decode("utf-8"))
    if (
        "kind" not in j
        or "content" not in j
        or "tags" not in j
        or "created_at" not in j
    ):
        raise wire.DataError("Invalid event")

    obj = [0, pubkey, j["created_at"], j["kind"], j["tags"], j["content"]]
    # pyright: off
    unsignedEvent_bytes = json.dumps(obj, separators=(",", ":")).encode("utf-8")
    # pyright: on

    hash = sha256(unsignedEvent_bytes).digest()
    node = keychain.derive(msg.address_n)
    signature = bip340_sign_internal(node, hash)

    result = {}
    result["pubkey"] = pubkey
    result["id"] = hexlify(hash).decode()
    result["sig"] = hexlify(signature).decode()
    # pyright: off
    return NostrSignedEvent(event=json.dumps(result, separators=(",", ":")))
    # pyright: on
