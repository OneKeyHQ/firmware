from typing import TYPE_CHECKING

from trezor import ui
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.strings import format_amount
from trezor.ui.layouts import confirm_address, confirm_output
from trezor.ui.layouts.lvgl.altcoin import confirm_total_tron
from trezor.utils import chunks

from . import tokens

if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context


def require_confirm_data(ctx: Context, data: bytes, data_total: int) -> Awaitable[None]:
    from trezor.ui.layouts import confirm_data

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
    to_str = to
    return confirm_output(
        ctx,
        address=to_str,
        amount=format_amount_trx(value, None),
        font_amount=ui.BOLD,
        color_to=ui.GREY,
        br_code=ButtonRequestType.SignTx,
    )


def require_confirm_trigger_trc20(
    ctx: Context,
    verified: bool,
    contract_address: str,
    amount: int,
    token: tokens.TokenInfo,
    toAddress: str,
) -> Awaitable[None]:
    if verified:
        return confirm_output(
            ctx,
            address=toAddress,
            amount=format_amount_trx(amount, token),
            font_amount=ui.BOLD,
            color_to=ui.GREY,
            br_code=ButtonRequestType.SignTx,
        )

    # Unknown token
    return confirm_address(
        ctx,
        _(i18n_keys.TITLE__UNKNOWN_TOKEN),
        contract_address,
        description=_(i18n_keys.LIST_KEY__CONTRACT__COLON),
        br_type="unknown_token",
        icon="A:/res/shriek.png",
        icon_color=ui.ORANGE,
        br_code=ButtonRequestType.SignTx,
    )


def require_confirm_fee(
    ctx: Context,
    token: tokens.TokenInfo | None = None,
    from_address: str | None = None,
    to_address: str | None = None,
    value: int = 0,
    fee_limit: int = 0,
    network: str | None = None,
) -> Awaitable[None]:
    if token is None:
        total_amount = format_amount_trx(value + fee_limit, None)
    else:
        total_amount = None
    return confirm_total_tron(
        ctx,
        from_address,
        to_address,
        format_amount_trx(value, token),
        format_amount_trx(fee_limit, None),
        total_amount,
        network,
    )


def format_amount_trx(value: int, token: tokens.TokenInfo | None) -> str:
    if token:
        suffix = token.symbol
        decimals = token.decimals
    else:
        suffix = "TRX"
        decimals = 6

    return f"{format_amount(value, decimals)} {suffix}"


def split_address(address):
    return chunks(address, 16)


def split_text(text):
    return chunks(text, 18)
