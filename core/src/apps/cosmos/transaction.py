from collections import namedtuple
from typing import Any

from trezor import wire
from trezor.lvglui.i18n import gettext as _, keys as i18n_keys

import ujson as json

from .networks import format_amount, formatAmont, getChainName

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
    ["msgs/value/vault", "Vault Type"],
    ["msgs/value/signer", "Signer"],
    ["msgs/value/paused", "Paused Type"],
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
    ["dollar/vaults/Lock", "Lock"],
    ["dollar/vaults/Unlock", "Unlock"],
    ["dollar/vaults/SetPausedState", "Set Paused State"],
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
        self.chain_name = getChainName(args.chain_id)
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
        if isinstance(j, (list, dict)):
            if level >= 2:
                self.msgs_item[key_prefix] = json.dumps(j)
                return

            if isinstance(j, dict):
                for key, value in j.items():
                    self.display(value, level + 1, f"{key_prefix}/{key}")
            else:
                for i, element in enumerate(j):
                    self.display(element, level + 1, f"{key_prefix}[{i}]")
        else:
            self.msgs_item[key_prefix] = j

    # not support combined & group tx
    def get_tx_type(self) -> None:
        for key, value in self.msgs_item.items():
            if __debug__:
                print(f"key: {key}, value: {value}")
            if key != "Type":
                continue
            if value == "Send":
                from_address = self.msgs_item["From"]
                to = self.msgs_item["To"]
                amount = self.msgs_item["Amount"]
                self.tx = SendTxn(from_address, to, amount)
            elif value == "Delegate":
                delegator = self.msgs_item["Delegator"]
                validator = self.msgs_item["Validator"]
                amount = self.msgs_item["Amount"]
                self.tx = DelegateTxn(delegator, validator, amount)
            elif value == "Undelegate":
                self.tx = UndelegateTxn()
            elif value == "Redelegate":
                self.tx = RedelegateTxn()
            elif value == "Propose":
                self.tx = ProposeTxn()
            elif value == "Deposit":
                self.tx = DepositTxn()
            elif value == "Vote":
                self.tx = VoteTxn()
                self.msgs_item.pop("Type", None)
            elif value == "Withdraw Reward":
                self.tx = WithdrawRewardTxn()
            elif value == "Withdraw Val. Commission":
                self.tx = WithdrawValCommissionTxn()
            elif value == "Multi Send":
                self.tx = MultiSendTxn()
            elif value in ["Lock", "Unlock", "Set Paused State"]:
                self.tx = NobleVaultTxn(value, self.msgs_item)
            else:
                self.tx = UnknownTxn(key, value)
            break

        # Separate page
        self.msgs_item.pop("Memo", None)
        if not hasattr(self, "tx"):
            self.tx = UnknownTxn("Unknown type", "")

    def tx_display_make_friendly(self) -> None:
        for key, value in self.msgs_item.items():
            if key in AMOUNT_KEY:
                j = json.loads(value)
                if isinstance(j, dict):
                    v = formatAmont(self.chain_id, j["amount"], j["denom"])
                elif isinstance(j, list):
                    v = formatAmont(self.chain_id, j[0]["amount"], j[0]["denom"])
                else:
                    try:
                        amount = int(j)
                    except Exception:
                        v = str(j)
                    else:
                        v = formatAmont(self.chain_id, str(amount), "UNKN")
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

        self.get_tx_type()

    @staticmethod
    def deserialize(raw_message: bytes) -> "Transaction":
        try:
            j = json.loads(raw_message.decode())
        except Exception:
            raise wire.DataError("invalid JSON")
        if any(
            k not in j
            for k in ["account_number", "chain_id", "fee", "memo", "msgs", "sequence"]
        ):
            raise wire.DataError("invalid payload")

        return Transaction(MessageArgs(**j))


class SendTxn:
    def __init__(self, from_address, to, amount):
        self.i18n_title = _(i18n_keys.TITLE__SEND)
        self.i18n_value = "MsgSend"
        self.from_address = from_address
        self.to = to
        self.amount = amount


class DelegateTxn:
    def __init__(self, delegator, validator, amount):
        self.i18n_title = _(i18n_keys.TITLE__DELEGATE)
        self.i18n_value = "MsgDelegate"
        self.delegator = delegator
        self.validator = validator
        self.amount = amount


class UndelegateTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__UNDELEGATE)
        self.i18n_value = "MsgUndelegate"


class RedelegateTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__REDELEGATE)
        self.i18n_value = "MsgBeginRedelegate"


class ProposeTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__PROPOSE)
        self.i18n_value = "MsgSubmitProposal"


class DepositTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__DEPOSIT)
        self.i18n_value = "MsgDeposit"


class VoteTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__VOTE)
        self.i18n_value = "MsgVote"


class WithdrawRewardTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__WITHDRAW_REWARD)
        self.i18n_value = "MsgWithdrawDelegationReward"


class WithdrawValCommissionTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__WITHDRAW_VALIDATOR_COMMISSION)
        self.i18n_value = "MsgWithdrawValidatorCommission"


class MultiSendTxn:
    def __init__(self):
        self.i18n_title = _(i18n_keys.TITLE__MULTI_SEND)
        self.i18n_value = "MsgMultiSend"


class NobleVaultTxn:
    DECIMAL_PLACES = 6
    TOKEN_SYMBOL = "USDN"
    VAULT_TYPES = {1: "STAKE", 2: "FLEXIBLE"}
    DEFAULT_VAULT_TYPE = "UNSPECIFIED"
    PAUSED_TYPES = {1: "LOCK", 2: "UNLOCK", 3: "ALL"}
    DEFAULT_PAUSED_TYPE = "NONE"

    def __init__(self, msg_type: str, msgs_item: dict):
        if msg_type in ["Lock", "Unlock"]:
            self._handle_lock_unlock(msg_type, msgs_item)
        elif msg_type == "Set Paused State":
            self._handle_paused_state(msgs_item)
        else:
            self.tx = UnknownTxn("Unknown type", "")

    def _handle_lock_unlock(self, msg_type: str, msgs_item: dict):
        amount = msgs_item["Amount"].split()[0]
        msgs_item[
            "Amount"
        ] = f"{format_amount(int(amount), self.DECIMAL_PLACES)} {self.TOKEN_SYMBOL}"

        vault_type = msgs_item["Vault Type"]
        msgs_item["Vault Type"] = self.VAULT_TYPES.get(
            vault_type, self.DEFAULT_VAULT_TYPE
        )

        self.i18n_title = f"Noble {msg_type}"
        self.i18n_value = f"Msg{msg_type}"

    def _handle_paused_state(self, msgs_item: dict):
        self.i18n_title = "Noble Paused"
        self.i18n_value = "MsgSetPausedState"
        paused_type = msgs_item["Paused Type"]
        msgs_item["Paused Type"] = self.PAUSED_TYPES.get(
            paused_type, self.DEFAULT_PAUSED_TYPE
        )


class UnknownTxn:
    def __init__(self, type, value):
        self.i18n_title = _(i18n_keys.TITLE__VIEW_TRANSACTION)
        self.i18n_value = value
        self.type = type
