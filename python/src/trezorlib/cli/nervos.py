#!/usr/bin/env python3

# This file is part of the Trezor project.
#
# Copyright (C) 2012-2022 Onekey and contributors
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

from typing import TYPE_CHECKING

import click

from .. import nervos, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient


PATH_HELP = "BIP-32 path, e.g. m/44'/309'/0'/0/0"


@click.group(name="nervos")
def cli():
    """Nervos commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-N", "--network", help="Network Name")
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, network: str,show_display: bool):
    """Get Nervos address for specified path."""
    address_n = tools.parse_path(address)
    res = nervos.get_address(client, address_n,network, show_display)
    return res


#
# Signing functions
#
@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP, default="m/44'/309'/0'/0/0")
@click.argument("message")
@click.option("-w", "--witness_buffer", help="witness_buffer")
@click.option("-N", "--network", help="Network Name")
@click.option("-d", "--data_length", help="data_length")
@with_client
def sign_tx(client: "TrezorClient", address: str, message:str,witness_buffer: str, network: str, data_length : int):
    """Sign a hex-encoded raw message which is the data used to calculate the bip143-like sig-hash.
    If more than one input is needed, the message should be separated by a dash (-).
    If more than one address is needed. the address should be separated by a dash (-).
    """
    address_n = tools.parse_path(address)
    message_bytes = bytes.fromhex(message)
    witness_buffer_bytes = bytes.fromhex(witness_buffer)
    data_length_int = None
    if data_length is not None:
        data_length_int = int(data_length)

    resp = nervos.sign_tx(client, address_n, message_bytes,witness_buffer_bytes,network,data_length_int)
    return resp.signature.hex()