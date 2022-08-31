from typing import TYPE_CHECKING

from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

if TYPE_CHECKING:
    from typing import Awaitable
    from trezor.wire import Context


def require_confirm_tx(
    ctx: Context,
    signer: str,
) -> Awaitable[None]:
    from trezor.ui.layouts.lvgl import confirm_near_blinding_sign

    return confirm_near_blinding_sign(ctx, signer)


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


async def confirm_final(ctx: Context):
    from trezor.ui.layouts.lvgl import confirm_action

    await confirm_action(
        ctx,
        "confirm_final",
        title=_(i18n_keys.TITLE__CONFIRM_TRANSACTION),
        action=_(i18n_keys.SUBTITLE__DO_YOU_WANT_TO_SIGN__THIS_TX),
        verb=_(i18n_keys.BUTTON__SLIDE_TO_SIGN),
        hold=True,
    )
