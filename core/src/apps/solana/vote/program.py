"""Byte layouts for vote program instructions."""


class InstructionType:
    """Instruction types for vote program."""

    WITHDRAW_FROM_VOTE_ACCOUNT = 3


# WITHDRAW_FROM_VOTE_ACCOUNT_LAYOUT:
# "lamports" / Int64ul

# VOTE_INSTRUCTIONS_LAYOUT:
#     "instruction_type" / Int32ul,
#     "args"
#     / Switch(
#         lambda this: this.instruction_type,
#         {
#             InstructionType.WITHDRAW_FROM_VOTE_ACCOUNT: WITHDRAW_FROM_VOTE_ACCOUNT_LAYOUT,
#         },
#     )


# # Instruction Params
# class WithdrawFromVoteAccountParams(NamedTuple):
#     """Transfer SOL from vote account to identity."""

#     vote_account_from_pubkey: PublicKey
#     """"""
#     to_pubkey: PublicKey
#     """"""
#     lamports: int
#     """"""
#     withdrawer: PublicKey
#     """"""
# TODO:
