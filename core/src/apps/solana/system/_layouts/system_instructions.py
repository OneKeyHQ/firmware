"""Byte layouts for system program instructions."""
import ustruct as struct
from collections import namedtuple

from ...publickey import PublicKey


class InstructionType:
    """Instruction types for system program."""

    CREATE_ACCOUNT = 0
    ASSIGN = 1
    TRANSFER = 2
    CREATE_ACCOUNT_WITH_SEED = 3
    ADVANCE_NONCE_ACCOUNT = 4
    WITHDRAW_NONCE_ACCOUNT = 5
    INITIALIZE_NONCE_ACCOUNT = 6
    AUTHORIZE_NONCE_ACCOUNT = 7
    ALLOCATE = 8
    ALLOCATE_WITH_SEED = 9
    ASSIGN_WITH_SEED = 10
    TRANSFER_WITH_SEED = 11


# PUBLIC_KEY_LAYOUT: bytes(32)
# RUST_STRING_LAYOUT:
#     "length" / Int64ul,
#     "chars" / bytes,

# CREATE_ACCOUNT_LAYOUT:
#     "lamports" / Int64ul,
#     "space" / Int64ul,
#     "program_id" / PUBLIC_KEY_LAYOUT,
class CREATE_ACCOUNT_LAYOUT(
    namedtuple("CREATE_ACCOUNT_LAYOUT", ["lamports", "space", "program_id"])
):
    @staticmethod
    def parse(data: bytes) -> "CREATE_ACCOUNT_LAYOUT":
        lamports, space = struct.unpack("<2Q", data[:16])
        program_id = PublicKey(data[16 : 16 + 32])
        return CREATE_ACCOUNT_LAYOUT(
            lamports=lamports,
            space=space,
            program_id=program_id,
        )


# ASSIGN_LAYOUT:
# "program_id" / PUBLIC_KEY_LAYOUT
class ASSIGN_LAYOUT(namedtuple("ASSIGN_LAYOUT", ["program_id"])):
    @staticmethod
    def parse(data: bytes) -> "ASSIGN_LAYOUT":
        return ASSIGN_LAYOUT(program_id=PublicKey(data[:32]))


# TRANSFER_LAYOUT:
# "lamports" / Int64ul
class TRANSFER_LAYOUT(namedtuple("TRANSFER_LAYOUT", ["lamports"])):
    @staticmethod
    def parse(data: bytes) -> "TRANSFER_LAYOUT":
        return TRANSFER_LAYOUT(*struct.unpack("<Q", data[:8]))


# CREATE_ACCOUNT_WTIH_SEED_LAYOUT:
#     "base_pubkey" / PUBLIC_KEY_LAYOUT,
#     "seed" / RUST_STRING_LAYOUT,
#     "lamports" / Int64ul,
#     "space" / Int64ul,
#     "program_id" / PUBLIC_KEY_LAYOUT,
class CREATE_ACCOUNT_WITH_SEED_LAYOUT(
    namedtuple(
        "CREATE_ACCOUNT_WTIH_SEED_LAYOUT",
        ["base_pubkey", "seed", "lamports", "space", "program_id"],
    )
):
    @staticmethod
    def parse(data: bytes) -> "CREATE_ACCOUNT_WITH_SEED_LAYOUT":
        base_pubkey = PublicKey(data[:32])
        seed_len = struct.unpack("<Q", data[32:40])[0]
        seed = data[40 : 40 + seed_len]
        lamports, space = struct.unpack("<2Q", data[40 + seed_len : 40 + seed_len + 16])
        return CREATE_ACCOUNT_WITH_SEED_LAYOUT(
            base_pubkey=base_pubkey,
            seed=seed.decode("utf-8"),
            lamports=lamports,
            space=space,
            program_id=PublicKey(
                data[40 + seed_len + 16 : 40 + seed_len + 16 + 32]
            ),  # 40+seed_len+16 +32
        )


# WITHDRAW_NONCE_ACCOUNT_LAYOUT:
# "lamports" / Int64ul
class WITHDRAW_NONCE_ACCOUNT_LAYOUT(
    namedtuple("WITHDRAW_NONCE_ACCOUNT_LAYOUT", ["lamports"])
):
    @staticmethod
    def parse(data: bytes) -> "WITHDRAW_NONCE_ACCOUNT_LAYOUT":
        return WITHDRAW_NONCE_ACCOUNT_LAYOUT(*struct.unpack("<Q", data[:8]))


# INITIALIZE_NONCE_ACCOUNT_LAYOUT:
# "authorized" / PUBLIC_KEY_LAYOUT
class INITIALIZE_NONCE_ACCOUNT_LAYOUT(
    namedtuple("INITIALIZE_NONCE_ACCOUNT_LAYOUT", ["authorized"])
):
    @staticmethod
    def parse(data: bytes) -> "INITIALIZE_NONCE_ACCOUNT_LAYOUT":
        return INITIALIZE_NONCE_ACCOUNT_LAYOUT(authorized=PublicKey(data[:32]))


