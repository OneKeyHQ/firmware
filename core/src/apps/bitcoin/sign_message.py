from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto.curve import secp256k1
from trezor.enums import InputScriptType
from trezor.lvglui.scrs import lv
from trezor.messages import MessageSignature
from trezor.ui.layouts import confirm_signverify

from apps.common.helpers import validate_message
from apps.common.paths import validate_path
from apps.common.signverify import decode_message, message_digest

from .addresses import address_short, get_address
from .keychain import validate_path_against_script_type, with_keychain

if TYPE_CHECKING:
    from trezor.messages import SignMessage

    from apps.common.coininfo import CoinInfo
    from apps.common.keychain import Keychain


@with_keychain
async def sign_message(
    ctx: wire.Context, msg: SignMessage, keychain: Keychain, coin: CoinInfo
) -> MessageSignature:
    message = msg.message
    address_n = msg.address_n
    script_type = msg.script_type or InputScriptType.SPENDADDRESS
    validate_message(message)
    await validate_path(
        ctx, keychain, address_n, validate_path_against_script_type(coin, msg)
    )

    node = keychain.derive(address_n)
    address = get_address(script_type, coin, node)
    ctx.primary_color, ctx.icon_path = (
        lv.color_hex(coin.primary_color),
        f"A:/res/{coin.icon}",
    )
    await confirm_signverify(
        ctx,
        coin.coin_shortcut,
        decode_message(message),
        address_short(coin, address),
        verify=False,
    )

    seckey = node.private_key()

    digest = message_digest(coin, message)
    signature = secp256k1.sign(seckey, digest)

    if script_type == InputScriptType.SPENDADDRESS:
        script_type_info = 0
    elif script_type == InputScriptType.SPENDP2SHWITNESS:
        script_type_info = 4
    elif script_type == InputScriptType.SPENDWITNESS:
        script_type_info = 8
    else:
        raise wire.ProcessError("Unsupported script type")

    # Add script type information to the recovery byte.
    if script_type_info != 0 and not msg.no_script_type:
        signature = bytes([signature[0] + script_type_info]) + signature[1:]

    return MessageSignature(address=address, signature=signature)
