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


@expect(messages.NexaAddress)
def get_address( client: "TrezorClient",
    n: "Address",
    show_display: bool = False,
    prefix: str = "nexa",
    ):
    return client.call(messages.NexaGetAddress(
        address_n=n,
        show_display=show_display,
        prefix=prefix,
        )
    )


def sign_tx(client: "TrezorClient", addresses: Sequence["Address"], raw_message: str, prefix: str = "nexa") -> Sequence[bytes]:
    inputs = raw_message.split("-")
    assert len(inputs) >= 1, "Invalid raw message"
    if len(addresses) != len(inputs) and len(addresses) != 1:
        raise ValueError("Number of addresses must match number of inputs")
    signatures = []

    resp = client.call(messages.NexaSignTx(address_n=addresses[0], raw_message=bytes.fromhex(inputs[0]), prefix=prefix, input_count=len(inputs)))

    while isinstance(resp, messages.NexaTxInputRequest):
        signatures.append(resp.signature)
        resp = client.call(messages.NexaTxInputAck(address_n=addresses[resp.request_index if len(addresses) > 1 else 0], raw_message=bytes.fromhex(inputs[resp.request_index])))

    if isinstance(resp, messages.NexaSignedTx):
        signatures.append(resp.signature)
    else:
        raise ValueError("Invalid response")
    return signatures
