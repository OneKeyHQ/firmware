from typing import TYPE_CHECKING

from trezor import utils, wire
from trezor.crypto import aes, hmac
from trezor.messages import CipheredKeyValue
from trezor.ui.layouts import confirm_action

from apps.common.keychain import get_keychain
from apps.common.paths import AlwaysMatchingSchema

if TYPE_CHECKING:
    from trezor.messages import CipherKeyValue

# This module implements the SLIP-0011 symmetric encryption of key-value pairs using a
# deterministic hierarchy, see https://github.com/satoshilabs/slips/blob/master/slip-0011.md.


async def cipher_key_value(ctx: wire.Context, msg: CipherKeyValue) -> CipheredKeyValue:
    keychain = await get_keychain(ctx, "secp256k1", [AlwaysMatchingSchema])
    from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

    if len(msg.value) % 16 > 0:
        raise wire.DataError("Value length must be a multiple of 16")

    encrypt = msg.encrypt
    decrypt = not msg.encrypt
    if (encrypt and msg.ask_on_encrypt) or (decrypt and msg.ask_on_decrypt):
        if encrypt:
            title = _(i18n_keys.TITLE__ENCRYPT_VALUE)
        else:
            title = _(i18n_keys.TITLE__DECRYPT_VALUE)
        await confirm_action(
            ctx, "cipher_key_value", title, description=msg.key, anim_dir=2
        )

    node = keychain.derive(msg.address_n)
    value = compute_cipher_key_value(msg, node.private_key())
    return CipheredKeyValue(value=value)


def compute_cipher_key_value(msg: CipherKeyValue, seckey: bytes) -> bytes:
    data = msg.key.encode()
    data += b"E1" if msg.ask_on_encrypt else b"E0"
    data += b"D1" if msg.ask_on_decrypt else b"D0"
    if utils.USE_THD89:
        from trezor.crypto import se_thd89

        if msg.iv and len(msg.iv) == 16:
            iv = msg.iv
            if msg.encrypt:
                return se_thd89.aes256_encrypt(data, msg.value, msg.iv)
            else:
                return se_thd89.aes256_decrypt(data, msg.value, msg.iv)
        else:
            if msg.encrypt:
                return se_thd89.aes256_encrypt(data, msg.value)
            else:
                return se_thd89.aes256_decrypt(data, msg.value)

    else:
        data = hmac(hmac.SHA512, seckey, data).digest()
        key = data[:32]
        if msg.iv and len(msg.iv) == 16:
            iv = msg.iv
        else:
            iv = None

        ctx = aes(aes.CBC, key, iv)
        if msg.encrypt:
            return ctx.encrypt(msg.value)
        else:
            return ctx.decrypt(msg.value)
