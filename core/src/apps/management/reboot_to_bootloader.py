from typing import TYPE_CHECKING

from trezor import io, loop, utils, wire
from trezor.messages import RebootToBootloader, Success
from trezor.ui.layouts import confirm_action

if TYPE_CHECKING:
    from typing import NoReturn

    pass


async def reboot_to_bootloader(ctx: wire.Context, msg: RebootToBootloader) -> NoReturn:
    from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

    await confirm_action(
        ctx,
        "reboot",
        _(i18n_keys.TITLE__SYSTEM_UPDATE),
        description=_(i18n_keys.SUBTITLE__SWITCH_TO_UPDATE_MODE_RECONFIRM),
        icon=None,
    )
    await ctx.write(Success(message="Rebooting"))
    # make sure the outgoing USB buffer is flushed
    await loop.wait(ctx.iface.iface_num() | io.POLL_WRITE)
    utils.reboot_to_bootloader()
    raise RuntimeError
