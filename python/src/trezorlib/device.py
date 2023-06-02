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
import time
from hashlib import blake2s
from typing import TYPE_CHECKING, Any, Callable, Optional, Sequence

from . import messages
from .exceptions import Cancelled, TrezorException
from .tools import Address, expect, session

if TYPE_CHECKING:
    from .client import TrezorClient
    from .protobuf import MessageType


RECOVERY_BACK = "\x08"  # backspace character, sent literally


@expect(messages.Success, field="message", ret_type=str)
@session
def apply_settings(
    client: "TrezorClient",
    label: Optional[str] = None,
    language: Optional[str] = None,
    use_passphrase: Optional[bool] = None,
    homescreen: Optional[bytes] = None,
    passphrase_always_on_device: Optional[bool] = None,
    auto_lock_delay_ms: Optional[int] = None,
    display_rotation: Optional[int] = None,
    safety_checks: Optional[messages.SafetyCheckLevel] = None,
    experimental_features: Optional[bool] = None,
) -> "MessageType":
    settings = messages.ApplySettings(
        label=label,
        language=language,
        use_passphrase=use_passphrase,
        homescreen=homescreen,
        passphrase_always_on_device=passphrase_always_on_device,
        auto_lock_delay_ms=auto_lock_delay_ms,
        display_rotation=display_rotation,
        safety_checks=safety_checks,
        experimental_features=experimental_features,
    )

    out = client.call(settings)
    client.refresh_features()
    return out


@expect(messages.Success, field="message", ret_type=str)
@session
def apply_flags(client: "TrezorClient", flags: int) -> "MessageType":
    out = client.call(messages.ApplyFlags(flags=flags))
    client.refresh_features()
    return out


@expect(messages.Success, field="message", ret_type=str)
@session
def change_pin(client: "TrezorClient", remove: bool = False) -> "MessageType":
    ret = client.call(messages.ChangePin(remove=remove))
    client.refresh_features()
    return ret


@expect(messages.Success, field="message", ret_type=str)
@session
def change_wipe_code(client: "TrezorClient", remove: bool = False) -> "MessageType":
    ret = client.call(messages.ChangeWipeCode(remove=remove))
    client.refresh_features()
    return ret


@expect(messages.Success, field="message", ret_type=str)
@session
def sd_protect(
    client: "TrezorClient", operation: messages.SdProtectOperationType
) -> "MessageType":
    ret = client.call(messages.SdProtect(operation=operation))
    client.refresh_features()
    return ret


@expect(messages.Success, field="message", ret_type=str)
@session
def wipe(client: "TrezorClient") -> "MessageType":
    ret = client.call(messages.WipeDevice())
    client.init_device()
    return ret


@session
def recover(
    client: "TrezorClient",
    word_count: int = 24,
    passphrase_protection: bool = False,
    pin_protection: bool = True,
    label: Optional[str] = None,
    language: str = "en-US",
    input_callback: Optional[Callable] = None,
    type: messages.RecoveryDeviceType = messages.RecoveryDeviceType.ScrambledWords,
    dry_run: bool = False,
    u2f_counter: Optional[int] = None,
) -> "MessageType":
    if client.features.model == "1" and input_callback is None:
        raise RuntimeError("Input callback required for Trezor One")

    if word_count not in (12, 18, 24):
        raise ValueError("Invalid word count. Use 12/18/24")

    if client.features.initialized and not dry_run:
        raise RuntimeError(
            "Device already initialized. Call device.wipe() and try again."
        )

    if u2f_counter is None:
        u2f_counter = int(time.time())

    msg = messages.RecoveryDevice(
        word_count=word_count, enforce_wordlist=True, type=type, dry_run=dry_run
    )

    if not dry_run:
        # set additional parameters
        msg.passphrase_protection = passphrase_protection
        msg.pin_protection = pin_protection
        msg.label = label
        msg.language = language
        msg.u2f_counter = u2f_counter

    res = client.call(msg)

    while isinstance(res, messages.WordRequest):
        try:
            assert input_callback is not None
            inp = input_callback(res.type)
            res = client.call(messages.WordAck(word=inp))
        except Cancelled:
            res = client.call(messages.Cancel())

    client.init_device()
    return res


