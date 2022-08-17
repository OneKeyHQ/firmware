"""Byte layouts for token instruction layouts."""
import ustruct as struct
from collections import namedtuple

from apps.solana.publickey import PublicKey


class InstructionType:
    """Token instruction types."""

    INITIALIZE_MINT = 0
    INITIALIZE_ACCOUNT = 1
    INITIALIZE_MULTISIG = 2
    TRANSFER = 3
    APPROVE = 4
    REVOKE = 5
    SET_AUTHORITY = 6
    MINT_TO = 7
    BURN = 8
    CLOSE_ACCOUNT = 9
    FREEZE_ACCOUNT = 10
    THAW_ACCOUNT = 11
    TRANSFER2 = 12
    APPROVE2 = 13
    MINT_TO2 = 14
    BURN2 = 15
    SYNC_NATIVE = 17


# INITIALIZE_MINT_LAYOUT:
#     "decimals" / Int8ul,
#     "mint_authority" / PUBLIC_KEY_LAYOUT,
#     "freeze_authority_option" / Int8ul,
#     "freeze_authority" / PUBLIC_KEY_LAYOUT,
class INITIALIZE_MINT_LAYOUT(
    namedtuple(
        "INITIALIZE_MINT_LAYOUT",
        ["decimals", "mint_authority", "freeze_authority_option", "freeze_authority"],
    )
):
    @staticmethod
    def parse(data: bytes) -> "INITIALIZE_MINT_LAYOUT":
        (decimals,) = struct.unpack("<B", data[0:1])
        mint_authority = PublicKey(data[1:33])
        (freeze_authority_option,) = struct.unpack("<B", data[33:34])
        freeze_authority = PublicKey(data[34:66])
        return INITIALIZE_MINT_LAYOUT(
            decimals=decimals,
            mint_authority=mint_authority,
            freeze_authority_option=freeze_authority_option,
            freeze_authority=freeze_authority,
        )


# INITIALIZE_MULTISIG_LAYOUT:
# "m" / Int8ul
class INITIALIZE_MULTISIG_LAYOUT(
    namedtuple(
        "INITIALIZE_MULTISIG_LAYOUT",
        ["m"],
    )
):
    @staticmethod
    def parse(data: bytes) -> "INITIALIZE_MULTISIG_LAYOUT":
        return INITIALIZE_MULTISIG_LAYOUT(*struct.unpack("<B", data[0:1]))


# AMOUNT_LAYOUT:
# "amount" / Int64ul
class AMOUNT_LAYOUT(
    namedtuple(
        "AMOUNT_LAYOUT",
        ["amount"],
    )
):
    @staticmethod
    def parse(data: bytes) -> "AMOUNT_LAYOUT":
        return AMOUNT_LAYOUT(*struct.unpack("<Q", data[0:8]))


# SET_AUTHORITY_LAYOUT:
# "authority_type" / Int8ul,
# "new_authority_option" / Int8ul,
# "new_authority" / PUBLIC_KEY_LAYOUT
class SET_AUTHORITY_LAYOUT(
    namedtuple(
        "SET_AUTHORITY_LAYOUT",
        ["authority_type", "new_authority_option", "new_authority"],
    )
):
    @staticmethod
    def parse(data: bytes) -> "SET_AUTHORITY_LAYOUT":
        authority_type, new_authority_option = struct.unpack("<2B", data[0:2])
        new_authority = PublicKey(data[2:34])
        return SET_AUTHORITY_LAYOUT(
            authority_type=authority_type,
            new_authority_option=new_authority_option,
            new_authority=new_authority,
        )


# AMOUNT2_LAYOUT:
# "amount" / Int64ul,
# "decimals" / Int8ul
class AMOUNT2_LAYOUT(namedtuple("AMOUNT2_LAYOUT", ["amount", "decimals"])):
    @staticmethod
    def parse(data: bytes) -> "AMOUNT2_LAYOUT":
        return AMOUNT2_LAYOUT(*struct.unpack("<QB", data[0:9]))


# INSTRUCTIONS_LAYOUT:
#     "instruction_type" / Int8ul,
#     "args"
#     / Switch(
#         lambda this: this.instruction_type,
#         {
#             InstructionType.INITIALIZE_MINT: INITIALIZE_MINT_LAYOUT,
#             InstructionType.INITIALIZE_ACCOUNT: Pass, # No args
#             InstructionType.INITIALIZE_MULTISIG: _INITIALIZE_MULTISIG_LAYOUT,
#             InstructionType.TRANSFER: AMOUNT_LAYOUT,
#             InstructionType.APPROVE: AMOUNT_LAYOUT,
#             InstructionType.REVOKE: Pass,
#             InstructionType.SET_AUTHORITY: SET_AUTHORITY_LAYOUT,
#             InstructionType.MINT_TO: AMOUNT_LAYOUT,
#             InstructionType.BURN: AMOUNT_LAYOUT,
#             InstructionType.CLOSE_ACCOUNT: Pass,
#             InstructionType.FREEZE_ACCOUNT: Pass,
#             InstructionType.THAW_ACCOUNT: Pass,
#             InstructionType.TRANSFER2: AMOUNT2_LAYOUT,
#             InstructionType.APPROVE2: AMOUNT2_LAYOUT,
#             InstructionType.MINT_TO2: AMOUNT2_LAYOUT,
#             InstructionType.BURN2: AMOUNT2_LAYOUT,
#             InstructionType.SYNC_NATIVE: Pass,
#         },
class TOKEN_INSTRUCTIONS_LAYOUT(
    namedtuple("TOKEN_INSTRUCTIONS_LAYOUT", ["instruction_type", "args"])
):
    @staticmethod
    def parse(data: bytes) -> "TOKEN_INSTRUCTIONS_LAYOUT":
        (instruction_type,) = struct.unpack("<B", data[:1])
        args = data[1:]
        return TOKEN_INSTRUCTIONS_LAYOUT(
            instruction_type=instruction_type,
            args=args,
        )
