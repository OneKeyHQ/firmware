# This file is part of the Trezor project.
#
# Copyright (C) 2012-2019 SatoshiLabs and contributors
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

from . import messages
from .exceptions import Cancelled
from .tools import expect, session

RECOVERY_BACK = "\x08"  # backspace character, sent literally


@expect(messages.Success, field="message")
def apply_settings(
    client,
    label=None,
    language=None,
    use_passphrase=None,
    homescreen=None,
    auto_lock_delay_ms=None,
    display_rotation=None,
    passphrase_always_on_device: bool = None,
    fastpay_pin: bool = None,
    use_ble: bool = None,
    use_se: bool = None,
    is_bixinapp: bool = None,
    fastpay_confirm: bool = None,
    fastpay_money_limit: int = None,
    fastpay_times: int = None,
):
    settings = messages.ApplySettings()
    if label is not None:
        settings.label = label
    if language:
        settings.language = language
    if use_passphrase is not None:
        settings.use_passphrase = use_passphrase
    if homescreen is not None:
        settings.homescreen = homescreen
    if passphrase_always_on_device is not None:
        settings.passphrase_always_on_device = passphrase_always_on_device
    if auto_lock_delay_ms is not None:
        settings.auto_lock_delay_ms = auto_lock_delay_ms
    if display_rotation is not None:
        settings.display_rotation = display_rotation
    if use_ble is not None:
        settings.use_ble = use_ble
    if use_se is not None:
        settings.use_se = use_se
    if is_bixinapp is not None:
        settings.is_bixinapp = is_bixinapp
    if fastpay_pin is not None:
        settings.fastpay_pin = fastpay_pin
    if fastpay_confirm is not None:
        settings.fastpay_confirm = fastpay_confirm
    if fastpay_money_limit is not None:
        settings.fastpay_money_limit = fastpay_money_limit
    if fastpay_times is not None:
        settings.fastpay_times = fastpay_times

    out = client.call(settings)
    client.init_device()  # Reload Features
    return out


@expect(messages.Success, field="message")
def apply_flags(client, flags):
    out = client.call(messages.ApplyFlags(flags=flags))
    client.init_device()  # Reload Features
    return out


@expect(messages.Success, field="message")
def change_pin(client, remove=False):
    ret = client.call(messages.ChangePin(remove=remove))
    client.init_device()  # Re-read features
    return ret


@expect(messages.Success, field="message")
def change_wipe_code(client, remove=False):
    ret = client.call(messages.ChangeWipeCode(remove=remove))
    client.init_device()  # Re-read features
    return ret


@expect(messages.Success, field="message")
def sd_protect(client, operation):
    ret = client.call(messages.SdProtect(operation=operation))
    client.init_device()
    return ret


@expect(messages.Success, field="message")
def wipe(client):
    ret = client.call(messages.WipeDevice())
    client.init_device()
    return ret


@expect(messages.Success, field="message")
def reboot(client):
    ret = client.call(messages.BixinReboot())
    return ret


def recover(
    client,
    word_count=24,
    passphrase_protection=False,
    pin_protection=True,
    label=None,
    language="en-US",
    input_callback=None,
    type=messages.RecoveryDeviceType.ScrambledWords,
    dry_run=False,
    u2f_counter=None,
):
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
            inp = input_callback(res.type)
            res = client.call(messages.WordAck(word=inp))
        except Cancelled:
            res = client.call(messages.Cancel())

    client.init_device()
    return res


@expect(messages.Success, field="message")
@session
def reset(
    client,
    display_random=False,
    strength=None,
    passphrase_protection=False,
    pin_protection=True,
    label=None,
    language="en-US",
    u2f_counter=0,
    skip_backup=False,
    no_backup=False,
    backup_type=messages.BackupType.Bip39,
):
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


@expect(messages.Success, field="message")
def backup(client):
    ret = client.call(messages.BackupDevice())
    return ret


@expect(messages.BixinBackupAck, field="data")
def se_backup(client):
    ret = client.call(messages.BixinBackupRequest())
    return ret


@expect(messages.Success, field="message")
def se_restore(
    client, data, language="en-US", label="BiXin Key", passphrase_protection=True
):
    ret = client.call(
        messages.BixinRestoreRequest(
            data=bytes.fromhex(data),
            language=language,
            label=label,
            passphrase_protection=bool(passphrase_protection),
        )
    )
    return ret


@expect(messages.BixinVerifyDeviceAck)
def se_verify(client, data):
    ret = client.call(messages.BixinVerifyDeviceRequest(data=data))
    return ret
