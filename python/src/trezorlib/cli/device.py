# This file is part of the Trezor project.
#
# Copyright (C) 2012-2022 SatoshiLabs and contributors
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the License along with this library.
# If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.

import os
import sys
import re
import time
from typing import TYPE_CHECKING, Optional, Sequence

import click

from .. import debuglink, device, exceptions, messages, ui
from . import ChoiceType, with_client

if TYPE_CHECKING:
    from ..client import TrezorClient
    from . import TrezorConnection
    from ..protobuf import MessageType

RECOVERY_TYPE = {
    "scrambled": messages.RecoveryDeviceType.ScrambledWords,
    "matrix": messages.RecoveryDeviceType.Matrix,
}

BACKUP_TYPE = {
    "single": messages.BackupType.Bip39,
    "shamir": messages.BackupType.Slip39_Basic,
    "advanced": messages.BackupType.Slip39_Advanced,
}

SD_PROTECT_OPERATIONS = {
    "on": messages.SdProtectOperationType.ENABLE,
    "off": messages.SdProtectOperationType.DISABLE,
    "refresh": messages.SdProtectOperationType.REFRESH,
}

RESOURCE_UPLOAD_PURPOSE = {
    "wallpaper": messages.ResourceType.WallPaper,
    "nft": messages.ResourceType.Nft,
}
BL_REBOOT_TYPE = {
    "firmware": messages.RebootType.Normal,
    "boardloader": messages.RebootType.Boardloader,
    "bootloader": messages.RebootType.BootLoader,
    }


@click.group(name="device")
def cli() -> None:
    """Device management commands - setup, recover seed, wipe, etc."""


@cli.command()
@with_client
def self_test(client: "TrezorClient") -> str:
    """Perform a factory self-test.

    Only available on PRODTEST firmware.
    """
    return debuglink.self_test(client)


@cli.command()
@click.option(
    "-b",
    "--bootloader",
    help="Wipe device in bootloader mode. This also erases the firmware.",
    is_flag=True,
)
@with_client
def wipe(client: "TrezorClient", bootloader: bool) -> str:
    """Reset device to factory defaults and remove all private data."""
    if bootloader:
        if not client.features.bootloader_mode:
            click.echo("Please switch your device to bootloader mode.")
            sys.exit(1)
        else:
            click.echo("Wiping user data and firmware!")
    else:
        if client.features.bootloader_mode:
            click.echo(
                "Your device is in bootloader mode. This operation would also erase firmware."
            )
            click.echo(
                'Specify "--bootloader" if that is what you want, or disconnect and reconnect device in normal mode.'
            )
            click.echo("Aborting.")
            sys.exit(1)
        else:
            click.echo("Wiping user data!")

    try:
        return device.wipe(client)
    except exceptions.TrezorFailure as e:
        click.echo("Action failed: {} {}".format(*e.args))
        sys.exit(3)


@cli.command()
@click.option("-m", "--mnemonic", multiple=True)
@click.option("-p", "--pin", default="")
@click.option("-r", "--passphrase-protection", is_flag=True)
@click.option("-l", "--label", default="")
@click.option("-i", "--ignore-checksum", is_flag=True)
@click.option("-s", "--slip0014", is_flag=True)
@click.option("-b", "--needs-backup", is_flag=True)
@click.option("-n", "--no-backup", is_flag=True)
@with_client
def load(
    client: "TrezorClient",
    mnemonic: Sequence[str],
    pin: str,
    passphrase_protection: bool,
    label: str,
    ignore_checksum: bool,
    slip0014: bool,
    needs_backup: bool,
    no_backup: bool,
) -> str:
    """Upload seed and custom configuration to the device.

    This functionality is only available in debug mode.
    """
    if slip0014 and mnemonic:
        raise click.ClickException("Cannot use -s and -m together.")

    if slip0014:
        mnemonic = [" ".join(["all"] * 12)]
        if not label:
            label = "SLIP-0014"

    try:
        return debuglink.load_device(
            client,
            mnemonic=list(mnemonic),
            pin=pin,
            passphrase_protection=passphrase_protection,
            label=label,
            language="en-US",
            skip_checksum=ignore_checksum,
            needs_backup=needs_backup,
            no_backup=no_backup,
        )
    except exceptions.TrezorFailure as e:
        if e.code == messages.FailureType.UnexpectedMessage:
            raise click.ClickException(
                "Unrecognized message. Make sure your Trezor is using debug firmware."
            )
        else:
            raise


