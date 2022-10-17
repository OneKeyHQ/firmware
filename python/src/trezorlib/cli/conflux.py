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
from typing import TYPE_CHECKING, Dict, TextIO, Tuple

import click

from .. import conflux, tools
from . import with_client

if TYPE_CHECKING:
    from ..client import TrezorClient


PATH_HELP = "BIP-32 path to key, e.g. m/44'/503'/0'/0/0"


@click.group(name="conflux")
def cli() -> None:
    """Conflux Chain commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-c", "--chain-id", type=int, default=1029, help="Conflux chain id")
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(
    client: "TrezorClient", address: str, chain_id: int, show_display: bool
) -> str:
    """Get Conflux address for specified path."""
    address_n = tools.parse_path(address)
    return conflux.get_address(client, address_n, chain_id, show_display)


@cli.command()
@click.argument("file", type=click.File("r"))
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-f", "--file", "_ignore", is_flag=True, hidden=True, expose_value=False)
@with_client
def sign_tx(
    client: "TrezorClient", address: str, file: TextIO
) -> Tuple[int, bytes, bytes]:
    """Sign Conflux transaction.

    Transaction must be provided as a JSON file.
    """
    address_n = tools.parse_path(address)
    return conflux.sign_tx(client, address_n, json.load(file))


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def sign_message(client: "TrezorClient", address: str, message: str) -> Dict[str, str]:
    """Sign message with Conflux address."""
    address_n = tools.parse_path(address)
    ret = conflux.sign_message(client, address_n, message)
    signature = ret.signature if ret.signature is not None else b""
    output = {
        "message": message,
        "address": ret.address,
        "signature": f"0x{signature.hex()}",
    }
    return output


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("domain-hash")
@click.argument("message-hash")
@with_client
def sign_message_cip23(
    client: "TrezorClient", address: str, domain_hash: str, message_hash: str
) -> Dict[str, str]:
    """Sign message with Conflux address."""
    address_n = tools.parse_path(address)
    domain_hash_bytes = conflux.decode_hex(domain_hash) if domain_hash else None
    message_hash_bytes = conflux.decode_hex(message_hash) if message_hash else None
    ret = conflux.sign_message_cip23(
        client, address_n, domain_hash_bytes, message_hash_bytes
    )
    signature = ret.signature if ret.signature is not None else b""
    output = {
        "domain_hash": domain_hash,
        "message_hash": message_hash,
        "address": ret.address,
        "signature": f"0x{signature.hex()}",
    }
    return output