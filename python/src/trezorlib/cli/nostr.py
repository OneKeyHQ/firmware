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

import json
from typing import TYPE_CHECKING, TextIO

import click

from .. import messages, protobuf, tools, nostr
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient


PATH_HELP = "BIP-32 path, e.g.  m/44'/1237'/<account>'/0/0"


@click.group(name="nostr")
def cli():
    """Nostr commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_public_key(client: "TrezorClient", address: str, show_display: bool) -> str:
    """Get nostr address in hex encoding."""
    address_n = tools.parse_path(address)
    return nostr.get_public_key(client, address_n, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option(
    "-f",
    "--file",
    type=click.File("r"),
    default="-",
    help="Transaction file in JSON format",
)
@with_client
def sign_tx(client: "TrezorClient", address: str, file: TextIO):
    """Sign Tron transaction."""
    address_n = tools.parse_path(address)
    msg = json.load(file)
    message_str = json.dumps(msg, separators=(",", ":"))
    message_bytes = message_str.encode("utf-8")
    resp = nostr.sign_tx(client, address_n, message_bytes)
    print(resp)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@click.option("-p", "--pubkey", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def encrypt(client: "TrezorClient", address: str, pubkey: str, message: str, show_display: bool):
    """encrypt messages."""
    address_n = tools.parse_path(address)
    return nostr.encrypt(client, address_n, pubkey, message, show_display)


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@click.option("-p", "--pubkey", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def decrypt(client: "TrezorClient", address: str, pubkey: str, message: str, show_display: bool):
    """decrypt messages."""
    address_n = tools.parse_path(address)
    return nostr.decrypt(client, address_n, pubkey, message, show_display)

@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("hash")
@with_client
def schnorr(client: "TrezorClient", address: str, hash: str):
    """sign hash."""
    address_n = tools.parse_path(address)
    return nostr.schnorr(client, address_n, hash)
