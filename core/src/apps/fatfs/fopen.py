# from typing import TYPE_CHECKING

import storage.cache
from trezor import io, wire
from trezor.messages import FatfsOpen, Success


async def fopen(ctx: wire.Context, msg: FatfsOpen) -> Success:
    try:
        storage.cache.current_open_file = io.fatfs.open(msg.path, msg.mode)
        return Success(message="Open file succeeded")
    except io.fatfs.FatFSError:
        raise wire.ProcessError("Open file failed")
