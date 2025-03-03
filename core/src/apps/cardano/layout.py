from typing import TYPE_CHECKING

from trezor import messages
from trezor.enums import (
    ButtonRequestType,
    CardanoAddressType,
    CardanoCertificateType,
    CardanoDRepType,
    CardanoNativeScriptHashDisplayFormat,
    CardanoNativeScriptType,
)
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys
from trezor.strings import format_amount
from trezor.ui import layouts
from trezor.ui.layouts import confirm_metadata, confirm_output, confirm_properties

from apps.common.paths import address_n_to_str

from . import addresses
from .helpers import bech32, protocol_magics
from .helpers.utils import (
    format_account_number,
    format_asset_fingerprint,
    format_optional_int,
    format_stake_pool_id,
)

if TYPE_CHECKING:
    from typing import Literal

    from trezor.ui.layouts.lvgl import PropertyType

    from .helpers.credential import Credential
    from .seed import Keychain
    from trezor import wire

ADDRESS_TYPE_NAMES = {
    CardanoAddressType.BYRON: "Legacy",
    CardanoAddressType.BASE: "Base",
    CardanoAddressType.BASE_SCRIPT_KEY: "Base",
    CardanoAddressType.BASE_KEY_SCRIPT: "Base",
    CardanoAddressType.BASE_SCRIPT_SCRIPT: "Base",
    CardanoAddressType.POINTER: "Pointer",
    CardanoAddressType.POINTER_SCRIPT: "Pointer",
    CardanoAddressType.ENTERPRISE: "Enterprise",
    CardanoAddressType.ENTERPRISE_SCRIPT: "Enterprise",
    CardanoAddressType.REWARD: "Reward",
    CardanoAddressType.REWARD_SCRIPT: "Reward",
}

SCRIPT_TYPE_NAMES = {
    CardanoNativeScriptType.PUB_KEY: "Key",
    CardanoNativeScriptType.ALL: "All",
    CardanoNativeScriptType.ANY: "Any",
    CardanoNativeScriptType.N_OF_K: "N of K",
    CardanoNativeScriptType.INVALID_BEFORE: "Invalid before",
    CardanoNativeScriptType.INVALID_HEREAFTER: "Invalid hereafter",
}

CERTIFICATE_TYPE_NAMES = {
    CardanoCertificateType.STAKE_REGISTRATION: "Stake key registration",
    CardanoCertificateType.STAKE_REGISTRATION_CONWAY: "Stake key registration",
    CardanoCertificateType.STAKE_DEREGISTRATION: "Stake key deregistration",
    CardanoCertificateType.STAKE_DEREGISTRATION_CONWAY: "Stake key deregistration",
    CardanoCertificateType.STAKE_DELEGATION: "Stake delegation",
    CardanoCertificateType.STAKE_POOL_REGISTRATION: "Stakepool registration",
    CardanoCertificateType.VOTE_DELEGATION: "Vote delegation",
}

TITLE = _(i18n_keys.TITLE__CONFIRM_TRANSACTION)

BRT_Other = ButtonRequestType.Other  # global_import_cache

CVOTE_REWARD_ELIGIBILITY_WARNING = "Reward eligibility warning"


def format_coin_amount(amount: int, network_id: int) -> str:
    from .helpers import network_ids

    currency = "ADA" if network_ids.is_mainnet(network_id) else "tADA"
    return f"{format_amount(amount, 6)} {currency}"


