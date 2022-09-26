# from typing import TYPE_CHECKING

import storage.cache
from trezor import io, wire
from trezor.messages import FatfsSeek, Success


async def fseek(ctx: wire.Context, msg: FatfsSeek) -> Success:
    if storage.cache.current_open_file is None:
        raise wire.ProcessError("Open file first")
    try:
        storage.cache.current_open_file.seek(msg.offset)
        return Success(message="File seek succeeded")
    except io.fatfs.FatFSError:
        raise wire.ProcessError("File seek failed")
