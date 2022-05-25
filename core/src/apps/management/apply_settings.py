from typing import TYPE_CHECKING

import storage.device
from trezor import ui, wire
from trezor.enums import ButtonRequestType, SafetyCheckLevel
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.messages import Success
from trezor.strings import format_duration_ms
from trezor.ui.layouts import confirm_action

from apps.base import reload_settings_from_storage
from apps.common import safety_checks

if TYPE_CHECKING:
    from trezor.messages import ApplySettings


def validate_homescreen(homescreen: bytes) -> None:
    if homescreen == b"":
        return

    if len(homescreen) > storage.device.HOMESCREEN_MAXSIZE:
        raise wire.DataError(
            f"Homescreen is too large, maximum size is {storage.device.HOMESCREEN_MAXSIZE} bytes"
        )

    try:
        w, h, grayscale = ui.display.toif_info(homescreen)
    except ValueError:
        raise wire.DataError("Invalid homescreen")
    if w != 144 or h != 144:
        raise wire.DataError("Homescreen must be 144x144 pixel large")
    if grayscale:
        raise wire.DataError("Homescreen must be full-color TOIF image")


async def apply_settings(ctx: wire.Context, msg: ApplySettings) -> Success:
    if not storage.device.is_initialized():
        raise wire.NotInitialized("Device is not initialized")
    if (
        msg.homescreen is None
        and msg.label is None
        and msg.use_passphrase is None
        and msg.passphrase_always_on_device is None
        and msg.display_rotation is None
        and msg.auto_lock_delay_ms is None
        and msg.safety_checks is None
        and msg.experimental_features is None
    ):
        raise wire.ProcessError("No setting provided")

    if msg.homescreen is not None:
        # validate_homescreen(msg.homescreen)
        # await require_confirm_change_homescreen(ctx)
        # try:
        #     storage.device.set_homescreen(msg.homescreen)
        # except ValueError:
        #     raise wire.DataError("Invalid homescreen")
        raise wire.ProcessError("Please set homescreen on the device")

    if msg.label is not None:
        if len(msg.label) > storage.device.LABEL_MAXLENGTH:
            raise wire.DataError("Label too long")
        await require_confirm_change_label(ctx, msg.label)
        storage.device.set_label(msg.label)

    if msg.use_passphrase is not None:
        await require_confirm_change_passphrase(ctx, msg.use_passphrase)
        storage.device.set_passphrase_enabled(msg.use_passphrase)

    if msg.passphrase_always_on_device is not None:
        if not storage.device.is_passphrase_enabled():
            raise wire.DataError("Passphrase is not enabled")
        await require_confirm_change_passphrase_source(
            ctx, msg.passphrase_always_on_device
        )
        storage.device.set_passphrase_always_on_device(msg.passphrase_always_on_device)

    if msg.auto_lock_delay_ms is not None:
        if msg.auto_lock_delay_ms < storage.device.AUTOLOCK_DELAY_MINIMUM:
            raise wire.ProcessError("Auto-lock delay too short")
        if msg.auto_lock_delay_ms > storage.device.AUTOLOCK_DELAY_MAXIMUM:
            raise wire.ProcessError("Auto-lock delay too long")
        await require_confirm_change_autolock_delay(ctx, msg.auto_lock_delay_ms)
        storage.device.set_autolock_delay_ms(msg.auto_lock_delay_ms)

    if msg.safety_checks is not None:
        await require_confirm_safety_checks(ctx, msg.safety_checks)
        safety_checks.apply_setting(msg.safety_checks)

    if msg.display_rotation is not None:
        await require_confirm_change_display_rotation(ctx, msg.display_rotation)
        storage.device.set_rotation(msg.display_rotation)

    if msg.experimental_features is not None:
        await require_confirm_experimental_features(ctx, msg.experimental_features)
        storage.device.set_experimental_features(msg.experimental_features)

    reload_settings_from_storage()

    return Success(message="Settings applied")