async def show_native_script(
    ctx: wire.Context,
    script: messages.CardanoNativeScript,
    indices: list[int] | None = None,
) -> None:
    CNST = CardanoNativeScriptType  # local_cache_global
    script_type = script.type  # local_cache_attribute
    key_path = script.key_path  # local_cache_attribute
    key_hash = script.key_hash  # local_cache_attribute
    scripts = script.scripts  # local_cache_attribute

    script_heading = "Script"
    if indices is None:
        indices = []
    if indices:
        script_heading += " " + ".".join(str(i) for i in indices)

    script_type_name_suffix = ""
    if script_type == CNST.PUB_KEY:
        if key_path:
            script_type_name_suffix = "path"
        elif key_hash:
            script_type_name_suffix = "hash"

    props: list[PropertyType] = [
        (
            f"{script_heading} - {SCRIPT_TYPE_NAMES[script_type]} {script_type_name_suffix}:",
            None,
        )
    ]
    append = props.append  # local_cache_attribute

    if script_type == CNST.PUB_KEY:
        assert key_hash is not None or key_path  # validate_script
        if key_hash:
            append((None, bech32.encode(bech32.HRP_SHARED_KEY_HASH, key_hash)))
        elif key_path:
            append((address_n_to_str(key_path), None))
    elif script_type == CNST.N_OF_K:
        assert script.required_signatures_count is not None  # validate_script
        append(
            (
                f"Requires {script.required_signatures_count} out of {len(scripts)} signatures.",
                None,
            )
        )
    elif script_type == CNST.INVALID_BEFORE:
        assert script.invalid_before is not None  # validate_script
        append((str(script.invalid_before), None))
    elif script_type == CNST.INVALID_HEREAFTER:
        assert script.invalid_hereafter is not None  # validate_script
        append((str(script.invalid_hereafter), None))

    if script_type in (
        CNST.ALL,
        CNST.ANY,
        CNST.N_OF_K,
    ):
        assert scripts  # validate_script
        append((f"Contains {len(scripts)} nested scripts.", None))

    await confirm_properties(
        ctx,
        "verify_script",
        "Verify script",
        props,
        br_code=BRT_Other,
    )

    for i, sub_script in enumerate(scripts):
        await show_native_script(ctx, sub_script, indices + [i + 1])


async def show_script_hash(
    ctx: wire.Context,
    script_hash: bytes,
    display_format: CardanoNativeScriptHashDisplayFormat,
) -> None:

    assert display_format in (
        CardanoNativeScriptHashDisplayFormat.BECH32,
        CardanoNativeScriptHashDisplayFormat.POLICY_ID,
    )

    if display_format == CardanoNativeScriptHashDisplayFormat.BECH32:
        await confirm_properties(
            ctx,
            "verify_script",
            "Verify script",
            (("Script hash:", bech32.encode(bech32.HRP_SCRIPT_HASH, script_hash)),),
            br_code=BRT_Other,
        )
    elif display_format == CardanoNativeScriptHashDisplayFormat.POLICY_ID:
        await layouts.confirm_blob(
            ctx,
            "verify_script",
            "Verify script",
            script_hash,
            "Policy ID:",
            br_code=BRT_Other,
        )


async def show_tx_init(ctx: wire.Context, title: str) -> bool:
    # should_show_details = await should_show_more(
    #     ctx,
    #     TITLE,
    #     (
    #         (
    #             ui.BOLD,
    #             title,
    #         ),
    #         (ui.NORMAL, "Choose level of details:"),
    #     ),
    #     "Show All",
    #     br_type="Show Simple",
    # )

    # return should_show_details
    return True


async def confirm_input(ctx: wire.Context, input: messages.CardanoTxInput) -> None:
    await confirm_properties(
        ctx,
        "confirm_input",
        TITLE,
        (
            (_(i18n_keys.LIST_KEY__INPUT_ID__COLON), input.prev_hash),
            (_(i18n_keys.LIST_KEY__INPUT_INDEX__COLON), str(input.prev_index)),
        ),
        br_code=BRT_Other,
    )


async def confirm_sending(
    ctx: wire.Context,
    ada_amount: int,
    to: str,
    output_type: Literal["address", "change", "collateral-return"],
    network_id: int,
    chunkify: bool = False,
) -> None:
    if output_type not in ("address", "change", "collateral-return"):
        raise RuntimeError  # should be unreachable

    await confirm_output(
        ctx,
        to,
        format_coin_amount(ada_amount, network_id),
        title=TITLE,
        subtitle="Cardano",
        br_code=ButtonRequestType.Other,
    )


