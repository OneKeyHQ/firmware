from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor import ui
from trezor.enums import ButtonRequestType, EthereumDataType, EthereumDataTypeOneKey
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.messages import (
    EthereumFieldType,
    EthereumFieldTypeOneKey,
    EthereumStructMember,
    EthereumStructMemberOneKey,
)
from trezor.strings import format_amount
from trezor.ui.layouts import (
    confirm_action,
    confirm_address,
    confirm_blob,
    confirm_output,
    confirm_sign_typed_hash,
    confirm_text,
    should_show_details,
    should_show_more,
)
from trezor.ui.layouts.lvgl.altcoin import (
    confirm_total_ethereum,
    confirm_total_ethereum_eip1559,
)

from . import networks, tokens
from .helpers import (
    address_from_bytes,
    decode_typed_data,
    get_type_name,
    get_type_name_onekey,
)

if TYPE_CHECKING:
    from typing import Awaitable, Iterable

    from trezor.wire import Context


def require_confirm_tx(
    ctx: Context,
    to_bytes: bytes,
    value: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    is_nft: bool = False,
) -> Awaitable[None]:
    if to_bytes:
        to_str = address_from_bytes(to_bytes, networks.by_chain_id(chain_id))
    else:
        to_str = _(i18n_keys.LIST_VALUE__NEW_CONTRACT)
    return confirm_output(
        ctx,
        address=to_str,
        amount=format_ethereum_amount(value, token, chain_id, is_nft),
        font_amount=ui.BOLD,
        color_to=ui.GREY,
        br_code=ButtonRequestType.SignTx,
    )


def require_show_overview(
    ctx: Context,
    network: str,
    to_bytes: bytes,
    value: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    is_nft: bool = False,
) -> Awaitable[bool]:
    if to_bytes:
        to_str = address_from_bytes(to_bytes, networks.by_chain_id(chain_id))
    else:
        to_str = _(i18n_keys.LIST_VALUE__NEW_CONTRACT)
    return should_show_details(
        ctx,
        title=_(i18n_keys.TITLE__STR_TRANSACTION).format(network),
        address=to_str,
        amount=format_ethereum_amount(value, token, chain_id, is_nft),
        br_code=ButtonRequestType.SignTx,
    )


def require_confirm_fee(
    ctx: Context,
    spending: int,
    gas_price: int,
    gas_limit: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    from_address: str | None = None,
    to_address: str | None = None,
    contract_addr: str | None = None,
    token_id: int | None = None,
    evm_chain_id: int | None = None,
    raw_data: bytes | None = None,
) -> Awaitable[None]:
    fee_max = gas_price * gas_limit
    return confirm_total_ethereum(
        ctx,
        format_ethereum_amount(
            spending, token, chain_id, is_nft=True if token_id else False
        ),
        format_ethereum_amount(gas_price, None, chain_id),
        format_ethereum_amount(fee_max, None, chain_id),
        from_address,
        to_address,
        format_ethereum_amount(spending + fee_max, None, chain_id)
        if (token is None and contract_addr is None)
        else None,
        contract_addr,
        token_id,
        evm_chain_id=evm_chain_id,
        raw_data=raw_data,
    )


async def require_confirm_eip1559_fee(
    ctx: Context,
    spending: int,
    max_priority_fee: int,
    max_gas_fee: int,
    gas_limit: int,
    chain_id: int,
    token: tokens.EthereumTokenInfo | None = None,
    from_address: str | None = None,
    to_address: str | None = None,
    contract_addr: str | None = None,
    token_id: int | None = None,
    evm_chain_id: int | None = None,
    raw_data: bytes | None = None,
) -> None:

    fee_max = max_gas_fee * gas_limit
    await confirm_total_ethereum_eip1559(
        ctx,
        format_ethereum_amount(
            spending, token, chain_id, is_nft=True if token_id else False
        ),
        format_ethereum_amount(max_priority_fee, None, chain_id),
        format_ethereum_amount(max_gas_fee, None, chain_id),
        format_ethereum_amount(fee_max, None, chain_id),
        from_address,
        to_address,
        format_ethereum_amount(spending + fee_max, None, chain_id)
        if (token is None and contract_addr is None)
        else None,
        contract_addr,
        token_id,
        evm_chain_id=evm_chain_id,
        raw_data=raw_data,
    )


