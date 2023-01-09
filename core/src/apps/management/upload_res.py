from micropython import const
from typing import TYPE_CHECKING

from storage import device
from trezor import io, wire
from trezor.crypto.hashlib import blake2s
from trezor.enums import ResourceType
from trezor.messages import ResourceAck, ResourceRequest, Success, ZoomRequest
from trezor.ui.layouts import confirm_collect_nft, confirm_set_homescreen

import ujson as json
import ure as re  # type: ignore[Import "ure" could not be resolved]

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
# FILE_PATH_COMPONENTS = (("wallpapers", "wp"), ("nfts", "nft"))
NFT_METADATA_ALLOWED_KEYS = ("header", "subheader", "network", "owner")
REQUEST_CHUNK_SIZE = const(16 * 1024)

MAX_WP_COUNTER = const(5)
MAX_NFT_COUNTER = const(24)

# a more precise version should be ^(nft|wp)-[0-9,a-f]{8}-\\d{13,}$, but micropython not support limit range {}
PATTERN = re.compile(r"^(nft|wp)-[0-9a-f]+-\d+$")


async def upload_res(ctx: wire.Context, msg: ResourceUpload) -> Success:
    res_type = msg.res_type
    res_ext = msg.extension
    res_size = msg.data_length
    res_zoom_size = msg.zoom_data_length
    if res_ext not in SUPPORTED_EXTS[res_type]:
        raise wire.DataError("Not supported resource extension")
    elif res_size >= SUPPORTED_MAX_RESOURCE_SIZE[res_ext]:
        raise wire.DataError("Data size overflow")
    if msg.file_name_no_ext:
        if PATTERN.match(msg.file_name_no_ext) is None:
            raise wire.DataError(
                "File name should follow the pattern (^(nft|wp)-[0-9a-f]{8}-\\d{13,}$)"
            )
    else:
        raise wire.DataError("File name required")
    if res_type == ResourceType.Nft:
        if msg.nft_meta_data is None:
            raise wire.DataError("NFT metadata required")
        try:
            metadata = json.loads(msg.nft_meta_data.decode("utf-8"))
        except BaseException as e:
            raise wire.DataError(f"Invalid metadata {e}")
        if any(key not in metadata.keys() for key in NFT_METADATA_ALLOWED_KEYS):
            raise wire.DataError("Invalid metadata")
    replace = False
    name_list = []
    try:
        file_counter = 0
        if res_type == ResourceType.WallPaper:
            for size, _attrs, name in io.fatfs.listdir("1:/res/wallpapers"):
                if size > 0 and name[:4] == "zoom":
                    file_counter += 1
                    name_list.append(name)
                if file_counter >= MAX_WP_COUNTER:
                    replace = True
                    break
        else:
            for size, _attrs, name in io.fatfs.listdir("1:/res/nfts/zooms"):
                if size > 0:
                    file_counter += 1
                    name_list.append(name)
                if file_counter >= MAX_NFT_COUNTER:
                    replace = True
                    break
    except BaseException as e:
        raise wire.FirmwareError(f"File system error {e}")
    file_name = msg.file_name_no_ext
    for name in name_list:
        if file_name[: file_name.rindex("-")] == name[5 : name.rindex("-")]:
            if res_type == ResourceType.WallPaper:
                old_path = "1:res/wallpapers/" + name[5:]
                new_path = f"1:/res/wallpapers/{file_name}.{res_ext}"
                old_path_zoom = f"1:res/wallpapers/{name}"
                new_path_zoom = f"1:/res/wallpapers/zoom-{file_name}.{res_ext}"
                io.fatfs.rename(old_path, new_path)
                io.fatfs.rename(old_path_zoom, new_path_zoom)
                await confirm_set_homescreen(ctx, False)
                device.set_homescreen(f"A:{new_path}")
                return Success(message="Success")
            else:
                raise wire.DataError("File already exists")
    # ask user for confirm
    if res_type == ResourceType.WallPaper:
        await confirm_set_homescreen(ctx, replace)
    elif res_type == ResourceType.Nft:
        await confirm_collect_nft(ctx, replace)

    config_path = ""
    if res_type == ResourceType.WallPaper:
        file_full_path = f"1:/res/wallpapers/{file_name}.{res_ext}"
        zoom_path = f"1:/res/wallpapers/zoom-{file_name}.{res_ext}"
    else:
        file_full_path = f"1:/res/nfts/imgs/{file_name}.{res_ext}"
        zoom_path = f"1:/res/nfts/zooms/zoom-{file_name}.{res_ext}"
        config_path = f"1:/res/nfts/desc/{file_name}.json"
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
        if res_type == ResourceType.Nft and config_path:
            with io.fatfs.open(config_path, "w") as f:
                assert msg.nft_meta_data
                f.write(msg.nft_meta_data)
                f.sync()
        if replace:
            name_list.sort(
                key=lambda name: int(name[5:].split("-")[-1][: -(len(res_ext) + 1)])
            )
            zoom_file = name_list[0]
            file_name = zoom_file[5:]
            if res_type == ResourceType.WallPaper:
                io.fatfs.unlink(f"1:/res/wallpapers/{zoom_file}")
                io.fatfs.unlink(f"1:/res/wallpapers/{file_name}")
            else:
                io.fatfs.unlink(f"1:/res/nfts/zooms/{zoom_file}")
                io.fatfs.unlink(f"1:/res/nfts/imgs/{file_name}")
                config_name = file_name[: -(len(res_ext) + 1)]
                io.fatfs.unlink(f"1:/res/nfts/desc/{config_name}.json")
        elif res_type == ResourceType.WallPaper:
            device.increase_wp_cnts()

    except BaseException as e:
        raise wire.FirmwareError(f"Failed to write file with error code {e}")
    if res_type == ResourceType.WallPaper:
        device.set_homescreen(f"A:{file_full_path}")
    return Success(message="Success")
