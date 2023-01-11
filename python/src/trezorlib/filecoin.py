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


@expect(messages.FilecoinAddress, field="address", ret_type=str)
def get_address(
    client: "TrezorClient",
    address_n: "Address",
    show_display: bool = False,
    testnet: bool = False
) -> "MessageType":
    return client.call(
        messages.FilecoinGetAddress(address_n=address_n, show_display=show_display, testnet=testnet)
    )


@expect(messages.FilecoinSignedTx)
def sign_tx(client: "TrezorClient", address_n: "Address", rawtx: bytes, testnet: bool = False):
    return client.call(messages.FilecoinSignTx(address_n=address_n, raw_tx=rawtx, testnet=testnet))
