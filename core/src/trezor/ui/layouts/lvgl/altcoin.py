from typing import Sequence

from trezor import ui, wire
from trezor.enums import ButtonRequestType
from trezor.utils import chunks_intersperse

from ...components.tt.confirm import HoldToConfirm
from ...components.tt.scroll import Paginated
from ...components.tt.text import Text
from ...constants.tt import MONO_ADDR_PER_LINE
from .common import interact, raise_if_cancelled


async def confirm_total_ethereum(
    ctx: wire.GenericContext,
    amount: str,
    gas_price: str,
    fee_max: str,
    from_address: str | None,
    to_address: str | None,
    network: str | None,
    total_amount: str | None = None,
) -> None:
    from trezor.lvglui.scrs.template import TransactionDetailsETH

    screen = TransactionDetailsETH(
        f"Sign {network} Transaction",
        from_address,
        to_address,
        amount,
        fee_max,
        gas_price=gas_price,
        total_amount=total_amount,
    )
    await raise_if_cancelled(
        interact(ctx, screen, "confirm_total", ButtonRequestType.SignTx)
    )


async def confirm_total_ethereum_eip1559(
    ctx: wire.GenericContext,
    amount: str,
    max_priority_fee_per_gas,
    max_fee_per_gas,
    fee_max: str,
    from_address: str | None,
    to_address: str | None,
    network: str | None,
    total_amount: str | None = None,
) -> None:
    from trezor.lvglui.scrs.template import TransactionDetailsETH

    screen = TransactionDetailsETH(
        f"Sign {network} Transaction",
        from_address,
        to_address,
        amount,
        fee_max,
        is_eip1559=True,
        max_fee_per_gas=max_fee_per_gas,
        max_priority_fee_per_gas=max_priority_fee_per_gas,
        total_amount=total_amount,
    )
    await raise_if_cancelled(
        interact(ctx, screen, "confirm_total", ButtonRequestType.SignTx)
    )


async def confirm_total_ripple(
    ctx: wire.GenericContext,
    address: str,
    amount: str,
) -> None:
    title = "Confirm sending"
    text = Text(title, ui.ICON_SEND, ui.GREEN, new_lines=False)
    text.bold(f"{amount} XRP\n")
    text.normal("to\n")
    text.mono(*chunks_intersperse(address, MONO_ADDR_PER_LINE))

    await raise_if_cancelled(
        interact(ctx, HoldToConfirm(text), "confirm_output", ButtonRequestType.SignTx)
    )


async def confirm_transfer_binance(
    ctx: wire.GenericContext, inputs_outputs: Sequence[tuple[str, str, str]]
) -> None:
    pages: list[ui.Component] = []
    for title, amount, address in inputs_outputs:
        coin_page = Text(title, ui.ICON_SEND, icon_color=ui.GREEN, new_lines=False)
        coin_page.bold(amount)
        coin_page.normal("\nto\n")
        coin_page.mono(*chunks_intersperse(address, MONO_ADDR_PER_LINE))
        pages.append(coin_page)

    pages[-1] = HoldToConfirm(pages[-1])

    await raise_if_cancelled(
        interact(
            ctx, Paginated(pages), "confirm_transfer", ButtonRequestType.ConfirmOutput
        )
    )


async def confirm_decred_sstx_submission(
    ctx: wire.GenericContext,
    address: str,
    amount: str,
) -> None:
    from trezor.lvglui.scrs.template import ConfirmDecredSstxSubmission

    screen = ConfirmDecredSstxSubmission(
        "Purchase ticket", "voting rights", amount, address
    )
    await raise_if_cancelled(
        interact(
            ctx,
            screen,
            "confirm_decred_sstx_submission",
            ButtonRequestType.ConfirmOutput,
        )
    )