async def confirm_sending_token(
    ctx: wire.Context, policy_id: bytes, token: messages.CardanoToken
) -> None:
    assert token.amount is not None  # _validate_token

    await confirm_properties(
        ctx,
        "confirm_token",
        TITLE,
        (
            (
                _(i18n_keys.LIST_KEY__ASSET_FINGERPRINT__COLON),
                format_asset_fingerprint(
                    policy_id=policy_id,
                    asset_name_bytes=token.asset_name_bytes,
                ),
            ),
            (_(i18n_keys.LIST_KEY__AMOUNT_SENT__COLON), format_amount(token.amount, 0)),
        ),
        br_code=BRT_Other,
    )


async def confirm_datum_hash(ctx: wire.Context, datum_hash: bytes) -> None:
    await confirm_properties(
        ctx,
        "confirm_datum_hash",
        TITLE,
        (
            (
                _(i18n_keys.LIST_KEY__DATUM_HASH__COLON),
                bech32.encode(bech32.HRP_OUTPUT_DATUM_HASH, datum_hash),
            ),
        ),
        br_code=BRT_Other,
    )


async def confirm_inline_datum(
    ctx: wire.Context, first_chunk: bytes, inline_datum_size: int
) -> None:
    await _confirm_data_chunk(
        ctx,
        "confirm_inline_datum",
        "Inline datum",
        first_chunk,
        inline_datum_size,
    )


async def confirm_reference_script(
    ctx: wire.Context, first_chunk: bytes, reference_script_size: int
) -> None:
    await _confirm_data_chunk(
        ctx,
        "confirm_reference_script",
        "Reference script",
        first_chunk,
        reference_script_size,
    )


async def _confirm_data_chunk(
    ctx: wire.Context, br_type: str, title: str, first_chunk: bytes, data_size: int
) -> None:
    MAX_DISPLAYED_SIZE = 56
    displayed_bytes = first_chunk[:MAX_DISPLAYED_SIZE]
    bytes_optional_plural = "byte" if data_size == 1 else "bytes"
    props: list[tuple[str, bytes | None]] = [
        (
            f"{title} ({data_size} {bytes_optional_plural}):",
            displayed_bytes,
        )
    ]
    if data_size > MAX_DISPLAYED_SIZE:
        props.append(("...", None))
    await confirm_properties(
        ctx,
        br_type,
        title=TITLE,
        props=props,
        br_code=BRT_Other,
    )


async def show_credentials(
    ctx: wire.Context,
    payment_credential: Credential,
    stake_credential: Credential,
) -> None:
    intro_text = "Address"
    await _show_credential(ctx, payment_credential, intro_text, purpose="address")
    await _show_credential(ctx, stake_credential, intro_text, purpose="address")


async def show_change_output_credentials(
    ctx: wire.Context,
    payment_credential: Credential,
    stake_credential: Credential,
) -> None:
    intro_text = "The following address is a change address. Its"
    await _show_credential(ctx, payment_credential, intro_text, purpose="output")
    await _show_credential(ctx, stake_credential, intro_text, purpose="output")


async def show_device_owned_output_credentials(
    ctx: wire.Context,
    payment_credential: Credential,
    stake_credential: Credential,
    show_both_credentials: bool,
) -> None:
    intro_text = "The following address is owned by this device. Its"
    await _show_credential(ctx, payment_credential, intro_text, purpose="output")
    if show_both_credentials:
        await _show_credential(ctx, stake_credential, intro_text, purpose="output")


async def show_cvote_registration_payment_credentials(
    ctx: wire.Context,
    payment_credential: Credential,
    stake_credential: Credential,
    show_both_credentials: bool,
    show_payment_warning: bool,
) -> None:
    intro_text = "Registration Payment"
    await _show_credential(
        ctx, payment_credential, intro_text, purpose="cvote_reg_payment_address"
    )
    if show_both_credentials or show_payment_warning:
        extra_text = CVOTE_REWARD_ELIGIBILITY_WARNING if show_payment_warning else None
        await _show_credential(
            ctx,
            stake_credential,
            intro_text,
            purpose="cvote_reg_payment_address",
            extra_text=extra_text,
        )


