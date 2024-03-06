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
from typing import TYPE_CHECKING, Sequence

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


# @expect(messages.NervosSignedTx)
# def sign_tx(
#     client: "TrezorClient",
#     address_n: "Address",
#     rawtx: bytes,
#     witness_buffer: bytes,
#     network: str,
# ):

#     return client.call(
#         messages.NervosSignTx(
#             address_n=address_n,
#             raw_message=rawtx,
#             witness_buffer=witness_buffer,
#             network=network,
#         )
#     )


def sign_tx(
    client: "TrezorClient",
    address_n: Sequence["Address"],
    rawtx: str,
    witness_buffer: str,
    network: str,
) -> Sequence[bytes]:

    inputs = rawtx.split("-")
    print("input0:" + str(inputs[0]))
    print("input1:" + str(inputs[1]))
    print("inputlenth:" + str(len(inputs)))
    # print("input1:"+str(inputs[1]))
    assert len(inputs) >= 1, "Invalid raw message"
    if len(address_n) != len(inputs) and len(address_n) != 1:
        raise ValueError("Number of addresses must match number of inputs")

    signatures = []

    resp = client.call(
        messages.NervosSignTx(
            address_n=address_n[0],
            raw_message=bytes.fromhex(inputs[0]),
            witness_buffer=bytes.fromhex(witness_buffer),
            network=network,
            input_count=len(inputs),
        )
    )

    while isinstance(resp, messages.NervosTxInputRequest):
        signatures.append(resp.signature)
        print("nervostxinputrequest:" + str(resp.request_index))
        resp = client.call(
            messages.NervosTxInputAck(
                address_n=address_n[resp.request_index if len(address_n) > 1 else 0],
                raw_message=bytes.fromhex(inputs[resp.request_index]),
            )
        )
    if isinstance(resp, messages.NervosSignedTx):
        signatures.append(resp.signature)
    else:
        raise ValueError("Invalid response")
    return signatures
