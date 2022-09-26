# from typing import TYPE_CHECKING

import storage.cache
from trezor import io, wire
from trezor.messages import FatfsWrite, Success


async def fwrite(ctx: wire.Context, msg: FatfsWrite) -> Success:
    if storage.cache.current_open_file is None:
        raise wire.ProcessError("Open file first")
    try:
        storage.cache.current_open_file.write(msg.data)
        storage.cache.current_open_file.sync()
        return Success(message="Write file succeeded")
    except io.fatfs.FatFSError:
        raise wire.ProcessError("Write file failed")
