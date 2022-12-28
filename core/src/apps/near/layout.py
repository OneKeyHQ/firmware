from typing import TYPE_CHECKING

from trezor import ui
from trezor.enums import ButtonRequestType

if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context


def require_confirm_tx(
    ctx: Context,
    to: str,
    value: int,
) -> Awaitable[None]:
    from trezor.ui.layouts import confirm_output

    # retain 5 decimal places
    suffix = "NEAR"
    decimals = 24
    d = pow(10, decimals)
    if value < pow(10, decimals - 5):
        amount_str = f"< 0.00001 {suffix}"
    else:
        dp = f"{value % d:0{decimals}}"
        amount_str = f"{value // d}.{dp}".rstrip("0").rstrip(".") + " " + suffix

    return confirm_output(
        ctx,
        address=to,
        amount=amount_str,
        font_amount=ui.BOLD,
        color_to=ui.GREY,
        br_code=ButtonRequestType.SignTx,
    )
