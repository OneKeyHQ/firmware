# Automatically generated by pb2py
# fmt: off
import protobuf as p

if __debug__:
    try:
        from typing import Dict, List  # noqa: F401
        from typing_extensions import Literal  # noqa: F401
    except ImportError:
        pass


class PublicKeyMultiple(p.MessageType):
    MESSAGE_WIRE_TYPE = 10211

    def __init__(
        self,
        *,
        xpubs: List[str] = None,
    ) -> None:
        self.xpubs = xpubs if xpubs is not None else []

    @classmethod
    def get_fields(cls) -> Dict:
        return {
            1: ('xpubs', p.UnicodeType, p.FLAG_REPEATED),
        }
