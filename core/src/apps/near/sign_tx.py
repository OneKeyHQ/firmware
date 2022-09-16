from ubinascii import hexlify

from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha256
from trezor.messages import NearSignedTx, NearSignTx

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from .layout import require_confirm_tx
from .transaction import Action_Transfer, Transaction


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: NearSignTx, keychain: Keychain
) -> NearSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    pubkey = node.public_key()[1:]
    address = "0x" + hexlify(pubkey).decode()

    # parse message
    try:
        tx = Transaction.deserialize(msg.raw_tx)
    except BaseException as e:
        raise wire.DataError(f"Invalid message {e}")

    if tx.action.action_type == Action_Transfer:
        from trezor.ui.layouts.lvgl import confirm_final

        await require_confirm_tx(ctx, tx.receiverId, tx.action.amount)
        await confirm_final(ctx)
    else:
        from trezor.ui.layouts.lvgl import confirm_blind_sign_common, confirm_final

        await confirm_blind_sign_common(ctx, address, msg.raw_tx)
        await confirm_final(ctx)

    hash = sha256(msg.raw_tx).digest()
    signature = ed25519.sign(node.private_key(), hash)

    return NearSignedTx(signature=signature)
