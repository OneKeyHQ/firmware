from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.messages import NervosSignedTx, NervosSignTx
from trezor.ui.layouts import confirm_final

from apps.common import paths
from apps.common.keychain import auto_keychain
from apps.nervos.get_address import generate_ckb_short_address
from apps.nervos.hash import ckb_hash, ckb_hasher

from .utils import extend_uint64, hex_to_bytes_custom

if TYPE_CHECKING:
    from apps.common.keychain import Keychain


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context,
    msg: NervosSignTx,
    keychain: Keychain,
) -> NervosSignedTx:
    if msg.network not in ["ckb", "ckt"]:
        raise ValueError(f"Invalid network: {msg.network}")
    await paths.validate_path(ctx, keychain, msg.address_n)
    tx_hash = ckb_hash(msg.raw_message)
    hasher = ckb_hasher()
    hasher.update(hex_to_bytes_custom(tx_hash))
    len_buffer = bytearray()
    extend_uint64(len_buffer, str(len(msg.witness_buffer)))
    hasher.update(len_buffer)
    hasher.update(msg.witness_buffer)
    message_byte = hasher.digest()
    node = keychain.derive(msg.address_n)
    address = generate_ckb_short_address(node, network=msg.network)
    private_key = node.private_key()
    await confirm_final(ctx, "NERVOS")
    unmodified_signature = secp256k1.sign(private_key, message_byte, False)
    adjusted_byte = (unmodified_signature[0] - 27) % 256
    signature = unmodified_signature[1:] + bytes([adjusted_byte])
    return NervosSignedTx(signature=signature, address=address)
