from . import lv_ui
from .scrs.pinscreen import InputPin
from . import globalvar as gl
from trezor import wire
from trezor.enums import ButtonRequestType

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Sequence

    NumberedWords = Sequence[tuple[int, str]]

from .common import (
    is_confirmed,
    raise_if_cancelled,
    button_request,
    interact,
)

def lv_screen_check(key:str=None):
    value = gl.get_value(key)
    if value:
        value.delete()

async def lv_confirm_reset_device(
    ctx: wire.GenericContext, prompt: str, recovery: bool = False
) -> None:
    lv_screen_check("ui_reset")
    if recovery:
        ui_reset = lv_ui.ScreenGeneric(
            cancel_btn= True,
            title= "Recovery mode",
            description= prompt,
            confirm_text= "Confirm",
        )
    else:
        ui_reset = lv_ui.ScreenGeneric(
            cancel_btn= True,
            title= "Create new wallet",
            description= prompt,
            confirm_text= "Confirm",
        )

    gl.set_value('ui_reset',ui_reset)

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


async def lv_confirm_backup(ctx: wire.GenericContext) -> bool:
    lv_screen_check("ui_backup")
    ui_backup = lv_ui.ScreenGeneric(
        cancel_btn= True,
        title= "New wallet created successfully!",
        description= "You should back up your new wallet right now.",
        confirm_text= "Confirm",
    )
    gl.set_value('ui_backup',ui_backup)

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
    ui_backup = lv_ui.ScreenGeneric(
        cancel_btn= True,
        title= "Are you sure you want to skip the backup?",
        description= "You should back up your new wallet right now.",
        confirm_text= "Confirm",
    )
    gl.set_value('ui_backup',ui_backup)

    confirmed = is_confirmed(
        await interact(
            ctx,
            ui_backup,
            "backup_device",
            ButtonRequestType.ResetDevice,
        )
    )
    return confirmed


async def lv_mnemonic_word_select(
    ctx: wire.GenericContext,
    words: Sequence[str],
    share_index: int | None,
    word_index: int,
    count: int,
    group_index: int | None = None,
):
    lv_screen_check("ui_candidate")
    ui_candidate = lv_ui.ScreenCandidateWords(words, f"Check word {word_index + 1}","Choose the correct word.","Next")
    return await ctx.wait(ui_candidate.candidate_word())


async def lv_confirm_wipe_device(ctx: wire.GenericContext) -> None:

    lv_screen_check("ui_wipe")

    ui_wipe = lv_ui.ScreenGeneric(
        cancel_btn= True,
        title= "Wipe Device",
        description= "To remove all data from your device, you can reset your device to factory default.",
        confirm_text= "Wipe",
        )
    gl.set_value('ui_wipe',ui_wipe)

    await raise_if_cancelled(
        interact(
            ctx,
            ui_wipe,
            "confirm_wipe",
            ButtonRequestType.WipeDevice,
            )
    )


async def lv_show_backup_warning(ctx: wire.GenericContext, slip39: bool = False) -> None:
    if slip39:
        description = "Never make a digital copy of your recovery shares and never upload them online!"
    else:
        description = "Never make a digital copy of your recovery seed and never upload it online!"

    lv_screen_check("ui_backup_warning")

    ui_backup_warning = lv_ui.ScreenGeneric(
        cancel_btn= False,
        title= "Caution",
        description= description,
        confirm_text= "I understand",
        )
    gl.set_value('ui_backup_warning',ui_backup_warning)

    await raise_if_cancelled(
        interact(
            ctx,
            ui_backup_warning,
            "backup_warning",
            ButtonRequestType.ResetDevice,
            )
    )


async def lv_show_share_words(
    ctx: wire.GenericContext,
    share_words: Sequence[str],
    share_index: int | None = None,
    group_index: int | None = None,
) -> None:

    if share_index is None:
        header_title = "Recovery seed"
    elif group_index is None:
        header_title = f"Recovery share #{share_index + 1}"
    else:
        header_title = f"Group {group_index + 1} - Share {share_index + 1}"

    lv_screen_check("ui_show_words")

    ui_show_words = lv_ui.ScreenShowWords(
        title=header_title,
        description= "Write down these " + f"{len(share_words)} words:",
        confirm_text="Confirm",
        share_words=share_words,
        )
    gl.set_value('ui_show_words',ui_show_words)

    # confirm the share
    await raise_if_cancelled(
        interact(
            ctx,
            ui_show_words,
            "backup_words",
            ButtonRequestType.ResetDevice,
            )
    )


async def lv_request_pin_on_device(
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

    pinscreen = InputPin(title=prompt, subtitle=subprompt)

    while True:
        result = await ctx.wait(pinscreen.request())
        if not result:
            raise wire.PinCancelled
        assert isinstance(result, str)
        return result
