from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.enums import WalletVersion, WorkChain
from trezor.lvglui.scrs import lv
from trezor.messages import TonSignedMessage, TonSignMessage
from trezor.strings import format_amount

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .tonsdk.contract.wallet import Wallets, WalletVersionEnum
from .tonsdk.utils._address import Address


@auto_keychain(__name__)
async def sign_message(
    ctx: wire.Context, msg: TonSignMessage, keychain: Keychain
) -> TonSignedMessage:

    await paths.validate_path(ctx, keychain, msg.address_n)

    assert msg.workchain in (
        WorkChain.BASECHAIN,
        WorkChain.MASTERCHAIN,
    ), "Invalid workchain provided."

    workchain = msg.workchain if msg.workchain != WorkChain.MASTERCHAIN else -1

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())

    if msg.version == WalletVersion.V3R1:
        version_str = WalletVersionEnum.v3r1
    elif msg.version == WalletVersion.V3R2:
        version_str = WalletVersionEnum.v3r2
    elif msg.version == WalletVersion.V4R1:
        version_str = WalletVersionEnum.v4r1
    elif msg.version == WalletVersion.V4R2:
        version_str = WalletVersionEnum.v4r2
    else:
        raise wire.DataError("Invalid version provided.")

    wallet = Wallets.ALL[version_str](
        public_key=pub_key_bytes, wallet_id=msg.wallet_id, wc=workchain
    )
    address = wallet.address.to_string(True, True, msg.bounceable, msg.is_test_only)
    # show display
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    from trezor.ui.layouts import confirm_output, confirm_ton_transfer

    recipient = Address(msg.recipient).to_string(True, True)
    format_value = f"{format_amount(msg.amount, 9)} TON"
    await confirm_output(ctx, recipient, format_value)

    await confirm_ton_transfer(ctx, address, recipient, format_value, msg.memo)
    from trezor.ui.layouts import confirm_final

    await confirm_final(ctx, "TON")
    digest = wallet.get_transfer_message_hash(
        msg.recipient, msg.amount, msg.seqno, msg.expiration_time, msg.memo
    )
    signature = ed25519.sign(node.private_key(), digest)
    return TonSignedMessage(signature=signature)
