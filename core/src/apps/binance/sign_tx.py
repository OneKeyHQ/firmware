from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha256
from trezor.enums import MessageType
from trezor.lvglui.scrs import lv
from trezor.messages import (
    BinanceCancelMsg,
    BinanceOrderMsg,
    BinanceSignedTx,
    BinanceSignTx,
    BinanceTransferMsg,
    BinanceTxRequest,
)

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR, helpers, layout


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, envelope: BinanceSignTx, keychain: Keychain
) -> BinanceSignedTx:
    # create transaction message -> sign it -> create signature/pubkey message -> serialize all
    if envelope.msg_count > 1:
        raise wire.DataError("Multiple messages not supported.")

    await paths.validate_path(ctx, keychain, envelope.address_n)
    node = keychain.derive(envelope.address_n)

    tx_req = BinanceTxRequest()

    msg = await ctx.call_any(
        tx_req,
        MessageType.BinanceCancelMsg,
        MessageType.BinanceOrderMsg,
        MessageType.BinanceTransferMsg,
    )

    if envelope.source < 0:
        raise wire.DataError("Source is invalid.")

    msg_json = helpers.produce_json_for_signing(envelope, msg)
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    if BinanceTransferMsg.is_type_of(msg):
        await layout.require_confirm_transfer(ctx, msg)
    elif BinanceOrderMsg.is_type_of(msg):
        await layout.require_confirm_order(ctx, msg)
    elif BinanceCancelMsg.is_type_of(msg):
        await layout.require_confirm_cancel(ctx, msg)
    else:
        raise wire.ProcessError("input message unrecognized")
    from trezor.ui.layouts import confirm_final

    await confirm_final(ctx, "BNB")
    signature_bytes = generate_content_signature(msg_json.encode(), node.private_key())

    return BinanceSignedTx(signature=signature_bytes, public_key=node.public_key())


def generate_content_signature(json: bytes, private_key: bytes) -> bytes:
    msghash = sha256(json).digest()
    return secp256k1.sign(private_key, msghash)[1:65]
