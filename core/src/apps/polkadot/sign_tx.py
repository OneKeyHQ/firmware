from trezor import utils, wire
from trezor.crypto.curve import ed25519
from trezor.messages import PolkadotSignedTx, PolkadotSignTx
from trezor.ui.layouts import confirm_final

from apps.common import paths
from apps.common.keychain import FORBIDDEN_KEY_PATH

from . import helper, seed, transaction


@seed.with_keychain
async def sign_tx(
    ctx: wire.Context, msg: PolkadotSignTx, keychain: seed.Keychain
) -> PolkadotSignedTx:
    await paths.validate_path(ctx, keychain, msg.address_n)
    if (
        len(msg.address_n) != 5
        or msg.address_n[0] != 0x8000002C
        or msg.address_n[1] != 0x80000162
    ):
        raise FORBIDDEN_KEY_PATH

    node = keychain.derive(msg.address_n)
    if utils.USE_THD89:
        public_key = node.public_key()[1:]
    else:
        public_key = ed25519.publickey(node.private_key())
    address_type = helper.get_address_type(msg.network)
    address = helper.ss58_encode(public_key, address_type)
    chain_name, symbol, decimal = helper.update_chain_res(ctx, msg.network)
    tx = transaction.Transaction.deserialize(msg.raw_tx, msg.network)
    await tx.layout(ctx, address, chain_name, symbol, decimal)
    await confirm_final(ctx)
    signature = ed25519.sign(node.private_key(), msg.raw_tx)

    return PolkadotSignedTx(signature=signature)
