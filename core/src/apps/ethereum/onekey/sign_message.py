from typing import TYPE_CHECKING

from trezor.crypto.curve import secp256k1
from trezor.messages import EthereumMessageSignatureOneKey as EthereumMessageSignature
from trezor.ui.layouts import confirm_signverify

from apps.common import paths
from apps.common.helpers import validate_message
from apps.common.signverify import decode_message

from .. import networks
from ..helpers import address_from_bytes, get_color_and_icon, get_display_network_name
from ..sign_message import message_digest
from .keychain import PATTERNS_ADDRESS, with_keychain_from_path

if TYPE_CHECKING:
    from trezor.messages import EthereumSignMessageOneKey as EthereumSignMessage
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@with_keychain_from_path(*PATTERNS_ADDRESS)
async def sign_message(
    ctx: Context, msg: EthereumSignMessage, keychain: Keychain
) -> EthereumMessageSignature:
    validate_message(msg.message)
    await paths.validate_path(ctx, keychain, msg.address_n, force_strict=False)

    node = keychain.derive(msg.address_n, force_strict=False)
    address = address_from_bytes(node.ethereum_pubkeyhash())

    if msg.chain_id:
        network = networks.by_chain_id(msg.chain_id)
    else:
        if len(msg.address_n) > 1:  # path has slip44 network identifier
            network = networks.by_slip44(msg.address_n[1] & 0x7FFF_FFFF)
        else:
            network = None

    ctx.primary_color, ctx.icon_path = get_color_and_icon(
        network.chain_id if network else None
    )
    await confirm_signverify(
        ctx,
        get_display_network_name(network),
        decode_message(msg.message),
        address,
        verify=False,
        evm_chain_id=None if network is not networks.UNKNOWN_NETWORK else msg.chain_id,
    )

    signature = secp256k1.sign(
        node.private_key(),
        message_digest(msg.message),
        False,
        secp256k1.CANONICAL_SIG_ETHEREUM,
    )

    return EthereumMessageSignature(
        address=address,
        signature=signature[1:] + bytearray([signature[0]]),
    )
