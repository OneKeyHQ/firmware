from collections import namedtuple

from trezor import wire

from apps.common import cbor

from . import helper

MessageArgs = namedtuple(
    "MessageArgs",
    [
        "version",
        "to",
        "source",
        "nonce",
        "value",
        "gaslimit",
        "gaspremium",
        "gasfeecap",
    ],
)


class Transaction:
    def __init__(self, args: MessageArgs) -> None:
        self.version = args.version
        self.to = args.to
        self.source = args.source
        self.nonce = args.nonce
        self.value = args.value
        self.gaslimit = args.gaslimit
        self.gaspremium = args.gaspremium
        self.gasfeecap = args.gasfeecap

    @staticmethod
    def deserialize(raw_message: bytes, network_prefix: str) -> "Transaction":
        tx = cbor.decode(raw_message)
        if isinstance(tx, list) is not True:
            wire.DataError("unexpected type")
        if len(tx) != 10 or len(tx) != 9:
            wire.DataError("unexpected number items")

        version = tx[0]
        to = helper.format_protocol(tx[1], network_prefix)
        source = helper.format_protocol(tx[2], network_prefix)
        nonce = tx[3]
        value = int.from_bytes(tx[4], "big")
        gaslimit = tx[5]
        gaspremium = int.from_bytes(tx[6], "big")
        gasfeecap = int.from_bytes(tx[7], "big")
        method = tx[8]
        if method != 0:
            wire.DataError("unexpected method")

        return Transaction(
            MessageArgs(
                version=version,
                to=to,
                source=source,
                nonce=nonce,
                value=value,
                gaslimit=gaslimit,
                gaspremium=gaspremium,
                gasfeecap=gasfeecap,
            )
        )