@cli.command()
@click.option("-w", "--words", type=click.Choice(["12", "18", "24"]), default="24")
@click.option("-e", "--expand", is_flag=True)
@click.option("-p", "--pin-protection", is_flag=True)
@click.option("-r", "--passphrase-protection", is_flag=True)
@click.option("-l", "--label")
@click.option("-u", "--u2f-counter", default=None, type=int)
@click.option(
    "-t", "--type", "rec_type", type=ChoiceType(RECOVERY_TYPE), default="scrambled"
)
@click.option("-d", "--dry-run", is_flag=True)
@with_client
def recover(
    client: "TrezorClient",
    words: str,
    expand: bool,
    pin_protection: bool,
    passphrase_protection: bool,
    label: Optional[str],
    u2f_counter: int,
    rec_type: messages.RecoveryDeviceType,
    dry_run: bool,
) -> "MessageType":
    """Start safe recovery workflow."""
    if rec_type == messages.RecoveryDeviceType.ScrambledWords:
        input_callback = ui.mnemonic_words(expand)
    else:
        input_callback = ui.matrix_words
        click.echo(ui.RECOVERY_MATRIX_DESCRIPTION)

    return device.recover(
        client,
        word_count=int(words),
        passphrase_protection=passphrase_protection,
        pin_protection=pin_protection,
        label=label,
        u2f_counter=u2f_counter,
        language="en-US",
        input_callback=input_callback,
        type=rec_type,
        dry_run=dry_run,
    )


@cli.command()
@click.option("-e", "--show-entropy", is_flag=True)
@click.option("-t", "--strength", type=click.Choice(["128", "192", "256"]))
@click.option("-r", "--passphrase-protection", is_flag=True)
@click.option("-p", "--pin-protection", is_flag=True)
@click.option("-l", "--label")
@click.option("-u", "--u2f-counter", default=0)
@click.option("-s", "--skip-backup", is_flag=True)
@click.option("-n", "--no-backup", is_flag=True)
@click.option("-b", "--backup-type", type=ChoiceType(BACKUP_TYPE), default="single")
@with_client
def setup(
    client: "TrezorClient",
    show_entropy: bool,
    strength: Optional[int],
    passphrase_protection: bool,
    pin_protection: bool,
    label: Optional[str],
    u2f_counter: int,
    skip_backup: bool,
    no_backup: bool,
    backup_type: messages.BackupType,
) -> str:
    """Perform device setup and generate new seed."""
    if strength:
        strength = int(strength)

    if (
        backup_type == messages.BackupType.Slip39_Basic
        and messages.Capability.Shamir not in client.features.capabilities
    ) or (
        backup_type == messages.BackupType.Slip39_Advanced
        and messages.Capability.ShamirGroups not in client.features.capabilities
    ):
        click.echo(
            "WARNING: Your Trezor device does not indicate support for the requested\n"
            "backup type. Traditional single-seed backup may be generated instead."
        )

    return device.reset(
        client,
        display_random=show_entropy,
        strength=strength,
        passphrase_protection=passphrase_protection,
        pin_protection=pin_protection,
        label=label,
        language="en-US",
        u2f_counter=u2f_counter,
        skip_backup=skip_backup,
        no_backup=no_backup,
        backup_type=backup_type,
    )


@cli.command()
@with_client
def backup(client: "TrezorClient") -> str:
    """Perform device seed backup."""
    return device.backup(client)


