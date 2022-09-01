from typing import TYPE_CHECKING

from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.messages import ConfluxMessageSignature
from trezor.ui.layouts import confirm_signverify
from trezor.utils import HashWriter

from apps.common import paths
from apps.common.helpers import validate_message
from apps.common.keychain import Keychain, auto_keychain
from apps.common.signverify import decode_message

from .helpers import address_from_bytes, address_from_hex

if TYPE_CHECKING:
    from trezor.messages import ConfluxSignMessage
    from trezor.wire import Context


def message_digest(message: bytes) -> bytes:
    h = HashWriter(sha3_256(keccak=True))
    signed_message_header = b"\x19Conflux Signed Message:\n"
    h.extend(signed_message_header)
    h.extend(str(len(message)).encode())
    h.extend(message)
    return h.get_digest()


@auto_keychain(__name__)
async def sign_message(
    ctx: Context, msg: ConfluxSignMessage, keychain: Keychain
) -> ConfluxMessageSignature:
    message = msg.message if msg.message is not None else b""
    validate_message(message)
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    address = address_from_bytes(node.ethereum_pubkeyhash())
    cfx_address = address_from_hex(address, 1029)
    await confirm_signverify(
        ctx, "CFX", decode_message(message), cfx_address, verify=False
    )

    digest = message_digest(message)
    signature = secp256k1.sign(
        node.private_key(),
        digest,
        False,
        secp256k1.CANONICAL_SIG_ETHEREUM,
    )

    return ConfluxMessageSignature(
        address=address,
        signature=signature[1:] + bytearray([signature[0] - 27]),
    )
