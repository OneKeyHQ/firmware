from ubinascii import unhexlify

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import blake2b
from trezor.lvglui.scrs import lv
from trezor.messages import FilecoinSignedTx, FilecoinSignTx
from trezor.strings import format_amount
from trezor.ui.layouts.lvgl import confirm_filecoin_payment, confirm_final

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .layout import require_confirm_tx
from .transaction import Transaction


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: FilecoinSignTx, keychain: Keychain
) -> FilecoinSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    # parse message
    try:
        tx = Transaction.deserialize(msg.raw_tx, "t" if msg.testnet else "f")
    except BaseException as e:
        raise wire.DataError(f"Invalid message {e}")

    await require_confirm_tx(ctx, tx.to, tx.value)
    fee_max = tx.gasfeecap * tx.gaslimit
    await confirm_filecoin_payment(
        ctx,
        tx.source,
        tx.to,
        f"{format_amount(tx.value, 18)} FIL",
        f"{tx.gaslimit}",
        f"{format_amount(tx.gasfeecap, 18)} FIL",
        f"{format_amount(tx.gaspremium, 18)} FIL",
        f"{format_amount(fee_max + tx.value, 18)} FIL",
    )

    hash = blake2b(data=msg.raw_tx, outlen=32).digest()
    prefix = unhexlify("0171a0e40220")  # PREFIX{0x01, 0x71, 0xa0, 0xe4, 0x02, 0x20}
    message_digest = blake2b(data=prefix + hash, outlen=32).digest()
    signature = secp256k1.sign(node.private_key(), message_digest, False)
    signature = signature[1:65] + bytes([(~signature[0] & 0x01)])

    await confirm_final(ctx, "Filecoin")
    return FilecoinSignedTx(signature=signature)
