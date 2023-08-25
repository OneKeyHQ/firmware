from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.messages import Success
from trezor.ui.layouts import confirm_signverify, show_success

from apps.common.signverify import decode_message

from .. import networks
from ..helpers import (
    address_from_bytes,
    bytes_from_address,
    get_color_and_icon,
    get_display_network_name,
)
from .sign_message import message_digest

if TYPE_CHECKING:
    from trezor.messages import EthereumVerifyMessageOneKey as EthereumVerifyMessage
    from trezor.wire import Context


async def verify_message(ctx: Context, msg: EthereumVerifyMessage) -> Success:
    digest = message_digest(msg.message)
    if len(msg.signature) != 65:
        raise wire.DataError("Invalid signature")
    sig = bytearray([msg.signature[64]]) + msg.signature[:64]

    pubkey = secp256k1.verify_recover(sig, digest)

    if not pubkey:
        raise wire.DataError("Invalid signature")

    pkh = sha3_256(pubkey[1:], keccak=True).digest()[-20:]

    address_bytes = bytes_from_address(msg.address)
    if address_bytes != pkh:
        raise wire.DataError("Invalid signature")

    address = address_from_bytes(address_bytes)

    if msg.chain_id:
        network = networks.by_chain_id(msg.chain_id)
    else:
        network = networks.UNKNOWN_NETWORK
    ctx.primary_color, ctx.icon_path = get_color_and_icon(network.chain_id)
    await confirm_signverify(
        ctx,
        get_display_network_name(network),
        decode_message(msg.message),
        address=address,
        verify=True,
        evm_chain_id=None if network is not networks.UNKNOWN_NETWORK else msg.chain_id,
    )
    from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

    await show_success(
        ctx,
        "verify_message",
        header=_(i18n_keys.TITLE__VERIFIED),
        content=_(i18n_keys.SUBTITLE__THE_SIGNATURE_IS_VALID),
        button=_(i18n_keys.BUTTON__DONE),
    )
    return Success(message="Message verified")