@expect(messages.Success, field="message", ret_type=str)
@session
def reset(
    client: "TrezorClient",
    display_random: bool = False,
    strength: Optional[int] = None,
    passphrase_protection: bool = False,
    pin_protection: bool = True,
    label: Optional[str] = None,
    language: str = "en-US",
    u2f_counter: int = 0,
    skip_backup: bool = False,
    no_backup: bool = False,
    backup_type: messages.BackupType = messages.BackupType.Bip39,
) -> "MessageType":
    if client.features.initialized:
        raise RuntimeError(
            "Device is initialized already. Call wipe_device() and try again."
        )

    if strength is None:
        if client.features.model == "1":
            strength = 256
        else:
            strength = 128

    # Begin with device reset workflow
    msg = messages.ResetDevice(
        display_random=bool(display_random),
        strength=strength,
        passphrase_protection=bool(passphrase_protection),
        pin_protection=bool(pin_protection),
        language=language,
        label=label,
        u2f_counter=u2f_counter,
        skip_backup=bool(skip_backup),
        no_backup=bool(no_backup),
        backup_type=backup_type,
    )

    resp = client.call(msg)
    if not isinstance(resp, messages.EntropyRequest):
        raise RuntimeError("Invalid response, expected EntropyRequest")

    external_entropy = os.urandom(32)
    # LOG.debug("Computer generated entropy: " + external_entropy.hex())
    ret = client.call(messages.EntropyAck(entropy=external_entropy))
    client.init_device()
    return ret


@expect(messages.Success, field="message", ret_type=str)
@session
def backup(client: "TrezorClient") -> "MessageType":
    ret = client.call(messages.BackupDevice())
    client.refresh_features()
    return ret


@expect(messages.Success, field="message", ret_type=str)
def cancel_authorization(client: "TrezorClient") -> "MessageType":
    return client.call(messages.CancelAuthorization())


@expect(messages.UnlockedPathRequest, field="mac", ret_type=bytes)
def unlock_path(client: "TrezorClient", n: "Address") -> "MessageType":
    resp = client.call(messages.UnlockPath(address_n=n))

    # Cancel the UnlockPath workflow now that we have the authentication code.
    try:
        client.call(messages.Cancel())
    except Cancelled:
        return resp
    else:
        raise TrezorException("Unexpected response in UnlockPath flow")


@session
@expect(messages.Success, field="message", ret_type=str)
def reboot(client: "TrezorClient", boot: bool=True) -> "MessageType":
    return client.call(messages.RebootToBootloader() if boot else messages.RebootToBoardloader())


@expect(messages.SEPublicCert)
def se_read_cert(client: "TrezorClient") -> "MessageType":
    out = client.call(messages.ReadSEPublicCert())
    return out


@session
def upload_res(
    client: "TrezorClient",
    ext: str,
    data: bytes,
    zoomdata: bytes,
    res_type: messages.ResourceType = messages.ResourceType.WallPaper,
    nft_metadata: Optional[str] = None,
    progress_update: Callable[[int], Any] = lambda _: None,
):
    mac = blake2s(data).digest()
    timestamp = int(round(time.time() * 1000))
    file_name_no_ext = f"{'wp' if res_type == messages.ResourceType.WallPaper else 'nft'}-{mac[:4].hex()}-{str(timestamp)}"
    resp = client.call(
        messages.ResourceUpload(
            extension=ext,
            data_length=len(data),
            res_type=res_type,
            zoom_data_length=len(zoomdata),
            file_name_no_ext=file_name_no_ext,
            nft_meta_data=nft_metadata.encode() if nft_metadata else None,
        )
    )

    while isinstance(resp, messages.ResourceRequest):
        offset = resp.offset
        length = resp.data_length
        assert offset is not None
        assert length is not None
        payload = data[offset : offset + length]
        digest = blake2s(payload).digest()
        resp = client.call(messages.ResourceAck(data_chunk=payload, hash=digest))
        progress_update(length)

    while isinstance(resp, messages.ZoomRequest):
        offset = resp.offset
        length = resp.data_length
        assert offset is not None
        assert length is not None
        payload = zoomdata[offset : offset + length]
        digest = blake2s(payload).digest()
        resp = client.call(messages.ResourceAck(data_chunk=payload, hash=digest))
        progress_update(length)

    if isinstance(resp, messages.Success):
        return
    else:
        raise RuntimeError(f"Unexpected message {resp}")


DATA_CHUNK_SIZE = 16 * 1024


