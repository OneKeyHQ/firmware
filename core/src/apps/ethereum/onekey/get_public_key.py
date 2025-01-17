from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor.messages import EthereumPublicKeyOneKey as EthereumPublicKey, HDNodeType
from trezor.ui.layouts import show_pubkey

from apps.common import coins, paths

from .. import networks
from .keychain import with_keychain_from_path

if TYPE_CHECKING:
    from trezor.messages import EthereumGetPublicKeyOneKey as EthereumGetPublicKey
    from trezor.wire import Context

    from apps.common.keychain import Keychain


@with_keychain_from_path(paths.PATTERN_BIP44_PUBKEY)
async def get_public_key(
    ctx: Context, msg: EthereumGetPublicKey, keychain: Keychain
) -> EthereumPublicKey:
    await paths.validate_path(ctx, keychain, msg.address_n, force_strict=False)
    node = keychain.derive(msg.address_n, force_strict=False)

    # we use the Bitcoin format for Ethereum xpubs
    btc = coins.by_name("Bitcoin")
    node_xpub = node.serialize_public(btc.xpub_magic)

    pubkey = node.public_key()
    if pubkey[0] == 1:
        pubkey = b"\x00" + pubkey[1:]
    node_type = HDNodeType(
        depth=node.depth(),
        child_num=node.child_num(),
        fingerprint=node.fingerprint(),
        chain_code=node.chain_code(),
        public_key=pubkey,
    )

    if msg.show_display:
        from ..helpers import get_color_and_icon, get_display_network_name

        if msg.chain_id:
            network = networks.by_chain_id(msg.chain_id)
        else:
            if len(msg.address_n) > 1:  # path has slip44 network identifier
                network = networks.by_slip44(msg.address_n[1] & 0x7FFF_FFFF)
            else:
                network = None
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = get_color_and_icon(
            network.chain_id if network else None
        )
        await show_pubkey(
            ctx,
            hexlify(pubkey).decode(),
            path=path,
            network=get_display_network_name(network),
        )

    return EthereumPublicKey(node=node_type, xpub=node_xpub)
