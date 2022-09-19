from typing import Callable, Iterable

from trezor import strings, ui, wire
from trezor.crypto.slip39 import MAX_SHARE_COUNT
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs.common import FullSizeWindow
from trezor.lvglui.scrs.request_word import WordEnter

from ...components.common.confirm import raise_if_cancelled
from ...components.tt.confirm import Confirm
from ...components.tt.scroll import Paginated
from ...components.tt.text import Text
from .common import button_request, interact


async def request_word_count(ctx: wire.GenericContext, dry_run: bool) -> int:
    await button_request(ctx, "word_count", code=ButtonRequestType.MnemonicWordCount)

    if dry_run:
        title = _(i18n_keys.TITLE__CHECK_RECOVERY_PHRASE)
    else:
        title = _(i18n_keys.TITLE__READY_TO_RESTORE)
    subtitle = _(i18n_keys.SUBTITLE__DEVICE_RECOVER_READY_TO_RESTORE)
    screen = FullSizeWindow(
        title,
        subtitle,
        confirm_text=_(i18n_keys.BUTTON__CONTINUE),
        options="12\n18\n24",
    )
    count = await ctx.wait(screen.request())
    # WordSelector can return int, or string if the value came from debuglink
    # ctx.wait has a return type Any
    # Hence, it is easier to convert the returned value to int explicitly
    return int(count)


async def request_word(
    ctx: wire.GenericContext, word_index: int, word_count: int, is_slip39: bool
) -> str:
    assert is_slip39 is False
    title = _(i18n_keys.TITLE__ENTER_WORD_STR).format(word_index + 1)
    screen = WordEnter(title)
    word: str = await ctx.wait(screen.request())
    return word


async def show_remaining_shares(
    ctx: wire.GenericContext,
    groups: Iterable[tuple[int, tuple[str, ...]]],  # remaining + list 3 words
    shares_remaining: list[int],
    group_threshold: int,
) -> None:
    pages: list[ui.Component] = []
    for remaining, group in groups:
        if 0 < remaining < MAX_SHARE_COUNT:
            text = Text("Remaining Shares")
            text.bold(
                strings.format_plural(
                    "{count} more {plural} starting", remaining, "share"
                )
            )
            for word in group:
                text.normal(word)
            pages.append(text)
        elif (
            remaining == MAX_SHARE_COUNT and shares_remaining.count(0) < group_threshold
        ):
            text = Text("Remaining Shares")
            groups_remaining = group_threshold - shares_remaining.count(0)
            text.bold(
                strings.format_plural(
                    "{count} more {plural} starting", groups_remaining, "group"
                )
            )
            for word in group:
                text.normal(word)
            pages.append(text)

    pages[-1] = Confirm(pages[-1], cancel=None)
    await raise_if_cancelled(
        interact(ctx, Paginated(pages), "show_shares", ButtonRequestType.Other)
    )


async def show_group_share_success(
    ctx: wire.GenericContext, share_index: int, group_index: int
) -> None:
    text = Text("Success", ui.ICON_CONFIRM)
    text.bold("You have entered")
    text.bold(f"Share {share_index + 1}")
    text.normal("from")
    text.bold(f"Group {group_index + 1}")

    await raise_if_cancelled(
        interact(
            ctx,
            Confirm(text, confirm="Continue", cancel=None),
            "share_success",
            ButtonRequestType.Other,
        )
    )


async def continue_recovery(
    ctx: wire.GenericContext,
    button_label: str,
    text: str,
    subtext: str | None,
    info_func: Callable | None,
) -> bool:
    screen = FullSizeWindow(
        text,
        _(i18n_keys.SUBTITLE__DEVICE_RECOVER_SELECT_NUMBER_OF_WORDS),
        confirm_text=button_label,
        cancel_text=_(i18n_keys.BUTTON__CANCEL),
    )
    return await interact(
        ctx,
        screen,
        "recovery",
        ButtonRequestType.RecoveryHomepage,
    )
