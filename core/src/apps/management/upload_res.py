from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import io, wire
from trezor.crypto.hashlib import blake2s
from trezor.enums import ResourceType
from trezor.messages import ResourceAck, ResourceRequest, Success, ZoomRequest
from trezor.ui.layouts import confirm_set_homescreen

if TYPE_CHECKING:
    from trezor.messages import ResourceUpload
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

SUPPORTED_EXTS = (("jpg", "png", "jpeg"), ("jpg", "jpeg", "png", "mp4"))

SUPPORTED_MAX_RESOURCE_SIZE = {
    "jpg": const(1024 * 1024),
    "jpeg": const(1024 * 1024),
    "png": const(1024 * 1024),
    "mp4": const(10 * 1024 * 1024),
}
FILE_PATH_COMPONENTS = (("wallpapers", "wp"), ("nfts", "nft"))

REQUEST_CHUNK_SIZE = const(16 * 1024)

MAX_WP_COUNTER = const(5)


async def upload_res(ctx: wire.Context, msg: ResourceUpload) -> Success:
    res_type = msg.res_type
    res_ext = msg.extension
    res_size = msg.data_length
    res_zoom_size = msg.zoom_data_length
    if res_ext not in SUPPORTED_EXTS[res_type]:
        raise wire.DataError("Not supported resource extension")
    elif res_size >= SUPPORTED_MAX_RESOURCE_SIZE[res_ext]:
        raise wire.DataError("Data size overflow")
    replace = False
    wallpapers = []
    try:
        file_counter = 0
        for size, _attrs, name in io.fatfs.listdir("1:/res/wallpapers"):
            if size > 0 and name[:4] == "zoom":
                file_counter += 1
                wallpapers.append(name)
            if file_counter >= MAX_WP_COUNTER:
                replace = True
                break
    except BaseException as e:
        raise wire.FirmwareError(f"File system error {e}")
    # ask user for confirm
    if res_type == ResourceType.WallPaper:
        await confirm_set_homescreen(ctx, replace)

    # random_data = hexlify(blake2s(random.bytes(32)).digest()[:4]).decode()
    file_id = device.get_wp_cnts()
    component = FILE_PATH_COMPONENTS[res_type]
    path_dir = f"1:/res/{component[0]}"
    file_name = f"{component[1]}-{file_id}.{res_ext}"
    file_full_path = f"{path_dir}/{file_name}"
    try:
        with io.fatfs.open(file_full_path, "w") as f:
            data_left = res_size
            offset = 0
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

        zoom_path = f"{path_dir}/zoom-{file_name}"
        with io.fatfs.open(zoom_path, "w") as f:
            data_left = res_zoom_size
            offset = 0
            while data_left > 0:
                request = ZoomRequest(data_length=REQUEST_CHUNK_SIZE, offset=offset)
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
        if replace:
            wallpapers.sort(
                key=lambda name: int(name[5:].split("-")[1][: -(len(res_ext) + 1)])
            )
            zoom_file = wallpapers[0]
            wallpaper = zoom_file[5:]
            io.fatfs.unlink(f"1:/res/wallpapers/{zoom_file}")
            io.fatfs.unlink(f"1:/res/wallpapers/{wallpaper}")
        device.increase_wp_cnts()
    except BaseException as e:
        raise wire.FirmwareError(f"Failed to write file with error code {e}")
    if res_type == ResourceType.WallPaper:
        device.set_homescreen(f"A:{file_full_path}")
    return Success(message="Success")
