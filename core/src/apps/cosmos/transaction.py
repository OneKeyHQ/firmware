from collections import namedtuple
from typing import Any

from trezor import wire

import ujson as json

MessageArgs = namedtuple(
    "MessageArgs",
    ["account_number", "chain_id", "fee", "memo", "msgs", "sequence"],
)

KEY_SUBSTITUTIONS = [
    ["chain_id", "Chain ID"],
    ["account_number", "Account"],
    ["sequence", "Sequence"],
    ["memo", "Memo"],
    ["fee/amount", "Fee"],
    ["fee/gas", "Gas"],
    ["fee/granter", "Granter"],
    ["fee/payer", "Payer"],
    ["msgs/type", "Type"],
    ["tip/amount", "Tip"],
    ["tip/tipper", "Tipper"],
    ["msgs/inputs/address", "Source Address"],
    ["msgs/inputs/coins", "Source Coins"],
    ["msgs/outputs/address", "Dest Address"],
    ["msgs/outputs/coins", "Dest Coins"],
    ["msgs/value/inputs/address", "Source Address"],
    ["msgs/value/inputs/coins", "Source Coins"],
    ["msgs/value/outputs/address", "Dest Address"],
    ["msgs/value/outputs/coins", "Dest Coins"],
    ["msgs/value/from_address", "From"],
    ["msgs/value/to_address", "To"],
    ["msgs/value/amount", "Amount"],
    ["msgs/value/delegator_address", "Delegator"],
    ["msgs/value/validator_address", "Validator"],
    ["msgs/value/validator_src_address", "Validator Source"],
    ["msgs/value/validator_dst_address", "Validator Dest"],
    ["msgs/value/description", "Description"],
    ["msgs/value/initial_deposit/amount", "Deposit Amount"],
    ["msgs/value/initial_deposit/denom", "Deposit Denom"],
    ["msgs/value/proposal_type", "Proposal"],
    ["msgs/value/proposer", "Proposer"],
    ["msgs/value/title", "Title"],
    ["msgs/value/depositer", "Sender"],
    ["msgs/value/proposal_id", "Proposal ID"],
    ["msgs/value/amount", "Amount"],
    ["msgs/value/voter", "Description"],
    ["msgs/value/option", "Option"],
]

VALUE_SUBSTITUTIONS = [
    ["cosmos-sdk/MsgSend", "Send"],
    ["cosmos-sdk/MsgDelegate", "Delegate"],
    ["cosmos-sdk/MsgUndelegate", "Undelegate"],
    ["cosmos-sdk/MsgBeginRedelegate", "Redelegate"],
    ["cosmos-sdk/MsgSubmitProposal", "Propose"],
    ["cosmos-sdk/MsgDeposit", "Deposit"],
    ["cosmos-sdk/MsgVote", "Vote"],
    ["cosmos-sdk/MsgWithdrawDelegationReward", "Withdraw Reward"],
    ["cosmos-sdk/MsgWithdrawValidatorCommission", "Withdraw Val. Commission"],
    ["cosmos-sdk/MsgMultiSend", "Multi Send"],
]

AMOUNT_KEY = [
    "fee/amount",
    "msgs/inputs/coins",
    "msgs/outputs/coins",
    "msgs/value/inputs/coins",
    "msgs/value/outputs/coins",
    "msgs/value/amount",
    "tip/amount",
]


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
        if len(self.fee["amount"]) > 0:
            self.amount = self.fee["amount"][0]["amount"]
            self.denom = self.fee["amount"][0]["denom"]
        else:
            self.amount = None
            self.denom = None
        self.msgs_item = {}

    def display(self, j: Any, level: int, key_prefix: str) -> None:
        if type(j) == str:
            self.msgs_item[key_prefix] = j
            return

        if level == 2:
            self.msgs_item[key_prefix] = json.dumps(j)
        elif type(j) == dict:
            for key, value in j.items():
                self.display(value, level + 1, key_prefix + "/" + key)
        elif type(j) == list:
            for element in j:
                self.display(element, level + 1, key_prefix)
        else:
            self.msgs_item[key_prefix] = json.dumps(j)

    def tx_display_make_friendly(self) -> None:
        for key, value in self.msgs_item.items():
            if key in AMOUNT_KEY:
                j = json.loads(value)
                v = j[0]["amount"] + " " + j[0]["denom"]
                self.msgs_item[key] = v
            else:
                for element in VALUE_SUBSTITUTIONS:
                    if value == element[0]:
                        self.msgs_item[key] = element[1]
                        break

        new_msgs_item = {}
        for key, value in self.msgs_item.items():
            has_key = False
            for element in KEY_SUBSTITUTIONS:
                if key == element[0]:
                    new_msgs_item[element[1]] = value
                    has_key = True
                    break
            if has_key is False:
                new_msgs_item[key] = value
        self.msgs_item = new_msgs_item

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
