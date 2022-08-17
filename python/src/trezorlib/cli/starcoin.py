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

from .. import starcoin, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient


PATH_HELP = "BIP-32 path, e.g. m/44'/101010'/0'/0'/0'"


@click.group(name="starcoin")
def cli():
    """Starcoin commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient", address: str, show_display: bool):
    """Get Starcoin address for specified path."""
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
