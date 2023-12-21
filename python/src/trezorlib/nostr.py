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


@expect(messages.NostrPublicKey, ret_type=str)
def get_public_key(
    client: "TrezorClient", address_n: "Address", show_display: bool = False
) -> "MessageType":
    return client.call(
        messages.NostrGetPublicKey(address_n=address_n, show_display=show_display)
    )


@expect(messages.NostrSignedEvent)
def sign_tx(client: "TrezorClient", address_n: "Address", rawtx: bytes):
    return client.call(messages.NostrSignEvent(address_n=address_n, event=rawtx))


@expect(messages.NostrEncryptedMessage)
def encrypt(client: "TrezorClient", address_n: "Address", pubkey: str, msg: bytes, show_display: bool = False):
    return client.call(messages.NostrEncryptMessage(address_n=address_n, pubkey=pubkey, msg=msg, show_display=show_display))


@expect(messages.NostrDecryptedMessage)
def decrypt(client: "TrezorClient", address_n: "Address", pubkey: str, msg: str, show_display: bool = False):
    return client.call(messages.NostrDecryptMessage(address_n=address_n, pubkey=pubkey, msg=msg, show_display=show_display))


@expect(messages.NostrSignedSchnorr)
def schnorr(client: "TrezorClient", address_n: "Address", hash: str):
    return client.call(messages.NostrSignSchnorr(address_n=address_n, hash=hash))
