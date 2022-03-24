from typing import TYPE_CHECKING

import storage
from trezor import ui, utils
from trezor.enums import ButtonRequestType
from trezor.messages import Success
from trezor.ui.layouts import confirm_action
from lvglui.lv_layouts import lv_confirm_wipe_device

from .apply_settings import reload_settings_from_storage

if TYPE_CHECKING:
    from trezor import wire
    from trezor.messages import WipeDevice


async def wipe_device(ctx: wire.GenericContext, msg: WipeDevice) -> Success:
    if utils.LVGL_UI:
        await lv_confirm_wipe_device(ctx)
    else:
        await confirm_action(
            ctx,
            "confirm_wipe",
            title="Wipe device",
            description="Do you really want to\nwipe the device?\n",
            action="All data will be lost.",
            reverse=True,
            verb="Hold to confirm",
            hold=True,
            hold_danger=True,
            icon=ui.ICON_WIPE,
            icon_color=ui.RED,
            br_code=ButtonRequestType.WipeDevice,
        )
    
    storage.wipe()
    reload_settings_from_storage()

    return Success(message="Device wiped")
