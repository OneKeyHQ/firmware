from collections import namedtuple

from trezor import wire

from ..publickey import PublicKey
from ._layouts.system_instructions import (
    ALLOCATE_LAYOUT,
    ALLOCATE_WITH_SEED_LAYOUT,
    ASSIGN_LAYOUT,
    ASSIGN_WITH_SEED_LAYOUT,
    AUTHORIZE_NONCE_ACCOUNT_LAYOUT,
    CREATE_ACCOUNT_LAYOUT,
    CREATE_ACCOUNT_WITH_SEED_LAYOUT,
    INITIALIZE_NONCE_ACCOUNT_LAYOUT,
    SYSTEM_INSTRUCTIONS_LAYOUT,
    TRANSFER_LAYOUT,
    TRANSFER_WITH_SEED_LAYOUT,
    WITHDRAW_NONCE_ACCOUNT_LAYOUT,
    InstructionType,
)

# Instruction Params
# class CreateAccountParams(NamedTuple):
#     """Create account system transaction params."""

#     from_pubkey: PublicKey
#     """"""
#     new_account_pubkey: PublicKey
#     """"""
#     lamports: int
#     """"""
#     space: int
#     """"""
#     program_id: PublicKey
#     """"""
CreateAccountParams = namedtuple(
    "CreateAccountParams",
    ["from_pubkey", "new_account_pubkey", "lamports", "space", "program_id"],
)

# class TransferParams(NamedTuple):
#     """Transfer system transaction params."""

#     from_pubkey: PublicKey
#     """"""
#     to_pubkey: PublicKey
#     """"""
#     lamports: int
#     """"""
TransferParams = namedtuple("TransferParams", ["from_pubkey", "to_pubkey", "lamports"])

# class AssignParams(NamedTuple):
#     """Assign system transaction params."""

#     account_pubkey: PublicKey
#     """"""
#     program_id: PublicKey
#     """"""

AssignParams = namedtuple("AssignParams", ["account_pubkey", "program_id"])

# class CreateAccountWithSeedParams(NamedTuple):
#     """Create account with seed system transaction params."""

#     from_pubkey: PublicKey
#     """"""
#     new_account_pubkey: PublicKey
#     """"""
#     base_pubkey: PublicKey
#     """"""
#     seed: str
#     """"""
#     lamports: int
#     """"""
#     space: int
#     """"""
#     program_id: PublicKey
#     """"""
CreateAccountWithSeedParams = namedtuple(
    "CreateAccountWithSeedParams",
    [
        "from_pubkey",
        "new_account_pubkey",
        "base_pubkey",
        "seed",
        "lamports",
        "space",
        "program_id",
    ],
)

# class CreateNonceAccountParams(NamedTuple):
#     """Create nonce account system transaction params."""
#     from_pubkey: PublicKey
#     """"""
#     nonce_pubkey: PublicKey
#     """"""
#     authorized_pubkey: PublicKey
#     """"""
#     lamports: int
#     """"""
CreateNonceAccountParams = namedtuple(
    "CreateNonceAccountParams",
    ["from_pubkey", "nonce_pubkey", "authorized_pubkey", "lamports"],
)

# class CreateNonceAccountWithSeedParams(NamedTuple):
#     """Create nonce account with seed system transaction params."""
#     from_pubkey: PublicKey
#     """"""
#     nonce_pubkey: PublicKey
#     """"""
#     authorized_pubkey: PublicKey
#     """"""
#     lamports: int
#     """"""
#     base_pubkey: PublicKey
#     """"""
#     seed: str
#     """"""
CreateNonceAccountWithSeedParams = namedtuple(
    "CreateNonceAccountWithSeedParams",
    [
        "from_pubkey",
        "nonce_pubkey",
        "authorized_pubkey",
        "lamports",
        "base_pubkey",
        "seed",
    ],
)

# class InitializeNonceParams(NamedTuple):
#     """Initialize nonce account system instruction params."""
#     nonce_pubkey: PublicKey
#     """"""
#     authorized_pubkey: PublicKey
#     """"""
InitializeNonceParams = namedtuple(
    "InitializeNonceParams", ["nonce_pubkey", "authorized_pubkey"]
)

