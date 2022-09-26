# from typing import TYPE_CHECKING

import storage.cache
from trezor import io, wire
from trezor.messages import FatfsClose, Success


async def fclose(ctx: wire.Context, msg: FatfsClose) -> Success:
    if storage.cache.current_open_file is None:
        raise wire.ProcessError("NO open files")
    try:
        storage.cache.current_open_file.close()
        return Success(message="Close file succeeded")
    except io.fatfs.FatFSError:
        raise wire.ProcessError("Close file failed")