async def _show_credential(
    ctx: wire.Context,
    credential: Credential,
    intro_text: str,
    purpose: Literal["address", "output", "cvote_reg_payment_address"],
    extra_text: str | None = None,
) -> None:
    title = {
        "address": f"{ADDRESS_TYPE_NAMES[credential.address_type]} address",
        "output": TITLE,
        "cvote_reg_payment_address": TITLE,
    }[purpose]

    props: list[PropertyType] = []
    append = props.append  # local_cache_attribute

    # Credential can be empty in case of enterprise address stake credential
    # and reward address payment credential. In that case we don't want to
    # show some of the "props".
    if credential.is_set():
        credential_title = credential.get_title()
        # TODO: handle translation
        append(
            (
                f"{intro_text} {credential.type_name} credential is a {credential_title}:",
                None,
            )
        )
        props.extend(credential.format())

    if credential.is_unusual_path:
        append((None, "Path is unusual."))
    if credential.is_mismatch:
        append((None, "Credential doesn't match payment credential."))
    if credential.is_reward and purpose != "cvote_reg_payment_address":
        # for cvote registrations, this is handled by extra_text at the end
        append(("Address is a reward address.", None))
    if credential.is_no_staking:
        append(
            (
                f"{ADDRESS_TYPE_NAMES[credential.address_type]} address - no staking rewards.",
                None,
            )
        )

    if extra_text:
        append((extra_text, None))

    if len(props) > 0:
        await confirm_properties(
            ctx,
            "confirm_credential",
            title,
            props,
            br_code=BRT_Other,
        )


async def warn_path(ctx: wire.Context, path: list[int], title: str) -> None:
    await layouts.confirm_path_warning(ctx, address_n_to_str(path), path_type=title)


async def warn_tx_output_contains_tokens(
    ctx: wire.Context, is_collateral_return: bool = False
) -> None:
    content = (
        "The collateral return\noutput contains tokens."
        if is_collateral_return
        else _(i18n_keys.SUBTITLE__ADA_TX_CONTAINS_TOKEN)
    )
    await confirm_metadata(
        ctx,
        "confirm_tokens",
        _(i18n_keys.TITLE__STR_TRANSACTION).format("Cardano"),
        content,
        br_code=BRT_Other,
    )


async def warn_tx_contains_mint(ctx: wire.Context) -> None:
    await confirm_metadata(
        ctx,
        "confirm_tokens",
        TITLE,
        "The transaction contains minting or burning of tokens.",
        br_code=BRT_Other,
    )


async def warn_tx_output_no_datum(ctx: wire.Context) -> None:
    await confirm_metadata(
        ctx,
        "confirm_no_datum_hash",
        TITLE,
        "The following transaction output contains a script address, but does not contain a datum.",
        br_code=BRT_Other,
    )


async def warn_no_script_data_hash(ctx: wire.Context) -> None:
    await confirm_metadata(
        ctx,
        "confirm_no_script_data_hash",
        TITLE,
        "The transaction contains no script data hash. Plutus script will not be able to run.",
        br_code=BRT_Other,
    )


async def warn_no_collateral_inputs(ctx: wire.Context) -> None:
    await confirm_metadata(
        ctx,
        "confirm_no_collateral_inputs",
        TITLE,
        "The transaction contains no collateral inputs. Plutus script will not be able to run.",
        br_code=BRT_Other,
    )


async def warn_unknown_total_collateral(ctx: wire.Context) -> None:
    await confirm_metadata(
        ctx,
        "confirm_unknown_total_collateral",
        title="Warning",
        content="Unknown collateral amount, check all items carefully.",
        br_code=ButtonRequestType.Other,
    )


async def confirm_witness_request(
    ctx: wire.Context,
    witness_path: list[int],
) -> None:
    from . import seed

    if seed.is_multisig_path(witness_path):
        path_title = "multi-sig path"
    elif seed.is_minting_path(witness_path):
        path_title = "token minting path"
    else:
        path_title = "path"

    await layouts.confirm_text(
        ctx,
        "confirm_total",
        TITLE,
        address_n_to_str(witness_path),
        f"Sign transaction with {path_title}:",
        BRT_Other,
    )


