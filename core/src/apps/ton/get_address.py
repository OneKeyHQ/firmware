from trezor import wire
from trezor.enums import WalletVersion, WorkChain
from trezor.lvglui.scrs import lv
from trezor.messages import TonAddress, TonGetAddress
from trezor.ui.layouts import show_address

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR
from .tonsdk.contract.wallet import Wallets, WalletVersionEnum


@auto_keychain(__name__)
async def get_address(
    ctx: wire.Context, msg: TonGetAddress, keychain: Keychain
) -> TonAddress:
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

    address = Wallets.ALL[version_str](
        public_key=pub_key_bytes, wallet_id=msg.wallet_id, wc=workchain
    ).address.to_string(True, True, msg.bounceable, msg.is_test_only)

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=address,
            address_n=path,
            network="TON",
        )
    return TonAddress(address=address, public_key=pub_key_bytes)
