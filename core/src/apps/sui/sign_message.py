from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import blake2b
from trezor.lvglui.scrs import lv
from trezor.messages import SuiMessageSignature, SuiSignMessage

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain
from apps.common.signverify import decode_message

from . import ICON, PRIMARY_COLOR
from .helper import PERSONALMESSAGE_INTENT_BYTES, sui_address_from_pubkey, uleb_encode


@auto_keychain(__name__)
async def sign_message(
    ctx: wire.Context, msg: SuiSignMessage, keychain: Keychain
) -> SuiMessageSignature:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = sui_address_from_pubkey(pub_key_bytes)

    len_bytes = uleb_encode(len(msg.message))
    intentMessage = PERSONALMESSAGE_INTENT_BYTES + len_bytes + msg.message

    from trezor.ui.layouts import confirm_signverify

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    await confirm_signverify(ctx, "Sui", decode_message(msg.message), address, False)

    signature = ed25519.sign(
        node.private_key(), blake2b(data=intentMessage, outlen=32).digest()
    )
    return SuiMessageSignature(signature=signature, address=address)
