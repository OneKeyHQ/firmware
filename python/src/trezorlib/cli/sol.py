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


from typing import TYPE_CHECKING, Optional

import base58
import click

from .. import solana, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient
PATH_HELP = "BIP-32 path, e.g. m/44'/501'/0'/0'"
PATH_RAW_TX = "Base58 encoded transaction"


@click.group(name="sol")
def cli():
    """Solana commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(
    client: "TrezorClient", address: str, show_display: bool
) -> Optional[str]:
    """Get Solana address for specified path."""
    address_n = tools.parse_path(address)
    return solana.get_address(client, address_n, show_display).address


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--raw_tx", required=True, help=PATH_RAW_TX)
@with_client
def sign_tx(client: "TrezorClient", address: str, raw_tx: str) -> str:
    """Sign Solala transaction."""
    address_n = tools.parse_path(address)
    transaction = solana.sign_tx(client, address_n, base58.b58decode(raw_tx))
    assert transaction.signature is not None
    return transaction.signature.hex()
