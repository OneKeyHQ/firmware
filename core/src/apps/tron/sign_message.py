from typing import TYPE_CHECKING

from trezor import utils
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.lvglui.scrs import lv
from trezor.messages import TronMessageSignature, TronSignMessage
from trezor.ui.layouts import confirm_signverify
from trezor.utils import HashWriter

from apps.common import paths
from apps.common.helpers import validate_message
from apps.common.keychain import Keychain, auto_keychain
from apps.common.signverify import decode_message
from apps.tron.address import get_address_from_public_key

from . import ICON, PRIMARY_COLOR

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def sign_message(
    ctx: Context, msg: TronSignMessage, keychain: Keychain
) -> TronMessageSignature:
    validate_message(msg.message)
    address_n = msg.address_n or ()
    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(address_n)

    if utils.USE_THD89:
        from trezor.crypto import se_thd89

        public_key = se_thd89.uncompress_pubkey("secp256k1", node.public_key())
    else:
        seckey = node.private_key()
        public_key = secp256k1.publickey(seckey, False)
    address = get_address_from_public_key(public_key[:65])
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    await confirm_signverify(
        ctx, "TRON", decode_message(msg.message), address, verify=False
    )

    # hash the message
    h = HashWriter(sha3_256(keccak=True))
    h.extend(msg.message)
    data_hash = h.get_digest()

    signature = secp256k1.sign(
        node.private_key(),
        message_digest(data_hash),
        False,
        secp256k1.CANONICAL_SIG_ETHEREUM,
    )

    return TronMessageSignature(
        address=bytes(address, "ascii"),
        signature=signature[1:] + bytearray([signature[0]]),
    )


def message_digest(message: bytes) -> bytes:
    h = HashWriter(sha3_256(keccak=True))
    signed_message_header = b"\x19TRON Signed Message:\n"
    h.extend(signed_message_header)
    h.extend(str(len(message)).encode())
    h.extend(message)
    return h.get_digest()
