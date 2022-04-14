from micropython import const
from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor import ui, wire
from trezor.enums import ButtonRequestType
from trezor.utils import chunks, chunks_intersperse

from ...components.common.confirm import (
    CONFIRMED,
    GO_BACK,
    SHOW_PAGINATED,
)
from ...components.tt.button import ButtonDefault
from ...components.tt.confirm import Confirm, HoldToConfirm, InfoConfirm
from ...components.tt.scroll import (
    PAGEBREAK,
    AskPaginated,
    Paginated,
    paginate_paragraphs,
)
from ...components.tt.text import LINE_WIDTH_PAGINATED, Span, Text
from ...constants.tt import MONO_ADDR_PER_LINE, MONO_HEX_PER_LINE, TEXT_MAX_LINES
from .common import button_request, interact, raise_if_cancelled

if TYPE_CHECKING:
    from typing import Any, Awaitable, Iterable, Iterator, NoReturn, Sequence

    from ..common import PropertyType, ExceptionType


__all__ = (
    "confirm_action",
    "confirm_address",
    "confirm_text",
    "confirm_amount",
    "confirm_reset_device",
    "confirm_backup",
    "confirm_path_warning",
    "confirm_sign_identity",
    "confirm_signverify",
    "show_address",
    "show_error_and_raise",
    "show_pubkey",
    "show_success",
    "show_xpub",
    "show_warning",
    "confirm_output",
    "confirm_payment_request",
    "confirm_blob",
    "confirm_properties",
    "confirm_total",
    "confirm_joint_total",
    "confirm_metadata",
    "confirm_replacement",
    "confirm_modify_output",
    "confirm_modify_fee",
    "confirm_coinjoin",
    "show_popup",
    "draw_simple_text",
    "request_passphrase_on_device",
    "request_pin_on_device",
    "should_show_more",
)


