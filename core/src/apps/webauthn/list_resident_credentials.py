from trezor import wire
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.messages import (
    WebAuthnCredential,
    WebAuthnCredentials,
    WebAuthnListResidentCredentials,
)
from trezor.ui.layouts import confirm_action

from . import resident_credentials


async def list_resident_credentials(
    ctx: wire.Context, msg: WebAuthnListResidentCredentials
) -> WebAuthnCredentials:
    await confirm_action(
        ctx,
        "credentials_list",
        title=_(i18n_keys.TITLE__LIST_CREDENTIALS),
        description=_(i18n_keys.SUBTITLE__LIST_CREDENTIALS),
    )
    creds = [
        WebAuthnCredential(
            index=cred.index,
            id=cred.id,
            rp_id=cred.rp_id,
            rp_name=cred.rp_name,
            user_id=cred.user_id,
            user_name=cred.user_name,
            user_display_name=cred.user_display_name,
            creation_time=cred.creation_time,
            hmac_secret=cred.hmac_secret,
            use_sign_count=cred.use_sign_count,
            algorithm=cred.algorithm,
            curve=cred.curve,
        )
        for cred in resident_credentials.find_all()
    ]
    return WebAuthnCredentials(credentials=creds)
