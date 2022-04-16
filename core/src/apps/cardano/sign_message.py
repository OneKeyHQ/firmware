from typing import TYPE_CHECKING

from trezor.crypto import hashlib
from trezor.ui.layouts import confirm_signverify

from apps.common import cbor
from apps.common.signverify import decode_message

from . import seed

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

    address = addresses.derive_human_readable(
        keychain,
        CardanoAddressParametersType(
            address_type=CardanoAddressType.ENTERPRISE,
            address_n=msg.address_n,
            address_n_staking=[],
            staking_key_hash=None,
            certificate_pointer=None,
            script_payment_hash=None,
            script_staking_hash=None,
        ),
        protocol_magics.MAINNET,
        msg.network_id,
    )
    await confirm_signverify(
        ctx, "ADA", decode_message(msg.message), address, verify=False
    )

    # verification_key
    node = keychain.derive(msg.address_n)
    verification_key = ed25519.publickey(node.private_key())
    verification_key_hash = hashlib.blake2b(verification_key, outlen=28).digest()
    KEY_NONE = 6  # Payment key hash only
    header = (KEY_NONE << 4 | msg.network_id).to_bytes(1, "big")
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
    phdr = {1: -8, "address": header + verification_key_hash}  # Algorithm: EdDSA,
    phdr_encoded = cbor.encode(phdr)

    # Sign1Message.uhdr
    uhdr = {"hashed": False}

    # Sign1Message.payload
    payload = msg.message

    sig_structure = ["Signature1", phdr_encoded, b"", payload]
    data = cbor.encode(sig_structure)
    signature = ed25519.sign(node.private_key(), data)

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
