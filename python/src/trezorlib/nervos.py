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

from . import messages
from .tools import expect

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address
    from .protobuf import MessageType


@expect(messages.NervosAddress, field="address", ret_type=str)
def get_address(
    client: "TrezorClient",
    address_n: "Address",
    network: str,
    show_display: bool = False,
) -> "MessageType":
    res = client.call(
        messages.NervosGetAddress(
            address_n=address_n, network=network, show_display=show_display
        )
    )
    return res


@expect(messages.NervosSignedTx)
def sign_tx(
    client: "TrezorClient",
    address_n: "Address",
    rawtx: bytes,
    witness_buffer: bytes,
    network: str,
    data_length: int,
):
    resp = client.call(
        messages.NervosSignTx(
            address_n=address_n,
            data_initial_chunk=rawtx,
            witness_buffer=witness_buffer,
            network=network,
            data_length=data_length,
        )
    )
    while isinstance(resp, messages.NervosTxRequest):
        print("ack request")
        data_chunk = bytes.fromhex("0000000000")
        resp = client.call(messages.NervosTxAck(data_chunk=data_chunk))
    return resp
