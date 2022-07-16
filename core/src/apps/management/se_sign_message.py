from typing import TYPE_CHECKING

from trezor import utils, wire
from trezor.messages import SEMessageSignature

if TYPE_CHECKING:
    from trezor.messages import SESignMessage


async def se_sign_message(ctx: wire.Context, msg: SESignMessage) -> SEMessageSignature:
    if utils.EMULATOR:
        raise wire.ProcessError("Not support by emulator.")

    from trezor.ui.layouts.lvgl import confirm_security_check

    await confirm_security_check(ctx)

    import atca  # type: ignore[Import "atca" could not be resolved]

    signature = bytes(64)
    if atca.se_sign_message(msg.message, len(msg.message), signature):
        return SEMessageSignature(signature=signature)
    else:
        raise wire.ProcessError("se sign failed")
