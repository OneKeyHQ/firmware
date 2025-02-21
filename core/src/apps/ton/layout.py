from typing import TYPE_CHECKING

from trezor import ui
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.strings import format_amount
from trezor.ui.layouts import confirm_address, should_show_details

from . import tokens

if TYPE_CHECKING:
    from typing import Awaitable

    from trezor.wire import Context


def require_confirm_fee(
    ctx: Context,
    from_address: str | None = None,
    to_address: str | None = None,
    value: int = 0,
    gas_price: int = 0,
    gas_limit: int = 0,
    token: tokens.TokenInfo | None = None,
    raw_data: bytes | None = None,
    is_raw_data: bool = False,
) -> Awaitable[None]:
    from trezor.ui.layouts.lvgl.altcoin import confirm_total_ton

    fee_limit = gas_price * gas_limit

    return confirm_total_ton(
        ctx,
        format_ton_amount(value, token),
        None,
        format_ton_amount(fee_limit, None),
        from_address,
        to_address,
        format_ton_amount(value + fee_limit, None) if token is None else None,
        raw_data=raw_data,
        is_raw_data=is_raw_data,
    )


def require_show_overview(
    ctx: Context,
    to_addr: str,
    value: int,
    token: tokens.TokenInfo | None = None,
) -> Awaitable[bool]:

    return should_show_details(
        ctx,
        title=_(i18n_keys.TITLE__STR_TRANSACTION).format("Ton"),
        address=to_addr,
        amount=format_ton_amount(value, token),
        br_code=ButtonRequestType.SignTx,
    )


def format_ton_amount(value: int, token: tokens.TokenInfo | None) -> str:
    if token:
        suffix = token.symbol
        decimals = token.decimals
    else:
        suffix = "TON"
        decimals = 9

    # Don't want to display wei values for tokens with small decimal numbers
    # if decimals > 9 and value < 10 ** (decimals - 9):
    #     suffix = "Drip " + suffix
    #     decimals = 0

    return f"{format_amount(value, decimals)} {suffix}"


def require_confirm_unknown_token(ctx: Context, address: str) -> Awaitable[None]:
    return confirm_address(
        ctx,
        _(i18n_keys.TITLE__UNKNOWN_TOKEN),
        address,
        description=_(i18n_keys.LIST_KEY__CONTRACT__COLON),
        br_type="unknown_token",
        icon="A:/res/shriek.png",
        icon_color=ui.ORANGE,
        br_code=ButtonRequestType.SignTx,
    )
