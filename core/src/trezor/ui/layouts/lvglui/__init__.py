from typing import TYPE_CHECKING

from trezor import log, ui, wire
from trezor.enums import ButtonRequestType

from ...constants.tt import MONO_ADDR_PER_LINE
from . import lv_ui
from .lv_common import button_request, interact, is_confirmed, raise_if_cancelled

if TYPE_CHECKING:
    from typing import Any, Awaitable, Iterable, NoReturn, Sequence

    from ..common import PropertyType, ExceptionType


async def mnemonic_word_select(
    ctx: wire.GenericContext,
    words: Sequence[str],
    share_index: int | None,
    word_index: int,
    count: int,
    group_index: int | None = None,
):

    ui_candidate = lv_ui.Screen_CandidateWords(
        words, f"Check word {word_index + 1}", "Choose the correct word.", "Next"
    )
    return await ctx.wait(ui_candidate.response())


async def confirm_action(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    action: str | None = None,
    description: str | None = None,
    description_param: str | None = None,
    description_param_font: int = ui.BOLD,
    verb: str | None = "CONFIRM",
    verb_cancel: str | None = None,
    hold: bool = False,
    hold_danger: bool = False,
    icon: str | None = None,
    icon_color: int | None = None,
    reverse: bool = False,
    larger_vspace: bool = False,
    exc: ExceptionType = wire.ActionCancelled,
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> None:
    if action and description:
        desc = description + action
    else:
        desc = description
    ui_confirm = lv_ui.Screen_Generic(
        cancel_btn=reverse,
        icon=icon,
        title=title,
        description=desc,
        confirm_text=verb,
        cancel_text=verb_cancel,
    )

    result = is_confirmed(
        await interact(
            ctx,
            ui_confirm,
            br_type,
            br_code,
        )
    )
    if result is not True:
        raise exc


async def confirm_reset_device(
    ctx: wire.GenericContext, prompt: str, recovery: bool = False
) -> None:
    if recovery:
        ui_reset = lv_ui.Screen_Generic(
            cancel_btn=True,
            title="Recovery mode",
            description=prompt,
            confirm_text="Confirm",
            cancel_text="Cancel",
        )
    else:
        ui_reset = lv_ui.Screen_Generic(
            cancel_btn=True,
            title="Create new wallet",
            description=prompt,
            confirm_text="Confirm",
        )

    await raise_if_cancelled(
        interact(
            ctx,
            ui_reset,
            "recover_device" if recovery else "setup_device",
            ButtonRequestType.ProtectCall
            if recovery
            else ButtonRequestType.ResetDevice,
        )
    )


async def confirm_backup(ctx: wire.GenericContext) -> bool:
    ui_backup = lv_ui.Screen_Generic(
        cancel_btn=True,
        title="New wallet created successfully!",
        description="You should back up your new wallet right now.",
        confirm_text="Confirm",
    )

    if is_confirmed(
        await interact(
            ctx,
            ui_backup,
            "backup_device",
            ButtonRequestType.ResetDevice,
        )
    ):
        return True
    ui_backup.delete()
    ui_backup = lv_ui.Screen_Generic(
        cancel_btn=True,
        title="Are you sure you want to skip the backup?",
        description="You should back up your new wallet right now.",
        confirm_text="Confirm",
    )

    confirmed = is_confirmed(
        await interact(
            ctx,
            ui_backup,
            "backup_device",
            ButtonRequestType.ResetDevice,
        )
    )
    return confirmed


async def confirm_path_warning(
    ctx: wire.GenericContext, path: str, path_type: str = "Path"
) -> None:
    raise NotImplementedError


async def show_xpub(
    ctx: wire.GenericContext, xpub: str, title: str, cancel: str
) -> None:
    raise NotImplementedError


async def show_address(
    ctx: wire.GenericContext,
    address: str,
    *,
    case_sensitive: bool = True,
    address_qr: str | None = None,
    title: str = "Confirm address",
    network: str | None = None,
    multisig_index: int | None = None,
    xpubs: Sequence[str] = (),
    address_extra: str | None = None,
    title_qr: str | None = None,
) -> None:
    raise NotImplementedError


def show_pubkey(
    ctx: wire.Context, pubkey: str, title: str = "Confirm public key"
) -> Awaitable[None]:
    return confirm_blob(
        ctx,
        br_type="show_pubkey",
        title="Confirm public key",
        data=pubkey,
        br_code=ButtonRequestType.PublicKey,
        icon=ui.ICON_RECEIVE,
    )


async def _show_modal(
    ctx: wire.GenericContext,
    br_type: str,
    br_code: ButtonRequestType,
    header: str,
    subheader: str | None,
    content: str,
    button_confirm: str | None,
    button_cancel: str | None,
    icon: str,
    icon_color: int,
    exc: ExceptionType = wire.ActionCancelled,
) -> None:
    raise NotImplementedError


async def show_error_and_raise(
    ctx: wire.GenericContext,
    br_type: str,
    content: str,
    header: str = "Error",
    subheader: str | None = None,
    button: str = "Close",
    red: bool = False,
    exc: ExceptionType = wire.ActionCancelled,
) -> NoReturn:
    await _show_modal(
        ctx,
        br_type=br_type,
        br_code=ButtonRequestType.Other,
        header=header,
        subheader=subheader,
        content=content,
        button_confirm=None,
        button_cancel=button,
        icon=ui.ICON_WRONG,
        icon_color=ui.RED if red else ui.ORANGE_ICON,
        exc=exc,
    )
    raise exc


async def show_warning(
    ctx: wire.GenericContext,
    br_type: str,
    content: str,
    header: str = "Warning",
    subheader: str | None = None,
    button: str = "Try again",
    br_code: ButtonRequestType = ButtonRequestType.Warning,
    icon: str = "A:/res/danger.png",
    icon_color: int = ui.RED,
):
    if subheader:
        desc = subheader + content
    else:
        desc = content
    ui_warning = lv_ui.Screen_Generic(
        cancel_btn=False,
        icon=icon,
        title=header,
        description=desc,
        confirm_text=button,
    )

    await raise_if_cancelled(
        interact(
            ctx,
            ui_warning,
            br_type,
            br_code,
        )
    )


async def show_success(
    ctx: wire.GenericContext,
    br_type: str,
    content: str,
    subheader: str | None = None,
    button: str = "Continue",
):

    ui_success = lv_ui.Screen_Generic(
        cancel_btn=False,
        icon="A:/res/success_icon.png",
        title=content,
        description=subheader,
        confirm_text=button,
    )

    await raise_if_cancelled(
        interact(
            ctx,
            ui_success,
            br_type,
            ButtonRequestType.Success,
        )
    )


async def confirm_output(
    ctx: wire.GenericContext,
    address: str,
    amount: str,
    font_amount: int = ui.NORMAL,  # TODO cleanup @ redesign
    title: str = "Confirm sending",
    subtitle: str | None = None,  # TODO cleanup @ redesign
    color_to: int = ui.FG,  # TODO cleanup @ redesign
    to_str: str = " to\n",  # TODO cleanup @ redesign
    to_paginated: bool = False,  # TODO cleanup @ redesign
    width: int = MONO_ADDR_PER_LINE,
    width_paginated: int = MONO_ADDR_PER_LINE - 1,
    br_code: ButtonRequestType = ButtonRequestType.ConfirmOutput,
    icon: str = ui.ICON_SEND,
) -> None:
    raise NotImplementedError


async def should_show_more(
    ctx: wire.GenericContext,
    title: str,
    para: Iterable[tuple[int, str]],
    button_text: str = "Show all",
    br_type: str = "should_show_more",
    br_code: ButtonRequestType = ButtonRequestType.Other,
    icon: str = ui.ICON_DEFAULT,
    icon_color: int = ui.ORANGE_ICON,
) -> bool:
    raise NotImplementedError


async def confirm_blob(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    data: bytes | str,
    description: str | None = None,
    hold: bool = False,
    br_code: ButtonRequestType = ButtonRequestType.Other,
    icon: str = ui.ICON_SEND,  # TODO cleanup @ redesign
    icon_color: int = ui.GREEN,  # TODO cleanup @ redesign
    ask_pagination: bool = False,
) -> None:
    raise NotImplementedError


def confirm_address(
    ctx: wire.GenericContext,
    title: str,
    address: str,
    description: str | None = "Address:",
    br_type: str = "confirm_address",
    br_code: ButtonRequestType = ButtonRequestType.Other,
    icon: str = ui.ICON_SEND,  # TODO cleanup @ redesign
    icon_color: int = ui.GREEN,  # TODO cleanup @ redesign
) -> Awaitable[None]:
    raise NotImplementedError


async def confirm_text(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    data: str,
    description: str | None = None,
    br_code: ButtonRequestType = ButtonRequestType.Other,
    icon: str = ui.ICON_SEND,  # TODO cleanup @ redesign
    icon_color: int = ui.GREEN,  # TODO cleanup @ redesign
) -> None:
    raise NotImplementedError


def confirm_amount(
    ctx: wire.GenericContext,
    title: str,
    amount: str,
    description: str = "Amount:",
    br_type: str = "confirm_amount",
    br_code: ButtonRequestType = ButtonRequestType.Other,
    icon: str = ui.ICON_SEND,  # TODO cleanup @ redesign
    icon_color: int = ui.GREEN,  # TODO cleanup @ redesign
) -> Awaitable[None]:
    raise NotImplementedError


async def confirm_properties(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    props: Iterable[PropertyType],
    icon: str = ui.ICON_SEND,  # TODO cleanup @ redesign
    icon_color: int = ui.GREEN,  # TODO cleanup @ redesign
    hold: bool = False,
    br_code: ButtonRequestType = ButtonRequestType.ConfirmOutput,
) -> None:
    raise NotImplementedError


async def confirm_total(
    ctx: wire.GenericContext,
    total_amount: str,
    fee_amount: str,
    title: str = "Confirm transaction",
    total_label: str = "Total amount:\n",
    fee_label: str = "\nincluding fee:\n",
    icon_color: int = ui.GREEN,
    br_type: str = "confirm_total",
    br_code: ButtonRequestType = ButtonRequestType.SignTx,
) -> None:
    raise NotImplementedError


async def confirm_joint_total(
    ctx: wire.GenericContext, spending_amount: str, total_amount: str
) -> None:
    raise NotImplementedError


async def confirm_metadata(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    content: str,
    param: str | None = None,
    br_code: ButtonRequestType = ButtonRequestType.SignTx,
    hide_continue: bool = False,
    hold: bool = False,
    param_font: int = ui.BOLD,
    icon: str = ui.ICON_SEND,  # TODO cleanup @ redesign
    icon_color: int = ui.GREEN,  # TODO cleanup @ redesign
    larger_vspace: bool = False,  # TODO cleanup @ redesign
) -> None:
    raise NotImplementedError


async def confirm_replacement(
    ctx: wire.GenericContext, description: str, txid: str
) -> None:
    raise NotImplementedError


async def confirm_modify_output(
    ctx: wire.GenericContext,
    address: str,
    sign: int,
    amount_change: str,
    amount_new: str,
) -> None:
    raise NotImplementedError


async def confirm_modify_fee(
    ctx: wire.GenericContext,
    sign: int,
    user_fee_change: str,
    total_fee_new: str,
) -> None:
    raise NotImplementedError


async def confirm_coinjoin(
    ctx: wire.GenericContext, coin_name: str, max_rounds: int, max_fee_per_vbyte: str
) -> None:
    raise NotImplementedError


# TODO cleanup @ redesign
async def confirm_sign_identity(
    ctx: wire.GenericContext, proto: str, identity: str, challenge_visual: str | None
) -> None:
    raise NotImplementedError


async def confirm_signverify(
    ctx: wire.GenericContext, coin: str, message: str, address: str, verify: bool
) -> None:
    raise NotImplementedError


async def show_popup(
    title: str,
    description: str,
    subtitle: str | None = None,
    description_param: str = "",
    timeout_ms: int = 3000,
) -> None:
    raise NotImplementedError


def draw_simple_text(title: str, description: str = "") -> None:
    raise NotImplementedError


async def request_passphrase_on_device(ctx: wire.GenericContext, max_len: int) -> str:
    raise NotImplementedError


async def request_pin_on_device(
    ctx: wire.GenericContext,
    prompt: str,
    attempts_remaining: int | None,
    allow_cancel: bool,
) -> str:
    await button_request(ctx, "pin_device", code=ButtonRequestType.PinEntry)

    if attempts_remaining is None:
        subprompt = None
    elif attempts_remaining == 1:
        subprompt = "This is your last attempt"
    else:
        subprompt = f"{attempts_remaining} attempts remaining"

    ui_input_pin = lv_ui.Screen_InputPIN(prompt, subprompt, allow_cancel)

    result = await ctx.wait(ui_input_pin.response())
    if result == "cancel":
        raise wire.PinCancelled
    assert isinstance(result, str)
    return result
