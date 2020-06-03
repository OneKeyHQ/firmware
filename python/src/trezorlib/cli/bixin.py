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


import click

from .. import debuglink, device
from . import with_client


@click.group(name="bixin")
def cli():
    """Device management commands - setup, recover seed, wipe, etc."""


@cli.command()
@with_client
def self_test(client):
    """Perform a self-test."""
    return debuglink.self_test(client)


@cli.command()
@with_client
def backup(client):
    """Perform device seed backup."""
    ret = device.se_backup(client)
    return "data: {}".format(ret.hex())


@cli.command()
@click.argument("hex_data")
@click.argument("language")
@click.argument("label")
@click.argument("passphrase_protection")
@with_client
def restore(
    client, hex_data, language="en-US", label="BiXin Key", passphrase_protection=True
):
    """Perform device seed restore."""
    return device.se_restore(
        client,
        data=hex_data,
        language=language,
        label=label,
        passphrase_protection=bool(passphrase_protection),
    )


@cli.command()
@click.argument("data")
@with_client
def verify(client, data):
    """Perform device verify."""
    from hashlib import sha256

    digest = sha256(data.encode("utf-8")).digest()
    ret = device.se_verify(client, digest)
    return "cert: {} \n signature: {}".format(ret.cert.hex(), ret.signature.hex())


@cli.command()
@click.option("-p", "--no-pin", type=bool, help="no pin")
@click.option("-c", "--no-confirm", type=bool, help="no confirm")
@click.option("-l", "--pay-limit", type=int)
@click.option("-t", "--pay-times", type=int, default=10, help="pay times")
@with_client
def free(client, no_pin, no_confirm, pay_limit, pay_times):
    """set pay no pin or no confirm"""
    return device.apply_settings(
        client,
        fastpay_pin=no_pin,
        fastpay_confirm=no_confirm,
        fastpay_money_limit=pay_limit,
        fastpay_times=pay_times,
    )
