from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor.crypto import der, hmac
from trezor.crypto.curve import secp256k1
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import LnurlAuthResp
from trezor.ui.layouts import confirm_lnurl_auth

from apps.common import paths
from apps.common.keychain import get_keychain

from . import PRIMARY_COLOR

if TYPE_CHECKING:
    from trezor.messages import LnurlAuth
    from trezor.wire import Context


async def auth(ctx: Context, msg: LnurlAuth) -> LnurlAuthResp:
    keychain = await get_keychain(ctx, "secp256k1", [paths.AlwaysMatchingSchema])
    # m/138'/0
    node = keychain.derive([2147483786, 0])
    hashingPrivKey = node.private_key()

    # ctx.primary_color, ctx.icon_path = (
    #     lv.color_hex(PRIMARY_COLOR),
    #     None,
    # )
    ctx.primary_color = lv.color_hex(PRIMARY_COLOR)
    await confirm_lnurl_auth(
        ctx,
        _(i18n_keys.TITLE__APPROVE_LNURL_AUTHORIZATION),
        msg.domain.decode(),
        hexlify(msg.data).decode() if msg.data is not None else "",
    )

    derivationMaterial = hmac(hmac.SHA256, msg.domain, hashingPrivKey).digest()

    long1 = int.from_bytes(derivationMaterial[0:4], "big")
    long2 = int.from_bytes(derivationMaterial[4:8], "big")
    long3 = int.from_bytes(derivationMaterial[8:12], "big")
    long4 = int.from_bytes(derivationMaterial[12:16], "big")

    node = keychain.derive([2147483786, long1, long2, long3, long4])

    path = f"m/138'/{long1}/{long2}/{long3}/{long4}"
    if msg.data is not None:
        if len(msg.data) != 32:
            raise ValueError("Invalid random data")
        signature = secp256k1.sign(node.private_key(), msg.data, False)
        sig_der = der.encode_seq((signature[1:33], signature[33:65]))
    else:
        sig_der = None

    return LnurlAuthResp(
        publickey=hexlify(node.public_key()).decode(), path=path, signature=sig_der
    )
