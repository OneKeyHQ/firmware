# This file is part of the OneKey project, https://onekey.so/
#
# Copyright (C) 2021 OneKey Team <core@onekey.so>
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <http://www.gnu.org/licenses/>.

from typing import TYPE_CHECKING

import click

from .. import polkadot, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient

PATH_HELP = "BIP-32 path to key, e.g. m/44'/354'/0'/0'/0'"


@click.group(name="polkadot")
def cli():
    """Polkadot commands."""


@cli.command()
@click.option(
    "-n",
    "--address",
    required=False,
    help=PATH_HELP,
    default=polkadot.DEFAULT_BIP32_PATH,
)
@click.option("-p", "--prefix", type=int, help="SS58 prefix, e.g. Polkadot is 0")
# https://github.com/paritytech/ss58-registry/blob/main/ss58-registry.json
@click.option("-N", "--network", help="Network Name")
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(
    client: "TrezorClient", address: str, prefix: int, network: str, show_display: bool
):
    """Get polkadot address."""
    address_n = tools.parse_path(address)
    return polkadot.get_address(client, address_n, prefix, network, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-N", "--network", help="Network Name")
@click.argument("message")
@with_client
def sign_raw_transaction(
    client: "TrezorClient", address: str, message: str, network: str
):
    """Sign a hex-encoded transaction ."""
    address_n = tools.parse_path(address)
    ret = polkadot.sign_tx(client, address_n, bytes.fromhex(message), network)
    output = {
        "signature": "0x%s" % ret.signature.hex(),
    }
    return output
