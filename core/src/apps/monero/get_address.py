from trezor.lvglui.scrs import lv
from trezor.messages import MoneroAddress
from trezor.ui.layouts import show_address

from apps.common import paths
from apps.common.keychain import auto_keychain
from apps.monero import misc
from apps.monero.xmr import addresses, crypto, monero
from apps.monero.xmr.networks import net_version

from . import ICON, PRIMARY_COLOR


@auto_keychain(__name__)
async def get_address(ctx, msg, keychain):
    await paths.validate_path(ctx, keychain, msg.address_n)

    creds = misc.get_creds(keychain, msg.address_n, msg.network_type)
    addr = creds.address

    if msg.payment_id:
        if len(msg.payment_id) != 8:
            raise ValueError("Invalid payment ID length")
        addr = addresses.encode_addr(
            net_version(msg.network_type, False, True),
            crypto.encodepoint(creds.spend_key_public),
            crypto.encodepoint(creds.view_key_public),
            msg.payment_id,
        )

    if msg.account or msg.minor:
        if msg.payment_id:
            raise ValueError("Subaddress cannot be integrated")

        pub_spend, pub_view = monero.generate_sub_address_keys(
            creds.view_key_private, creds.spend_key_public, msg.account, msg.minor
        )

        addr = addresses.encode_addr(
            net_version(msg.network_type, True, False),
            crypto.encodepoint(pub_spend),
            crypto.encodepoint(pub_view),
        )

    if msg.show_display:
        path = paths.address_n_to_str(msg.address_n)

        ctx.primary_color, ctx.icon = lv.color_hex(PRIMARY_COLOR), ICON
        await show_address(
            ctx,
            address=addr.decode(),
            address_n=path,
            network="XMR",
        )

    return MoneroAddress(address=addr)
