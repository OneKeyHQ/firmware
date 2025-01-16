from typing import TYPE_CHECKING

from typing import Optional

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

@expect(messages.SolanaSignedMessage)
def sign_message(
    client: "TrezorClient",
    n: "Address",
    message: bytes,
    message_version: messages.SolanaMessageVersion,
    message_format: messages.SolanaMessageFormat,
    application_domain: Optional[str] = None
):
    msg = messages.SolanaSignMessage(
        message=message,
        address_n=n,
        message_version=message_version,
        message_format=message_format,
        application_domain=bytes.fromhex(application_domain) if application_domain else None,
    )
    return client.call(msg)
