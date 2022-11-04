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

from typing import TYPE_CHECKING, AnyStr, Optional, Tuple

from . import messages
from .tools import expect, prepare_message_bytes, session

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address
    from .protobuf import MessageType


def int_to_big_endian(value: int) -> bytes:
    return value.to_bytes((value.bit_length() + 7) // 8, "big")


def decode_hex(value: str) -> bytes:
    if value.startswith(("0x", "0X")):
        return bytes.fromhex(value[2:])
    else:
        return bytes.fromhex(value)


@expect(messages.ConfluxAddress, field="address", ret_type=str)
def get_address(
    client: "TrezorClient",
    address_n: "Address",
    chain_id: int,
    show_display: bool = False,
) -> "MessageType":
    return client.call(
        messages.ConfluxGetAddress(
            address_n=address_n, chain_id=chain_id, show_display=show_display
        )
    )


@session
def sign_tx(
    client: "TrezorClient", address_n: "Address", tx_json: dict
) -> Tuple[int, bytes, bytes]:
    tx_msg = tx_json.copy()

    data = b""
    if "data" in tx_msg.keys():
        data = decode_hex(tx_msg["data"])

    data_length = len(data)
    data, chunk = data[1024:], data[:1024]

    msg = messages.ConfluxSignTx(
        address_n=address_n,
        to=tx_msg["to"],
        nonce=int_to_big_endian(tx_msg["nonce"]),
        value=int_to_big_endian(tx_msg["value"]),
        gas_limit=int_to_big_endian(tx_msg["gas_limit"]),
        gas_price=int_to_big_endian(tx_msg["gas_price"]),
        storage_limit=int_to_big_endian(tx_msg["storage_limit"]),
        epoch_height=int_to_big_endian(tx_msg["epoch_height"]),
        chain_id=tx_msg["chain_id"],
        data_length=data_length,
        data_initial_chunk=chunk,
    )

    response = client.call(msg)
    assert isinstance(response, messages.ConfluxTxRequest)

    while response.data_length is not None:
        data_length = response.data_length
        data, chunk = data[data_length:], data[:data_length]
        response = client.call(messages.ConfluxTxAck(data_chunk=chunk))
        assert isinstance(response, messages.ConfluxTxRequest)

    assert response.signature_v is not None
    assert response.signature_r is not None
    assert response.signature_s is not None

    return response.signature_v, response.signature_r, response.signature_s


@expect(messages.ConfluxMessageSignature)
def sign_message(
    client: "TrezorClient", n: "Address", message: AnyStr
) -> "MessageType":
    return client.call(
        messages.ConfluxSignMessage(address_n=n, message=prepare_message_bytes(message))
    )


@expect(messages.ConfluxMessageSignature)
def sign_message_cip23(
    client: "TrezorClient",
    n: "Address",
    domain_hash: Optional[bytes],
    message_hash: Optional[bytes],
) -> "MessageType":
    return client.call(
        messages.ConfluxSignMessageCIP23(
            address_n=n,
            domain_hash=domain_hash,
            message_hash=message_hash,
        )
    )
