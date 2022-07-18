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
import json
from typing import TYPE_CHECKING, TextIO

import click

from .. import messages, protobuf, tools, tron
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient


PATH_HELP = "BIP-32 path to key, e.g. m/44'/195'/0'/0/0"


@click.group(name="tron")
def cli() -> None:
    """Tron Chain commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, show_display: bool):
    """Get Tron address for specified path."""
    address_n = tools.parse_path(address)
    return tron.get_address(client, address_n, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def sign_message(
    client: "TrezorClient",
    address: str,
    message: str,
) -> dict:
    """Sign message with Tron address."""
    address_n = tools.parse_path(address)
    ret = tron.sign_message(client, address_n, message)
    output = {
        "message": message,
        "address": ret.address,
        "signature": ret.signature.hex(),
    }
    return output


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option(
    "-f",
    "--file",
    type=click.File("r"),
    default="-",
    help="Transaction file in JSON format (byte fields should be hexlified)",
)
@with_client
def sign_tx(
    client: "TrezorClient",
    address: str,
    file: TextIO,
):
    """Sign tron transaction.
    Transaction must be provided as a JSON file.
    """
    address_n = tools.parse_path(address)
    msg = json.load(file)
    msg_proto = protobuf.dict_to_proto(messages.TronSignTx, msg)
    return tron.sign_tx(client, address_n, msg_proto)
