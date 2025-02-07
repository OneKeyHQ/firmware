from typing import TYPE_CHECKING

from trezor import wire

from .. import seed

if __debug__:
    from trezor import log
if TYPE_CHECKING:
    from typing import Type

    from trezor.messages import CardanoSignTxFinished, CardanoSignTxInit


@seed.with_keychain
async def sign_tx(
    ctx: wire.Context, msg: CardanoSignTxInit, keychain: seed.Keychain
) -> CardanoSignTxFinished:
    from trezor.enums import CardanoTxSigningMode
    from trezor.messages import CardanoSignTxFinished

    from .signer import Signer

    signing_mode = msg.signing_mode  # local_cache_attribute
    from trezor.lvglui.scrs import lv
    from .. import ICON, PRIMARY_COLOR

    ctx.primary_color, ctx.icon_path = lv.color_hex(PRIMARY_COLOR), ICON

    signer_type: Type[Signer]
    if signing_mode == CardanoTxSigningMode.ORDINARY_TRANSACTION:
        from .ordinary_signer import OrdinarySigner

        signer_type = OrdinarySigner
    elif signing_mode == CardanoTxSigningMode.POOL_REGISTRATION_AS_OWNER:
        from .pool_owner_signer import PoolOwnerSigner

        signer_type = PoolOwnerSigner
    elif signing_mode == CardanoTxSigningMode.MULTISIG_TRANSACTION:
        from .multisig_signer import MultisigSigner

        signer_type = MultisigSigner
    elif signing_mode == CardanoTxSigningMode.PLUTUS_TRANSACTION:
        from .plutus_signer import PlutusSigner

        signer_type = PlutusSigner
    else:
        raise RuntimeError  # should be unreachable

    signer = signer_type(ctx, msg, keychain)

    try:
        await signer.sign()
    except ValueError as e:
        if __debug__:
            log.exception(__name__, e)
        raise wire.ProcessError("Signing failed")

    return CardanoSignTxFinished()
