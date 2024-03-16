from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.messages import NervosSignedTx, NervosSignTx, NervosTxAck, NervosTxRequest
from trezor.ui.layouts import confirm_final

from apps.common import paths
from apps.common.keychain import auto_keychain
from apps.nervos.get_address import generate_ckb_short_address
from apps.nervos.hash import ckb_hash, ckb_hasher

from .utils import bytes_to_hex_str, extend_uint64, hex_to_bytes_custom

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

    if msg.data_length is not None and msg.data_length > 0:
        data_total = msg.data_length
        data = bytearray()
        data_left = data_total
        hasher = ckb_hasher()
        hasher.update(msg.data_initial_chunk)
        print("lendata_left:" + str(data_left))
        data_left -= len(msg.data_initial_chunk)
        print("len(msg.raw_message):" + str(len(msg.data_initial_chunk)))
        while data_left > 0:
            resp = await send_request_chunk(ctx, data_left)
            data_left -= len(resp.data_chunk)
            hasher.update(resp.data_chunk)
            data += resp.data_chunk
        hash_bytes = hasher.digest()
        hex_str = bytes_to_hex_str(hash_bytes)
        tx_hash = "0x" + hex_str
        # await confirm_blind_sign_common(ctx, address, bytes(data))
    else:
        tx_hash = ckb_hash(msg.data_initial_chunk)

    # tx_hash = ckb_hash(msg.raw_message)
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


async def send_request_chunk(ctx: wire.Context, data_left: int) -> NervosTxAck:
    req = NervosTxRequest()
    if data_left <= 1024:
        req.data_length = data_left
    else:
        req.data_length = 1024

    return await ctx.call(req, NervosTxAck)
