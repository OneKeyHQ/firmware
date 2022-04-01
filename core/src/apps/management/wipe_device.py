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
