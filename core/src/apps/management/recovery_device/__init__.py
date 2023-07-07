from typing import TYPE_CHECKING

import storage
import storage.recovery
from trezor import config, loop, utils, wire
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, i18n_refresh, keys as i18n_keys
from trezor.messages import Success
from trezor.ui.layouts import confirm_action, confirm_reset_device

from apps.common.request_pin import (
    error_pin_invalid,
    request_pin_and_sd_salt,
    request_pin_confirm,
)

from .homescreen import recovery_process

if TYPE_CHECKING:
    from trezor.messages import RecoveryDevice


# List of RecoveryDevice fields that can be set when doing dry-run recovery.
# All except `dry_run` are allowed for T1 compatibility, but their values are ignored.
# If set, `enforce_wordlist` must be True, because we do not support non-enforcing.
DRY_RUN_ALLOWED_FIELDS = ("dry_run", "word_count", "enforce_wordlist", "type")


async def recovery_device(ctx: wire.Context, msg: RecoveryDevice) -> Success:
    """
    Recover BIP39/SLIP39 seed into empty device.
    Recovery is also possible with replugged Trezor. We call this process Persistence.
    User starts the process here using the RecoveryDevice msg and then they can unplug
    the device anytime and continue without a computer.
    """
    _validate(msg)
    if not msg.dry_run:
        from trezor.ui.layouts import show_popup

        if msg.language is not None:
            i18n_refresh(msg.language)
        await show_popup(_(i18n_keys.TITLE__PLEASE_WAIT), None, timeout_ms=1000)
        # wipe storage to make sure the device is in a clear state
        storage.reset()
        if msg.language is not None:
            storage.device.set_language(msg.language)
            i18n_refresh()
    if storage.recovery.is_in_progress():
        return await recovery_process(ctx)

    await _continue_dialog(ctx, msg)

    if isinstance(ctx, wire.DummyContext):
        utils.play_dead()

    try:  # for dry run pin needs to be entered
        if msg.dry_run:
            curpin, salt = await request_pin_and_sd_salt(
                ctx, _(i18n_keys.TITLE__ENTER_PIN)
            )
            if not config.check_pin(curpin, salt):
                await error_pin_invalid(ctx)
        newpin = None

        if not msg.dry_run:
            # set up pin if requested
            if msg.pin_protection:
                newpin = await request_pin_confirm(ctx, allow_cancel=False)
                # config.change_pin("", newpin, None, None)

            storage.device.set_passphrase_enabled(bool(msg.passphrase_protection))
            if msg.u2f_counter is not None:
                storage.device.set_u2f_counter(msg.u2f_counter)
            if msg.label is not None:
                storage.device.set_label(msg.label)

        storage.recovery.set_in_progress(True)
        storage.recovery.set_dry_run(bool(msg.dry_run))
        # workflow.set_default(recovery_homescreen)
        result = await recovery_process(ctx)
        if Success.is_type_of(result):
            if newpin is not None:
                config.change_pin("", newpin, None, None)
                config.unlock(newpin, None)
    except BaseException as e:
        raise e
    else:
        return result
    finally:
        if isinstance(ctx, wire.DummyContext):
            if msg.dry_run:
                utils.set_up()
            else:
                loop.clear()


def _validate(msg: RecoveryDevice) -> None:
    if not msg.dry_run and storage.device.is_initialized():
        raise wire.UnexpectedMessage("Already initialized")
    if msg.dry_run and not storage.device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")

    if msg.enforce_wordlist is False:
        raise wire.ProcessError(
            "Value enforce_wordlist must be True, Trezor Core enforces words automatically."
        )

    if msg.dry_run:
        # check that only allowed fields are set
        for key, value in msg.__dict__.items():
            if key not in DRY_RUN_ALLOWED_FIELDS and value is not None:
                raise wire.ProcessError(f"Forbidden field set in dry-run: {key}")


async def _continue_dialog(ctx: wire.Context, msg: RecoveryDevice) -> None:
    if not msg.dry_run:
        await confirm_reset_device(
            ctx, _(i18n_keys.SUBTITLE__DEVICE_RECOVER_RESTORE_WALLET), recovery=True
        )
    else:
        await confirm_action(
            ctx,
            "confirm_seedcheck",
            title=_(i18n_keys.TITLE__CHECK_RECOVERY_PHRASE),
            description=_(
                i18n_keys.SUBTITLE__DEVICE_RECOVER_CHECK_CHECK_RECOVERY_PHRASE
            ),
            verb=_(i18n_keys.BUTTON__CONTINUE),
            icon="A:/res/check-seed.png",
            br_code=ButtonRequestType.ProtectCall,
            anim_dir=2,
        )
