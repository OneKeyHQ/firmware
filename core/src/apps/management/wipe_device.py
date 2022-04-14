from typing import TYPE_CHECKING

import storage
from trezor import ui, utils
from trezor.enums import ButtonRequestType
from trezor.messages import Success
from trezor.ui.layouts import confirm_action

from ..common.request_pin import verify_user_pin
from .apply_settings import reload_settings_from_storage

if TYPE_CHECKING:
    from trezor import wire
    from trezor.messages import WipeDevice


async def wipe_device(ctx: wire.GenericContext, msg: WipeDevice) -> Success:
    if utils.LVGL_UI:
        from trezor.ui.layouts.lvgl import (
            confirm_wipe_device,
            confirm_wipe_device_tips,
            confirm_wipe_device_success,
        )

        await confirm_wipe_device(ctx)
        # verify user pin
        await verify_user_pin(ctx)
        # show tips
        await confirm_wipe_device_tips(ctx)
        storage.wipe()
        reload_settings_from_storage()
        await confirm_wipe_device_success(ctx)
        return Success(message="Device wiped")
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
            icon="A:/res/trash.png",
            icon_color=ui.RED,
            br_code=ButtonRequestType.WipeDevice,
        )

    storage.wipe()
    reload_settings_from_storage()

    return Success(message="Device wiped")
