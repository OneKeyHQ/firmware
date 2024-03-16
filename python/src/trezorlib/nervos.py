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
    res =  client.call(
        messages.NervosGetAddress(address_n=address_n, network=network, show_display=show_display)
    )
    return res


@expect(messages.NervosSignedTx)
def sign_tx(client: "TrezorClient", address_n: "Address", rawtx: str,witness_buffer: str,network: str, data_length:int):
   resp = client.call(messages.NervosSignTx(address_n=address_n,  data_initial_chunk=rawtx,witness_buffer = witness_buffer, network=network,data_length = data_length))
   while isinstance(resp, messages.NervosTxRequest):
        print("NervosTxRequest error"+str(resp.data_length))
        data_chunk = bytes.fromhex("5f0100001c00000020000000490000004d0000007d0000004b0100000000000001000000f8de3bb47d055cdf460d93a2a6e1b05f7432f9777c8c474abf4eec1d4aee5d37000000000100000000010000000000000000000000e1450bbdbdebf15882b9c99c13d60b94794b3796eb571c4985f0d30c1763601c00000000ce0000000c0000006d0000006100000010000000180000006100000000dd0ee901000000490000001000000030000000310000009bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce80114000000efaec7014e49775e6e4a55f36b39fd5b403bfb7d6100000010000000180000006100000000d0ed902e000000490000001000000030000000310000009bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce80114000000be61c42b787a0dbbce370468d9ddaf6f46cb5f3b140000000c000000100000000000000000000000")
        resp = client.call(messages.NervosTxAck(data_chunk=data_chunk))
   return resp