async def confirm_action(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    action: str | None = None,
    description: str | None = None,
    description_param: str | None = None,
    description_param_font: int = ui.BOLD,
    verb: str = "Confirm",
    verb_cancel: str = "Cancel",
    hold: bool = False,
    hold_danger: bool = False,
    icon: str | None = "A:/res/shriek.png",  # TODO cleanup @ redesign
    icon_color: int | None = None,  # TODO cleanup @ redesign
    reverse: bool = False,  # TODO cleanup @ redesign
    larger_vspace: bool = False,  # TODO cleanup @ redesign
    exc: ExceptionType = wire.ActionCancelled,
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> None:
    from trezor.lvglui.scrs.common import FullSizeWindow

    if description and description_param:
        description = description.format(description_param)
    confirm_screen = FullSizeWindow(
        title,
        f"{description}{' ' + (action or '')}",
        verb,
        cancel_text=verb_cancel,
        icon_path=icon,
    )
    await raise_if_cancelled(
        interact(ctx, confirm_screen, br_type, br_code),
        exc,
    )


async def confirm_reset_device(
    ctx: wire.GenericContext, prompt: str, recovery: bool = False
) -> None:
    from trezor.lvglui.scrs.common import FullSizeWindow

    if recovery:
        title = "Recovery mode"
        icon = "A:/res/recovery.png"
    else:
        title = "Create new wallet"
        icon = "A:/res/add.png"
    description = prompt
    confirm_text = "Confirm"
    restscreen = FullSizeWindow(title, description, confirm_text, icon_path=icon)
    await raise_if_cancelled(
        interact(
            ctx,
            restscreen,
            "recover_device" if recovery else "setup_device",
            ButtonRequestType.ProtectCall
            if recovery
            else ButtonRequestType.ResetDevice,
        )
    )


async def confirm_wipe_device(ctx: wire.GenericContext):
    from trezor.lvglui.scrs.wipe_device import WipeDevice

    confirm_screen = WipeDevice()
    await raise_if_cancelled(
        interact(ctx, confirm_screen, "wipe_device", ButtonRequestType.WipeDevice)
    )


async def confirm_wipe_device_tips(ctx: wire.GenericContext):
    from trezor.lvglui.scrs.wipe_device import WipeDeviceTips

    confirm_screen = WipeDeviceTips()
    await raise_if_cancelled(
        interact(ctx, confirm_screen, "wipe_device", ButtonRequestType.WipeDevice)
    )


async def confirm_wipe_device_success(ctx: wire.GenericContext):
    from trezor.lvglui.scrs.wipe_device import WipeDeviceSuccess

    confirm_screen = WipeDeviceSuccess()
    return await interact(
        ctx, confirm_screen, "wipe_device", ButtonRequestType.WipeDevice
    )


# TODO cleanup @ redesign
async def confirm_backup(ctx: wire.GenericContext) -> bool:
    from trezor.lvglui.scrs.common import FullSizeWindow

    title = "Success"
    subtitle = (
        "New wallet created successfully! You should back up your new wallet right now."
    )
    icon = "A:/res/success_icon.png"

    screen = FullSizeWindow(title, subtitle, "Backup", "Skip", icon_path=icon)
    confirmed = await interact(
        ctx,
        screen,
        "backup_device",
        ButtonRequestType.ResetDevice,
    )
    if confirmed:
        from trezor.lvglui.scrs.reset_device import BackupTips

        await BackupTips().request()
        return True

    title = "Warning"
    subtitle = "Are you sure you want to skip the backup? You can back up your OneKey once, at any time."
    icon = "A:/res/shriek.png"
    screen = FullSizeWindow(title, subtitle, "Backup", "Skip", icon_path=icon)
    confirmed = await interact(
        ctx,
        screen,
        "backup_device",
        ButtonRequestType.ResetDevice,
    )
    return bool(confirmed)


async def confirm_path_warning(
    ctx: wire.GenericContext, path: str, path_type: str = "Path"
) -> None:
    from trezor.lvglui.scrs.common import FullSizeWindow

    await raise_if_cancelled(
        interact(
            ctx,
            FullSizeWindow(
                "Confirm path",
                f"{path_type}: {path} is unknown, Are you sure?",
                "Confirm",
                "Cancel",
                icon_path="A:/res/warning.png",
            ),
            "path_warning",
            ButtonRequestType.UnknownDerivationPath,
        )
    )


def _truncate_hex(
    hex_data: str,
    lines: int = TEXT_MAX_LINES,
    width: int = MONO_HEX_PER_LINE,
    middle: bool = False,
    ellipsis: str = "...",  # TODO: cleanup @ redesign
) -> Iterator[str]:
    ell_len = len(ellipsis)
    if len(hex_data) > width * lines:
        if middle:
            hex_data = (
                hex_data[: lines * width // 2 - (ell_len // 2)]
                + ellipsis
                + hex_data[-lines * width // 2 + (ell_len - ell_len // 2) :]
            )
        else:
            hex_data = hex_data[: (width * lines - ell_len)] + ellipsis
    return chunks_intersperse(hex_data, width)


def _show_xpub(xpub: str, title: str, cancel: str) -> Paginated:
    pages: list[ui.Component] = []
    for lines in chunks(list(chunks_intersperse(xpub, 16)), TEXT_MAX_LINES * 2):
        text = Text(title, ui.ICON_RECEIVE, ui.GREEN, new_lines=False)
        text.mono(*lines)
        pages.append(text)

    content = Paginated(pages)

    content.pages[-1] = Confirm(
        content.pages[-1],
        cancel=cancel,
        cancel_style=ButtonDefault,
    )

    return content


async def show_xpub(
    ctx: wire.GenericContext,
    xpub: str,
    title: str = "",
    cancel: str = "",
    path: str = "",
    network: str = "BTC",
) -> None:
    from trezor.lvglui.scrs.template import XpubOrPub

    await raise_if_cancelled(
        interact(
            ctx,
            XpubOrPub(f"{network} Public Key", path=path, xpub=xpub),
            "show_pubkey",
            ButtonRequestType.PublicKey,
        )
    )


async def show_address(
    ctx: wire.GenericContext,
    address: str,
    *,
    address_qr: str | None = None,
    case_sensitive: bool = True,
    address_n: str = "Confirm address",
    network: str | None = None,
    multisig_index: int | None = None,
    xpubs: Sequence[str] = (),
    address_extra: str | None = None,
    title_qr: str | None = None,
    title: str = "",
) -> None:
    is_multisig = len(xpubs) > 0
    from trezor.lvglui.scrs.template import Address

    while True:
        if await interact(
            ctx,
            Address(f"{network.upper()} Address", address_n, address),
            "show_address",
            ButtonRequestType.Address,
        ):
            break
        # TODO: MULTISIG @ redesign
        if is_multisig:
            await interact(
                ctx,
                Address(
                    f"{network.upper()} MultisigAddress {title}",
                    address_n,
                    address,
                    xpubs,
                    multisig_index,
                ),
                ButtonRequestType.Address,
            )


async def show_pubkey(
    ctx: wire.Context,
    pubkey: str,
    title: str = "Confirm public key",
    network: str = "ETH",
    path: str = "",
) -> None:
    from trezor.lvglui.scrs.template import XpubOrPub

    await raise_if_cancelled(
        interact(
            ctx,
            XpubOrPub(f"{network} Public Key", path=path, pubkey=pubkey),
            "show_pubkey",
            ButtonRequestType.PublicKey,
        )
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
    from trezor.lvglui.scrs.common import FullSizeWindow

    screen = FullSizeWindow(
        header,
        content,
        confirm_text=button_confirm,
        cancel_text=button_cancel,
        icon_path=icon,
    )
    await raise_if_cancelled(
        interact(
            ctx,
            screen,
            br_type,
            br_code,
        ),
        exc,
    )


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
        icon="A:/res/danger.png",
        icon_color=ui.RED if red else ui.ORANGE_ICON,
        exc=exc,
    )
    raise exc


def show_warning(
    ctx: wire.GenericContext,
    br_type: str,
    content: str,
    header: str = "Warning",
    subheader: str | None = None,
    button: str = "Try again",
    br_code: ButtonRequestType = ButtonRequestType.Warning,
    icon: str = "A:/res/warning.png",
    icon_color: int = ui.RED,
) -> Awaitable[None]:
    return _show_modal(
        ctx,
        br_type=br_type,
        br_code=br_code,
        header=header,
        subheader=subheader,
        content=content,
        button_confirm=button,
        button_cancel=None,
        icon=icon,
        icon_color=icon_color,
    )


def show_success(
    ctx: wire.GenericContext,
    br_type: str,
    content: str,
    subheader: str | None = None,
    button: str = "Done",
) -> Awaitable[None]:
    return _show_modal(
        ctx,
        br_type=br_type,
        br_code=ButtonRequestType.Success,
        header="Success",
        subheader=subheader,
        content=content,
        button_confirm=button,
        button_cancel=None,
        icon="A:/res/success_icon.png",
        icon_color=ui.GREEN,
    )


async def confirm_output(
    ctx: wire.GenericContext,
    address: str,
    amount: str,
    font_amount: int = ui.NORMAL,  # TODO cleanup @ redesign
    title: str = "Overview Transaction",
    subtitle: str | None = None,  # TODO cleanup @ redesign
    color_to: int = ui.FG,  # TODO cleanup @ redesign
    to_str: str = " to\n",  # TODO cleanup @ redesign
    to_paginated: bool = False,  # TODO cleanup @ redesign
    width: int = MONO_ADDR_PER_LINE,
    width_paginated: int = MONO_ADDR_PER_LINE - 1,
    br_code: ButtonRequestType = ButtonRequestType.ConfirmOutput,
    icon: str = ui.ICON_SEND,
) -> None:
    from trezor.lvglui.scrs.template import TransactionOverview

    await raise_if_cancelled(
        interact(
            ctx, TransactionOverview(title, amount, address), "confirm_output", br_code
        )
    )


async def confirm_payment_request(
    ctx: wire.GenericContext,
    recipient_name: str,
    amount: str,
    memos: list[str],
) -> Any:
    para = [(ui.NORMAL, f"{amount} to\n{recipient_name}")]
    para.extend((ui.NORMAL, memo) for memo in memos)
    content = paginate_paragraphs(
        para,
        "Confirm sending",
        ui.ICON_SEND,
        ui.GREEN,
        confirm=lambda text: InfoConfirm(text, info="Details"),
    )
    return await raise_if_cancelled(
        interact(
            ctx, content, "confirm_payment_request", ButtonRequestType.ConfirmOutput
        )
    )


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
    """Return True if the user wants to show more (they click a special button)
    and False when the user wants to continue without showing details.

    Raises ActionCancelled if the user cancels.
    """
    page = Text(
        title,
        header_icon=icon,
        icon_color=icon_color,
        new_lines=False,
        max_lines=TEXT_MAX_LINES - 2,
    )
    for font, text in para:
        page.content.extend((font, text, "\n"))
    ask_dialog = Confirm(AskPaginated(page, button_text))

    result = await raise_if_cancelled(interact(ctx, ask_dialog, br_type, br_code))
    assert result in (SHOW_PAGINATED, CONFIRMED)

    return result is SHOW_PAGINATED


async def _confirm_ask_pagination(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    para: Iterable[tuple[int, str]],
    para_truncated: Iterable[tuple[int, str]],
    br_code: ButtonRequestType,
    icon: str,
    icon_color: int,
) -> None:
    paginated: ui.Layout | None = None
    while True:
        if not await should_show_more(
            ctx,
            title,
            para=para_truncated,
            br_type=br_type,
            br_code=br_code,
            icon=icon,
            icon_color=icon_color,
        ):
            return

        if paginated is None:
            paginated = paginate_paragraphs(
                para,
                header=None,
                back_button=True,
                confirm=lambda content: Confirm(
                    content, cancel=None, confirm="Close", confirm_style=ButtonDefault
                ),
            )
        result = await interact(ctx, paginated, br_type, br_code)
        assert result in (CONFIRMED, GO_BACK)

    assert False


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
    """Confirm data blob.

    Applicable for public keys, signatures, hashes. In general, any kind of
    data that is not human-readable, and can be wrapped at any character.

    For addresses, use `confirm_address`.

    Displays in monospace font. Paginates automatically.
    If data is provided as bytes or bytearray, it is converted to hex.
    """
    from trezor.lvglui.scrs.template import BlobDisPlay

    if isinstance(data, (bytes, bytearray)):
        data_str = hexlify(data).decode()
    else:
        data_str = data
    blob = BlobDisPlay(title, description, data_str)
    return await raise_if_cancelled(interact(ctx, blob, br_type, br_code))


async def confirm_data(
    ctx: wire.GenericContext,
    br_type: str,
    title: str,
    data: bytes | str,
    description: str | None = None,
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> None:
    from trezor.lvglui.scrs.template import ContractDataOverview

    if isinstance(data, (bytes, bytearray)):
        data_str = "0x" + hexlify(data).decode()
    else:
        data_str = data
    return await raise_if_cancelled(
        interact(
            ctx, ContractDataOverview(title, description, data_str), br_type, br_code
        )
    )


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
    # TODO clarify API - this should be pretty limited to support mainly confirming
    # destinations and similar
    return confirm_blob(
        ctx,
        br_type=br_type,
        title=title,
        data=address,
        description=description,
        br_code=br_code,
        icon=icon,
        icon_color=icon_color,
    )


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
    """Confirm textual data.

    Applicable for human-readable strings, numbers, date/time values etc.

    For amounts, use `confirm_amount`.

    Displays in bold font. Paginates automatically.
    """
    span = Span()
    lines = 0
    if description is not None:
        span.reset(description, 0, ui.NORMAL)
        lines += span.count_lines()
    span.reset(data, 0, ui.BOLD)
    lines += span.count_lines()

    if lines <= TEXT_MAX_LINES:
        text = Text(title, icon, icon_color, new_lines=False)
        if description is not None:
            text.normal(description)
            text.br()
        text.bold(data)
        content: ui.Layout = Confirm(text)

    else:
        para = []
        if description is not None:
            para.append((ui.NORMAL, description))
        para.append((ui.BOLD, data))
        content = paginate_paragraphs(para, title, icon, icon_color)
    await raise_if_cancelled(interact(ctx, content, br_type, br_code))


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
    """Confirm amount."""
    # TODO clarify API - this should be pretty limited to support mainly confirming
    # destinations and similar
    return confirm_text(
        ctx,
        br_type=br_type,
        title=title,
        data=amount,
        description=description,
        br_code=br_code,
        icon=icon,
        icon_color=icon_color,
    )


_SCREEN_FULL_THRESHOLD = const(2)


# TODO keep name and value on the same page if possible
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
    span = Span()
    para = []
    used_lines = 0
    for key, val in props:
        span.reset(key or "", 0, ui.NORMAL, line_width=LINE_WIDTH_PAGINATED)
        key_lines = span.count_lines()

        if isinstance(val, str):
            span.reset(val, 0, ui.BOLD, line_width=LINE_WIDTH_PAGINATED)
            val_lines = span.count_lines()
        elif isinstance(val, bytes):
            val_lines = (len(val) * 2 + MONO_HEX_PER_LINE - 1) // MONO_HEX_PER_LINE
        else:
            val_lines = 0

        remaining_lines = TEXT_MAX_LINES - used_lines
        used_lines = (used_lines + key_lines + val_lines) % TEXT_MAX_LINES

        if key_lines + val_lines > remaining_lines:
            if remaining_lines <= _SCREEN_FULL_THRESHOLD:
                # there are only 2 remaining lines, don't try to fit and put everything
                # on next page
                para.append(PAGEBREAK)
                used_lines = (key_lines + val_lines) % TEXT_MAX_LINES

            elif val_lines > 0 and key_lines >= remaining_lines:
                # more than 2 remaining lines so try to fit something -- but won't fit
                # at least one line of value
                para.append(PAGEBREAK)
                used_lines = (key_lines + val_lines) % TEXT_MAX_LINES

            elif key_lines + val_lines <= TEXT_MAX_LINES:
                # Whole property won't fit to the page, but it will fit on a page
                # by itself
                para.append(PAGEBREAK)
                used_lines = (key_lines + val_lines) % TEXT_MAX_LINES

            # else:
            # None of the above. Continue fitting on the same page.

        if key:
            para.append((ui.NORMAL, key))
        if isinstance(val, bytes):
            para.extend(
                (ui.MONO, line)
                for line in chunks(hexlify(val).decode(), MONO_HEX_PER_LINE - 2)
            )
        elif isinstance(val, str):
            para.append((ui.BOLD, val))
    content = paginate_paragraphs(
        para, title, icon, icon_color, confirm=HoldToConfirm if hold else Confirm
    )
    await raise_if_cancelled(interact(ctx, content, br_type, br_code))


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
    text = Text(title, ui.ICON_SEND, icon_color, new_lines=False)
    text.normal(total_label)
    text.bold(total_amount)
    text.normal(fee_label)
    text.bold(fee_amount)
    await raise_if_cancelled(interact(ctx, HoldToConfirm(text), br_type, br_code))


async def confirm_joint_total(
    ctx: wire.GenericContext, spending_amount: str, total_amount: str
) -> None:
    text = Text("Joint transaction", ui.ICON_SEND, ui.GREEN, new_lines=False)
    text.normal("You are contributing:\n")
    text.bold(spending_amount)
    text.normal("\nto the total amount:\n")
    text.bold(total_amount)
    await raise_if_cancelled(
        interact(
            ctx, HoldToConfirm(text), "confirm_joint_total", ButtonRequestType.SignTx
        )
    )


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
    text = Text(title, icon, icon_color, new_lines=False)
    text.format_parametrized(
        content, param if param is not None else "", param_font=param_font
    )

    if not hide_continue:
        text.br()
        if larger_vspace:
            text.br_half()
        text.normal("Continue?")

    cls = HoldToConfirm if hold else Confirm

    await raise_if_cancelled(interact(ctx, cls(text), br_type, br_code))


async def confirm_replacement(
    ctx: wire.GenericContext, description: str, txid: str
) -> None:
    text = Text(description, ui.ICON_SEND, ui.GREEN, new_lines=False)
    text.normal("Confirm transaction ID:\n")
    text.mono(*_truncate_hex(txid, TEXT_MAX_LINES - 1))
    await raise_if_cancelled(
        interact(ctx, Confirm(text), "confirm_replacement", ButtonRequestType.SignTx)
    )


async def confirm_modify_output(
    ctx: wire.GenericContext,
    address: str,
    sign: int,
    amount_change: str,
    amount_new: str,
) -> None:
    page1 = Text("Modify amount", ui.ICON_SEND, ui.GREEN, new_lines=False)
    page1.normal("Address:\n")
    page1.br_half()
    page1.mono(*chunks_intersperse(address, MONO_ADDR_PER_LINE))

    page2 = Text("Modify amount", ui.ICON_SEND, ui.GREEN, new_lines=False)
    if sign < 0:
        page2.normal("Decrease amount by:\n")
    else:
        page2.normal("Increase amount by:\n")
    page2.bold(amount_change)
    page2.br_half()
    page2.normal("\nNew amount:\n")
    page2.bold(amount_new)

    await raise_if_cancelled(
        interact(
            ctx,
            Paginated([page1, Confirm(page2)]),
            "modify_output",
            ButtonRequestType.ConfirmOutput,
        )
    )


async def confirm_modify_fee(
    ctx: wire.GenericContext,
    sign: int,
    user_fee_change: str,
    total_fee_new: str,
) -> None:
    text = Text("Modify fee", ui.ICON_SEND, ui.GREEN, new_lines=False)
    if sign == 0:
        text.normal("Your fee did not change.\n")
    else:
        if sign < 0:
            text.normal("Decrease your fee by:\n")
        else:
            text.normal("Increase your fee by:\n")
        text.bold(user_fee_change)
        text.br()
    text.br_half()
    text.normal("Transaction fee:\n")
    text.bold(total_fee_new)
    await raise_if_cancelled(
        interact(ctx, HoldToConfirm(text), "modify_fee", ButtonRequestType.SignTx)
    )


async def confirm_coinjoin(
    ctx: wire.GenericContext, coin_name: str, max_rounds: int, max_fee_per_vbyte: str
) -> None:
    text = Text("Authorize CoinJoin", ui.ICON_RECOVERY, new_lines=False)
    text.normal("Coin name: ")
    text.bold(f"{coin_name}\n")
    text.br_half()
    text.normal("Maximum rounds: ")
    text.bold(f"{max_rounds}\n")
    text.br_half()
    text.normal("Maximum mining fee:\n")
    text.bold(f"{max_fee_per_vbyte} sats/vbyte")
    await raise_if_cancelled(
        interact(ctx, HoldToConfirm(text), "coinjoin_final", ButtonRequestType.Other)
    )


# TODO cleanup @ redesign
async def confirm_sign_identity(
    ctx: wire.GenericContext, proto: str, identity: str, challenge_visual: str | None
) -> None:
    text = Text(f"Sign {proto}", new_lines=False)
    if challenge_visual:
        text.normal(challenge_visual)
        text.br()
    text.mono(*chunks_intersperse(identity, 18))
    await raise_if_cancelled(
        interact(ctx, Confirm(text), "sign_identity", ButtonRequestType.Other)
    )


async def confirm_signverify(
    ctx: wire.GenericContext, coin: str, message: str, address: str, verify: bool
) -> None:
    if verify:
        header = f"Verify {coin} message"
        br_type = "verify_message"
    else:
        header = f"Sign {coin} message"
        br_type = "sign_message"
    from trezor.lvglui.scrs.template import Message

    await raise_if_cancelled(
        interact(
            ctx, Message(header, address, message), br_type, ButtonRequestType.Other
        )
    )


async def show_popup(
    title: str,
    description: str,
    subtitle: str | None = None,
    description_param: str = "",
    timeout_ms: int = 3000,
) -> None:
    from trezor.lvglui.scrs.components.popup import PopupSample

    if description and description_param:
        description = description.format(description_param)
    subtitle = f"{subtitle or ''} {description or ''}"
    PopupSample(title, subtitle, "A:/res/warning.png", timeout_ms)


def draw_simple_text(title: str, description: str = "") -> None:
    from trezor.lvglui.scrs.common import FullSizeWindow

    FullSizeWindow(title, description, icon_path="A:/res/shriek.png")


async def request_passphrase_on_device(ctx: wire.GenericContext, max_len: int) -> str:
    await button_request(
        ctx, "passphrase_device", code=ButtonRequestType.PassphraseEntry
    )
    from trezor.lvglui.scrs.passphrase import PassphraseRequest

    screen = PassphraseRequest()
    result = await ctx.wait(screen.request())
    if not result:
        raise wire.ActionCancelled("Passphrase entry cancelled")

    assert isinstance(result, str)
    return result


async def request_pin_on_device(
    ctx: wire.GenericContext,
    prompt: str,
    attempts_remaining: int | None,
    allow_cancel: bool,
) -> str:
    await button_request(ctx, "pin_device", code=ButtonRequestType.PinEntry)
    if attempts_remaining is None:
        subprompt = ""
    elif attempts_remaining == 1:
        subprompt = "This is your last attempt"
    else:
        subprompt = f"{attempts_remaining} attempts remaining"
    from trezor.lvglui.scrs.pinscreen import InputPin

    pinscreen = InputPin(title=prompt, subtitle=subprompt)
    result = await ctx.wait(pinscreen.request())
    if not result:
        raise wire.PinCancelled
    assert isinstance(result, str)
    return result


async def request_pin_tips(ctx: wire.GenericContext) -> None:
    from trezor.lvglui.scrs.pinscreen import PinTip, FullSizeWindow

    tipscreen = PinTip()
    await ctx.wait(tipscreen.request())
