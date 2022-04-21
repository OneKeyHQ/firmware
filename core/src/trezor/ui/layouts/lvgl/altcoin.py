from typing import Sequence

from trezor import wire
from trezor.enums import ButtonRequestType

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
    from trezor.ui.layouts import confirm_output

    await confirm_output(ctx, address, f"{amount} XRP")


async def confirm_transfer_binance(
    ctx: wire.GenericContext, inputs_outputs: Sequence[tuple[str, str, str]]
) -> None:
    from trezor.lvglui.scrs.template import ConfirmTransferBinance

    screen = ConfirmTransferBinance(inputs_outputs)
    await raise_if_cancelled(
        interact(ctx, screen, "confirm_transfer", ButtonRequestType.ConfirmOutput)
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
