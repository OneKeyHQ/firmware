import storage.device
from trezor import wire
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.messages import Success, WebAuthnAddResidentCredential
from trezor.ui.components.common.webauthn import ConfirmInfo
from trezor.ui.layouts import show_error_and_raise
from trezor.ui.layouts.lvgl.webauthn import confirm_webauthn

from .credential import Fido2Credential
from .resident_credentials import store_resident_credential


class ConfirmAddCredential(ConfirmInfo):
    def __init__(self, cred: Fido2Credential):
        super().__init__()
        self._cred = cred
        self.load_icon(cred.rp_id_hash)

    def get_header(self) -> str:
        return _(i18n_keys.TITLE__IMPORT_CREDENTIAL)

    def app_name(self) -> str:
        return self._cred.app_name()

    def account_name(self) -> str | None:
        return self._cred.account_name()


async def add_resident_credential(
    ctx: wire.Context, msg: WebAuthnAddResidentCredential
) -> Success:
    if not storage.device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")
    if not msg.credential_id:
        raise wire.ProcessError("Missing credential ID parameter.")

    try:
        cred = Fido2Credential.from_cred_id(bytes(msg.credential_id), None)
    except Exception:
        await show_error_and_raise(
            ctx,
            "warning_credential",
            header=_(i18n_keys.TITLE__IMPORT_CREDENTIAL),
            button=_(i18n_keys.BUTTON__CLOSE),
            content=_(i18n_keys.SUBTITLE__IMPORT_CREDENTIAL),
            red=True,
        )

    if not await confirm_webauthn(ctx, ConfirmAddCredential(cred)):
        raise wire.ActionCancelled

    if store_resident_credential(cred):
        return Success(message="Credential added")
    else:
        raise wire.ProcessError("Internal credential storage is full.")
