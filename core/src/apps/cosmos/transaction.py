from collections import namedtuple

from trezor import wire

import ujson as json

MessageArgs = namedtuple(
    "MessageArgs",
    ["account_number", "chain_id", "fee", "memo", "msgs", "sequence"],
)


class Transaction:
    """
    Transactions passed to the Onekey device will be in the following format.
    {
        "account_number": {number},
        "chain_id": {string},
        "fee": {
            "amount": [{"amount": {number}, "denom": {string}}, ...],
            "gas": {number}
        },
        "memo": {string},
        "msgs": [{arbitrary}],
        "sequence": {number}
    }

    Note:
        1. all the {number} values must be passed as string.
        2. msgs is a list of messages, which are arbitrary JSON structures.
        3. No spaces or newlines can be included, other than escaped in strings
        4. All dictionaries must be serialized in lexicographical key order
    """

    def __init__(self, args: MessageArgs) -> None:
        self.account_number = args.account_number
        self.chain_id = args.chain_id
        self.fee = args.fee
        self.memo = args.memo
        self.msgs = args.msgs
        self.sequence = args.sequence
        self.gas = self.fee["gas"]
        self.amount = self.fee["amount"][0]["amount"]
        self.denom = self.fee["amount"][0]["denom"]

    @staticmethod
    def deserialize(raw_message: bytes) -> "Transaction":
        j = json.loads(raw_message.decode("utf-8"))
        if (
            "account_number" not in j
            or "chain_id" not in j
            or "fee" not in j
            or "memo" not in j
            or "msgs" not in j
            or "sequence" not in j
        ):
            raise wire.DataError("Invalid action")

        account_number = j["account_number"]
        chain_id = j["chain_id"]
        fee = j["fee"]
        memo = j["memo"]
        msgs = j["msgs"]
        sequence = j["sequence"]

        return Transaction(
            MessageArgs(
                account_number=account_number,
                chain_id=chain_id,
                fee=fee,
                memo=memo,
                msgs=msgs,
                sequence=sequence,
            )
        )