async def confirm_tx(
    ctx: wire.Context,
    fee: int,
    network_id: int,
    protocol_magic: int,
    ttl: int | None,
    validity_interval_start: int | None,
    total_collateral: int | None,
    is_network_id_verifiable: bool,
    tx_hash: bytes | None,
) -> None:
    props: list[PropertyType] = [
        (
            _(i18n_keys.LIST_KEY__TRANSACTION_FEE__COLON),
            format_coin_amount(fee, network_id),
        ),
    ]
    append = props.append  # local_cache_attribute

    if total_collateral is not None:
        append(("Total collateral:", format_coin_amount(total_collateral, network_id)))

    if is_network_id_verifiable:
        append((f"Network: {protocol_magics.to_ui_string(protocol_magic)}", None))

    append((f"Valid since: {format_optional_int(validity_interval_start)}", None))
    append((f"TTL: {format_optional_int(ttl)}", None))

    if tx_hash:
        append((_(i18n_keys.LIST_KEY__TRANSACTION_ID__COLON), tx_hash))

    await confirm_properties(
        ctx,
        "confirm_total",
        TITLE,
        props,
        hold=True,
        br_code=BRT_Other,
    )


async def confirm_certificate(
    ctx: wire.Context,
    certificate: messages.CardanoTxCertificate,
    network_id: int,
) -> None:
    # stake pool registration requires custom confirmation logic not covered
    # in this call
    assert certificate.type != CardanoCertificateType.STAKE_POOL_REGISTRATION

    if certificate.type == CardanoCertificateType.STAKE_REGISTRATION:
        transaction_type_value = _(i18n_keys.LIST_VALUE__STAKE_KEY_REGISTRATION)
    elif certificate.type == CardanoCertificateType.STAKE_DEREGISTRATION:
        transaction_type_value = _(i18n_keys.LIST_VALUE__STAKE_KEY_DEREGISTRATION)
    elif certificate.type == CardanoCertificateType.STAKE_DELEGATION:
        transaction_type_value = _(i18n_keys.LIST_VALUE__STAKE_DELEGATION)
    else:
        transaction_type_value = CERTIFICATE_TYPE_NAMES[certificate.type]

    props: list[PropertyType] = [
        (_(i18n_keys.LIST_KEY__TRANSACTION_TYPE__COLON), transaction_type_value),
        _format_stake_credential(
            certificate.path, certificate.script_hash, certificate.key_hash
        ),
    ]

    if certificate.type == CardanoCertificateType.STAKE_DELEGATION:
        assert certificate.pool is not None  # validate_certificate
        props.append(
            (
                _(i18n_keys.LIST_KEY__TO_POOL__COLON),
                format_stake_pool_id(certificate.pool),
            )
        )
    elif certificate.type in (
        CardanoCertificateType.STAKE_REGISTRATION_CONWAY,
        CardanoCertificateType.STAKE_DEREGISTRATION_CONWAY,
    ):
        assert certificate.deposit is not None  # validate_certificate
        props.append(
            (
                _(i18n_keys.LIST_VALUE__DEPOSIT) + ":",
                format_coin_amount(certificate.deposit, network_id),
            )
        )

    elif certificate.type == CardanoCertificateType.VOTE_DELEGATION:
        assert certificate.drep is not None  # validate_certificate
        props.append(_format_drep(certificate.drep))

    await confirm_properties(
        ctx,
        "confirm_certificate",
        TITLE,
        props,
        br_code=BRT_Other,
    )


