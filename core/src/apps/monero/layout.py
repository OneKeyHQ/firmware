from typing import TYPE_CHECKING

from trezor import strings, ui
from trezor.enums import ButtonRequestType
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.ui.layouts import (
    confirm_action,
    confirm_blob,
    confirm_metadata,
    confirm_output,
)

# from trezor.ui.popup import Popup

DUMMY_PAYMENT_ID = b"\x00\x00\x00\x00\x00\x00\x00\x00"


if TYPE_CHECKING:
    from trezor.enums import MoneroNetworkType
    from trezor.messages import (
        MoneroTransactionData,
        MoneroTransactionDestinationEntry,
    )
    from trezor.wire import Context

    from .signing.state import State


def _format_amount(value: int) -> str:
    return f"{strings.format_amount(value, 12)} XMR"


async def require_confirm_watchkey(ctx: Context) -> None:
    await confirm_action(
        ctx,
        "get_watchkey",
        _(i18n_keys.TITLE__CONFIRM_EXPORT),
        description=_(i18n_keys.SUBTITLE__CONFIRM_EXPORT_WATCH_ONLY_CREDENTIALS),
        verb=_(i18n_keys.BUTTON__EXPORT),
        br_code=ButtonRequestType.SignTx,
    )


async def require_confirm_keyimage_sync(ctx: Context) -> None:
    await confirm_action(
        ctx,
        "key_image_sync",
        _(i18n_keys.TITLE__CONFIRM_KEY_IMAGE_SYNC),
        description=_(i18n_keys.SUBTITLE__CONFIRM_KEY_IMAGE_SYNC),
        verb=_(i18n_keys.BUTTON__SYNC),
        br_code=ButtonRequestType.SignTx,
    )


async def require_confirm_live_refresh(ctx: Context) -> None:
    await confirm_action(
        ctx,
        "live_refresh",
        _(i18n_keys.TITLE__CONFIRM_REFRESH),
        description=_(i18n_keys.SUBTITLE__CONFIRM_REFRESH),
        verb=_(i18n_keys.BUTTON__FRESH),
        br_code=ButtonRequestType.SignTx,
    )


async def require_confirm_tx_key(ctx: Context, export_key: bool = False) -> None:
    if export_key:
        description = _(i18n_keys.SUBTITLE__CONFIRM_EXPORT_TX_KEY)
    else:
        description = _(i18n_keys.SUBTITLE__CONFIRM_EXPORT_TX_DER_FOR_TX_PROOF)
    await confirm_action(
        ctx,
        "export_tx_key",
        _(i18n_keys.TITLE__CONFIRM_EXPORT),
        description=description,
        verb=_(i18n_keys.BUTTON__EXPORT),
        br_code=ButtonRequestType.SignTx,
    )


async def require_confirm_transaction(
    ctx: Context,
    state: State,
    tsx_data: MoneroTransactionData,
    network_type: MoneroNetworkType,
) -> None:
    """
    Ask for confirmation from user.
    """
    from apps.monero.xmr.addresses import get_change_addr_idx

    outputs = tsx_data.outputs
    change_idx = get_change_addr_idx(outputs, tsx_data.change_dts)

    if tsx_data.unlock_time != 0:
        await _require_confirm_unlock_time(ctx, tsx_data.unlock_time)

    for idx, dst in enumerate(outputs):
        is_change = change_idx is not None and idx == change_idx
        if is_change:
            continue  # Change output does not need confirmation
        is_dummy = change_idx is None and dst.amount == 0 and len(outputs) == 2
        if is_dummy:
            continue  # Dummy output does not need confirmation
        if tsx_data.integrated_indices and idx in tsx_data.integrated_indices:
            cur_payment = tsx_data.payment_id
        else:
            cur_payment = None
        await _require_confirm_output(ctx, dst, network_type, cur_payment)

    if (
        tsx_data.payment_id
        and not tsx_data.integrated_indices
        and tsx_data.payment_id != DUMMY_PAYMENT_ID
    ):
        await _require_confirm_payment_id(ctx, tsx_data.payment_id)

    await _require_confirm_fee(ctx, tsx_data.fee)
    await transaction_step(state, 0)


async def _require_confirm_output(
    ctx: Context,
    dst: MoneroTransactionDestinationEntry,
    network_type: MoneroNetworkType,
    payment_id: bytes | None,
) -> None:
    """
    Single transaction destination confirmation
    """
    from apps.monero.xmr.addresses import encode_addr
    from apps.monero.xmr.networks import net_version

    version = net_version(network_type, dst.is_subaddress, payment_id is not None)
    addr = encode_addr(
        version, dst.addr.spend_public_key, dst.addr.view_public_key, payment_id
    )
    assert dst.amount is not None
    await confirm_output(
        ctx,
        address=addr,
        amount=_format_amount(dst.amount),
        br_code=ButtonRequestType.SignTx,
    )


