from typing import TYPE_CHECKING

from trezor.crypto.curve import secp256k1
from trezor.messages import EthereumSignTypedHash, EthereumTypedDataSignature

from apps.common import paths
from apps.common.signverify import decode_message

from .helpers import address_from_bytes, get_color_and_icon, get_display_network_name
from .keychain import PATTERNS_ADDRESS, with_keychain_from_path
from .layout import confirm_typed_hash, confirm_typed_hash_final
from .sign_typed_data import keccak256

if TYPE_CHECKING:
    from apps.common.keychain import Keychain
    from trezor.wire import Context
    from .definitions import Definitions


@with_keychain_from_path(*PATTERNS_ADDRESS)
async def sign_typed_data_hash(
    ctx: Context, msg: EthereumSignTypedHash, keychain: Keychain, defs: Definitions
) -> EthereumTypedDataSignature:
    await paths.validate_path(ctx, keychain, msg.address_n, force_strict=False)

    network = defs.network
    ctx.primary_color, ctx.icon_path = get_color_and_icon(
        network.chain_id if network else None
    )
    ctx.name = get_display_network_name(network)
    domain_hash = msg.domain_separator_hash
    message_hash = msg.message_hash or b""
    await confirm_typed_hash(
        ctx, decode_message(domain_hash), decode_message(message_hash)
    )
    data_hash = keccak256(b"\x19\x01" + domain_hash + message_hash)

    await confirm_typed_hash_final(ctx)

    node = keychain.derive(msg.address_n, force_strict=False)
    signature = secp256k1.sign(
        node.private_key(), data_hash, False, secp256k1.CANONICAL_SIG_ETHEREUM
    )

    return EthereumTypedDataSignature(
        address=address_from_bytes(node.ethereum_pubkeyhash()),
        signature=signature[1:] + signature[0:1],
    )
