from trezor import utils, wire
from trezor.messages import SEMessageSignature, SESignMessage

if not utils.EMULATOR:
    import atca  # type: ignore[Import "atca" could not be resolved]


async def se_sign_message(ctx: wire.Context, msg: SESignMessage) -> SEMessageSignature:

    sign = bytes(64)
    if atca.se_sign_message(msg.message, len(msg.message), sign):
        return SEMessageSignature(signature=sign)
    else:
        raise wire.ProcessError("se sign failed")