async def require_confirm_change_homescreen(ctx: wire.GenericContext) -> None:
    await confirm_action(
        ctx,
        "set_homescreen",
        "Set homescreen",
        description="Do you really want to change the homescreen image?",
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_label(ctx: wire.GenericContext, label: str) -> None:
    await confirm_action(
        ctx,
        "set_label",
        _(i18n_keys.TITLE__CHANGE_LABEL),
        description=_(i18n_keys.SUBTITLE__SET_LABEL_CHANGE_LABEL),
        description_param=label,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_passphrase(
    ctx: wire.GenericContext, use: bool
) -> None:
    if use:
        description = _(i18n_keys.SUBTITLE__SET_PASSPHRASE_ENABLED)
    else:
        description = _(i18n_keys.SUBTITLE__SET_PASSPHRASE_DISABLED)
    await confirm_action(
        ctx,
        "set_passphrase",
        _(i18n_keys.TITLE__ENABLE_PASSPHRASE)
        if use
        else _(i18n_keys.TITLE__DISABLE_PASSPHRASE),
        description=description,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_passphrase_source(
    ctx: wire.GenericContext, passphrase_always_on_device: bool
) -> None:
    if passphrase_always_on_device:
        description = _(i18n_keys.SUBTITLE__SET_PASSPHRASE_ENABLED_FORCE_ON_DEVICE)
    else:
        description = _(i18n_keys.SUBTITLE__SET_PASSPHRASE_ENABLED_NO_FORCE_ON_DEVICE)
    await confirm_action(
        ctx,
        "set_passphrase_source",
        _(i18n_keys.TITLE__PASSPHRASE_SOURCE),
        description=description,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_display_rotation(
    ctx: wire.GenericContext, rotation: int
) -> None:
    if rotation == 0:
        label = "north"
    elif rotation == 90:
        label = "east"
    elif rotation == 180:
        label = "south"
    elif rotation == 270:
        label = "west"
    else:
        raise wire.DataError("Unsupported display rotation")
    await confirm_action(
        ctx,
        "set_rotation",
        "Change rotation",
        description="Do you really want to change display rotation to {}?",
        description_param=label,
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_change_autolock_delay(
    ctx: wire.GenericContext, delay_ms: int
) -> None:
    await confirm_action(
        ctx,
        "set_autolock_delay",
        _(i18n_keys.TITLE__AUTO_LOCK),
        description=_(
            i18n_keys.SUBTITLE__DO_YOU_REALLY_WANT_TO_AUTO_LOCK_YOUR_DEVICE_AFTER_STR
        ),
        description_param=format_duration_ms(delay_ms),
        br_code=ButtonRequestType.ProtectCall,
    )


async def require_confirm_safety_checks(
    ctx: wire.GenericContext, level: SafetyCheckLevel
) -> None:
    if level == SafetyCheckLevel.PromptAlways:
        await confirm_action(
            ctx,
            "set_safety_checks",
            _(i18n_keys.TITLE__SAFETY_OVERRIDE),
            hold=True,
            verb=_(i18n_keys.BUTTON__HOLD_TO_CONFIRM),
            description=_(
                i18n_keys.SUBTITLE__ONEKEY_WILL_ALLOW_YOU_TO_APPROVE_SOME_ACTIONS_WHICH_MIGHT_BE_UNSAFE
            ),
            action="",
            reverse=True,
            larger_vspace=True,
            br_code=ButtonRequestType.ProtectCall,
        )
    elif level == SafetyCheckLevel.PromptTemporarily:
        await confirm_action(
            ctx,
            "set_safety_checks",
            _(i18n_keys.TITLE__SAFETY_OVERRIDE),
            hold=True,
            verb=_(i18n_keys.BUTTON__HOLD_TO_CONFIRM),
            description=_(
                i18n_keys.SUBTITLE__ONEKEY_WILL_TEMPORARILY_ALLOW_YOU_TO_APPROVE_SOME_ACTIONS_WHICH_MIGHT_BE_UNSAFE
            ),
            action="",
            reverse=True,
            br_code=ButtonRequestType.ProtectCall,
        )
    elif level == SafetyCheckLevel.Strict:
        await confirm_action(
            ctx,
            "set_safety_checks",
            _(i18n_keys.TITLE__SAFETY_CHECKS),
            description=_(
                i18n_keys.SUBTITLE__DO_YOU_REALLY_WANT_TO_ENFORCE_STRICT_SAFETY_CHECKS_RECOMMENDED
            ),
            br_code=ButtonRequestType.ProtectCall,
        )
    else:
        raise ValueError  # enum value out of range


async def require_confirm_experimental_features(
    ctx: wire.GenericContext, enable: bool
) -> None:
    if enable:
        await confirm_action(
            ctx,
            "set_experimental_features",
            _(i18n_keys.TITLE__EXPERIMENTAL_MODE),
            description=_(i18n_keys.SUBTITLE__ENABLE_EXPERIMENTAL_FEATURES),
            action="",  # Only for development and beta testing!
            reverse=True,
            br_code=ButtonRequestType.ProtectCall,
        )
