# from typing import TYPE_CHECKING

from trezor import io, wire
from trezor.messages import FatfsOpen, Success


async def funlink(ctx: wire.Context, msg: FatfsOpen) -> Success:
    try:
        io.fatfs.unlink(msg.path)
        return Success(message="Remove file succeeded")
    except io.fatfs.FatFSError:
        raise wire.ProcessError("Remove file failed")
