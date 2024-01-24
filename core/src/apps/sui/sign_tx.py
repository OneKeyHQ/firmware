from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import blake2b
from trezor.lvglui.scrs import lv
from trezor.messages import SuiSignedTx, SuiSignTx, SuiTxAck, SuiTxRequest

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .helper import INTENT_BYTES, sui_address_from_pubkey


@auto_keychain(__name__)
async def sign_tx(ctx: wire.Context, msg: SuiSignTx, keychain: Keychain) -> SuiSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = sui_address_from_pubkey(pub_key_bytes)

    from trezor.ui.layouts import confirm_blind_sign_common, confirm_final

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON

    if msg.data_length is not None and msg.data_length > 0:
        intent = msg.data_initial_chunk[:3]
        if INTENT_BYTES != intent:
            raise wire.DataError("Invalid raw tx")

        data_total = msg.data_length
        data = bytearray()
        data += msg.data_initial_chunk
        data_left = data_total - len(msg.data_initial_chunk)

        hash_fn = blake2b(outlen=32)
        hash_fn.update(msg.data_initial_chunk)
        while data_left > 0:
            resp = await send_request_chunk(ctx, data_left)
            data_left -= len(resp.data_chunk)
            hash_fn.update(resp.data_chunk)

        hash = hash_fn.digest()
        await confirm_blind_sign_common(ctx, address, hash, hash)
    else:
        intent = msg.raw_tx[:3]
        if INTENT_BYTES != intent:
            raise wire.DataError("Invalid raw tx")

        hash = blake2b(data=msg.raw_tx, outlen=32).digest()
        await confirm_blind_sign_common(ctx, address, msg.raw_tx)

    await confirm_final(ctx, "SUI")
    signature = ed25519.sign(node.private_key(), hash)

    return SuiSignedTx(public_key=pub_key_bytes, signature=signature)


async def send_request_chunk(ctx: wire.Context, data_left: int) -> SuiTxAck:
    req = SuiTxRequest()
    if data_left <= 1024:
        req.data_length = data_left
    else:
        req.data_length = 1024

    return await ctx.call(req, SuiTxAck)
