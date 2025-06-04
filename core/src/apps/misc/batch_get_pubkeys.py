from typing import TYPE_CHECKING

from trezor import wire
from trezor.messages import EcdsaPublicKeys, HDNodeType

from apps.common import paths
from apps.common.keychain import get_keychain
from apps.common.paths import AlwaysMatchingSchema

if TYPE_CHECKING:
    from trezor.messages import BatchGetPublickeys

SUPPORTED_CURVES = ("secp256k1", "ed25519", "ed25519-keccak")
MAX_BATCH_SIZE = 20
MIN_PATH_DEPTH = 3


async def batch_get_pubkeys(
    ctx: wire.Context, msg: BatchGetPublickeys
) -> EcdsaPublicKeys:
    validate(msg)
    keychain = await get_keychain(ctx, msg.ecdsa_curve_name, [AlwaysMatchingSchema])
    pubkeys = []
    nodes = []
    translator = (
        (lambda pubkey: pubkey)
        if "secp256k1" == msg.ecdsa_curve_name
        else (lambda pubkey: pubkey[1:])
    )
    include_node = False
    root_fingerprint = None
    if "ed25519" not in msg.ecdsa_curve_name and msg.include_node:
        include_node = True
        root_fingerprint = keychain.root_fingerprint()
    for path in msg.paths:
        node = keychain.derive(path.address_n)
        pubkey = translator(node.public_key())
        if include_node:
            nodes.append(
                HDNodeType(
                    depth=node.depth(),
                    child_num=node.child_num(),
                    fingerprint=node.fingerprint(),
                    chain_code=node.chain_code(),
                    public_key=pubkey,
                )
            )
        else:
            pubkeys.append(pubkey)
    return EcdsaPublicKeys(
        public_keys=pubkeys or None,
        hd_nodes=nodes or None,
        root_fingerprint=root_fingerprint,
    )


def validate(msg: BatchGetPublickeys):
    if msg.ecdsa_curve_name not in SUPPORTED_CURVES:
        raise wire.DataError(f"Curve {msg.ecdsa_curve_name} not support")
    if len(msg.paths) > MAX_BATCH_SIZE:
        raise wire.DataError(f"Batch size should must be <= {MAX_BATCH_SIZE}")
    if not all(
        len(path.address_n) >= MIN_PATH_DEPTH
        and paths.path_is_hardened(path.address_n[:3])
        for path in msg.paths
    ):
        raise wire.DataError(
            f"Path length must be >= {MIN_PATH_DEPTH} and start with harden prefix"
        )