@cli.command()
@click.argument("operation", type=ChoiceType(SD_PROTECT_OPERATIONS))
@with_client
def sd_protect(
    client: "TrezorClient", operation: messages.SdProtectOperationType
) -> str:
    """Secure the device with SD card protection.

    When SD card protection is enabled, a randomly generated secret is stored
    on the SD card. During every PIN checking and unlocking operation this
    secret is combined with the entered PIN value to decrypt data stored on
    the device. The SD card will thus be needed every time you unlock the
    device. The options are:

    \b
    on - Generate SD card secret and use it to protect the PIN and storage.
    off - Remove SD card secret protection.
    refresh - Replace the current SD card secret with a new one.
    """
    if client.features.model == "1":
        raise click.ClickException("Trezor One does not support SD card protection.")
    return device.sd_protect(client, operation)


@cli.command()
@click.pass_obj
def reboot_to_bootloader(obj: "TrezorConnection") -> str:
    """Reboot device into bootloader mode.
    """
    # avoid using @with_client because it closes the session afterwards,
    # which triggers double prompt on device
    with obj.client_context() as client:
        return device.reboot(client)

@cli.command()
@click.pass_obj
def reboot_to_boardloader(obj: "TrezorConnection") -> str:
    """Reboot device into boardloader mode.

    Currently only supported on Trezor Model T.
    """
    # avoid using @with_client because it closes the session afterwards,
    # which triggers double prompt on device
    with obj.client_context() as client:
        if client.features.model != "T":
            click.echo(f"Reboot to boardloader is not support on OneKey {client.features.model}")
        return device.reboot(client, False)


@cli.command()
@click.pass_obj
def se_read_cert(obj: "TrezorConnection") -> bytes:
    """Get device se cert.

    Used in device verify.
    """
    with obj.client_context() as client:
        cert_bytes = device.se_read_cert(client).public_cert
        return cert_bytes


@cli.command()
# fmt: off
@click.option("-f", "--fullpath", help="The full path of the file to upload")
@click.option("-z", "--zoompath", help="The zoom file of the image to upload")
@click.option("-p", "--purpose", type=ChoiceType(RESOURCE_UPLOAD_PURPOSE), default="wallpaper", help="The upload file used for")
@click.option("-m", "--metadata", help="the metadata of the nft, a json string include header, subheader, network and owner fields")
# fmt: on
@with_client
def upload_res(
    client: "TrezorClient",
    fullpath: str,
    zoompath: str,
    purpose: messages.ResourceType,
    metadata: str
) -> None:
    """Upload wallpaper/nft to device."""
    if fullpath:
        ext = fullpath.split(".")[-1]
        with open(zoompath, "rb") as f:
            zoomdata = f.read()
        with open(fullpath, "rb") as f:
            data = f.read()
        try:
            click.echo("Please confirm the action on your Trezor device")

            click.echo("Uploading...\r", nl=False)
            with click.progressbar(
                label="Uploading", length=len(data) + len(zoomdata), show_eta=False
            ) as bar:
                device.upload_res(
                    client,
                    ext,
                    data,
                    zoomdata,
                    progress_update=bar.update,
                    res_type=purpose,
                    nft_metadata=metadata if metadata else None,
                )
        except exceptions.Cancelled:
            click.echo("Upload aborted on device.")
        except exceptions.TrezorException as e:
            click.echo(f"Upload failed: {e}")
            sys.exit(3)


@cli.command()
# fmt: off
@click.option("-f", "--fullpath", help="The full path of the file to update")
# fmt: on
@with_client
def update_res(
    client: "TrezorClient",
    fullpath: str,
) -> None:
    """Update internal static resource(internal icons)."""
    if fullpath:
        file_name = fullpath.split("/")[-1]
        with open(fullpath, "rb") as f:
            data = f.read()
        try:
            click.echo("Uploading...\r", nl=False)
            with click.progressbar(
                label="Uploading", length=len(data), show_eta=False
            ) as bar:
                device.update_res(client, file_name, data, progress_update=bar.update)
        except exceptions.Cancelled:
            click.echo("Upload aborted on device.")
        except exceptions.TrezorException as e:
            click.echo(f"Update failed: {e}")
            sys.exit(3)

