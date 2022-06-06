from trezor import wire
from trezor.crypto import rlp
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.enums import MessageType
from trezor.messages import ConfluxSignTx, ConfluxTxAck, ConfluxTxRequest
from trezor.utils import HashWriter

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from .helpers import bytes_from_address, decode_hex_address
from .layout import require_confirm_data, require_confirm_tx


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: ConfluxSignTx, keychain: Keychain
) -> ConfluxTxRequest:

    data_total = msg.data_length

    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)

    await require_confirm_tx(ctx, msg.to, int.from_bytes(msg.value, "big"))
    if msg.data_length > 0:
        await require_confirm_data(ctx, msg.data_initial_chunk, data_total)

    msg.to = decode_hex_address(msg.to)

    data = bytearray()
    data += msg.data_initial_chunk
    data_left = data_total - len(msg.data_initial_chunk)

    total_length = get_total_length(msg, data_total)

    sha = HashWriter(sha3_256(keccak=True))
    rlp.write_header(sha, total_length, rlp.LIST_HEADER_BYTE)
    rlp.write(sha, msg.nonce)
    rlp.write(sha, msg.gas_price)
    rlp.write(sha, msg.gas)
    rlp.write(sha, bytes_from_address(msg.to))
    rlp.write(sha, msg.value)
    rlp.write(sha, msg.storage_limit)
    rlp.write(sha, msg.epoch_height)
    rlp.write(sha, msg.chain_id)

    if data_left == 0:
        rlp.write(sha, data)
    else:
        rlp.write_header(sha, data_total, rlp.STRING_HEADER_BYTE, data)
        sha.extend(data)

    while data_left > 0:
        resp = await send_request_chunk(ctx, data_left)
        data_left -= len(resp.data_chunk)
        sha.extend(resp.data_chunk)

    digest = sha.get_digest()
    signature = secp256k1.sign(
        node.private_key(), digest, False, secp256k1.CANONICAL_SIG_ETHEREUM
    )

    req = ConfluxTxRequest()
    req.signature_v = signature[0] - 27
    req.signature_r = signature[1:33]
    req.signature_s = signature[33:]

    return req


def get_total_length(msg: ConfluxSignTx, data_total: int) -> int:
    length = 0

    fields: tuple[rlp.RLPItem, ...] = (
        msg.nonce,
        msg.gas_price,
        msg.gas,
        bytes_from_address(msg.to),
        msg.value,
        msg.storage_limit,
        msg.epoch_height,
        msg.chain_id,
    )

    for field in fields:
        length += rlp.length(field)

    length += rlp.header_length(data_total, msg.data_initial_chunk)
    length += data_total

    return length


async def send_request_chunk(ctx: wire.Context, data_left: int) -> ConfluxTxAck:
    req = ConfluxTxRequest()
    if data_left <= 1024:
        req.data_length = data_left
    else:
        req.data_length = 1024

    return await ctx.call(req, ConfluxTxAck)
