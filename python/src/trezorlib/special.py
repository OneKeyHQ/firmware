import warnings
from copy import copy
from decimal import Decimal
from binascii import unhexlify
from typing import TYPE_CHECKING, Any, Dict, Sequence, Tuple

from . import exceptions, messages
from .tools import expect, normalize_nfc, session

if TYPE_CHECKING:
    from .client import TrezorClient


@expect(messages.DigestSignature)
def sign_digest(
    client, coin_name, n, digest
):
    return client.call(
        messages.SignDigest(
            coin_name=coin_name,
            address_n=n,
            digest=digest
        )
    )

@expect(messages.DataSignature)
def sign_data(
    client, coin_name, n, data
):
    return client.call(
        messages.SignData(
            coin_name=coin_name,
            address_n=n,
            data=data
        )
    )

@expect(messages.Ed25519PublicKey)
def export_ed25519_pubkey(
    client, n
):
    return client.call(
        messages.ExportEd25519PublicKey(
            address_n=n
        )
    )

@expect(messages.Ed25519Nonce)
def get_ed25519_nonce(
    client, n, data, ctr
):
    return client.call(
        messages.GetEd25519Nonce(
            address_n=n,
            data=data,
            ctr=ctr
        )
    )
