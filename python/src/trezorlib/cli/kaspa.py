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

from typing import TYPE_CHECKING

import click

from .. import kaspa, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient

PATH_HELP = "BIP-32 paths, e.g. m/44'/111111'/0'/0/0 or m/44'/111111'/0'/0/0-m/44'/111111'/0'/0/1 if more than one address is needed."

@click.group(name="kaspa")
def cli() -> None:
    """Kaspa commands"""


#
# Address functions
#
@cli.command()
@click.option("-n", "--address", required=True, help="BIP-32 path", default="m/44'/111111'/0'/0/0")
@click.option("-p", "--prefix", help="Address prefix", default="kaspa")
@click.option("-d", "--show-display", is_flag=True)
@click.option("-s", "--schema", type=str, help="address schema (options: schnorr, ecdsa)", default="schnorr")
@with_client
def get_address(
    client: "TrezorClient",
    address: str,
    show_display: bool,
    prefix: str,
    schema: str,
) -> str:
    """Get address for specified path.
    """
    address_n = tools.parse_path(address)

    return kaspa.get_address(
        client,
        address_n,
        show_display,
        prefix=prefix,
        address_schema=schema,
    )

#
# Signing functions
#
@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP, default="m/44'/111111'/0'/0/0")
@click.option("-p", "--prefix", help="Address prefix", default="kaspa")
@click.option("-s", "--schema", type=str, help="signature schema (options: schnorr, ecdsa)", default="schnorr")
@click.argument("message")
@with_client
def sign_tx(client: "TrezorClient", address: str, message:str, prefix:str, schema:str):
    """Sign a hex-encoded raw message which is the data used to calculate the bip143-like sig-hash.

    If more than one input is needed, the message should be separated by a dash (-).
    If more than one address is needed. the address should be separated by a dash (-).
    """
    addresses = [tools.parse_path(addr) for addr in address.split("-")]
    resp = kaspa.sign_tx(client, addresses, message, prefix=prefix, schema=schema)

    signatures_str = ""
    for i, sig in enumerate(resp):
        signatures_str = signatures_str + (f"\ninput{i} signature: 0x{sig.hex()}")

    result = {
        "signatures": signatures_str
    }
    return result
