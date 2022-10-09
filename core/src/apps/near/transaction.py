from collections import namedtuple

from trezor import wire

MessageArgs = namedtuple(
    "MessageArgs",
    ["signerId", "publicKey", "nonce", "receiverId", "blockHash", "action"],
)

# action enum
Action_CreateAccount = 0
Action_DeployContract = 1
Action_FunctionCall = 2
Action_Transfer = 3
Action_Stake = 4
Action_AddKey = 5
Action_DeleteKey = 6
Action_DeleteAccount = 7


class Action:
    def __init__(self, action_type: int) -> None:
        self.action_type = action_type

    def set_transfer(self, amount: int) -> None:
        self.amount = amount


class Transaction:
    # {
    #     'kind': 'struct',
    #     'fields': [
    #         ['signerId', 'string'],
    #         ['publicKey', PublicKey],
    #         ['nonce', 'u64'],
    #         ['receiverId', 'string'],
    #         ['blockHash', [32]],
    #         ['actions', [Action]],
    #     ],
    # },
    def __init__(self, args: MessageArgs) -> None:
        self.signerId = args.signerId
        self.publicKey = args.publicKey
        self.nonce = args.nonce
        self.receiverId = args.receiverId
        self.blockHash = args.blockHash
        self.action = args.action

    @staticmethod
    def deserialize(raw_message: bytes) -> "Transaction":
        # singer
        len = int.from_bytes(raw_message[0:4], "little")
        signerId = "0x" + raw_message[4 : len + 4].decode("utf-8")
        offset = len + 4
        # publicKey
        publicKey = raw_message[offset : offset + 33]
        offset += 33
        # nonce
        nonce = raw_message[offset : offset + 8]
        offset += 8
        # receiverId
        len = int.from_bytes(raw_message[offset : offset + 4], "little")
        receiverId = "0x" + raw_message[offset + 4 : offset + 4 + len].decode("utf-8")
        offset += len + 4
        # blockHash
        blockHash = raw_message[offset : offset + 32]
        offset += 32
        # actions_len
        # actions_len = int.from_bytes(raw_message[offset : offset + 4], "little")
        offset += 4
        # if actions_len != 1:
        #     raise wire.DataError("Multiple action")
        # action_type
        action_type = int.from_bytes(raw_message[offset : offset + 1], "little")

        if action_type not in (
            Action_CreateAccount,
            Action_DeployContract,
            Action_FunctionCall,
            Action_Transfer,
            Action_Stake,
            Action_AddKey,
            Action_DeleteKey,
            Action_DeleteAccount,
        ):
            raise wire.DataError("Invalid action")

        action = Action(action_type)
        if action_type == Action_Transfer:
            amount = int.from_bytes(raw_message[offset + 1 : offset + 17], "little")
            action.set_transfer(amount)

        return Transaction(
            MessageArgs(
                signerId=signerId,
                publicKey=publicKey,
                nonce=nonce,
                receiverId=receiverId,
                blockHash=blockHash,
                action=action,
            )
        )
