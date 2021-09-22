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


import click

from .. import starcoin, tools
from . import with_client

PATH_HELP = "BIP32 path. Always use hardened paths and the m/44'/101010'/0'/0'/ prefix"


@click.group(name="starcoin")
def cli():
    """Stellar commands."""


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
def get_address(client, address, show_display):
    """Get starcoin public address."""
    address_n = tools.parse_path(address)
    return starcoin.get_address(client, address_n, show_display)

@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_public_key(client, address, show_display):
    """Get starcoin public key for specified path."""
    address_n = tools.parse_path(address)
    res = starcoin.get_public_key(client, address_n, show_display)
    output = {"public_key": res.public_key.hex()}
    return output

@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.argument("message")
@with_client
def sign_raw_transaction(client, address, message):
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
def sign_message(client, address, message):
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
def verify_message(client, pubkey, signature, message):
    """Verify message signed with Starcoin address."""
    signature = bytes.fromhex(signature)
    pubkey = bytes.fromhex(pubkey)
    return starcoin.verify_message(
        client, pubkey, signature, bytes(message, encoding="utf8")
    )
