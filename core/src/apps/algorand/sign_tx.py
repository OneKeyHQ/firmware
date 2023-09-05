from ubinascii import b2a_base64

from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.lvglui.scrs import lv
from trezor.messages import AlgorandSignedTx, AlgorandSignTx
from trezor.strings import format_amount
from trezor.ui.layouts.lvgl import confirm_final

from apps.common import paths
from apps.common.keychain import Keychain, auto_keychain

from . import ICON, PRIMARY_COLOR, encoding, tokens, transactions


@auto_keychain(__name__)
async def sign_tx(
    ctx: wire.Context, msg: AlgorandSignTx, keychain: Keychain
) -> AlgorandSignedTx:

    await paths.validate_path(ctx, keychain, msg.address_n)
    node = keychain.derive(msg.address_n)
    public_key = node.public_key()[1:]
    address = encoding.encode_address(public_key)

    txn = encoding.future_msgpack_decode(msg.raw_tx[2:])

    sender = txn.sender if txn is not None else ""
    fee = txn.fee if txn is not None else 0
    note = txn.note if txn is not None else None
    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON
    if type(txn) == transactions.transaction.AssetFreezeTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_asset_freeze

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        await confirm_algo_asset_freeze(
            ctx,
            sender,
            txn.rekey_to if txn is not None else None,
            fee,
            str(txn.index)
            if type(txn) is transactions.transaction.AssetFreezeTxn
            else "",
            txn.target if type(txn) is transactions.transaction.AssetFreezeTxn else "",
            txn.new_freeze_state
            if type(txn) is transactions.transaction.AssetFreezeTxn
            else False,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
        )
    elif type(txn) == transactions.transaction.PaymentTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_payment

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        amt = txn.amt if type(txn) is transactions.transaction.PaymentTxn else 0
        receiver = (
            txn.receiver if type(txn) is transactions.transaction.PaymentTxn else ""
        )
        close_remainder_to = (
            txn.close_remainder_to
            if type(txn) is transactions.transaction.PaymentTxn
            else ""
        )
        amount = f"{format_amount(amt, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        await confirm_algo_payment(
            ctx,
            sender,
            receiver,
            close_remainder_to,
            txn.rekey_to if txn is not None else None,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
            fee,
            amount,
        )
    elif type(txn) == transactions.transaction.AssetTransferTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_asset_xfer

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        receiver = (
            txn.receiver
            if type(txn) is transactions.transaction.AssetTransferTxn
            else ""
        )
        index = (
            txn.index if type(txn) is transactions.transaction.AssetTransferTxn else 0
        )
        amount = (
            txn.amount if type(txn) is transactions.transaction.AssetTransferTxn else 0
        )

        token = tokens.token_by_address("ASA", str(index))
        if token == tokens.UNKNOWN_TOKEN:
            amount = str(amount)
        else:
            amount = f"{format_amount(amount, token.decimals)} {token.symbol}"

        await confirm_algo_asset_xfer(
            ctx,
            sender,
            receiver,
            str(index),
            fee,
            amount,
            txn.close_assets_to
            if type(txn) is transactions.transaction.AssetTransferTxn
            else None,
            txn.revocation_target
            if type(txn) is transactions.transaction.AssetTransferTxn
            else None,
            txn.rekey_to if txn is not None else None,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
        )
    elif type(txn) == transactions.transaction.AssetConfigTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_asset_cfg

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        index = txn.index if type(txn) is transactions.transaction.AssetConfigTxn else 0
        total = txn.total if type(txn) is transactions.transaction.AssetConfigTxn else 0
        metadata_hash = (
            txn.metadata_hash
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None
        )
        await confirm_algo_asset_cfg(
            ctx,
            fee,
            sender,
            str(index),
            str(total),
            txn.default_frozen
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.unit_name
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.asset_name
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            str(txn.decimals)
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.manager
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.reserve
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.freeze
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.clawback
            if type(txn) is transactions.transaction.AssetConfigTxn
            else None,
            txn.url if type(txn) is transactions.transaction.AssetConfigTxn else None,
            b2a_base64(metadata_hash)[:-1].decode()
            if metadata_hash is not None
            else None,
            txn.rekey_to if txn is not None else None,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
        )
    elif type(txn) == transactions.transaction.KeyregNonparticipatingTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_keyregNonparticipating

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        nonpart = (
            txn.nonpart
            if type(txn) is transactions.transaction.KeyregNonparticipatingTxn
            else None
        )
        await confirm_algo_keyregNonparticipating(
            ctx,
            sender,
            fee,
            nonpart if type(nonpart) is bool else False,
            txn.rekey_to if txn is not None else None,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
        )
    elif type(txn) == transactions.transaction.KeyregOfflineTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_keyregOnline

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        await confirm_algo_keyregOnline(
            ctx,
            "KEYREG OFFLINE",
            sender,
            fee,
            None,
            None,
            None,
            txn.rekey_to if txn is not None else None,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
        )
    elif type(txn) == transactions.transaction.KeyregOnlineTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_keyregOnline

        fee = f"{format_amount(fee, transactions.constants.COIN_DECIMALS)} {transactions.constants.COIN_SUFFIX}"
        await confirm_algo_keyregOnline(
            ctx,
            "KEYREG ONLINE",
            sender,
            fee,
            txn.votepk if type(txn) is transactions.transaction.KeyregOnlineTxn else "",
            txn.selkey if type(txn) is transactions.transaction.KeyregOnlineTxn else "",
            txn.sprfkey
            if type(txn) is transactions.transaction.KeyregOnlineTxn
            else None,
            txn.rekey_to if txn is not None else None,
            txn.genesis_id if txn is not None else None,
            note.decode("utf-8") if note is not None else None,
        )
    elif type(txn) == transactions.transaction.ApplicationCallTxn:
        from trezor.ui.layouts.lvgl import confirm_algo_app

        await confirm_algo_app(ctx, address, msg.raw_tx)
    else:
        from trezor.ui.layouts.lvgl import confirm_blind_sign_common

        await confirm_blind_sign_common(ctx, address, msg.raw_tx)

    signature = ed25519.sign(node.private_key(), msg.raw_tx)
    await confirm_final(ctx, "ALGO")
    return AlgorandSignedTx(signature=signature)
