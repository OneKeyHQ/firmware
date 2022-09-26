# from typing import TYPE_CHECKING

import storage.cache
from trezor import io, wire
from trezor.messages import FatfsRead, FatfsReadData


async def fread(ctx: wire.Context, msg: FatfsRead) -> FatfsReadData:
    if storage.cache.current_open_file is None:
        raise wire.ProcessError("Open file first")
    try:
        data_buffer = bytearray(msg.num_to_read)
        data_len = storage.cache.current_open_file.read(data_buffer)
        return FatfsReadData(data=data_buffer[:data_len])
    except io.fatfs.FatFSError:
        raise wire.ProcessError("Read file failed")
