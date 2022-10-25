from trezor import wire
from trezor.crypto import rlp
from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha3_256
from trezor.messages import ConfluxSignTx, ConfluxTxAck, ConfluxTxRequest
from trezor.utils import HashWriter

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from ..ethereum import tokens
from .helpers import address_from_bytes, address_from_hex, bytes_from_address
from .layout import (
    require_confirm_data,
    require_confirm_fee,
    require_confirm_tx,
    require_confirm_unknown_token,
)


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: ConfluxSignTx, keychain: Keychain
) -> ConfluxTxRequest:

    data_total = msg.data_length if msg.data_length is not None else 0

    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)

    if (
        msg.gas_limit is None
        or msg.gas_price is None
        or msg.epoch_height is None
        or msg.storage_limit is None
    ):
        raise wire.DataError("Invalid params")

    owner_address = address_from_bytes(node.ethereum_pubkeyhash(), None)
    value = msg.value if msg.value is not None else b""
    gas_price = msg.gas_price if msg.gas_price is not None else b""
    gas_limit = msg.gas_limit if msg.gas_limit is not None else b""
    to = msg.to if msg.to is not None else ""
    storage_limit = msg.storage_limit if msg.storage_limit is not None else b""
    epoch_height = msg.epoch_height if msg.epoch_height is not None else b""
    nonce = msg.nonce if msg.nonce is not None else b""
    chain_id = msg.chain_id if msg.chain_id is not None else 1029
    data_initial_chunk = (
        msg.data_initial_chunk if msg.data_initial_chunk is not None else b""
    )
    owner_cfx_address = address_from_hex(owner_address, chain_id)
    if len(to) > 0:
        cfx_to = address_from_hex(to, chain_id)
        token = None
        # detect ERC-20 like token
        if (
            len(value) == 0
            and data_total == 68
            and len(data_initial_chunk) == 68
            and data_initial_chunk[:16]
            == b"\xa9\x05\x9c\xbb\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        ):
            # not support tokens display right now, return unknown recipient directly
            amount = int.from_bytes(data_initial_chunk[36:68], "big")
            address = address_from_hex(
                address_from_bytes(data_initial_chunk[16:36], None), chain_id
            )
            if cfx_to == "cfx:acf2rcsh8payyxpg6xj7b0ztswwh81ute60tsw35j7":
                token = tokens.TokenInfo("cUSDT", 18)
            else:
                token = tokens.TokenInfo("UNKN", 0)
                await require_confirm_unknown_token(ctx, cfx_to)
        else:
            amount = int.from_bytes(value, "big")
            address = cfx_to

        await require_confirm_tx(ctx, address, amount, token)
        if data_total > 0:
            await require_confirm_data(ctx, data_initial_chunk, data_total)
        await require_confirm_fee(
            ctx,
            from_address=owner_cfx_address,
            to_address=address,
            value=amount,
            gas_price=int.from_bytes(gas_price, "big"),
            gas_limit=int.from_bytes(gas_limit, "big"),
            network="CFX",
            token=token,
        )
    else:
        address = "new contract?"
        await require_confirm_tx(ctx, address, int.from_bytes(value, "big"), None)
        if data_total > 0:
            await require_confirm_data(ctx, data_initial_chunk, data_total)
        await require_confirm_fee(
            ctx,
            from_address=owner_cfx_address,
            to_address=address,
            value=int.from_bytes(value, "big"),
            gas_price=int.from_bytes(gas_price, "big"),
            gas_limit=int.from_bytes(gas_limit, "big"),
            network="CFX",
        )

    data = bytearray()
    data += data_initial_chunk
    data_left = data_total - len(data_initial_chunk)

    total_length = get_total_length(
        value=value,
        gas_price=gas_price,
        gas_limit=gas_limit,
        to=to,
        storage_limit=storage_limit,
        epoch_height=epoch_height,
        nonce=nonce,
        chain_id=chain_id,
        data_initial_chunk=data_initial_chunk,
        data_total=data_total,
    )

    sha = HashWriter(sha3_256(keccak=True))
    rlp.write_header(sha, total_length, rlp.LIST_HEADER_BYTE)
    rlp.write(sha, nonce)
    rlp.write(sha, gas_price)
    rlp.write(sha, gas_limit)
    rlp.write(sha, bytes_from_address(to))
    rlp.write(sha, value)
    rlp.write(sha, storage_limit)
    rlp.write(sha, epoch_height)
    rlp.write(sha, chain_id)

    if data_left == 0:
        rlp.write(sha, data)
    else:
        rlp.write_header(sha, data_total, rlp.STRING_HEADER_BYTE, data)
        sha.extend(data)

    while data_left > 0:
        resp = await send_request_chunk(ctx, data_left)
        data_chunk = resp.data_chunk if resp.data_chunk is not None else b""
        data_left -= len(data_chunk)
        sha.extend(data_chunk)

    digest = sha.get_digest()
    signature = secp256k1.sign(
        node.private_key(), digest, False, secp256k1.CANONICAL_SIG_ETHEREUM
    )

    req = ConfluxTxRequest()
    req.signature_v = signature[0] - 27
    req.signature_r = signature[1:33]
    req.signature_s = signature[33:]

    return req


def get_total_length(
    value: bytes,
    gas_price: bytes,
    gas_limit: bytes,
    to: str,
    storage_limit: bytes,
    epoch_height: bytes,
    nonce: bytes,
    chain_id: int,
    data_initial_chunk: bytes,
    data_total: int,
) -> int:
    length = 0

    fields: tuple[rlp.RLPItem, ...] = (
        nonce,
        gas_price,
        gas_limit,
        bytes_from_address(to),
        value,
        storage_limit,
        epoch_height,
        chain_id,
    )

    for field in fields:
        length += rlp.length(field)

    length += rlp.header_length(data_total, data_initial_chunk)
    length += data_total

    return length


async def send_request_chunk(ctx: wire.Context, data_left: int) -> ConfluxTxAck:
    req = ConfluxTxRequest()
    if data_left <= 1024:
        req.data_length = data_left
    else:
        req.data_length = 1024

    return await ctx.call(req, ConfluxTxAck)
