from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha256
from trezor.messages import CosmosSignedTx, CosmosSignTx
from trezor.ui.layouts.lvgl import confirm_cosmos_sign_common, confirm_final

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from .transaction import Transaction


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: CosmosSignTx, keychain: Keychain
) -> CosmosSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    privkey = node.private_key()

    # parse message
    try:
        tx = Transaction.deserialize(msg.raw_tx)
    except BaseException as e:
        raise wire.DataError(f"Invalid message {e}")

    for element in tx.msgs:
        tx.display(element, 0, "msgs")
    tx.tx_display_make_friendly()
    fee = None
    if tx.amount is not None:
        fee = tx.amount + " " + tx.denom
    await confirm_cosmos_sign_common(
        ctx, tx.chain_id, fee, tx.gas, tx.memo, tx.msgs_item
    )
    await confirm_final(ctx)

    data_hash = sha256(msg.raw_tx).digest()
    signature = secp256k1.sign(privkey, data_hash, False)[1:]

    return CosmosSignedTx(signature=signature)
