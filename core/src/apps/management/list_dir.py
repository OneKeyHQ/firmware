from trezor import io, wire
from trezor.messages import FileInfo, FileInfoList, ListResDir


async def list_dir(ctx: wire.Context, msg: ListResDir) -> FileInfoList:
    try:
        files = []
        for size, attrs, name in io.fatfs.listdir(msg.path):
            if attrs[1] != "h":
                files.append(FileInfo(name=name, size=size))
    except io.fatfs.FatFSError as e:
        raise wire.DataError(f"Fatfs error {e}")

    return FileInfoList(files=files)
