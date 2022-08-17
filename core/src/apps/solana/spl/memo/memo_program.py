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

    params = MemoParams(signer=accounts[0], message=data)
    memo = params.message.decode("utf-8")
    from trezor.ui.layouts.lvgl import confirm_sol_memo

    await confirm_sol_memo(
        ctx, _(i18n_keys.TITLE__MEMO), _(i18n_keys.LIST_KEY__MESSAGE__COLON), memo
    )
