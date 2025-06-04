from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto import base58
from trezor.enums import AmountUnit, OutputScriptType
from trezor.lvglui.scrs import lv
from trezor.messages import SignedPsbt, SignTx, TxInput, TxOutput

from apps.common import address_type

from .common import (
    SigHashType,
    bip340_sign,
    bip340_sign_internal,
    encode_bech32_address,
    tagged_hashwriter,
)
from .keychain import with_keychain
from .psbt.psbt import PSBT
from .psbt.script import is_witness
from .psbt.serialize import ser_string
from .sign_tx import layout
from .sign_tx.sig_hasher import BitcoinSigHasher

# from .sign_tx import tx_weight

if TYPE_CHECKING:
    from apps.common.coininfo import CoinInfo
    from apps.common.keychain import Keychain
    from trezor.messages import SignPsbt


@with_keychain
async def sign_taproot(
    ctx: wire.Context, msg: SignPsbt, keychain: Keychain, coin: CoinInfo
) -> SignedPsbt:
    if not msg.psbt:
        raise wire.DataError("Missing psbt")
    try:
        psbt = PSBT()
        psbt.deserialize(msg.psbt)
    except Exception as e:
        if __debug__:
            import sys

            sys.print_exception(e)  # type: ignore["print_exception" is not a known member of module]
        raise wire.DataError("Invalid psbt")

    sig_hasher = BitcoinSigHasher()
    # weight = tx_weight.TxWeightCalculator()
    total_in = 0
    total_out = 0
    change_out = 0
    master_fp = keychain.root_fingerprint().to_bytes(4, "big")
    found_ours = False
    contains_script_path_spending = False
    for i, input in enumerate(psbt.inputs):
        assert input.prev_txid is not None
        assert input.prev_out is not None
        assert input.sequence is not None

        if input.non_witness_utxo is not None:
            # TODO: check if non-witness UTXO is presigned
            raise wire.DataError("Non-witness UTXO is not allowed")
        if input.witness_utxo is None:
            raise wire.DataError("Missing required witness UTXO")
        if list(input.tap_bip32_paths.items()):
            found_ours = True

        scriptPub = input.witness_utxo.scriptPubKey
        amount = input.witness_utxo.nValue
        is_wit, wit_ver, _ = is_witness(scriptPub)

        assert is_wit and wit_ver == 1, "Only taproot input is allowed"
        total_in += amount
        for key, (_, origin) in input.tap_bip32_paths.items():
            if origin.fingerprint != master_fp:
                if __debug__:
                    print(
                        f"Key fingerprint {origin.fingerprint} does not match master key {master_fp}"
                    )
                raise wire.DataError("Wallet mismatch")
            node = keychain.derive(origin.path)
            intend_key = node.public_key()[1:]
            assert intend_key == key, "Invalid key"
            if not input.tap_scripts:
                assert key == input.tap_internal_key, "Invalid internal key"
            else:
                script, _ = list(input.tap_scripts.keys())[0]
                assert key in script, "Invalid script"
                if not contains_script_path_spending:
                    contains_script_path_spending = True

        sig_hasher.add_input(
            txi=TxInput(
                prev_hash=bytes(reversed(input.prev_txid)),
                prev_index=input.prev_out,
                sequence=input.sequence,
                amount=amount,
            ),
            script_pubkey=scriptPub,
        )
    if not found_ours:
        raise wire.DataError("Invalid PSBT, no tap_bip32_paths present")
    ctx.primary_color, ctx.icon_path = (
        lv.color_hex(coin.primary_color),
        f"A:/res/{coin.icon}",
    )
    for i, output in enumerate(psbt.outputs):
        is_change_out = False
        op_return_data = None
        out = output.get_txout()
        total_out += out.nValue
        wit, ver, prog = out.is_witness()
        out_address = None
        if wit:
            assert coin.bech32_prefix is not None
            out_address = encode_bech32_address(coin.bech32_prefix, ver, prog)
        elif out.is_p2pkh():
            out_address = base58.encode_check(
                address_type.tobytes(coin.address_type) + out.scriptPubKey[3:23],
                coin.b58_hash,
            )
        elif out.is_p2sh():
            out_address = base58.encode_check(
                address_type.tobytes(coin.address_type_p2sh) + out.scriptPubKey[2:22],
                coin.b58_hash,
            )
        elif out.is_opreturn():
            if out.nValue != 0:
                assert (
                    contains_script_path_spending and len(psbt.inputs) == 1
                ), "OpReturn output should have 0 value"
            op_return_data = out.scriptPubKey[2:]
        else:
            raise Exception("Invalid output type")

        if not wit or (wit and ver == 0):
            for _, keypath in output.hd_keypaths.items():
                if keypath.fingerprint != master_fp:
                    if __debug__:
                        print(
                            f"Key fingerprint {keypath.fingerprint} does not match master key {master_fp}"
                        )
                    else:
                        raise wire.DataError(
                            "Master fingerprint does not match master key"
                        )
                change_out += out.nValue
                is_change_out = True
        elif wit and ver == 1:
            for key, (_, origin) in output.tap_bip32_paths.items():
                if not (
                    key == output.tap_internal_key and origin.fingerprint == master_fp
                ):
                    raise wire.DataError(
                        "Invalid parameters, only key path change is allowed"
                    )
                change_out += out.nValue
                is_change_out = True
        sig_hasher.add_output(
            txo=TxOutput(
                amount=out.nValue,
            ),
            script_pubkey=out.scriptPubKey,
        )

        if not is_change_out:
            # display the the output
            output_params = (
                {
                    "op_return_data": op_return_data,
                    "script_type": OutputScriptType.PAYTOOPRETURN,
                }
                if op_return_data
                else {}
            )
            await layout.confirm_output(
                ctx,
                TxOutput(amount=out.nValue, address=out_address, **output_params),
                coin,
                AmountUnit.BITCOIN,
            )
    if total_in <= total_out:
        raise wire.DataError("Insufficient funds")
    tx_locktime = psbt.compute_lock_time()

    if tx_locktime > 0:
        await layout.confirm_nondefault_locktime(
            ctx, tx_locktime, lock_time_disabled=psbt.lock_time_disabled()
        )
    fee = total_in - total_out
    spending = total_in - change_out
    await layout.confirm_total(ctx, spending, fee, 0, coin, AmountUnit.BITCOIN)

    from trezor.ui.layouts import confirm_final

    await confirm_final(ctx, coin.coin_name)

    for i, input in enumerate(psbt.inputs):
        for key, (_, origin) in input.tap_bip32_paths.items():
            if not input.tap_scripts:
                script_path_spending = False
                leaf_hash = None
            else:
                script_path_spending = True
                script, leaf_version = list(input.tap_scripts.keys())[0]
                leaf_hash_writer = tagged_hashwriter(b"TapLeaf")
                leaf_hash_writer.extend(bytes([leaf_version]) + ser_string(script))
                leaf_hash = leaf_hash_writer.get_digest()

            node = keychain.derive(origin.path)
            sigmsg_digest = sig_hasher.hash341(
                i,
                SignTx(
                    outputs_count=0,
                    inputs_count=0,
                    version=psbt.tx_version,
                    lock_time=tx_locktime,
                ),
                SigHashType.SIGHASH_ALL_TAPROOT,
                leaf_hash=leaf_hash,
            )
            if not script_path_spending:
                signature = bip340_sign(node, sigmsg_digest)
            else:
                signature = bip340_sign_internal(node, sigmsg_digest)
            if not script_path_spending:
                input.tap_key_sig = signature
            else:
                assert leaf_hash is not None
                input.tap_script_sigs[(key, leaf_hash)] = signature

    return SignedPsbt(psbt=psbt.serialize())
