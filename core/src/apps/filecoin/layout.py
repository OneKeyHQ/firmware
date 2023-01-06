from typing import TYPE_CHECKING

from trezor.enums import ButtonRequestType
from trezor.strings import format_amount

if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context


def require_confirm_tx(
    ctx: Context,
    to: str,
    value: int,
) -> Awaitable[None]:
    from trezor.ui.layouts.lvgl import confirm_filecoin_tx

    return confirm_filecoin_tx(
        ctx,
        address=to,
        amount=f"{format_amount(value, 18)} FIL",
        br_code=ButtonRequestType.SignTx,
    )
