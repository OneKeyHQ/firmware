from typing import TYPE_CHECKING

from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.strings import format_amount
from trezor.ui.layouts import confirm_metadata
from trezor.ui.layouts.lvgl.altcoin import confirm_total_ripple

from . import helpers

if TYPE_CHECKING:
    from trezor.wire import Context


async def require_confirm_fee(ctx: Context, fee: int) -> None:
    await confirm_metadata(
        ctx,
        "confirm_fee",
        title=_(i18n_keys.TITLE__CONFIRM_FEE),
        content=_(i18n_keys.SUBTITLE__TRANSACTION_FEE),
        param=format_amount(fee, helpers.DECIMALS) + " XRP",
        hide_continue=True,
        br_code=ButtonRequestType.ConfirmOutput,
        description=_(i18n_keys.LIST_KEY__FEE__COLON),
    )


async def require_confirm_destination_tag(ctx: Context, tag: int) -> None:
    await confirm_metadata(
        ctx,
        "confirm_destination_tag",
        title="Confirm tag",
        content="Destination tag",
        param=str(tag),
        hide_continue=True,
        br_code=ButtonRequestType.ConfirmOutput,
        description="DESTINATION TAG",
    )


async def require_confirm_tx(ctx: Context, to: str, value: int) -> None:
    await confirm_total_ripple(ctx, to, format_amount(value, helpers.DECIMALS))