@session
def update_res(
    client: "TrezorClient",
    file_name: str,
    data: bytes,
    progress_update: Callable[[int], Any] = lambda _: None,
):
    data_len = len(data)
    initial_data = data[:DATA_CHUNK_SIZE]
    digest = blake2s(initial_data).digest()
    resp = client.call(
        messages.ResourceUpdate(
            file_name=file_name,
            data_length=data_len,
            initial_data_chunk=initial_data,
            hash=digest,
        )
    )
    progress_update(len(initial_data))
    while isinstance(resp, messages.ResourceRequest):
        offset = resp.offset
        length = resp.data_length
        assert offset is not None
        assert length is not None
        payload = data[offset : offset + length]
        digest = blake2s(payload).digest()
        resp = client.call(messages.ResourceAck(data_chunk=payload, hash=digest))
        progress_update(length)

    if isinstance(resp, messages.Success):
        return
    else:
        raise RuntimeError(f"Unexpected message {resp}")

@session
def list_dir(client: "TrezorClient", path_dir: str) -> Sequence[messages.FileInfo]:
    resp = client.call(messages.ListResDir(path=path_dir))
    if isinstance(resp, messages.FileInfoList):
        return resp.files
    else:
        raise RuntimeError(f"Unexpected message {resp}")


@expect(messages.Success, field="message", ret_type=str)
@session
def set_busy(client: "TrezorClient", expiry_ms: Optional[int]) -> "MessageType":
    """Sets or clears the busy state of the device.

    In the busy state the device shows a "Do not disconnect" message instead of the homescreen.
    Setting `expiry_ms=None` clears the busy state.
    """
    ret = client.call(messages.SetBusy(expiry_ms=expiry_ms))
    client.refresh_features()
    return ret

# new feautres
# Reboot
@session
def reboot(client: "TrezorClient", reboot_type:messages.RebootType) -> "MessageType":
    resp = client.call(messages.Reboot(reboot_type))
    return resp

# FirmwareUpdateEmmc
@session
def firmware_update_emmc(client: "TrezorClient", path_file: str, force_erease: bool) -> "MessageType":
    msg = messages.FirmwareUpdateEmmc(path=path_file, force_erease=force_erease)
    resp = client.call(msg)
    return resp

# EmmcPathInfo
@session
def emmc_path_info(client: "TrezorClient", path: str) -> "MessageType":
    resp = client.call(messages.EmmcPathInfo(path=path))
    return resp

# EmmcFixPermission
@session
def emmc_fix_permission(client: "TrezorClient") -> "MessageType":
    resp = client.call(messages.EmmcFixPermission())
    return resp

# EmmcFileRead
@session
def emmc_file_read(client: "TrezorClient", path_file: str, len:int, offset: int, ui_percentage:int) -> "MessageType":
    msg_file = messages.EmmcFile(path=path_file, len=len, offset=offset, data=None)
    msg = messages.EmmcFileRead(file=msg_file, ui_percentage=ui_percentage)
    resp = client.call(msg)
    return resp

# EmmcFileWrite
@session
def emmc_file_write(client: "TrezorClient", path_file: str, len:int, offset: int, data: bytes, overwrite: bool, append: bool, ui_percentage:int) -> "MessageType":
    msg_file = messages.EmmcFile(path=path_file, len=len, offset=offset, data=data)
    msg = messages.EmmcFileWrite(file=msg_file, overwrite=overwrite, append=append, ui_percentage=ui_percentage)
    resp = client.call(msg)
    return resp

# EmmcFileDelete
@session
def emmc_file_delete(client: "TrezorClient", path_file: str) -> "MessageType":
    resp = client.call(messages.EmmcFileDelete(path=path_file))
    return resp

# EmmcDirList
@session
def emmc_dir_list(client: "TrezorClient", path_dir: str) -> "MessageType":
    resp = client.call(messages.EmmcDirList(path=path_dir))
    return resp

# EmmcDirMake
@session
def emmc_dir_make(client: "TrezorClient", path_dir: str) -> "MessageType":
    resp = client.call(messages.EmmcDirMake(path=path_dir))
    return resp

# EmmcDirRemove
@session
def emmc_dir_remove(client: "TrezorClient", path_dir: str) -> "MessageType":
    resp = client.call(messages.EmmcDirRemove(path=path_dir))
    return resp
