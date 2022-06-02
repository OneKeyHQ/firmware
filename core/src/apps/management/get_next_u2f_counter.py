import storage.device
from trezor import ui, wire
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.messages import GetNextU2FCounter, NextU2FCounter
from trezor.ui.layouts import confirm_action


async def get_next_u2f_counter(
    ctx: wire.Context, msg: GetNextU2FCounter
) -> NextU2FCounter:
    if not storage.device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")
    await confirm_action(
        ctx,
        "get_u2f_counter",
        title=_(i18n_keys.TITLE__GET_NEXT_U2F_COUNTER),
        description=_(i18n_keys.SUBTITLE__GET_NEXT_U2F_COUNTER),
        icon=ui.ICON_CONFIG,
        br_code=ButtonRequestType.ProtectCall,
    )

    return NextU2FCounter(u2f_counter=storage.device.next_u2f_counter())
