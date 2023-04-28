# This file is part of the Trezor project.
#
# Copyright (C) 2012-2022 SatoshiLabs and contributors
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
from typing import Sequence

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address


@expect(messages.KaspaAddress, field="address", ret_type=str)
def get_address( client: "TrezorClient",
    n: "Address",
    show_display: bool = False,
    prefix: str = "kaspa",
    address_schema: str = "schnorr"
    ):
    return client.call(messages.KaspaGetAddress(
        address_n=n,
        show_display=show_display,
        prefix=prefix,
        scheme=address_schema,
        )
    )


def sign_tx(client: "TrezorClient", addresses: Sequence["Address"], raw_message: str, prefix: str = "kaspa", schema: str = "schnorr") -> Sequence[bytes]:
    inputs = raw_message.split("-")
    assert len(inputs) >= 1, "Invalid raw message"
    if len(addresses) != len(inputs) and len(addresses) != 1:
        raise ValueError("Number of addresses must match number of inputs")
    signatures = []

    resp = client.call(messages.KaspaSignTx(address_n=addresses[0], raw_message=bytes.fromhex(inputs[0]), prefix=prefix, scheme=schema, input_count=len(inputs)))

    while isinstance(resp, messages.KaspaTxInputRequest):
        signatures.append(resp.signature)
        resp = client.call(messages.KaspaTxInputAck(address_n=addresses[resp.request_index if len(addresses) > 1 else 0], raw_message=bytes.fromhex(inputs[resp.request_index])))

    if isinstance(resp, messages.KaspaSignedTx):
        signatures.append(resp.signature)
    else:
        raise ValueError("Invalid response")
    return signatures
