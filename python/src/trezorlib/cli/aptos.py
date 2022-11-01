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

from .. import aptos, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient

PATH_HELP = "BIP-32 path, e.g. m/44'/637'/0'/0'/0'"


@click.group(name="aptos")
def cli():
    """Aptos commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, show_display: bool) -> str:
    """Get Aptos address in hex encoding."""
    address_n = tools.parse_path(address)
    return aptos.get_address(client, address_n, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def sign_raw_tx(client: "TrezorClient", address: str, message: str):
    """Sign a hex-encoded raw message with out prefix_bytes sha256(APTOS::RawTransaction)."""
    address_n = tools.parse_path(address)
    resp = aptos.sign_tx(client, address_n, bytes.fromhex(message))
    result = {
        "public_key": f"0x{resp.public_key.hex()}",
        "signature": f"0x{resp.signature.hex()}",
    }
    return result

@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-a", "--addr")
@click.option("-c", "--chain_id")
@click.option("-app", "--application")
@click.argument("nonce")
@click.argument("message")
@with_client
def sign_message(client: "TrezorClient", address: str, addr:str, chain_id:str, application:str, nonce: str, message: str):
    """Sign formatted message."""
    address_n = tools.parse_path(address)
    resp = aptos.sign_message(client, address_n, {
        "address": addr,
        "chain_id": chain_id,
        "application": application,
        "nonce":nonce,
        "message": message
    })
    result = {
        "address": resp.address,
        "signature": f"0x{resp.signature.hex()}",
    }
    return result