# class AdvanceNonceParams(NamedTuple):
#     """Advance nonce account system instruction params."""
#     nonce_pubkey: PublicKey
#     """"""
#     authorized_pubkey: PublicKey
#     """"""
AdvanceNonceParams = namedtuple(
    "AdvanceNonceParams", ["nonce_pubkey", "authorized_pubkey"]
)

# class WithdrawNonceParams(NamedTuple):
#     """Withdraw nonce account system transaction params."""

#     nonce_pubkey: PublicKey
#     """"""
#     authorized_pubkey: PublicKey
#     """"""
#     to_pubkey: PublicKey
#     """"""
#     lamports: int
#     """"""
WithdrawNonceParams = namedtuple(
    "WithdrawNonceParams",
    ["nonce_pubkey", "authorized_pubkey", "to_pubkey", "lamports"],
)

# class AuthorizeNonceParams(NamedTuple):
#     """Authorize nonce account system transaction params."""

#     nonce_pubkey: PublicKey
#     """"""
#     authorized_pubkey: PublicKey
#     """"""
#     new_authorized_pubkey: PublicKey
#     """"""
AuthorizeNonceParams = namedtuple(
    "AuthorizeNonceParams",
    ["nonce_pubkey", "authorized_pubkey", "new_authorized_pubkey"],
)

# class AllocateParams(NamedTuple):
#     """Allocate account with seed system transaction params."""

#     account_pubkey: PublicKey
#     """"""
#     space: int
#     """"""
AllocateParams = namedtuple("AllocateParams", ["account_pubkey", "space"])

# class AllocateWithSeedParams(NamedTuple):
#     """Allocate account with seed system transaction params."""

#     account_pubkey: PublicKey
#     """"""
#     base_pubkey: PublicKey
#     """"""
#     seed: str
#     """"""
#     space: int
#     """"""
#     program_id: PublicKey
#     """"""
AllocateWithSeedParams = namedtuple(
    "AllocateWithSeedParams",
    ["account_pubkey", "base_pubkey", "seed", "space", "program_id"],
)

# class AssignWithSeedParams(NamedTuple):
#     """Assign account with seed system transaction params."""

#     account_pubkey: PublicKey
#     """"""
#     base_pubkey: PublicKey
#     """"""
#     seed: str
#     """"""
#     program_id: PublicKey
#     """"""
AssignWithSeedParams = namedtuple(
    "AssignWithSeedParams", ["account_pubkey", "base_pubkey", "seed", "program_id"]
)
# class TransferWithSeedParams(NamedTuple):
#     """Transfer with seed system transaction params."""

#     from_pubkey: Pubkey
#     """Account that will transfer lamports."""
#     from_base: Pubkey
#     """Base public key to use to derive the funding account address."""
#     to_pubkey: Pubkey
#     """Account that will receive transferred lamports."""
#     lamports: int
#     """Amount of lamports to transfer."""
#     from_seed: str
#     """Seed to use to derive the funding account address."""
#     from_owner: Pubkey
#     """Program id to use to derive the funding account address."""

TransferWithSeedParams = namedtuple(
    "TransferWithSeedParams",
    ["from_pubkey", "from_base", "to_pubkey", "lamports", "from_seed", "from_owner"],
)


