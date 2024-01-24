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


from typing import TYPE_CHECKING, AnyStr

from . import messages
from .tools import expect, prepare_message_bytes

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address
    from .protobuf import MessageType


@expect(messages.SuiAddress, field="address", ret_type=str)
def get_address(
    client: "TrezorClient", address_n: "Address", show_display: bool = False
) -> "MessageType":
    return client.call(
        messages.SuiGetAddress(address_n=address_n, show_display=show_display)
    )

@expect(messages.SuiSignedTx)
def sign_tx(client: "TrezorClient", address_n: "Address", rawtx: bytes):
    return client.call(messages.SuiSignTx(address_n=address_n, raw_tx=rawtx))

@expect(messages.SuiSignedTx)
def sign_tx_v2(client: "TrezorClient", address_n: "Address", data: bytes):
    msg = messages.SuiSignTx(
        address_n=address_n,
        raw_tx=b'',
    )
    msg.data_length = len(data)
    data, chunk = data[1024:], data[:1024]
    msg.data_initial_chunk = chunk

    response = client.call(msg)
    if isinstance(response, messages.SuiSignedTx):
        return response
    assert isinstance(response, messages.SuiTxRequest)
    while response.data_length is not None:
        data_length = response.data_length
        data, chunk = data[data_length:], data[:data_length]
        response = client.call(messages.SuiTxAck(data_chunk=chunk))
        if isinstance(response, messages.SuiSignedTx):
            break

    assert response.signature is not None
    return response


@expect(messages.SuiMessageSignature)
def sign_message(
    client: "TrezorClient",
    n: "Address",
    message: AnyStr,
) -> "MessageType":
    return client.call(
        messages.SuiSignMessage(
            address_n=n,
            message=prepare_message_bytes(message),
        )
    )
