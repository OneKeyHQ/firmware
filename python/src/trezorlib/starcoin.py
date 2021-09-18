# This file is part of the Trezor project.
#
# Copyright (C) 2012-2019 SatoshiLabs and contributors
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


from . import exceptions, messages
from .tools import expect

DEFAULT_BIP32_PATH = "m/44h/101010h/0h/0h/0h"


# ====== Client functions ====== #


@expect(messages.StarcoinAddress, field="address")
def get_address(client, address_n, show_display=False):
    return client.call(
        messages.StarcoinGetAddress(address_n=address_n, show_display=show_display)
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
