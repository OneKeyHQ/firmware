from typing import TYPE_CHECKING

from . import messages
from .tools import expect

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address


@expect(messages.SolanaAddress)
def get_address(client: "TrezorClient", n: "Address", show_display: bool = False):
    return client.call(
        messages.SolanaGetAddress(address_n=n, show_display=show_display)
    )


@expect(messages.SolanaSignedTx)
def sign_tx(
    client: "TrezorClient",
    n: "Address",  # fee_payer
    raw_tx: bytes,
):
    msg = messages.SolanaSignTx(
        raw_tx=raw_tx,
        address_n=n,
    )
    return client.call(msg)
