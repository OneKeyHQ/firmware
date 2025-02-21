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
    "base": messages.TonWorkChain.BASECHAIN,
    "master": messages.TonWorkChain.MASTERCHAIN,
}
WALLET_VERSION = {
    # "v3r1": messages.TonWalletVersion.V3R1,
    # "v3r2": messages.TonWalletVersion.V3R2,
    # "v4r1": messages.TonWalletVersion.V4R1,
    "v4r2": messages.TonWalletVersion.V4R2,
}
@click.group(name="ton")
def cli():
    """Ton commands."""


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-b", "--bounceable", is_flag=True)
@click.option("-t", "--test-only", is_flag=True)
@click.option("-i", "--wallet-id", type=int, default=698983191)
@click.option("-v", "--version", type=ChoiceType(WALLET_VERSION), default="v4r2")
@click.option("-w", "--workchain", type=ChoiceType(WORKCHAIN), default="base")
@click.option("-d", "--show-display", is_flag=True)
@with_client
def get_address(client: "TrezorClient",
                address: str,
                bounceable: bool,
                test_only: bool,
                wallet_id: int,
                version: messages.TonWalletVersion,
                workchain: messages.TonWorkChain,
                show_display: bool
                ) -> str:
    """Get Ton address for specified path."""
    address_n = tools.parse_path(address)
    resp = ton.get_address(client, address_n, version, workchain, bounceable, test_only, wallet_id, show_display)
    public_key = resp.public_key.hex()
    return {"public_key": f"{public_key}", "address": f"{resp.address}"}


@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-d", "--destination", type=str, required=True)
@click.option("-j", "--jetton_master_address", type=str)
@click.option("-jw", "--jetton_wallet_address", type=str)
@click.option("-ta", "--ton_amount", type=int, required=True)
@click.option("-ja", "--jetton_amount", type=int)
@click.option("-jab", "--jetton_amount_bytes", type=str)
@click.option("-f", "--fwd_fee", type=int)
@click.option("-c", "--comment", type=str)
@click.option("-r", "--is_raw_data", is_flag=True)
@click.option("-m", "--mode", type=int)
@click.option("-s", "--seqno", type=int, required=True)
@click.option("-e", "--expire_at", type=int, required=True)
@click.option("-v", "--version", type=ChoiceType(WALLET_VERSION), default="v4r2")
@click.option("-i", "--wallet-id", type=int, default=698983191)
@click.option("-w", "--workchain", type=ChoiceType(WORKCHAIN), default="base")
@click.option("-b", "--bounceable", is_flag=True)
@click.option("-t", "--test-only", is_flag=True)
@click.option("-ed", "--ext-destination", multiple=True, type=str)
@click.option("-ea", "--ext-ton-amount", multiple=True, type=int)
@click.option("-ep", "--ext-payload", multiple=True, type=str)
@click.option("-h", "--signing-message-hash", type=str)
@with_client
def sign_message(client: "TrezorClient",
                address: str,
                destination: str,
                jetton_master_address: str,
                jetton_wallet_address: str,
                ton_amount: int,
                jetton_amount: int,
                jetton_amount_bytes: str,
                fwd_fee: int,
                mode: int,
                seqno: int,
                expire_at: int,
                comment: str,
                is_raw_data: bool,
                version: messages.TonWalletVersion,
                wallet_id: int,
                workchain: messages.TonWorkChain,
                bounceable: bool,
                test_only: bool,
                ext_destination: tuple[str, ...],
                ext_ton_amount: tuple[int, ...],
                ext_payload: tuple[str, ...],
                signing_message_hash: str
                ) -> bytes:
    """Sign Ton Transaction."""
    address_n = tools.parse_path(address)
    # expire_at = int(time.time()) + 300
    resp = ton.sign_message(
                client,
                address_n,
                destination,
                jetton_master_address,
                jetton_wallet_address,
                ton_amount,
                jetton_amount,
                jetton_amount_bytes,
                fwd_fee,
                mode,
                seqno,
                expire_at,
                comment,
                is_raw_data,
                version,
                wallet_id,
                workchain,
                bounceable,
                test_only,
                list(ext_destination),
                list(ext_ton_amount),
                list(ext_payload),
                signing_message_hash
    )

    return resp.signature.hex(), resp.signning_message.hex()

@cli.command()
@click.option("-n", "--address", required=True, help=PATH_HELP)
@click.option("-a", "--appdomain", required=True, type=str)
@click.option("-c", "--comment", type=str)
@click.option("-v", "--version", type=ChoiceType(WALLET_VERSION), default="v4r2")
@click.option("-i", "--wallet-id", type=int, default=698983191)
@click.option("-w", "--workchain", type=ChoiceType(WORKCHAIN), default="base")
@click.option("-b", "--bounceable", is_flag=True)
@click.option("-t", "--test-only", is_flag=True)
@with_client
def sign_proof(client: "TrezorClient",
                address: str,
                # expire_at: int,
                appdomain: str,
                comment: str,
                version: messages.TonWalletVersion,
                wallet_id: int,
                workchain: messages.TonWorkChain,
                bounceable: bool,
                test_only: bool
                ) -> bytes:
    """Sign Ton Proof."""
    address_n = tools.parse_path(address)
    # expire_at = int(time.time()) + 300
    expire_at = 1979465599
    signature = ton.sign_proof(
                client,
                address_n,
                expire_at,
                appdomain,
                comment,
                version,
                wallet_id,
                workchain,
                bounceable,
                test_only
    ).signature.hex()

    return {"signature": f"0x{signature}"}