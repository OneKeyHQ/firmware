from typing import TYPE_CHECKING

from trezor.ui.layouts import confirm_signverify

from apps.common import cbor
from apps.common.seed import remove_ed25519_prefix
from apps.common.signverify import decode_message

from . import seed
from .addresses import assert_params_cond
from .helpers.paths import SCHEMA_STAKING_ANY_ACCOUNT

if TYPE_CHECKING:
    from trezor.wire import Context
    from trezor.messages import CardanoSignMessage, CardanoMessageSignature


@seed.with_keychain
async def sign_message(
    ctx: Context, msg: CardanoSignMessage, keychain: seed.Keychain
) -> CardanoMessageSignature:
    from trezor.messages import CardanoMessageSignature, CardanoAddressParametersType
    from trezor.enums import CardanoAddressType
    from apps.common import paths
    from .helpers.paths import SCHEMA_MINT, SCHEMA_PUBKEY
    from trezor.crypto.curve import ed25519
    from trezor import wire
    from .helpers import network_ids, protocol_magics
    from . import addresses
    from trezor.lvglui.scrs import lv
    from . import ICON, PRIMARY_COLOR

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON

    await paths.validate_path(
        ctx,
        keychain,
        msg.address_n,
        # path must match the PUBKEY schema
        SCHEMA_PUBKEY.match(msg.address_n) or SCHEMA_MINT.match(msg.address_n),
    )
    if msg.network_id != network_ids.MAINNET:
        raise wire.ProcessError("Invalid Networ ID")

    address_type = msg.address_type if msg.address_type else CardanoAddressType.BASE
    address_n = msg.address_n
    staking_path = None
    if address_type == CardanoAddressType.BYRON:
        raise wire.ProcessError("Byron addresses are not supported")
    elif address_type in (CardanoAddressType.BASE, CardanoAddressType.REWARD):
        assert len(msg.address_n) == 5, "Invalid address_n length"
        staking_path = msg.address_n[:3]
        staking_path.extend([2, 0])
        assert_params_cond(SCHEMA_STAKING_ANY_ACCOUNT.match(staking_path))
        if address_type == CardanoAddressType.REWARD:
            address_n = None
    elif address_type in (CardanoAddressType.ENTERPRISE,):
        pass
    else:
        raise wire.ProcessError(f"Address type {address_type}not supported")
    address_bytes = addresses.derive_bytes(
        keychain,
        CardanoAddressParametersType(
            address_type=address_type,
            address_n=address_n,
            address_n_staking=staking_path,
            staking_key_hash=None,
            certificate_pointer=None,
            script_payment_hash=None,
            script_staking_hash=None,
        ),
        protocol_magics.MAINNET,
        msg.network_id,
    )
    address = addresses.encode_human_readable(address_bytes)
    await confirm_signverify(
        ctx, "ADA", decode_message(msg.message), address, verify=False
    )

    # verification_key
    node = keychain.derive(msg.address_n)
    verification_key = remove_ed25519_prefix(node.public_key())
    # Sign1Message
    # msg = Sign1Message(
    #     phdr={
    #         Algorithm: EdDSA,
    #         "address": Address(verification_key.hash(), network=network).to_primitive(),
    #     },
    #     payload=message.encode("utf-8"),
    #     uhdr={"hashed": False},
    #     cose_key = {
    #         KpKty: KtyOKP,
    #         OKPKpCurve: Ed25519,
    #         KpKeyOps: [SignOp, VerifyOp],
    #         OKPKpD: signing_key.payload,  # private key
    #         OKPKpX: verification_key.payload,  # public key
    #     }
    # )
    # Sign1Message.phdr
    phdr = {1: -8, "address": address_bytes}  # Algorithm: EdDSA,
    phdr_encoded = cbor.encode(phdr)

    # Sign1Message.uhdr
    uhdr = {"hashed": False}

    # Sign1Message.payload
    payload = msg.message

    sig_structure = ["Signature1", phdr_encoded, b"", payload]
    data = cbor.encode(sig_structure)
    signature = ed25519.sign_ext(node.private_key(), node.private_key_ext(), data)
    message = [phdr_encoded, uhdr, payload, signature]
    signed_message = cbor.encode(message)

    key_to_return = {
        1: 1,  # KpKty: KtyOKP,
        3: -8,  # KpAlg: EdDSA,
        -1: 6,  # OKPKpCurve: Ed25519,
        -2: verification_key,  # OKPKpX: public key
    }
    key = cbor.encode(key_to_return)
    return CardanoMessageSignature(signature=signed_message, key=key)