@cli.command()
# fmt: off
@click.option("-p", "--path_dir", help="The path of dir to enum")
# fmt: on
@with_client
def list_dir(client: "TrezorClient", path_dir: str) -> None:
    files_info = device.list_dir(client, path_dir)
    for info in files_info:
        click.echo(f"file_name {info.name} with size {info.size} bytes")

@cli.command()
@click.argument("enable", type=ChoiceType({"on": True, "off": False}), required=False)
@click.option(
    "-e",
    "--expiry",
    type=int,
    help="Dialog expiry in seconds.",
)
@with_client
def set_busy(
    client: "TrezorClient", enable: Optional[bool], expiry: Optional[int]
) -> str:
    """Show a "Do not disconnect" dialog."""
    if enable is False:
        return device.set_busy(client, None)

    if expiry is None:
        raise click.ClickException("Missing option '-e' / '--expiry'.")

    if expiry <= 0:
        raise click.ClickException(
            f"Invalid value for '-e' / '--expiry': '{expiry}' is not a positive integer."
        )

    return device.set_busy(client, expiry * 1000)

# new feautres only available in bootloader

# helper functions
def header_printer(header:str) -> None:
    print('====================================')
    print(header)
    print('------------------------------------')

def print_buffer(buffer:bytes) -> None:
    print(''.join(format(x, '02x') for x in buffer))

def parse_human_readable_size(size:str):

    # units = {"B": 1, "KB": 10**3, "MB": 10**6, "GB": 10**9, "TB": 10**12} #Unix
    units = {"B": 1, "KB": 2**10, "MB": 2**20, "GB": 2**30, "TB": 2**40} #Windows

    match_result = re.search(r'(\d*)\s{0,1}(\D{0,2})', size)

    if(match_result == None):
        raise RuntimeError(f"{size} is not a valid size, format error!")

    number:float = float(match_result.group(1))
    if(match_result.group(2) != ''):
        try:
            unit_factor:float = float(units[match_result.group(2)])
        except KeyError:
            raise RuntimeError(f"{size} is not a valid size, unit error!")
    else:
        unit_factor:float = 1

    return int(number*unit_factor)

# settings
max_chunk_size:str = "10MB"
default_chunk_size:str = "256KB"
LINE_UP = '\033[1A'
LINE_CLEAR = '\x1b[2K'

# EmmcPathInfo
@cli.command()
# fmt: off
@click.option("-t", "--reboot_type", type=ChoiceType(BL_REBOOT_TYPE), default="firmware", help="Reboot type")
# fmt: on
@with_client
def bl_reboot(client: "TrezorClient", reboot_type: messages.RebootType) -> str:
    result = device.bl_reboot(client, reboot_type=reboot_type)
    return result
# FirmwareUpdateEmmc
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True, help="Remote firmware file path (e.g. 0:fw.bin)")
@click.option("-f", "--force_erease", required=False, is_flag=True, help="Force erease and factory reset the device")
# fmt: on
@with_client
def firmware_update_emmc(client: "TrezorClient",path:str, force_erease:bool) -> None:
    result = device.firmware_update_emmc(client, path_file=path, force_erease=force_erease)
    if isinstance(result, messages.Success):
        print(result.message)
    elif isinstance(result, messages.Failure):
        print(result.code)
        print(result.message)
    else:
        raise RuntimeError(f"Unexpected message {result}")


# EmmcFixPermission
@cli.command()
@with_client
def emmc_fix_permission(client: "TrezorClient") -> None:
    result = device.emmc_fix_permission(client)
    if isinstance(result, messages.Success):
        print(result.message)
    elif isinstance(result, messages.Failure):
        print(result.code)
        print(result.message)
    else:
        raise RuntimeError(f"Unexpected message {result}")

# EmmcPathInfo
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True)
# fmt: on
@with_client
def emmc_path_info(client: "TrezorClient", path: str) -> None:
    result = device.emmc_path_info(client, path)
    print(result)

