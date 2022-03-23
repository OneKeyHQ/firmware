from . import lv_ui
from . import globalvar as gl
from trezor import wire, log
from trezor.enums import ButtonRequestType

from .common import (
    is_confirmed,
    raise_if_cancelled,
    interact,
)

async def lv_confirm_reset_device(
    ctx: wire.GenericContext, prompt: str, recovery: bool = False
) -> None:
    if recovery:
        ui_reset = lv_ui.UiResetDevice("Recovery mode",prompt)
    else:
        ui_reset = lv_ui.UiResetDevice("Create new wallet",prompt)
    
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
    ui_backup = lv_ui.UiBackUp(
        "New wallet created successfully!",
        "You should back up your new wallet right now."
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

    ui_backup2 = lv_ui.UiBackUp(
        "Are you sure you want to skip the backup?",
        "You should back up your new wallet right now."
        )
    gl.set_value('ui_backup2',ui_backup2)

    confirmed = is_confirmed(
        await interact(
            ctx,
            ui_backup2,
            "backup_device",
            ButtonRequestType.ResetDevice,
        )
    )
    return confirmed

async def lv_confirm_wipe_device(ctx: wire.GenericContext) -> bool:
    ui_wipe = lv_ui.UiWipeDevice(
        "Reset Device",
        "To remove all data from your device, you can reset your device to factory default."
        )    
    gl.set_value('ui_wipe',ui_wipe)

    confirmed = is_confirmed(
        await interact(
            ctx,
            ui_wipe,
            "confirm_wipe",
            ButtonRequestType.WipeDevice,
        )
    )
    return confirmed
