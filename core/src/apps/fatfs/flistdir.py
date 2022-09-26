# from typing import TYPE_CHECKING

from trezor import io, wire
from trezor.messages import FatfsDirlist, FatfsListdir


async def flistdir(ctx: wire.Context, msg: FatfsListdir) -> FatfsDirlist:
    try:
        items = [e for e in io.fatfs.listdir(msg.path)]
        return FatfsDirlist(filelist=items)
    except io.fatfs.FatFSError:
        raise wire.ProcessError("list dir failed")
