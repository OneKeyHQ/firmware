from typing import TYPE_CHECKING

from trezor import wire
from trezor.enums import CardanoNativeScriptHashDisplayFormat
from trezor.lvglui.scrs import lv
from trezor.messages import CardanoNativeScriptHash

from . import ICON, PRIMARY_COLOR, native_script, seed
from .layout import show_native_script, show_script_hash

if TYPE_CHECKING:
    from trezor.messages import CardanoGetNativeScriptHash


@seed.with_keychain
async def get_native_script_hash(
    ctx: wire.Context, msg: CardanoGetNativeScriptHash, keychain: seed.Keychain
) -> CardanoNativeScriptHash:
    native_script.validate_native_script(msg.script)

    script_hash = native_script.get_native_script_hash(keychain, msg.script)

    if msg.display_format != CardanoNativeScriptHashDisplayFormat.HIDE:
        ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
        await show_native_script(ctx, msg.script)
        await show_script_hash(ctx, script_hash, msg.display_format)

    return CardanoNativeScriptHash(script_hash=script_hash)
