from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.messages import SuiSignedTx, SuiSignTx

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from .helper import TRANSACTION_PREFIX, sui_address_from_pubkey


@auto_keychain(__name__)
async def sign_tx(ctx: wire.Context, msg: SuiSignTx, keychain: Keychain) -> SuiSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = sui_address_from_pubkey(pub_key_bytes)

    from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

    type_tag = msg.raw_tx[:17]
    if TRANSACTION_PREFIX != type_tag:
        raise wire.DataError("Invalid raw tx")

    await confirm_blind_sign_common(ctx, address, msg.raw_tx)
    await confirm_final(ctx)
    signature = ed25519.sign(node.private_key(), msg.raw_tx)

    return SuiSignedTx(public_key=pub_key_bytes, signature=signature)