async def _require_confirm_payment_id(ctx: Context, payment_id: bytes) -> None:
    await confirm_blob(
        ctx,
        "confirm_payment_id",
        title=_(i18n_keys.TITLE__PAYMENT_ID),
        data=payment_id,
        br_code=ButtonRequestType.SignTx,
    )


async def _require_confirm_fee(ctx: Context, fee: int) -> None:
    await confirm_metadata(
        ctx,
        "confirm_final",
        title=_(i18n_keys.TITLE__CONFIRM_FEE),
        content="",
        param=_format_amount(fee),
        description=_(i18n_keys.LIST_KEY__FEE__COLON),
    )


async def _require_confirm_unlock_time(ctx: Context, unlock_time: int) -> None:
    await confirm_metadata(
        ctx,
        "confirm_locktime",
        _(i18n_keys.TITLE__CONFIRM_UNLOCK_TIME),
        _(i18n_keys.SUBTITLE__UNLOCK_TIME_SET_TO_STR).format(unlock_time),
        br_code=ButtonRequestType.SignTx,
    )


class TransactionStep(ui.Component):
    def __init__(self, state: State, info: list[str]) -> None:
        super().__init__()
        self.state = state
        self.info = info

    def on_render(self) -> None:
        return
        state = self.state
        info = self.info
        ui.header("Signing transaction", ui.ICON_SEND, ui.TITLE_GREY, ui.BG, ui.BLUE)
        p = 1000 * state.progress_cur // state.progress_total
        ui.display.loader(p, False, -4, ui.WHITE, ui.BG)
        ui.display.text_center(ui.WIDTH // 2, 210, info[0], ui.NORMAL, ui.FG, ui.BG)
        if len(info) > 1:
            ui.display.text_center(ui.WIDTH // 2, 235, info[1], ui.NORMAL, ui.FG, ui.BG)


class KeyImageSyncStep(ui.Component):
    def __init__(self, current: int, total_num: int) -> None:
        super().__init__()
        self.current = current
        self.total_num = total_num

    def on_render(self) -> None:
        return
        current = self.current
        total_num = self.total_num
        ui.header("Syncing", ui.ICON_SEND, ui.TITLE_GREY, ui.BG, ui.BLUE)
        p = (1000 * (current + 1) // total_num) if total_num > 0 else 0
        ui.display.loader(p, False, 18, ui.WHITE, ui.BG)


class LiveRefreshStep(ui.Component):
    def __init__(self, current: int) -> None:
        super().__init__()
        self.current = current

    def on_render(self) -> None:
        return
        current = self.current
        ui.header("Refreshing", ui.ICON_SEND, ui.TITLE_GREY, ui.BG, ui.BLUE)
        p = (1000 * current // 8) % 1000
        ui.display.loader(p, True, 18, ui.WHITE, ui.BG)
        ui.display.text_center(
            ui.WIDTH // 2, 145, str(current), ui.NORMAL, ui.FG, ui.BG
        )


async def transaction_step(state: State, step: int, sub_step: int = 0) -> None:
    return
    # if step == 0:
    #     info = ["Signing..."]
    # elif step == state.STEP_INP:
    #     info = ["Processing inputs", f"{sub_step + 1}/{state.input_count}"]
    # elif step == state.STEP_PERM:
    #     info = ["Sorting..."]
    # elif step == state.STEP_VINI:
    #     info = ["Hashing inputs", f"{sub_step + 1}/{state.input_count}"]
    # elif step == state.STEP_ALL_IN:
    #     info = ["Processing..."]
    # elif step == state.STEP_OUT:
    #     info = ["Processing outputs", f"{sub_step + 1}/{state.output_count}"]
    # elif step == state.STEP_ALL_OUT:
    #     info = ["Postprocessing..."]
    # elif step == state.STEP_SIGN:
    #     info = ["Signing inputs", f"{sub_step + 1}/{state.input_count}"]
    # else:
    #     info = ["Processing..."]

    state.progress_cur += 1
    # await Popup(TransactionStep(state, info))


async def keyimage_sync_step(ctx: Context, current: int | None, total_num: int) -> None:
    return
    if current is None:
        return
    # await Popup(KeyImageSyncStep(current, total_num))


async def live_refresh_step(ctx: Context, current: int | None) -> None:
    return
    if current is None:
        return
    # await Popup(LiveRefreshStep(current))
