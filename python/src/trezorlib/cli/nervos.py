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
@click.option("-d", "--show-display", is_flag=True) 
@click.option("-h", "--hash-type", type=int, default=1, help="Hash Type <0:data 1:type(Default)>")
@click.option("-t", "--testnet", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, show_display: bool, hash_type: int, testnet: bool) -> str:
    """Get Nervos address in hex encoding."""
    address_n = tools.parse_path(address)
    return nervos.get_address(client, address_n, show_display, hash_type, testnet)

