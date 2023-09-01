from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import schnorr_bch
from trezor.crypto.hashlib import sha256
from trezor.lvglui.scrs import lv
from trezor.messages import NexaSignedTx, NexaTxInputAck, NexaTxInputRequest
from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

from apps.common import paths
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR
from .addresses import Prefix, encode_address

if TYPE_CHECKING:
    from trezor.messages import (
        NexaSignTx,
    )

    from apps.common.keychain import Keychain

    Signable = NexaSignTx | NexaTxInputAck


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context,
    msg: NexaSignTx,
    keychain: Keychain,
) -> NexaSignedTx:

    if not Prefix.is_valid(msg.prefix):
        raise wire.DataError("Invalid prefix provided.")

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    global address_prefix, addresses, request_index, input_count
    address_prefix = msg.prefix
    input_count = msg.input_count
    request_index = 0
    addresses = []
    signature = b""
    while input_count > 0:
        signature = await sign_input(ctx, msg, keychain)
        input_count -= 1
        request_index += 1
        if input_count > 0:
            # pyright: off
            msg = await ctx.call(
                NexaTxInputRequest(request_index=request_index, signature=signature),
                NexaTxInputAck,
            )
            # pyright: on
        else:
            break

    await confirm_final(ctx, "NEXA")
    return NexaSignedTx(signature=signature)


async def sign_input(ctx, msg: Signable, keychain) -> bytes:
    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)

    address = encode_address(node, prefix=address_prefix)
    if msg.address_n not in addresses:
        addresses.append(msg.address_n)
        await confirm_blind_sign_common(ctx, address, msg.raw_message)
    assert msg.raw_message[-4:] == b"\x00\x00\x00\x00", "Invalid sighash type."
    sig_hash = sha256(sha256(msg.raw_message).digest()).digest()
    signature = schnorr_bch.sign(node.private_key(), sig_hash)
    return signature
