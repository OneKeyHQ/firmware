from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.messages import EosGetPublicKey, EosPublicKey

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from .helpers import public_key_to_wif

if TYPE_CHECKING:
    from trezor.crypto import bip32


def _get_public_key(node: bip32.HDNode) -> tuple[str, bytes]:
    seckey = node.private_key()
    public_key = secp256k1.publickey(seckey, True)
    wif = public_key_to_wif(public_key)
    return wif, public_key


@auto_keychain(__name__)
async def get_public_key(
    ctx: wire.Context, msg: EosGetPublicKey, keychain: Keychain
) -> EosPublicKey:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    wif, public_key = _get_public_key(node)
    if msg.show_display:
        # await require_get_public_key(ctx, wif)
        from trezor.ui.layouts import show_pubkey

        path = paths.address_n_to_str(msg.address_n)
        await show_pubkey(ctx, wif, path=path, network="EOS")
    return EosPublicKey(wif_public_key=wif, raw_public_key=public_key)
