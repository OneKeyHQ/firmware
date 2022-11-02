from trezor import wire
from trezor.crypto.curve import ed25519
from trezor.messages import AptosMessageSignature, AptosSignMessage

from apps.common import paths, seed
from apps.common.keychain import Keychain, auto_keychain

from .helper import aptos_address_from_pubkey


@auto_keychain(__name__)
async def sign_message(
    ctx: wire.Context, msg: AptosSignMessage, keychain: Keychain
) -> AptosMessageSignature:

    await paths.validate_path(ctx, keychain, msg.address_n)

    node = keychain.derive(msg.address_n)
    pub_key_bytes = seed.remove_ed25519_prefix(node.public_key())
    address = aptos_address_from_pubkey(pub_key_bytes)

    full_message = "APTOS\n"
    payload = msg.payload
    if payload.address:
        full_message += "address: " + payload.address + "\n"
    if payload.application:
        full_message += "application: " + payload.application + "\n"
    if payload.chain_id:
        full_message += "chainId: " + payload.chain_id + "\n"
    full_message += "message: " + payload.message + "\n"
    full_message += "nonce: " + payload.nonce
    # display here
    from trezor.ui.layouts import confirm_signverify

    await confirm_signverify(ctx, "Aptos", full_message, address, False)

    signature = ed25519.sign(node.private_key(), full_message.encode())
    return AptosMessageSignature(signature=signature, address=address)
