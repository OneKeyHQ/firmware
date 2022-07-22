from typing import TYPE_CHECKING

from trezor import utils, wire
from trezor.messages import SEPublicCert

if TYPE_CHECKING:
    from trezor.messages import ReadSEPublicCert


async def se_read_cert(ctx: wire.Context, msg: ReadSEPublicCert) -> SEPublicCert:
    if utils.EMULATOR:
        raise wire.ProcessError("Not support by emulator.")

    import atca  # type: ignore[Import "atca" could not be resolved]

    cert = bytes(412)
    cert_len = bytes(4)

    success = atca.se_read_certificate(cert, cert_len)
    if success:
        cert_len = int.from_bytes(cert_len, "little")
        return SEPublicCert(public_cert=cert[:cert_len])
    else:
        raise wire.ProcessError("se read cert failed")