def require_confirm_unknown_token(
    ctx: Context, address_bytes: bytes
) -> Awaitable[None]:
    contract_address_hex = "0x" + hexlify(address_bytes).decode()
    return confirm_address(
        ctx,
        _(i18n_keys.TITLE__UNKNOWN_TOKEN),
        contract_address_hex,
        description=_(i18n_keys.LIST_KEY__CONTRACT__COLON),
        br_type="unknown_token",
        icon="A:/res/warning.png",
        icon_color=ui.ORANGE,
        br_code=ButtonRequestType.SignTx,
    )


def require_confirm_data(ctx: Context, data: bytes, data_total: int) -> Awaitable[None]:
    from trezor.ui.layouts import confirm_data

    return confirm_data(
        ctx,
        "confirm_data",
        title=_(i18n_keys.TITLE__VIEW_DATA),
        description=_(i18n_keys.SUBTITLE__STR_BYTES).format(data_total),
        data=data,
        br_code=ButtonRequestType.SignTx,
    )


async def confirm_typed_data_final(ctx: Context) -> None:
    await confirm_action(
        ctx,
        "confirm_typed_data_final",
        title=_(i18n_keys.TITLE__SIGN_STR_TYPED_DATA).format(ctx.name),
        action=_(i18n_keys.SUBTITLE__SIGN_712_TYPED_DATA),
        verb=_(i18n_keys.BUTTON__SLIDE_TO_CONFIRM),
        icon=None,
        hold=True,
        anim_dir=0,
    )


async def confirm_typed_hash_final(ctx: Context) -> None:
    await confirm_action(
        ctx,
        "confirm_typed_hash_final",
        title=_(i18n_keys.TITLE__SIGN_STR_TYPED_HASH).format(ctx.name),
        action=_(i18n_keys.SUBTITLE__SIGN_STR_TYPED_HASH),
        verb=_(i18n_keys.BUTTON__SLIDE_TO_CONFIRM),
        icon=None,
        hold=True,
        anim_dir=0,
    )


async def confirm_typed_hash(ctx: Context, domain_hash, message_hash) -> None:
    await confirm_sign_typed_hash(ctx, domain_hash, message_hash)


def confirm_empty_typed_message(ctx: Context) -> Awaitable[None]:
    return confirm_text(
        ctx,
        "confirm_empty_typed_message",
        title=_(i18n_keys.TITLE__CONFIRM_MESSAGE),
        data="",
        description=_(i18n_keys.SUBTITLE__NO_MESSAGE_FIELD),
    )


async def confirm_domain(ctx: Context, domain: dict[str, bytes]) -> None:
    domain_name = (
        decode_typed_data(domain["name"], "string") if domain.get("name") else None
    )
    domain_version = (
        decode_typed_data(domain["version"], "string")
        if domain.get("version")
        else None
    )
    chain_id = (
        decode_typed_data(domain["chainId"], "uint256")
        if domain.get("chainId")
        else None
    )
    verifying_contract = (
        decode_typed_data(domain["verifyingContract"], "address")
        if domain.get("verifyingContract")
        else None
    )
    salt = decode_typed_data(domain["salt"], "bytes32") if domain.get("salt") else None
    from trezor.ui.layouts import confirm_domain

    await confirm_domain(
        ctx,
        **{
            "name": domain_name,
            "version": domain_version,
            "chainId": chain_id,
            "verifyingContract": verifying_contract,
            "salt": salt,
        },
    )


async def should_show_domain(ctx: Context, name: bytes, version: bytes) -> bool:
    domain_name = decode_typed_data(name, "string")
    domain_version = decode_typed_data(version, "string")

    para = (
        (ui.NORMAL, "Name and version"),
        (ui.BOLD, domain_name),
        (ui.BOLD, domain_version),
    )
    return await should_show_more(
        ctx,
        title="Confirm domain",
        para=para,
        button_text="Show full domain",
        br_type="should_show_domain",
    )


