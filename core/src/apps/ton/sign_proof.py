from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha256
from trezor.enums import TonWalletVersion, TonWorkChain
from trezor.lvglui.scrs import lv
from trezor.messages import TonSignedProof, TonSignProof

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .tonsdk.contract.wallet import Wallets, WalletVersionEnum

if TYPE_CHECKING:
    from trezor.wire import Context


@auto_keychain(__name__)
async def sign_proof(
    ctx: Context, msg: TonSignProof, keychain: Keychain
) -> TonSignedProof:
    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    public_key = seed.remove_ed25519_prefix(node.public_key())
    workchain = (
        -1 if msg.workchain == TonWorkChain.MASTERCHAIN else TonWorkChain.BASECHAIN
    )

    if msg.wallet_version == TonWalletVersion.V4R2:
        wallet_version = WalletVersionEnum.v4r2
    else:
        raise wire.DataError("Invalid wallet version.")

    wallet = Wallets.ALL[wallet_version](
        public_key=public_key, wallet_id=msg.wallet_id, wc=workchain
    )
    address = wallet.address.to_string(
        is_user_friendly=True,
        is_url_safe=True,
        is_bounceable=msg.is_bounceable,
        is_test_only=msg.is_testnet_only,
    )

    # display
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    from trezor.ui.layouts import confirm_ton_signverify

    if msg.appdomain is None:
        raise ValueError("Domain cannot be None")
    await confirm_ton_signverify(
        ctx,
        "TON",
        msg.comment.decode("UTF-8") if msg.comment else "",
        address,
        msg.appdomain.decode("UTF-8"),
        verify=False,
    )

    ton_proof_prefix = "ton-proof-item-v2/"
    ton_connect_prefix = "ton-connect"

    message = (
        ton_proof_prefix.encode("utf-8")
        + workchain.to_bytes(4, "big")
        + wallet.address.get_hash_part()
        + len(msg.appdomain).to_bytes(4, "little")
        + msg.appdomain
        + msg.expire_at.to_bytes(8, "little")
        + msg.comment
    )
    message_hash = sha256(message).digest()

    full_message = b"\xff\xff" + ton_connect_prefix.encode("utf-8") + message_hash

    signature = ed25519.sign(node.private_key(), sha256(full_message).digest())

    return TonSignedProof(signature=signature)