async def confirm_stake_pool_parameters(
    ctx: wire.Context,
    pool_parameters: messages.CardanoPoolParametersType,
    network_id: int,
) -> None:
    margin_percentage = (
        100.0 * pool_parameters.margin_numerator / pool_parameters.margin_denominator
    )
    percentage_formatted = str(float(margin_percentage)).rstrip("0").rstrip(".")
    await confirm_properties(
        ctx,
        "confirm_pool_registration",
        TITLE,
        (
            (
                "Stake pool registration\nPool ID:",
                format_stake_pool_id(pool_parameters.pool_id),
            ),
            ("Pool reward account:", pool_parameters.reward_account),
            (
                f"Pledge: {format_coin_amount(pool_parameters.pledge, network_id)}\n"
                + f"Cost: {format_coin_amount(pool_parameters.cost, network_id)}\n"
                + f"Margin: {percentage_formatted}%",
                None,
            ),
        ),
        br_code=BRT_Other,
    )


async def confirm_stake_pool_owner(
    ctx: wire.Context,
    keychain: Keychain,
    owner: messages.CardanoPoolOwner,
    protocol_magic: int,
    network_id: int,
) -> None:

    props: list[tuple[str, str | None]] = []
    if owner.staking_key_path:
        props.append(("Pool owner:", address_n_to_str(owner.staking_key_path)))
        props.append(
            (
                addresses.derive_human_readable(
                    keychain,
                    messages.CardanoAddressParametersType(
                        address_type=CardanoAddressType.REWARD,
                        address_n=owner.staking_key_path,
                    ),
                    protocol_magic,
                    network_id,
                ),
                None,
            )
        )
    else:
        assert owner.staking_key_hash is not None  # validate_pool_owners
        props.append(
            (
                "Pool owner:",
                addresses.derive_human_readable(
                    keychain,
                    messages.CardanoAddressParametersType(
                        address_type=CardanoAddressType.REWARD,
                        staking_key_hash=owner.staking_key_hash,
                    ),
                    protocol_magic,
                    network_id,
                ),
            )
        )

    await confirm_properties(
        ctx,
        "confirm_pool_owners",
        TITLE,
        props,
        br_code=BRT_Other,
    )


async def confirm_stake_pool_metadata(
    ctx: wire.Context,
    metadata: messages.CardanoPoolMetadataType | None,
) -> None:
    if metadata is None:
        await confirm_properties(
            ctx,
            "confirm_pool_metadata",
            TITLE,
            (("Pool has no metadata (anonymous pool)", None),),
            br_code=BRT_Other,
        )
        return

    await confirm_properties(
        ctx,
        "confirm_pool_metadata",
        TITLE,
        (
            ("Pool metadata url:", metadata.url),
            ("Pool metadata hash:", metadata.hash),
        ),
        br_code=BRT_Other,
    )


async def confirm_stake_pool_registration_final(
    ctx: wire.Context,
    protocol_magic: int,
    ttl: int | None,
    validity_interval_start: int | None,
) -> None:
    await confirm_properties(
        ctx,
        "confirm_pool_final",
        TITLE,
        (
            ("Confirm signing the stake pool registration as an owner.", None),
            ("Network:", protocol_magics.to_ui_string(protocol_magic)),
            ("Valid since:", format_optional_int(validity_interval_start)),
            ("TTL:", format_optional_int(ttl)),
        ),
        hold=True,
        br_code=BRT_Other,
    )


async def confirm_withdrawal(
    ctx: wire.Context,
    withdrawal: messages.CardanoTxWithdrawal,
    address_bytes: bytes,
    network_id: int,
) -> None:
    address_type_name = "script reward" if withdrawal.script_hash else "reward"
    address = addresses.encode_human_readable(address_bytes)
    props: list[PropertyType] = [
        (f"Confirm withdrawal for {address_type_name} address:", address),
    ]

    if withdrawal.path:
        props.append(
            _format_stake_credential(
                withdrawal.path, withdrawal.script_hash, withdrawal.key_hash
            )
        )

    props.append(
        (
            _(i18n_keys.LIST_KEY__AMOUNT__COLON),
            format_coin_amount(withdrawal.amount, network_id),
        )
    )

    await confirm_properties(
        ctx,
        "confirm_withdrawal",
        TITLE,
        props,
        br_code=BRT_Other,
    )