async def parse(ctx: wire.Context, accounts: list[PublicKey], data: bytes) -> None:
    system_instruction = SYSTEM_INSTRUCTIONS_LAYOUT.parse(data)
    instruction_type = system_instruction.instruction_type
    data = system_instruction.args
    if instruction_type == InstructionType.CREATE_ACCOUNT:
        parsed_data = CREATE_ACCOUNT_LAYOUT.parse(data)
        _params = CreateAccountParams(  # noqa: F841
            from_pubkey=accounts[0],
            new_account_pubkey=accounts[1],
            lamports=parsed_data.lamports,
            space=parsed_data.space,
            program_id=parsed_data.program_id,
        )
    elif instruction_type == InstructionType.ASSIGN:
        parsed_data = ASSIGN_LAYOUT.parse(data)
        _params = AssignParams(
            account_pubkey=accounts[0], program_id=parsed_data.program_id
        )
    elif instruction_type == InstructionType.TRANSFER:
        parsed_data = TRANSFER_LAYOUT.parse(data)
        transfer_params = TransferParams(
            from_pubkey=accounts[0],
            to_pubkey=accounts[1],
            lamports=parsed_data.lamports,
        )
        await sol_transaction_confirm(ctx, transfer_params)

    elif instruction_type == InstructionType.CREATE_ACCOUNT_WITH_SEED:
        parsed_data = CREATE_ACCOUNT_WITH_SEED_LAYOUT.parse(data)
        _params = CreateAccountWithSeedParams(
            from_pubkey=accounts[0],
            new_account_pubkey=accounts[1],
            base_pubkey=parsed_data.base_pubkey,
            seed=parsed_data.seed,
            lamports=parsed_data.lamports,
            space=parsed_data.space,
            program_id=parsed_data.program_id,
        )
    elif instruction_type == InstructionType.ADVANCE_NONCE_ACCOUNT:
        _params = AdvanceNonceParams(
            nonce_pubkey=accounts[0], authorized_pubkey=accounts[1]
        )
    elif instruction_type == InstructionType.WITHDRAW_NONCE_ACCOUNT:
        parsed_data = WITHDRAW_NONCE_ACCOUNT_LAYOUT.parse(data)
        _params = WithdrawNonceParams(
            nonce_pubkey=accounts[0],
            authorized_pubkey=accounts[1],
            to_pubkey=accounts[2],
            lamports=parsed_data.lamports,
        )
    elif instruction_type == InstructionType.INITIALIZE_NONCE_ACCOUNT:
        parsed_data = INITIALIZE_NONCE_ACCOUNT_LAYOUT.parse(data)
        _params = InitializeNonceParams(
            nonce_pubkey=accounts[0], authorized_pubkey=parsed_data.authorized
        )
    elif instruction_type == InstructionType.AUTHORIZE_NONCE_ACCOUNT:
        parsed_data = AUTHORIZE_NONCE_ACCOUNT_LAYOUT.parse(data)
        _params = AuthorizeNonceParams(
            nonce_pubkey=accounts[0],
            authorized_pubkey=accounts[1],
            new_authorized_pubkey=parsed_data.authorized,
        )
    elif instruction_type == InstructionType.ALLOCATE:
        parsed_data = ALLOCATE_LAYOUT.parse(data)
        _params = AllocateParams(account_pubkey=accounts[0], space=parsed_data.space)
    elif instruction_type == InstructionType.ALLOCATE_WITH_SEED:
        parsed_data = ALLOCATE_WITH_SEED_LAYOUT.parse(data)
        _params = AllocateWithSeedParams(
            account_pubkey=accounts[0],
            base_pubkey=parsed_data.base,
            seed=parsed_data.seed,
            space=parsed_data.space,
            program_id=parsed_data.program_id,
        )
    elif instruction_type == InstructionType.ASSIGN_WITH_SEED:
        parsed_data = ASSIGN_WITH_SEED_LAYOUT.parse(data)
        _params = AssignWithSeedParams(
            account_pubkey=accounts[0],
            base_pubkey=parsed_data.base,
            seed=parsed_data.seed,
            program_id=parsed_data.program_id,
        )  # noqa: F841
    elif instruction_type == InstructionType.TRANSFER_WITH_SEED:
        parsed_data = TRANSFER_WITH_SEED_LAYOUT.parse(data)
        _params = TransferWithSeedParams(  # noqa: F841
            from_pubkey=accounts[0],
            from_base=accounts[1],
            to_pubkey=accounts[2],
            lamports=parsed_data.lamports,
            from_seed=parsed_data.from_seed,
            from_owner=parsed_data.from_owner,
        )
    else:

        raise wire.DataError(f"Unknown instruction type: {instruction_type}")


async def sol_transaction_confirm(
    ctx: wire.Context, transfer_params: TransferParams
) -> None:
    from trezor.ui.layouts.lvgl import confirm_output
    from trezor.ui.layouts.lvgl import confirm_sol_transfer
    from ..utils.helpers import sol_format_amount

    await confirm_output(
        ctx,
        address=str(transfer_params.to_pubkey),
        amount=sol_format_amount(transfer_params.lamports),
    )
    await confirm_sol_transfer(
        ctx,
        from_addr=str(transfer_params.from_pubkey),
        to_addr=str(transfer_params.to_pubkey),
        amount=sol_format_amount(transfer_params.lamports),
        fee_payer=str(transfer_params.from_pubkey),
    )
