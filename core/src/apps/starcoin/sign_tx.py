from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha3_256
from trezor.messages import StarcoinSignedTx, StarcoinSignTx

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from .helper import get_address_from_public_key
from .layout import confirm_final, require_confirm_data, require_confirm_tx


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: StarcoinSignTx, keychain: Keychain
) -> StarcoinSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pubkey = node.public_key()[1:]
    address = get_address_from_public_key(pubkey)
    raw_tx = msg.raw_tx if msg.raw_tx is not None else b""
    raw_tx_size = len(msg.raw_tx) if msg.raw_tx is not None else 0

    await require_confirm_tx(ctx, address)
    await require_confirm_data(ctx, raw_tx, raw_tx_size)
    await confirm_final(ctx)

    prefix = sha3_256(b"STARCOIN::RawUserTransaction", keccak=False).digest()
    data = prefix + raw_tx
    signature = ed25519.sign(node.private_key(), data)

    return StarcoinSignedTx(public_key=pubkey, signature=signature)
