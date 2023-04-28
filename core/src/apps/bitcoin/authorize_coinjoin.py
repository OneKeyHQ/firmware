from micropython import const
from typing import TYPE_CHECKING

from trezor import wire
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.lvglui.scrs import lv
from trezor.messages import AuthorizeCoinJoin, Success
from trezor.ui.layouts import confirm_action, confirm_coinjoin, confirm_metadata

from apps.common import authorization, safety_checks
from apps.common.keychain import FORBIDDEN_KEY_PATH
from apps.common.paths import SLIP25_PURPOSE, validate_path

from .authorization import FEE_RATE_DECIMALS
from .common import BIP32_WALLET_DEPTH, format_fee_rate
from .keychain import validate_path_against_script_type, with_keychain

if TYPE_CHECKING:
    from apps.common.coininfo import CoinInfo
    from apps.common.keychain import Keychain

_MAX_COORDINATOR_LEN = const(36)
_MAX_ROUNDS = const(500)
_MAX_COORDINATOR_FEE_RATE = 5 * pow(10, FEE_RATE_DECIMALS)  # 5 %


@with_keychain
async def authorize_coinjoin(
    ctx: wire.Context, msg: AuthorizeCoinJoin, keychain: Keychain, coin: CoinInfo
) -> Success:
    if len(msg.coordinator) > _MAX_COORDINATOR_LEN or not all(
        32 <= ord(x) <= 126 for x in msg.coordinator
    ):
        raise wire.DataError("Invalid coordinator name.")

    if msg.max_rounds > _MAX_ROUNDS and safety_checks.is_strict():
        raise wire.DataError("The number of rounds is unexpectedly large.")

    if (
        msg.max_coordinator_fee_rate > _MAX_COORDINATOR_FEE_RATE
        and safety_checks.is_strict()
    ):
        raise wire.DataError("The coordination fee rate is unexpectedly large.")

    if msg.max_fee_per_kvbyte > 10 * coin.maxfee_kb and safety_checks.is_strict():
        raise wire.DataError("The fee per vbyte is unexpectedly large.")

    if not msg.address_n:
        raise wire.DataError("Empty path not allowed.")
    ctx.primary_color, ctx.icon_path = (
        lv.color_hex(coin.primary_color),
        f"A:/res/{coin.icon}",
    )
    if msg.address_n[0] != SLIP25_PURPOSE and safety_checks.is_strict():
        raise FORBIDDEN_KEY_PATH

    await confirm_action(
        ctx,
        "coinjoin_coordinator",
        title=_(i18n_keys.TITLE__AUTHORIZE_COINJOIN),
        description="Do you really want to take part in a CoinJoin transaction at:\n{}",
        description_param=msg.coordinator,
        primary_color=ctx.primary_color,
    )

    validation_path = msg.address_n + [0] * BIP32_WALLET_DEPTH
    await validate_path(
        ctx,
        keychain,
        validation_path,
        msg.address_n[0] == SLIP25_PURPOSE,
        validate_path_against_script_type(
            coin, address_n=validation_path, script_type=msg.script_type
        ),
    )

    max_fee_per_vbyte = format_fee_rate(
        msg.max_fee_per_kvbyte / 1000, coin, include_shortcut=True
    )

    if msg.max_fee_per_kvbyte > coin.maxfee_kb:
        await confirm_metadata(
            ctx,
            "fee_over_threshold",
            "High mining fee",
            "The mining fee of\n{}\nis unexpectedly high.",
            max_fee_per_vbyte,
            ButtonRequestType.FeeOverThreshold,
        )

    await confirm_coinjoin(ctx, coin.coin_name, msg.max_rounds, max_fee_per_vbyte)

    authorization.set(msg)

    return Success(message="CoinJoin authorized")
