from typing import TYPE_CHECKING

from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.messages import ConfluxMessageSignature
from trezor.ui.layouts import confirm_signverify
from trezor.utils import HashWriter

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain
from apps.common.signverify import decode_message

from .helpers import address_from_bytes, address_from_hex

if TYPE_CHECKING:
    from trezor.messages import ConfluxSignMessage
    from trezor.wire import Context

    from apps.common.keychain import Keychain


def message_digest(domain_hash: bytes, message_hash: bytes) -> bytes:
    h = HashWriter(sha3_256(keccak=True))
    h.extend(b"\x19\x01")
    h.extend(domain_hash)
    h.extend(message_hash)
    return h.get_digest()


@auto_keychain(__name__)
async def sign_message_cip23(
    ctx: Context, msg: ConfluxSignMessage, keychain: Keychain
) -> ConfluxMessageSignature:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)

    address = address_from_bytes(node.ethereum_pubkeyhash())
    await confirm_signverify(
        ctx,
        "CFX",
        decode_message(msg.domain_hash + msg.message_hash),
        address,
        verify=False,
    )

    address = address_from_bytes(node.ethereum_pubkeyhash(), None)
    cfx_address = address_from_hex(address, 1029)

    digest = message_digest(msg.domain_hash, msg.message_hash)
    signature = secp256k1.sign(
        node.private_key(),
        digest,
        False,
        secp256k1.CANONICAL_SIG_ETHEREUM,
    )

    return ConfluxMessageSignature(
        address=cfx_address,
        signature=signature[1:] + bytearray([signature[0] - 27]),
    )