def _format_stake_credential(
    path: list[int], script_hash: bytes | None, key_hash: bytes | None
) -> tuple[str, str]:
    from .helpers.paths import ADDRESS_INDEX_PATH_INDEX, RECOMMENDED_ADDRESS_INDEX

    if path:
        account_number = format_account_number(path)
        address_index = path[ADDRESS_INDEX_PATH_INDEX]
        if address_index == RECOMMENDED_ADDRESS_INDEX:
            return (
                f"for account {account_number}:",
                address_n_to_str(path),
            )
        return (
            f"for account {account_number} and index {address_index}:",
            address_n_to_str(path),
        )
    elif key_hash:
        return ("for key hash:", bech32.encode(bech32.HRP_STAKE_KEY_HASH, key_hash))
    elif script_hash:
        return ("for script:", bech32.encode(bech32.HRP_SCRIPT_HASH, script_hash))
    else:
        # should be unreachable unless there's a bug in validation
        raise ValueError


def _format_drep(drep: messages.CardanoDRep) -> tuple[str, str]:
    if drep.type == CardanoDRepType.KEY_HASH:
        assert drep.key_hash is not None  # validate_drep
        return (
            "Delegating to key hash:",
            bech32.encode(bech32.HRP_DREP_KEY_HASH, drep.key_hash),
        )
    elif drep.type == CardanoDRepType.SCRIPT_HASH:
        assert drep.script_hash is not None  # validate_drep
        return (
            "Delegating to script:",
            bech32.encode(bech32.HRP_DREP_SCRIPT_HASH, drep.script_hash),
        )
    elif drep.type == CardanoDRepType.ABSTAIN:
        return ("Delegating to:", "Always Abstain")
    elif drep.type == CardanoDRepType.NO_CONFIDENCE:
        return ("Delegating to:", "Always No Confidence")
    else:
        # should be unreachable unless there's a bug in validation
        raise ValueError


async def confirm_cvote_registration_delegation(
    ctx: wire.Context,
    public_key: str,
    weight: int,
) -> None:
    props: list[PropertyType] = [
        ("Vote key registration (CIP-36)", None),
        ("Delegating to:", public_key),
    ]
    if weight is not None:
        props.append(("Weight:", str(weight)))

    await confirm_properties(
        ctx,
        "confirm_cvote_registration_delegation",
        title=TITLE,
        props=props,
        br_code=ButtonRequestType.Other,
    )


async def confirm_cvote_registration_payment_address(
    ctx: wire.Context,
    payment_address: str,
    should_show_payment_warning: bool,
) -> None:
    props = [
        ("Vote key registration (CIP-36)", None),
        ("Rewards go to:", payment_address),
    ]
    if should_show_payment_warning:
        props.append((CVOTE_REWARD_ELIGIBILITY_WARNING, None))
    await confirm_properties(
        ctx,
        "confirm_cvote_registration_payment_address",
        title=TITLE,
        props=props,
        br_code=ButtonRequestType.Other,
    )


async def confirm_cvote_registration(
    ctx: wire.Context,
    vote_public_key: str | None,
    staking_path: list[int],
    nonce: int,
    voting_purpose: int | None,
) -> None:
    props: list[PropertyType] = [("Vote key registration (CIP-36)", None)]
    if vote_public_key is not None:
        props.append(("Vote public key:", vote_public_key))
    props.extend(
        [
            (
                f"Staking key for account: {format_account_number(staking_path)}:",
                address_n_to_str(staking_path),
            ),
            ("Nonce:", str(nonce)),
        ]
    )
    if voting_purpose is not None:
        props.append(
            (
                "Voting purpose:",
                ("Catalyst" if voting_purpose == 0 else f"{voting_purpose} (Other)"),
            )
        )

    await confirm_properties(
        ctx,
        "confirm_cvote_registration",
        title=TITLE,
        props=props,
        br_code=ButtonRequestType.Other,
    )


async def show_auxiliary_data_hash(
    ctx: wire.Context, auxiliary_data_hash: bytes
) -> None:
    await confirm_properties(
        ctx,
        "confirm_auxiliary_data",
        TITLE,
        ((_(i18n_keys.LIST_KEY__AUXILIARY_DATA_HASH__COLON), auxiliary_data_hash),),
        br_code=BRT_Other,
    )


