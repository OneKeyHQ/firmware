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


from . import exceptions, messages
from .tools import expect

DEFAULT_BIP32_PATH = "m/44h/101010h/0h/0h/0h"


# ====== Client functions ====== #


@expect(messages.StarcoinAddress, field="address")
def get_address(client, address_n, show_display=False):
    return client.call(
        messages.StarcoinGetAddress(address_n=address_n, show_display=show_display)
    )

@expect(messages.StarcoinPublicKey)
def get_public_key(client, n, show_display=False):
    return client.call(
        messages.StarcoinGetPublicKey(address_n=n, show_display=show_display)
    )

@expect(messages.StarcoinSignedTx)
def sign_tx(client, address_n, rawtx):
    return client.call(messages.StarcoinSignTx(address_n=address_n, raw_tx=rawtx))


@expect(messages.StarcoinMessageSignature)
def sign_message(client, n, message):
    return client.call(messages.StarcoinSignMessage(address_n=n, message=message))


def verify_message(client, pubkey, signature, message):
    try:
        resp = client.call(
            messages.StarcoinVerifyMessage(
                signature=signature, public_key=pubkey, message=message
            )
        )
    except exceptions.TrezorFailure:
        return False
    return isinstance(resp, messages.Success)