# EmmcFileRead
@cli.command()
# fmt: off
@click.option("-l", "--local", required=True, help="Local file path. (e.g. /home/xxx/test.bin)")
@click.option("-r", "--remote", required=True, help="Remote file path. (e.g. 0:test.bin)")
@click.option("-o", "--offset", required=False, default=0, help="Offset")
@click.option("-s", "--len", required=False, default=-1, help="len to read/write at offset")
@click.option("-a", "--append", required=False, is_flag=True, help="Append to LOCAL file")
@click.option("-f", "--force", required=False, is_flag=True, help="Force overwrite LOCAL file")
# fmt: on
@with_client
def emmc_file_read_lowlevel(client: "TrezorClient", local: str, remote: str, offset: int, len: int, append:bool, force: bool) -> None:

    if len < 0: # auto len
        len = os.path.getsize(local)

    if len > parse_human_readable_size(max_chunk_size):
        raise RuntimeError(f"Max len is limited to {max_chunk_size}!")

    if append and force:
        raise RuntimeError("Both append and overwrite are enabled!")

    emmc_file = device.emmc_file_read(client, remote, len, offset, ui_percentage=None)

    file = open(local, 'wb')
    if file.seek(offset, 0) < 0:
        raise RuntimeError("File seek failed!")
    file.write(emmc_file.data)
    file.close()

    print(f'Wrote {emmc_file.processed_byte} bytes to {local} at offset {emmc_file.offset}\n')

@cli.command()
# fmt: off
@click.option("-l", "--local", required=True, help="Local file path. (e.g. /home/xxx/test.bin)")
@click.option("-r", "--remote", required=True, help="Remote file path. (e.g. 0:test.bin)")
@click.option("-f", "--force", required=False, is_flag=True, help="Force overwrite LOCAL file")
@click.option("-cs", "--chunk_size", required=False, default=f"{default_chunk_size}", help="Chunk size for deviding a big transfer")
# fmt: on
@with_client
def emmc_file_read(client: "TrezorClient", local: str, remote: str, force: bool, chunk_size:str) -> None:

    chunk_size_bytes = parse_human_readable_size(chunk_size)
    remote_file_size = device.emmc_path_info(client, path=remote).size
    processed_size = 0

    print(f'Chunk size {chunk_size_bytes}')

    if os.path.exists(local) and not force:
        raise RuntimeError(f"{local} exist and force set to false!")

    file = open(local, 'wb')

    while processed_size < remote_file_size:

        # reset line
        if processed_size > 0:
            print(LINE_UP, end=LINE_CLEAR)

        # decide how many bytes to process this turn
        if (remote_file_size - processed_size) > chunk_size_bytes:
            next_process_size = chunk_size_bytes
        else:
            next_process_size = remote_file_size - processed_size

        # ui percentage
        ui_percentage: int = int(100*((processed_size+next_process_size)/remote_file_size))

        # package and send command
        emmc_file = device.emmc_file_read(client, remote, next_process_size, processed_size, ui_percentage=ui_percentage)

        # seek and write
        file.seek(processed_size)
        file.write(emmc_file.data)

        # record size processed
        processed_size += emmc_file.processed_byte

        # display progress
        print(f'\rProgress: {round(100*(processed_size/remote_file_size),2)}% {processed_size}/{remote_file_size}\nui_percentage={ui_percentage}', end='', flush=True)

    file.close()

    print(f'\nWrote {processed_size} bytes to {local}\n')

