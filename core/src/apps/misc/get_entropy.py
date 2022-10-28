from typing import TYPE_CHECKING

from trezor.crypto import random
from trezor.enums import ButtonRequestType
from trezor.messages import Entropy
from trezor.ui.layouts import confirm_action

if TYPE_CHECKING:
    from trezor.wire import Context
    from trezor.messages import GetEntropy


async def get_entropy(ctx: Context, msg: GetEntropy) -> Entropy:
    from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

    await confirm_action(
        ctx,
        "get_entropy",
        _(i18n_keys.TITLE__CONFIRM_ENTROPY),
        description=_(i18n_keys.SUBTITLE__CONFIRM_ENTROPY),
        br_code=ButtonRequestType.ProtectCall,
        anim_dir=2,
    )

    size = min(msg.size, 1024)
    entropy = random.bytes(size)

    return Entropy(entropy=entropy)
