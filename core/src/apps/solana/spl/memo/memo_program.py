from collections import namedtuple

from trezor import wire

from apps.solana.publickey import PublicKey

#     """Create memo transaction params."""
#     signer: PublicKey
#     """Signing account."""
#     message: bytes
#     """Memo message in bytes."""


MemoParams = namedtuple("MemoParams", ["signer", "message"])


async def parse(ctx: wire.Context, accounts: list[PublicKey], data: bytes) -> None:
    """Parse memo instruction params."""
    from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

    signer = accounts[0]
    params = MemoParams(signer=signer, message=data)
    from apps.common.signverify import decode_message

    memo = decode_message(params.message)
    from trezor.ui.layouts.lvgl import confirm_sol_memo

    await confirm_sol_memo(ctx, _(i18n_keys.TITLE__MEMO), memo, str(signer))
