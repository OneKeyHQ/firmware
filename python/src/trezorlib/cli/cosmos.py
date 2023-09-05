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

import json
from typing import TYPE_CHECKING, TextIO, Tuple

import click

from .. import cosmos, tools, messages, protobuf
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient


PATH_HELP = "BIP-32 path, e.g. m/44'/118'/0'/0/0"


@click.group(name="cosmos")
def cli():
    """Cosmos commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-h", "--hrp", help="BECH32 HRP, e.g. cosmos")
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, hrp: str, show_display: bool):
    """Get Cosmos address for specified path."""
    address_n = tools.parse_path(address)
    return cosmos.get_address(client, address_n, hrp, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option(
    "-f",
    "--file",
    type=click.File("r"),
    default="-",
    help="Transaction file in JSON format",
    required=False,
)
@click.option("-r", "--raw", help="Raw transaction in hex format", required=False)
@with_client
def sign_tx(client: "TrezorClient", address: str, file: TextIO, raw: str):
    """Sign Cosmos transaction."""
    address_n = tools.parse_path(address)
    msg = json.load(file)
    if raw:
        message_bytes = bytes.fromhex(raw)
    else:
        msg = json.load(file)
        message_str = json.dumps(msg, separators=(",", ":"), sort_keys=True)
        message_bytes = message_str.encode("utf-8")
    ret = cosmos.sign_tx(client, address_n, message_bytes)
    output = {
        "signature": "0x%s" % ret.signature.hex(),
    }
    return output
