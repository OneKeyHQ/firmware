from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.messages import AptosSignedTx, AptosSignTx

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from .helper import TRANSACTION_PREFIX, aptos_address_from_pubkey


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: AptosSignTx, keychain: Keychain
) -> AptosSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = aptos_address_from_pubkey(pub_key_bytes)

    from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

    await confirm_blind_sign_common(ctx, address, msg.raw_tx)
    await confirm_final(ctx)
    raw_tx = TRANSACTION_PREFIX + msg.raw_tx
    signature = ed25519.sign(node.private_key(), raw_tx)

    return AptosSignedTx(public_key=pub_key_bytes, signature=signature)
