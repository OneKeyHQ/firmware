from micropython import const
from typing import TYPE_CHECKING

from storage.cache import show_update_res_confirm
from trezor import io, loop, utils, wire
from trezor.crypto.hashlib import blake2s
from trezor.messages import ResourceAck, ResourceRequest, Success
from trezor.ui.layouts import confirm_update_res

if TYPE_CHECKING:
    from trezor.messages import ResourceUpdate
# Error code - 255
# FR_OK: int                   # (0) Succeeded
# FR_DISK_ERR: int             # (1) A hard error occurred in the low level disk I/O layer
# FR_INT_ERR: int              # (2) Assertion failed
# FR_NOT_READY: int            # (3) The physical drive cannot work
# FR_NO_FILE: int              # (4) Could not find the file
# FR_NO_PATH: int              # (5) Could not find the path
# FR_INVALID_NAME: int         # (6) The path name format is invalid
# FR_DENIED: int               # (7) Access denied due to prohibited access or directory full
# FR_EXIST: int                # (8) Access denied due to prohibited access
# FR_INVALID_OBJECT: int       # (9) The file/directory object is invalid
# FR_WRITE_PROTECTED: int      # (10) The physical drive is write protected
# FR_INVALID_DRIVE: int        # (11) The logical drive number is invalid
# FR_NOT_ENABLED: int          # (12) The volume has no work area
# FR_NO_FILESYSTEM: int        # (13) There is no valid FAT volume
# FR_MKFS_ABORTED: int         # (14) The f_mkfs() aborted due to any problem
# FR_TIMEOUT: int              # (15) Could not get a grant to access the volume within defined period
# FR_LOCKED: int               # (16) The operation is rejected according to the file sharing policy
# FR_NOT_ENOUGH_CORE: int      # (17) LFN working buffer could not be allocated
# FR_TOO_MANY_OPEN_FILES: int  # (18) Number of open files > FF_FS_LOCK
# FR_INVALID_PARAMETER: int    # (19) Given parameter is invalid

REQUEST_CHUNK_SIZE = const(16 * 1024)

BOOTLOADER_NAME = "bootloader.bin"

BOOTLOADER_TEMP_NAME = BOOTLOADER_NAME + ".tmp"


async def update_res(ctx: wire.Context, msg: ResourceUpdate) -> Success:
    is_update_boot = msg.file_name == BOOTLOADER_NAME
    if show_update_res_confirm(is_update_boot):
        await confirm_update_res(ctx, is_update_boot)
        msg.file_name = BOOTLOADER_TEMP_NAME
    res_size = msg.data_length
    initial_data = msg.initial_data_chunk
    if blake2s(initial_data).digest() != msg.hash:
        raise wire.DataError("Date digest is inconsistent")

    file_path = make_file_path(msg.file_name, is_update_boot)
    data_left = res_size - REQUEST_CHUNK_SIZE
    offset = REQUEST_CHUNK_SIZE
    try:
        with io.fatfs.open(file_path, "w") as f:
            f.write(initial_data)

            while data_left > 0:
                request = ResourceRequest(data_length=REQUEST_CHUNK_SIZE, offset=offset)
                ack: ResourceAck = await ctx.call(request, ResourceAck)
                data = ack.data_chunk
                digest = blake2s(data).digest()
                if digest != ack.hash:
                    raise wire.DataError("Date digest is inconsistent")
                f.write(data)
                offset += (
                    REQUEST_CHUNK_SIZE if data_left > REQUEST_CHUNK_SIZE else data_left
                )
                data_left -= REQUEST_CHUNK_SIZE
            # force refresh to disk
            f.sync()
    except BaseException as e:
        raise wire.FirmwareError(f"Failed to write file with error code {e}")
    else:
        if is_update_boot:
            try:
                size, _, _ = io.fatfs.stat(file_path)
                if size != calc_bootloader_size(msg.initial_data_chunk):
                    io.fatfs.unlink(file_path)
                    raise wire.FirmwareError("Invalid bootloader detected")

                bootloader_path = make_file_path(BOOTLOADER_NAME, True)
                try:
                    # delete the existing file
                    io.fatfs.unlink(bootloader_path)
                except BaseException:
                    # the file was not exist, ignored
                    pass
                io.fatfs.rename(file_path, bootloader_path)
            except BaseException as e:
                raise wire.FirmwareError(f"File system error {e}")
            else:
                await ctx.write(Success(message="Restarting"))
                # make sure the outgoing USB buffer is flushed
                await loop.wait(ctx.iface.iface_num() | io.POLL_WRITE)
                utils.reset()
                assert False  # this should be not reachable
        return Success(message="Success")


def make_file_path(file_name, update_boot: bool = False) -> str:
    return f"/{'boot' if update_boot else 'res'}/{file_name}"


def calc_bootloader_size(header: bytes) -> int:
    # bootloader_size = hdrlen + _codelen
    return int.from_bytes(header[4:8], "little") + int.from_bytes(
        header[12:16], "little"
    )
