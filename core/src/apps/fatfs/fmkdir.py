# from typing import TYPE_CHECKING

from trezor import io, wire
from trezor.messages import FatfsMkdir, Success


async def fmkdir(ctx: wire.Context, msg: FatfsMkdir) -> Success:
    try:
        io.fatfs.mkdir(msg.path, True)
        return Success(message="Create directory succeeded")
    except io.fatfs.FatFSError:
        raise wire.ProcessError("Create directory failed")
