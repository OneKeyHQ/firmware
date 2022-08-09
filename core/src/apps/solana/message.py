from collections import namedtuple

from trezor.crypto import base58

from .constents import HEADER_LENGTH
from .publickey import PublicKey
from .utils import shortvec_encoding as shortvec

#  """An instruction to execute by a program."""
#  accounts: bytes | list[int]
#  """Ordered indices into the transaction keys array indicating which accounts to pass to the program."""
#  program_id_index: int
#  """Index into the transaction keys array indicating the program account that executes this instruction."""
#  data: bytes
#  """The program input data encoded as base58."""
CompiledInstruction = namedtuple(
    "CompiledInstruction", ["accounts", "program_id_index", "data"]
)

#  """The message header, identifying signed and read-only account."""
#  num_required_signatures: int
#  """The number of signatures required for this message to be considered valid."""
#  num_readonly_signed_accounts: int
#  """The last `numReadonlySignedAccounts` of the signed keys are read-only accounts."""
#  num_readonly_unsigned_accounts: int
#  """The last `numReadonlySignedAccounts` of the unsigned keys are read-only accounts."""
MessageHeader = namedtuple(
    "MessageHeader",
    [
        "num_required_signatures",
        "num_readonly_signed_accounts",
        "num_readonly_unsigned_accounts",
    ],
)

#  """Message constructor arguments."""
#  header: MessageHeader
#  """The message header, identifying signed and read-only `accountKeys`."""
#  account_keys: list[str]
#  """All the account keys used by this transaction."""
#  recent_blockhash: str
#  """The hash of a recent ledger block."""
#  instructions: list[CompiledInstruction]
#  """Instructions that will be executed in sequence and committed in one atomic transaction if all succeed."""
MessageArgs = namedtuple(
    "MessageArgs", ["header", "account_keys", "recent_blockhash", "instructions"]
)


class Message:
    """Message object to be used to to build a transaction.

    A message contains a header, followed by a compact-array of account addresses, followed by a recent blockhash,
    followed by a compact-array of instructions.
    """

    def __init__(self, args: MessageArgs) -> None:
        """Init message object."""
        self.header = args.header
        self.account_keys = [PublicKey(key) for key in args.account_keys]
        self.recent_blockhash = args.recent_blockhash
        self.instructions = args.instructions

    @staticmethod
    def deserialize(raw_message: bytes) -> "Message":

        num_required_signatures = raw_message[0]
        num_readonly_signed_accounts = raw_message[1]
        num_readonly_unsigned_accounts = raw_message[2]
        header = MessageHeader(
            num_required_signatures=num_required_signatures,
            num_readonly_signed_accounts=num_readonly_signed_accounts,
            num_readonly_unsigned_accounts=num_readonly_unsigned_accounts,
        )
        raw_message = raw_message[HEADER_LENGTH:]

        account_keys = []
        accounts_length, accounts_offset = shortvec.decode_length(raw_message)
        for _ in range(accounts_length):
            key_bytes = raw_message[
                accounts_offset : accounts_offset + PublicKey.LENGTH
            ]
            account_keys.append(str(PublicKey(key_bytes)))
            accounts_offset += PublicKey.LENGTH
        raw_message = raw_message[accounts_offset:]

        recent_blockhash = base58.encode(raw_message[: PublicKey.LENGTH])
        raw_message = raw_message[PublicKey.LENGTH :]  # noqa: E203

        instructions = []
        instruction_count, offset = shortvec.decode_length(raw_message)
        raw_message = raw_message[offset:]
        for _ in range(instruction_count):
            program_id_index = raw_message[0]
            raw_message = raw_message[1:]

            accounts_length, offset = shortvec.decode_length(raw_message)
            raw_message = raw_message[offset:]
            accounts = raw_message[:accounts_length]
            raw_message = raw_message[accounts_length:]

            data_length, offset = shortvec.decode_length(raw_message)
            raw_message = raw_message[offset:]
            data = raw_message[:data_length]  # base58.encode(raw_message[:data_length])
            raw_message = raw_message[data_length:]

            instructions.append(
                CompiledInstruction(
                    program_id_index=program_id_index, accounts=accounts, data=data
                )
            )

        return Message(
            MessageArgs(
                header=header,
                account_keys=account_keys,
                recent_blockhash=recent_blockhash,
                instructions=instructions,
            )
        )
