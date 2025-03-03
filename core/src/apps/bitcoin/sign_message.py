from typing import TYPE_CHECKING

from trezor import utils, wire
from trezor.crypto.curve import bip340, secp256k1
from trezor.enums import InputScriptType
from trezor.lvglui.scrs import lv
from trezor.messages import MessageSignature
from trezor.ui.layouts import confirm_signverify

from apps.common.helpers import validate_message
from apps.common.paths import validate_path
from apps.common.signverify import decode_message, message_digest

from .addresses import address_short, get_address
from .keychain import validate_path_against_script_type, with_keychain
from .scripts import output_script_native_segwit

if TYPE_CHECKING:
    from trezor.messages import SignMessage

    from apps.common.coininfo import CoinInfo
    from apps.common.keychain import Keychain


@with_keychain
async def sign_message(
    ctx: wire.Context, msg: SignMessage, keychain: Keychain, coin: CoinInfo
) -> MessageSignature:
    message = msg.message
    address_n = msg.address_n
    script_type = msg.script_type or InputScriptType.SPENDADDRESS
    no_script_type = msg.no_script_type
    validate_message(message)
    await validate_path(
        ctx, keychain, address_n, validate_path_against_script_type(coin, msg)
    )

    node = keychain.derive(address_n)
    address = get_address(script_type, coin, node)
    ctx.primary_color, ctx.icon_path = (
        lv.color_hex(coin.primary_color),
        f"A:/res/{coin.icon}",
    )
    is_standard = (not no_script_type) or script_type == InputScriptType.SPENDADDRESS
    await confirm_signverify(
        ctx,
        coin.coin_name,
        decode_message(message),
        address_short(coin, address),
        verify=False,
        is_standard=is_standard,
    )
    if not is_standard:
        script_type = InputScriptType.SPENDADDRESS
    seckey = node.private_key()
    if msg.is_bip322_simple:
        if script_type == InputScriptType.SPENDWITNESS:
            from .bip322_simple import sighash_bip143
            from .common import ecdsa_hash_pubkey, SigHashType, ecdsa_sign
            from .scripts import write_witness_p2wpkh

            pubkey_hash = ecdsa_hash_pubkey(node.public_key(), coin)
            script_pub = output_script_native_segwit(0, pubkey_hash)
            sighash = sighash_bip143(
                message, script_pub, pubkey_hash, coin.sign_hash_double
            )
            signature = ecdsa_sign(node, sighash)
            witness = utils.empty_bytearray(
                1 + 1 + len(signature) + 1 + len(node.public_key()) + 1
            )
            write_witness_p2wpkh(
                witness, signature, node.public_key(), SigHashType.SIGHASH_ALL
            )
            signature = witness
        elif script_type == InputScriptType.SPENDTAPROOT:
            from .bip322_simple import sighash_bip341
            from .common import bip340_sign
            from .scripts import write_witness_p2tr
            from .common import ecdsa_hash_pubkey, SigHashType

            output_pubkey = bip340.tweak_public_key(node.public_key()[1:])
            script_pub = output_script_native_segwit(1, output_pubkey)
            sighash = sighash_bip341(message, script_pub)
            signature = bip340_sign(node, sighash)
            witness = utils.empty_bytearray(1 + 1 + len(signature))
            write_witness_p2tr(witness, signature, SigHashType.SIGHASH_ALL_TAPROOT)
            signature = witness
        else:
            raise wire.ProcessError("Unsupported script type")
    else:
        digest = message_digest(coin, message)
        signature = secp256k1.sign(seckey, digest)
        if script_type == InputScriptType.SPENDADDRESS:
            script_type_info = 0
        elif script_type == InputScriptType.SPENDP2SHWITNESS:
            script_type_info = 4
        elif script_type == InputScriptType.SPENDWITNESS:
            script_type_info = 8
        else:
            raise wire.ProcessError("Unsupported script type")

        # Add script type information to the recovery byte.
        if script_type_info != 0 and not no_script_type:
            signature = bytes([signature[0] + script_type_info]) + signature[1:]

    return MessageSignature(address=address, signature=signature)
