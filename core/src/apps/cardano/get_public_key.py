from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor import wire
from trezor.messages import CardanoPublicKey

from . import seed

if __debug__:
    from trezor import log

if TYPE_CHECKING:
    from trezor.messages import CardanoGetPublicKey


@seed.with_keychain
async def get_public_key(
    ctx: wire.Context, msg: CardanoGetPublicKey, keychain: seed.Keychain
) -> CardanoPublicKey:

    from trezor.ui.layouts import show_pubkey

    from apps.common import paths

    from .helpers.paths import SCHEMA_MINT, SCHEMA_PUBKEY

    address_n = msg.address_n  # local_cache_attribute

    await paths.validate_path(
        ctx,
        keychain,
        address_n,
        # path must match the PUBKEY schema
        SCHEMA_PUBKEY.match(address_n) or SCHEMA_MINT.match(address_n),
    )
    from . import ICON, PRIMARY_COLOR
    from trezor.lvglui.scrs import lv

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON

    try:
        key = _get_public_key(keychain, address_n)
    except ValueError as e:
        if __debug__:
            log.exception(__name__, e)
        raise wire.ProcessError("Deriving public key failed")

    if msg.show_display:
        from apps.common.paths import address_n_to_str

        path = address_n_to_str(address_n)
        await show_pubkey(ctx, key.xpub, path=path, network="Cardano")
    return key


def _get_public_key(
    keychain: seed.Keychain, derivation_path: list[int]
) -> CardanoPublicKey:
    from trezor.messages import HDNodeType

    from .helpers.utils import derive_public_key

    node = keychain.derive(derivation_path)
    public_key = derive_public_key(keychain, derivation_path)
    chain_code = node.chain_code()
    xpub_key = hexlify(public_key + chain_code).decode()

    node_type = HDNodeType(
        depth=node.depth(),
        child_num=node.child_num(),
        fingerprint=node.fingerprint(),
        chain_code=chain_code,
        public_key=public_key,
    )

    return CardanoPublicKey(node=node_type, xpub=xpub_key)