# AUTHORIZE_NONCE_ACCOUNT_LAYOUT:
# "authorized" / PUBLIC_KEY_LAYOUT
class AUTHORIZE_NONCE_ACCOUNT_LAYOUT(
    namedtuple("AUTHORIZE_NONCE_ACCOUNT_LAYOUT", ["authorized"])
):
    @staticmethod
    def parse(data: bytes) -> "AUTHORIZE_NONCE_ACCOUNT_LAYOUT":
        return AUTHORIZE_NONCE_ACCOUNT_LAYOUT(authorized=PublicKey(data[:32]))


# ALLOCATE_LAYOUT:
# "space" / Int64ul
class ALLOCATE_LAYOUT(namedtuple("ALLOCATE_LAYOUT", ["space"])):
    @staticmethod
    def parse(data: bytes) -> "ALLOCATE_LAYOUT":
        return ALLOCATE_LAYOUT(*struct.unpack("<Q", data[:8]))


# ALLOCATE_WITH_SEED_LAYOUT:
#     "base" / PUBLIC_KEY_LAYOUT,
#     "seed" / RUST_STRING_LAYOUT,
#     "space" / Int64ul,
#     "program_id" / PUBLIC_KEY_LAYOUT,
class ALLOCATE_WITH_SEED_LAYOUT(
    namedtuple("ALLOCATE_WITH_SEED_LAYOUT", ["base", "seed", "space", "program_id"])
):
    @staticmethod
    def parse(data: bytes) -> "ALLOCATE_WITH_SEED_LAYOUT":
        base = PublicKey(data[:32])
        seed_len = struct.unpack("<Q", data[32:40])[0]
        seed = data[40 : 40 + seed_len]
        space = struct.unpack("<Q", data[40 + seed_len : 40 + seed_len + 8])
        program_id = PublicKey(data[40 + seed_len + 8 : 40 + seed_len + 8 + 32])
        return ALLOCATE_WITH_SEED_LAYOUT(
            base=base,
            seed=seed.decode("utf-8"),
            space=space,
            program_id=program_id,
        )


# ASSIGN_WITH_SEED_LAYOUT:
#     "base" / PUBLIC_KEY_LAYOUT,
#     "seed" / RUST_STRING_LAYOUT,
#     "program_id" / PUBLIC_KEY_LAYOUT,
class ASSIGN_WITH_SEED_LAYOUT(
    namedtuple("ASSIGN_WITH_SEED_LAYOUT", ["base", "seed", "program_id"])
):
    @staticmethod
    def parse(data: bytes) -> "ASSIGN_WITH_SEED_LAYOUT":
        base = PublicKey(data[:32])
        seed_len = struct.unpack("<Q", data[32:40])[0]
        seed = data[40 : 40 + seed_len]
        return ASSIGN_WITH_SEED_LAYOUT(
            base=base,
            seed=seed.decode("utf-8"),
            program_id=PublicKey(data[40 + seed_len :]),  # 40+seed_len+16 +32
        )


# TRANSFER_WITH_SEED_LAYOUT:
#     "lamports" / Int64ul,
#     "from_seed" / RUST_STRING_LAYOUT,
#     "from_owner" / PUBLIC_KEY_LAYOUT,
class TRANSFER_WITH_SEED_LAYOUT(
    namedtuple("TRANSFER_WITH_SEED_LAYOUT", ["lamports", "from_seed", "from_owner"])
):
    @staticmethod
    def parse(data: bytes) -> "TRANSFER_WITH_SEED_LAYOUT":
        lamports, from_seed_len = struct.unpack("<2Q", data[:16])
        from_seed = data[16 : 16 + from_seed_len]
        from_owner = PublicKey(data[16 + from_seed_len :])  # 16+from_seed_len+16 +32
        return TRANSFER_WITH_SEED_LAYOUT(
            lamports=lamports,
            from_seed=from_seed.decode("utf-8"),
            from_owner=from_owner,
        )


# SYSTEM_INSTRUCTIONS_LAYOUT:
#     "instruction_type" / Int32ul,
#     "args" / instruction_type_args_layout,


class SYSTEM_INSTRUCTIONS_LAYOUT(
    namedtuple("SYSTEM_INSTRUCTIONS_LAYOUT", ["instruction_type", "args"])
):
    @staticmethod
    def parse(data: bytes) -> "SYSTEM_INSTRUCTIONS_LAYOUT":
        (instruction_type,) = struct.unpack("<I", data[:4])
        args = data[4:]
        return SYSTEM_INSTRUCTIONS_LAYOUT(
            instruction_type=instruction_type,
            args=args,
        )