async def confirm_token_minting(
    ctx: wire.Context, policy_id: bytes, token: messages.CardanoToken
) -> None:
    assert token.mint_amount is not None  # _validate_token
    await confirm_properties(
        ctx,
        "confirm_mint",
        TITLE,
        (
            (
                "Asset fingerprint:",
                format_asset_fingerprint(
                    policy_id,
                    token.asset_name_bytes,
                ),
            ),
            (
                "Amount minted (decimals unknown):"
                if token.mint_amount >= 0
                else "Amount burned (decimals unknown):",
                format_amount(token.mint_amount, 0),
            ),
        ),
        br_code=BRT_Other,
    )


async def warn_tx_network_unverifiable(ctx: wire.Context) -> None:
    await confirm_metadata(
        ctx,
        "warning_no_outputs",
        "Warning",
        "Transaction has no outputs, network cannot be verified.",
        br_code=BRT_Other,
    )


async def confirm_script_data_hash(ctx: wire.Context, script_data_hash: bytes) -> None:
    await confirm_properties(
        ctx,
        "confirm_script_data_hash",
        TITLE,
        (
            (
                _(i18n_keys.LIST_KEY__SCRIPT_DATA_HASH__COLON),
                bech32.encode(bech32.HRP_SCRIPT_DATA_HASH, script_data_hash),
            ),
        ),
        br_code=BRT_Other,
    )


async def confirm_collateral_input(
    ctx: wire.Context, collateral_input: messages.CardanoTxCollateralInput
) -> None:
    await confirm_properties(
        ctx,
        "confirm_collateral_input",
        TITLE,
        (
            ("Collateral input ID:", collateral_input.prev_hash),
            ("Collateral input index:", str(collateral_input.prev_index)),
        ),
        br_code=BRT_Other,
    )


async def confirm_reference_input(
    ctx: wire.Context, reference_input: messages.CardanoTxReferenceInput
) -> None:
    await confirm_properties(
        ctx,
        "confirm_reference_input",
        title=TITLE,
        props=[
            ("Reference input ID:", reference_input.prev_hash),
            ("Reference input index:", str(reference_input.prev_index)),
        ],
        br_code=ButtonRequestType.Other,
    )


async def confirm_required_signer(
    ctx: wire.Context, required_signer: messages.CardanoTxRequiredSigner
) -> None:
    assert (
        required_signer.key_hash is not None or required_signer.key_path
    )  # _validate_required_signer
    formatted_signer = (
        bech32.encode(bech32.HRP_REQUIRED_SIGNER_KEY_HASH, required_signer.key_hash)
        if required_signer.key_hash is not None
        else address_n_to_str(required_signer.key_path)
    )

    await confirm_properties(
        ctx,
        "confirm_required_signer",
        TITLE,
        (("Required signer", formatted_signer),),
        br_code=BRT_Other,
    )


async def show_cardano_address(
    ctx: wire.Context,
    address_parameters: messages.CardanoAddressParametersType,
    address: str,
    protocol_magic: int,
    chunkify: bool,
) -> None:
    CAT = CardanoAddressType  # local_cache_global

    network_name = "Cardano"
    if not protocol_magics.is_mainnet(protocol_magic):
        network_name = protocol_magics.to_ui_string(protocol_magic)

    title = f"{ADDRESS_TYPE_NAMES[address_parameters.address_type]} address"
    path = None
    if address_parameters.address_type in (
        CAT.BYRON,
        CAT.BASE,
        CAT.BASE_KEY_SCRIPT,
        CAT.POINTER,
        CAT.ENTERPRISE,
        CAT.REWARD,
    ):
        if address_parameters.address_n:
            path = address_n_to_str(address_parameters.address_n)
        elif address_parameters.address_n_staking:
            path = address_n_to_str(address_parameters.address_n_staking)

    await layouts.show_address(
        ctx,
        address,
        network=network_name,
        address_n=path,
        addr_type=title,
    )
