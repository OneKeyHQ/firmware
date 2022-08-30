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

from .. import starcoin, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient

PATH_HELP = "BIP32 path. Always use hardened paths and the m/44'/101010'/0'/0'/ prefix"


@click.group(name="starcoin")
def cli():
    """Starcoin commands."""


@cli.command()
@click.option(
    "-n",
    "--address",
    required=False,
    help=PATH_HELP,
    default=starcoin.DEFAULT_BIP32_PATH,
)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, show_display: bool):
    """Get starcoin public address."""
    address_n = tools.parse_path(address)
    return starcoin.get_address(client, address_n, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_public_key(client: "TrezorClient", address: str, show_display: bool):
    """Get starcoin public key for specified path."""
    address_n = tools.parse_path(address)
    res = starcoin.get_public_key(client, address_n, show_display)
    output = {"public_key": res.public_key.hex()}
    return output


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def sign_raw_transaction(client: "TrezorClient", address: str, message: str):
    """Sign a hex-encoded transaction ."""
    address_n = tools.parse_path(address)
    ret = starcoin.sign_tx(client, address_n, bytes.fromhex(message))
    output = {
        "public_key": "0x%s" % ret.public_key.hex(),
        "signature": "0x%s" % ret.signature.hex(),
    }
    return output


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def sign_message(client: "TrezorClient", address: str, message: str) -> dict:
    """Sign message with Starcoin address."""
    address_n = tools.parse_path(address)
    res = starcoin.sign_message(client, address_n, bytes(message, encoding="utf8"))
    output = {
        "message": message,
        "public_key": res.public_key.hex(),
        "signature": res.signature.hex(),
    }
    return output


@cli.command()
@click.argument("pubkey")
@click.argument("signature")
@click.argument("message")
@with_client
def verify_message(
    client: "TrezorClient", pubkey: str, signature: str, message: str
) -> bool:
    """Verify message signed with Starcoin address."""
    return starcoin.verify_message(
        client,
        bytes.fromhex(pubkey),
        bytes.fromhex(signature),
        bytes(message, encoding="utf8"),
    )
