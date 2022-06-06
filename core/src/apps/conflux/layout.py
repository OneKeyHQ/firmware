from typing import TYPE_CHECKING

from trezor import ui
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.strings import format_amount
from trezor.ui.layouts import confirm_output

from . import helpers

if TYPE_CHECKING:
    from typing import Awaitable

    from trezor.wire import Context


def require_confirm_data(ctx: Context, data: bytes, data_total: int) -> Awaitable[None]:
    from trezor.ui.layouts import confirm_data  # type: ignore["confirm_data" is unknown import symbol]

    return confirm_data(
        ctx,
        "confirm_data",
        title=_(i18n_keys.TITLE__VIEW_DATA),
        description=_(i18n_keys.SUBTITLE__STR_BYTES).format(data_total),
        data=data,
        br_code=ButtonRequestType.SignTx,
    )


def require_confirm_tx(
    ctx: Context,
    to: str,
    value: int,
) -> Awaitable[None]:
    addr_type = helpers.get_address_type(to)
    if addr_type == helpers.CONFLUX_TYPE_USER:
        to_str = to
    else:
        to_str = _(i18n_keys.LIST_VALUE__NEW_CONTRACT)

    return confirm_output(
        ctx,
        address=to_str,
        amount=format_conflux_amount(value),
        font_amount=ui.BOLD,
        color_to=ui.GREY,
        br_code=ButtonRequestType.SignTx,
    )


def format_conflux_amount(value: int) -> str:
    suffix = "CFX"
    decimals = helpers.DECIMALS

    # Don't want to display wei values for tokens with small decimal numbers
    if decimals > 9 and value < 10 ** (decimals - 9):
        suffix = "Drip " + suffix
        decimals = 0

    return f"{format_amount(value, decimals)} {suffix}"