# EmmcFileWrite
@cli.command()
# fmt: off
@click.option("-l", "--local", required=True, help="Local file path. (e.g. /home/xxx/test.bin)")
@click.option("-r", "--remote", required=True, help="Remote file path. (e.g. 0:test.bin)")
@click.option("-o", "--offset", required=False, default=0, help="Offset")
@click.option("-s", "--len", required=False, default=-1, help="len to read/write at offset")
@click.option("-a", "--append", required=False, is_flag=True, help="Append to REMOTE file")
@click.option("-f", "--force", required=False, is_flag=True, help="Force overwrite REMOTE file (only available in bootloader)")
# fmt: on
@with_client
def emmc_file_write_lowlevel(client: "TrezorClient", local: str, remote: str, offset: int, len: int, append:bool, force: bool) -> None:

    if len < 0: # auto len
        len = os.path.getsize(local)

    if len > parse_human_readable_size(max_chunk_size):
        raise RuntimeError(f"Max len is limited to {max_chunk_size}!")

    if append and force:
        raise RuntimeError("Both append and overwrite are enabled!")

    file = open(local, 'rb')
    file.seek(offset)
    data:bytes = file.read(len)
    file.close()

    emmc_file = device.emmc_file_write(client, remote, len, offset, data, overwrite=force, append=append, ui_percentage=None)

    print(f'Wrote {emmc_file.processed_byte} bytes to {emmc_file.path} at offset {emmc_file.offset}\n')

@cli.command()
# fmt: off
@click.option("-l", "--local", required=True, help="Local file path. (e.g. /home/xxx/test.bin)")
@click.option("-r", "--remote", required=True, help="Remote file path. (e.g. 0:test.bin)")
@click.option("-f", "--force", required=False, is_flag=True, help="Force overwrite REMOTE file")
@click.option("-cs", "--chunk_size", required=False, default=f"{default_chunk_size}", help="Chunk size for deviding a big transfer")
# fmt: on
@with_client
def emmc_file_write(client: "TrezorClient", local: str, remote: str, force: bool, chunk_size:str) -> None:

    chunk_size_bytes = parse_human_readable_size(chunk_size)
    local_file_size = os.path.getsize(local)
    processed_size = 0

    print(f'Chunk size {chunk_size_bytes}')

    file = open(local, 'rb')

    while processed_size < local_file_size:

        # reset line
        if processed_size > 0:
            print(LINE_UP, end=LINE_CLEAR)

        # decide how many bytes to process this turn
        if (local_file_size - processed_size) > chunk_size_bytes:
            next_process_size = chunk_size_bytes
        else:
            next_process_size = local_file_size - processed_size

        # seek and read
        file.seek(processed_size)
        data:bytes = file.read(next_process_size)

        # ui percentage
        ui_percentage: int = int(100*((processed_size+next_process_size)/local_file_size))

        # package and send command
        emmc_file = device.emmc_file_write(client, remote, next_process_size, processed_size, data, overwrite=(force and (processed_size==0)), append=(processed_size!=0), ui_percentage=ui_percentage)

        # record size processed
        processed_size += emmc_file.processed_byte

        # display progress
        print(f'\rProgress: {round(100*(processed_size/local_file_size),2)}% {processed_size}/{local_file_size}\nui_percentage={ui_percentage}', end='', flush=True)

    file.close()

    print(f'\nWrote {processed_size} bytes to {emmc_file.path}')

# EmmcFileDelete
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True)
# fmt: on
@with_client
def emmc_file_delete(client: "TrezorClient", path: str) -> None:
    device.emmc_file_delete(client, path)

# EmmcDirList
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True)
# fmt: on
@with_client
def emmc_dir_list(client: "TrezorClient", path: str) -> None:
    emmc_dir = device.emmc_dir_list(client, path)

    print("Folders:")
    for subdir in emmc_dir.child_dirs.split('\n'):
        click.echo(f'{subdir}')
    print("Files:")
    for file in emmc_dir.child_files.split('\n'):
        click.echo(f'{file}')

# EmmcDirMake
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True)
# fmt: on
@with_client
def emmc_dir_make(client: "TrezorClient", path: str) -> None:
    device.emmc_dir_make(client, path)

# EmmcDirRemove
@cli.command()
# fmt: off
@click.option("-p", "--path", required=True)
# fmt: on
@with_client
def emmc_dir_remove(client: "TrezorClient", path: str) -> None:
    device.emmc_dir_remove(client, path)
