from typing import TYPE_CHECKING

from . import messages
from .tools import expect

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address


@expect(messages.TonAddress)
def get_address(client: "TrezorClient",
                n: "Address",
                version: messages.WalletVersion=messages.WalletVersion.V3R2,
                workchain: messages.WorkChain=messages.WorkChain.BASECHAIN,
                bounceable: bool = False,
                test_only: bool = False,
                wallet_id: int = 698983191,
                show_display: bool = False):
    return client.call(
        messages.TonGetAddress(
            address_n=n,
            version=version,
            workchain=workchain,
            bounceable=bounceable,
            is_test_only=test_only,
            wallet_id=wallet_id,
            show_display=show_display)
    )


@expect(messages.TonSignedMessage)
def sign_message(
    client: "TrezorClient",
    n: "Address",
    recipient: str,
    value: int,
    expiration_time: int,
    seqno: int,
    wallet_id: int = 698983191,
    memo: str = "",
    version: messages.WalletVersion=messages.WalletVersion.V3R2,
    workchain: messages.WorkChain=messages.WorkChain.BASECHAIN,
    bounceable: bool = False,
    test_only: bool = False,
):
    msg = messages.TonSignMessage(
        address_n=n,
        recipient=recipient,
        amount=value,
        expiration_time=expiration_time,
        seqno=seqno,
        wallet_id=wallet_id,
        memo=memo,
        version=version,
        workchain=workchain,
        bounceable=bounceable,
        is_test_only=test_only,
    )
    return client.call(msg)
