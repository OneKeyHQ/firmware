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


from typing import TYPE_CHECKING

import click
import time
from trezorlib import ton, tools
from trezorlib.cli import with_client, ChoiceType
from trezorlib import messages
if TYPE_CHECKING:
    from ..client import TrezorClient
PATH_HELP = "BIP-32 path, e.g. m/44'/607'/0'/0'"

WORKCHAIN = {
    "base": messages.WorkChain.BASECHAIN,
    "master": messages.WorkChain.MASTERCHAIN,
}
WALLET_VERSION = {
    "v3r1": messages.WalletVersion.V3R1,
    "v3r2": messages.WalletVersion.V3R2,
    "v4r1": messages.WalletVersion.V4R1,
    "v4r2": messages.WalletVersion.V4R2,
}
@click.group(name="ton")
def cli():
    """Ton commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-b", "--bounceable", is_flag=True)
@click.option("-t", "--test-only", is_flag=True)
@click.option("-i", "--wallet-id", type=int, default=698983191)
@click.option("-v", "--version", type=ChoiceType(WALLET_VERSION), default="v3r2")
@click.option("-w", "--workchain", type=ChoiceType(WORKCHAIN), default="base")
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient",
                address: str,
                bounceable: bool,
                test_only: bool,
                wallet_id: int,
                version: messages.WalletVersion,
                workchain: messages.WorkChain,
                show_display: bool
                ) -> str:
    """Get Ton address for specified path."""
    address_n = tools.parse_path(address)
    return ton.get_address(client, address_n, version, workchain, bounceable, test_only, wallet_id, show_display).address


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-r", "--recipient", required=True)
@click.option("-va", "--value", type=int, required=True)
@click.option("-s", "--seqno", type=int, required=False)
@click.option("-i", "--wallet-id", type=int, default=698983191)
@click.option("-m", "--memo", type=str, required=False)
@click.option("-v", "--version", type=ChoiceType(WALLET_VERSION), default="v3r2")
@click.option("-w", "--workchain", type=ChoiceType(WORKCHAIN), default="base")
@click.option("-b", "--bounceable", is_flag=True)
@click.option("-t", "--test-only", is_flag=True)
@with_client
def sign_message(client: "TrezorClient",
                    address: str,
                    recipient: str,
                    value: int,
                    seqno:int,
                    wallet_id:int,
                    memo:str,
                    version: messages.WalletVersion,
                    workchain: messages.WorkChain,
                    bounceable: bool,
                    test_only: bool,
                ) -> str:
    """Sign Ton message."""
    address_n = tools.parse_path(address)
    expiration_time = int(time.time()) + 300
    transaction = ton.sign_message(
        client,
        address_n,
        recipient,
        value,
        expiration_time,
        seqno,
        wallet_id,
        memo,
        version=version,
        workchain=workchain,
        bounceable=bounceable,
        test_only=test_only,
        )
    return transaction.signature.hex()
