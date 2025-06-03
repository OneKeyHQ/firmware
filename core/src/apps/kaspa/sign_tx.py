from typing import TYPE_CHECKING

from trezor import wire
from trezor.lvglui.scrs import lv
from trezor.messages import KaspaSignedTx, KaspaTxInputAck, KaspaTxInputRequest
from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

from apps.bitcoin.common import bip340_sign, bip340_sign_internal, ecdsa_sign
from apps.common import paths
from apps.common.keychain import auto_keychain

from . import ICON, PRIMARY_COLOR
from .addresses import Prefix, encode_address
from .common import ALLOWED_SCHEMES, hashwriter, hashwriter_ecdsa

if TYPE_CHECKING:
    from trezor.messages import (
        KaspaSignTx,
    )

    from apps.common.keychain import Keychain

    Signable = KaspaSignTx | KaspaTxInputAck


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context,
    msg: KaspaSignTx,
    keychain: Keychain,
) -> KaspaSignedTx:

    if msg.scheme not in ALLOWED_SCHEMES:
        raise wire.DataError("Invalid signature scheme provided.")

    if not Prefix.is_valid(msg.prefix):
        raise wire.DataError("Invalid prefix provided.")

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    global address_prefix, scheme, addresses, request_index, input_count
    address_prefix = msg.prefix
    scheme = msg.scheme
    input_count = msg.input_count
    request_index = 0
    addresses = []
    signature = b""
    use_tweak = getattr(msg, "use_tweak", True)
    while input_count > 0:
        signature = await sign_input(ctx, msg, keychain, use_tweak=use_tweak)
        input_count -= 1
        request_index += 1
        if input_count > 0:
            # pyright: off
            msg = await ctx.call(
                KaspaTxInputRequest(request_index=request_index, signature=signature),
                KaspaTxInputAck,
            )
            # pyright: on
        else:
            break

    await confirm_final(ctx, "KAS")
    return KaspaSignedTx(signature=signature)


async def sign_input(ctx, msg: Signable, keychain, use_tweak: bool = True) -> bytes:
    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    hasher = hashwriter()
    hasher.write(msg.raw_message)
    sig_hash = hasher.get_digest()

    if scheme == "ecdsa":
        hasher_ecdsa = hashwriter_ecdsa()
        hasher_ecdsa.write(sig_hash)
        sig_hash = hasher_ecdsa.get_digest()

    address = encode_address(
        node, prefix=address_prefix, scheme=scheme, use_tweak=use_tweak
    )
    if msg.address_n not in addresses:
        addresses.append(msg.address_n)
        await confirm_blind_sign_common(ctx, address, msg.raw_message)

    if scheme == "schnorr":
        signature = (
            bip340_sign(node, sig_hash)
            if use_tweak
            else bip340_sign_internal(node, sig_hash)
        )
    else:
        signature = ecdsa_sign(node, sig_hash)
    return signature