async def should_show_struct(
    ctx: Context,
    description: str,
    data_members: list[EthereumStructMember] | list[EthereumStructMemberOneKey],
    title: str = "Confirm struct",
    button_text: str = "Show full struct",
) -> bool:
    para = (
        (ui.BOLD, description),
        (
            ui.NORMAL,
            _(i18n_keys.LIST_KEY__CONTAINS_STR_KEY).format(len(data_members))
            # format_plural("Contains {count} {plural}", len(data_members), "key"),
        ),
        (ui.NORMAL, ", ".join(field.name for field in data_members)),
    )
    return await should_show_more(
        ctx,
        title=title,
        para=para,
        button_text=button_text,
        br_type="should_show_struct",
    )


async def should_show_array(
    ctx: Context,
    parent_objects: Iterable[str],
    data_type: str,
    size: int,
) -> bool:
    para = (
        (ui.NORMAL, _(i18n_keys.INSERT__ARRAY_OF_STR_STR).format(size, data_type)),
        # format_plural("Array of {count} {plural}", size, data_type)),
    )
    return await should_show_more(
        ctx,
        title=limit_str(".".join(parent_objects)),
        para=para,
        button_text=_(i18n_keys.BUTTON__VIEW_FULL_ARRAY),
        br_type="should_show_array",
    )


async def confirm_typed_value(
    ctx: Context,
    name: str,
    value: bytes,
    parent_objects: list[str],
    field: EthereumFieldType,
    array_index: int | None = None,
) -> None:
    type_name = get_type_name(field)

    if array_index is not None:
        title = limit_str(".".join(parent_objects + [name]))
        description = f"[{array_index}] ({type_name}):"
    else:
        title = limit_str(".".join(parent_objects))
        description = f"{name} ({type_name}):"

    data = decode_typed_data(value, type_name)

    if field.data_type in (EthereumDataType.ADDRESS, EthereumDataType.BYTES):
        await confirm_blob(
            ctx,
            "confirm_typed_value",
            title=title,
            data=data,
            description=description,
            ask_pagination=True,
            icon=None,
        )
    else:
        await confirm_text(
            ctx,
            "confirm_typed_value",
            title=title,
            data=data,
            description=description,
            icon=None,
        )


async def confirm_typed_value_onekey(
    ctx: Context,
    name: str,
    value: bytes,
    parent_objects: list[str],
    field: EthereumFieldTypeOneKey,
    array_index: int | None = None,
) -> None:
    type_name = get_type_name_onekey(field)

    if array_index is not None:
        title = limit_str(".".join(parent_objects + [name]))
        description = f"[{array_index}] ({type_name}):"
    else:
        title = limit_str(".".join(parent_objects))
        description = f"{name} ({type_name}):"

    data = decode_typed_data(value, type_name)

    if field.data_type in (
        EthereumDataTypeOneKey.ADDRESS,
        EthereumDataTypeOneKey.BYTES,
    ):
        await confirm_blob(
            ctx,
            "confirm_typed_value",
            title=title,
            data=data,
            description=description,
            ask_pagination=True,
            icon=None,
        )
    else:
        await confirm_text(
            ctx,
            "confirm_typed_value",
            title=title,
            data=data,
            description=description,
            icon=None,
        )


def format_ethereum_amount(
    value: int,
    token: tokens.EthereumTokenInfo | None,
    chain_id: int,
    is_nft: bool = False,
) -> str:
    if is_nft:
        return f"{value} NFT"
    if token:
        suffix = token.symbol
        decimals = token.decimals
    else:
        suffix = networks.shortcut_by_chain_id(chain_id)
        decimals = 18

    # Don't want to display wei values for tokens with small decimal numbers
    # if decimals > 9 and value < 10 ** (decimals - 9):
    #     suffix = "Wei " + suffix
    #     decimals = 0

    return f"{format_amount(value, decimals)} {suffix}"


def limit_str(s: str, limit: int = 16) -> str:
    """Shortens string to show the last <limit> characters."""
    if len(s) <= limit + 2:
        return s

    return ".." + s[-limit:]
