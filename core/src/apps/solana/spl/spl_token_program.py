# Instruction Params
from collections import namedtuple

from trezor import wire

from apps.solana.publickey import PublicKey

from ._layouts.token_instructions import (
    AMOUNT2_LAYOUT,
    AMOUNT_LAYOUT,
    INITIALIZE_MINT_LAYOUT,
    INITIALIZE_MULTISIG_LAYOUT,
    SET_AUTHORITY_LAYOUT,
    TOKEN_INSTRUCTIONS_LAYOUT,
    InstructionType,
)


class AuthorityType:
    """Specifies the authority type for SetAuthority instructions."""

    MINT_TOKENS = 0
    """"Authority to mint new tokens."""
    FREEZE_ACCOUNT = 1
    """Authority to freeze any account associated with the Mint."""
    ACCOUNT_OWNER = 2
    """Owner of a given token account."""
    CLOSE_ACCOUNT = 3
    """Authority to close a token account."""


# class InitializeMintParams(NamedTuple):
#     """Initialize token mint transaction params."""
#     mint: PublicKey
#     """Public key of the minter account."""

#     decimals: int
#     """Number of base 10 digits to the right of the decimal place."""
#     mint_authority: PublicKey
#     """The authority/multisignature to mint tokens."""
#     freeze_authority: Optional[PublicKey] = None
#     """The freeze authority/multisignature of the mint."""
InitializeMintParams = namedtuple(
    "InitializeMintParams",
    ["mint", "decimals", "mint_authority", "freeze_authority"],
)

# class InitializeAccountParams(NamedTuple):
#     """Initialize token account transaction params."""
#     account: PublicKey
#     """Public key of the new account."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     owner: PublicKey
#     """Owner of the new account."""
InitializeAccountParams = namedtuple(
    "InitializeAccountParams", ["account", "mint", "owner"]
)


# class InitializeMultisigParams(NamedTuple):
#     """Initialize multisig token account transaction params."""
#     multisig: PublicKey
#     """New multisig account address."""
#     signers: List[PublicKey] = []
#     """Addresses of multisig signers."""
#     m: int
#     """The number of signers (M) required to validate this multisignature account."""
InitializeMultisigParams = namedtuple(
    "InitializeMultisigParams", ["multisig", "signers", "m"]
)

# class TransferParams(NamedTuple):
#     """Transfer token transaction params."""
#     source: PublicKey
#     """Source account."""
#     dest: PublicKey
#     """Destination account."""
#     owner: PublicKey
#     """Owner of the source account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig."""
#     amount: int
#     """Number of tokens to transfer."""
TransferParams = namedtuple(
    "TransferParams", ["source", "dest", "owner", "signers", "amount"]
)

# class ApproveParams(NamedTuple):
#     """Approve token transaction params."""
#     source: PublicKey
#     """Source account."""
#     delegate: PublicKey
#     """Delegate account authorized to perform a transfer of tokens from the source account."""
#     owner: PublicKey
#     """Owner of the source account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig."""
#     amount: int
#     """Maximum number of tokens the delegate may transfer."""
ApproveParams = namedtuple(
    "ApproveParams", ["source", "delegate", "owner", "signers", "amount"]
)

# class RevokeParams(NamedTuple):
#     """Revoke token transaction params."""
#     account: PublicKey
#     """Source account for which transfer authority is being revoked."""
#     owner: PublicKey
#     """Owner of the source account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig."""
RevokeParams = namedtuple("RevokeParams", ["account", "owner", "signers"])

# class SetAuthorityParams(NamedTuple):
#     """Set token authority transaction params."""
#     account: PublicKey
#     """Public key of the token account."""
#     current_authority: PublicKey
#     """Current authority of the specified type."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `current_authority` is a multiSig."""

#     authority: AuthorityType
#     """The type of authority to update."""
#     new_authority: Optional[PublicKey] = None
#     """New authority of the account."""
SetAuthorityParams = namedtuple(
    "SetAuthorityParams",
    [
        "account",
        "current_authority",
        "signers",
        "authority",
        "new_authority",
    ],
)

# class MintToParams(NamedTuple):
#     """Mint token transaction params."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     dest: PublicKey
#     """Public key of the account to mint to."""
#     mint_authority: PublicKey
#     """The mint authority."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `mint_authority` is a multiSig."""

#     amount: int
#     """Amount to mint."""
MintToParams = namedtuple(
    "MintToParams",
    ["mint", "dest", "mint_authority", "signers", "amount"],
)

# class BurnParams(NamedTuple):
#     """Burn token transaction params."""
#     account: PublicKey
#     """Account to burn tokens from."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     owner: PublicKey
#     """Owner of the account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig"""
#     amount: int
#     """Amount to burn."""
BurnParams = namedtuple("BurnParams", ["account", "mint", "owner", "signers", "amount"])

# class CloseAccountParams(NamedTuple):
#     """Close token account transaction params."""
#     account: PublicKey
#     """Address of account to close."""
#     dest: PublicKey
#     """Address of account to receive the remaining balance of the closed account."""
#     owner: PublicKey
#     """Owner of the account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig"""
CloseAccountParams = namedtuple(
    "CloseAccountParams", ["account", "dest", "owner", "signers"]
)

# class FreezeAccountParams(NamedTuple):
#     """Freeze token account transaction params."""
#     account: PublicKey
#     """Account to freeze."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     authority: PublicKey
#     """Mint freeze authority"""
#     multi_signers: List[PublicKey] = []
#     """Signing accounts if `authority` is a multiSig"""
FreezeAccountParams = namedtuple(
    "FreezeAccountParams",
    ["account", "mint", "authority", "multi_signers"],
)

# class ThawAccountParams(NamedTuple):
#     """Thaw token account transaction params."""
#     account: PublicKey
#     """Account to thaw."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     authority: PublicKey
#     """Mint freeze authority"""
#     multi_signers: List[PublicKey] = []
#     """Signing accounts if `authority` is a multiSig"""
ThawAccountParams = namedtuple(
    "ThawAccountParams", ["account", "mint", "authority", "multi_signers"]
)
# class Transfer2Params(NamedTuple):
#     """TransferChecked token transaction params."""
#     source: PublicKey
#     """Source account."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     dest: PublicKey
#     """Destination account."""
#     owner: PublicKey
#     """Owner of the source account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig."""

#     amount: int
#     """Number of tokens to transfer."""
#     decimals: int
#     """Amount decimals."""
Transfer2Params = namedtuple(
    "Transfer2Params",
    ["source", "mint", "dest", "owner", "signers", "amount", "decimals"],
)

# class Approve2Params(NamedTuple):
#     """ApproveChecked token transaction params."""
#     source: PublicKey
#     """Source account."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     delegate: PublicKey
#     """Delegate account authorized to perform a transfer of tokens from the source account."""
#     owner: PublicKey
#     """Owner of the source account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig."""

#     amount: int
#     """Maximum number of tokens the delegate may transfer."""
#     decimals: int
#     """Amount decimals."""
Approve2Params = namedtuple(
    "Approve2Params",
    [
        "source",
        "mint",
        "delegate",
        "owner",
        "signers",
        "amount",
        "decimals",
    ],
)

# class MintTo2Params(NamedTuple):
#     """MintToChecked token transaction params."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     dest: PublicKey
#     """Public key of the account to mint to."""
#     mint_authority: PublicKey
#     """The mint authority."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `mint_authority` is a multiSig."""

#     amount: int
#     """Amount to mint."""
#     decimals: int
#     """Amount decimals."""
MintTo2Params = namedtuple(
    "MintTo2Params",
    ["mint", "dest", "mint_authority", "signers", "amount", "decimals"],
)

# class Burn2Params(NamedTuple):
#     """BurnChecked token transaction params."""
#     mint: PublicKey
#     """Public key of the minter account."""
#     account: PublicKey
#     """Account to burn tokens from."""
#     owner: PublicKey
#     """Owner of the account."""
#     signers: List[PublicKey] = []
#     """Signing accounts if `owner` is a multiSig"""

#     amount: int
#     """Amount to burn."""
#     decimals: int
#     """Amount decimals."""
Burn2Params = namedtuple(
    "Burn2Params",
    ["mint", "account", "owner", "signers", "amount", "decimals"],
)
# class SyncNativeParams(NamedTuple):
#     """Sync native token transaction params."""
#     account: PublicKey
#     """Native account to sync lamports from."""

SyncNativeParams = namedtuple("SyncNativeParams", ["account"])


async def parse(ctx: wire.Context, accounts: list[PublicKey], data: bytes) -> None:
    spl_token_instruction_layout = TOKEN_INSTRUCTIONS_LAYOUT.parse(data)
    instruction_type = spl_token_instruction_layout.instruction_type
    data = spl_token_instruction_layout.args
    # pyright: off
    if instruction_type == InstructionType.INITIALIZE_MINT:
        parsed_data = INITIALIZE_MINT_LAYOUT.parse(data)
        params = InitializeMintParams(
            mint=accounts[0],
            decimals=parsed_data.decimals,
            authority=parsed_data.mint_authority,
            freeze_authority=parsed_data.freeze_authority
            if parsed_data.freeze_authority_option
            else None,
        )
    elif instruction_type == InstructionType.INITIALIZE_ACCOUNT:
        params = InitializeAccountParams(
            account=accounts[0],
            mint=accounts[1],
            owner=accounts[2],
        )
    elif instruction_type == InstructionType.INITIALIZE_MULTISIG:
        parsed_data = INITIALIZE_MULTISIG_LAYOUT.parse(data)
        params = InitializeMultisigParams(
            multisig=accounts[0],
            m=parsed_data.m,
            signers=accounts[1:],
        )
    elif instruction_type == InstructionType.TRANSFER:
        parsed_data = AMOUNT_LAYOUT.parse(data)
        params = TransferParams(
            source=accounts[0],
            dest=accounts[1],
            owner=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
            amount=parsed_data.amount,
        )
        from trezor.ui.layouts import confirm_sol_token_transfer

        await confirm_sol_token_transfer(
            ctx,
            from_addr=str(params.source),
            to_addr=str(params.dest),
            amount=f"{params.amount} Lamports Token",
            source_owner=str(params.owner),
            fee_payer=str(params.owner),
        )
    elif instruction_type == InstructionType.APPROVE:
        parsed_data = AMOUNT_LAYOUT.parse(data)
        params = ApproveParams(
            source=accounts[0],
            delegate=accounts[1],
            owner=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
            amount=parsed_data.amount,
        )
    elif instruction_type == InstructionType.REVOKE:
        params = RevokeParams(
            account=accounts[0],
            owner=accounts[1],
            signers=accounts[2:] if len(accounts) > 2 else [],
        )
    elif instruction_type == InstructionType.SET_AUTHORITY:
        parsed_data = SET_AUTHORITY_LAYOUT.parse(data)
        params = SetAuthorityParams(
            account=accounts[0],
            current_authority=accounts[1],
            signers=accounts[2:] if len(accounts) > 2 else [],
            authority=parsed_data.authority_type,
            new_authority=parsed_data.new_authority
            if parsed_data.new_authority_option
            else None,
        )
    elif instruction_type == InstructionType.MINT_TO:
        parsed_data = AMOUNT_LAYOUT.parse(data)
        params = MintToParams(
            mint=accounts[0],
            dest=accounts[1],
            mint_authority=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
            amount=parsed_data.amount,
        )
    elif instruction_type == InstructionType.BURN:
        parsed_data = AMOUNT_LAYOUT.parse(data)
        params = BurnParams(
            account=accounts[0],
            mint=accounts[1],
            owner=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
            amount=parsed_data.amount,
        )
    elif instruction_type == InstructionType.CLOSE_ACCOUNT:
        params = CloseAccountParams(
            account=accounts[0],
            dest=accounts[1],
            owner=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
        )
    elif instruction_type == InstructionType.FREEZE_ACCOUNT:
        params = FreezeAccountParams(
            account=accounts[0],
            mint=accounts[1],
            authority=accounts[2],
            multi_signers=accounts[3:] if len(accounts) > 3 else [],
        )
    elif instruction_type == InstructionType.THAW_ACCOUNT:
        params = ThawAccountParams(
            account=accounts[0],
            mint=accounts[1],
            authority=accounts[2],
            multi_signers=accounts[3:] if len(accounts) > 3 else [],
        )
    elif instruction_type == InstructionType.TRANSFER2:
        parsed_data = AMOUNT2_LAYOUT.parse(data)
        params = Transfer2Params(
            source=accounts[0],
            mint=accounts[1],
            dest=accounts[2],
            owner=accounts[3],
            signers=accounts[4:] if len(accounts) > 4 else [],
            amount=parsed_data.amount,
            decimals=parsed_data.decimals,
        )
        from trezor.ui.layouts import confirm_sol_token_transfer
        from ..utils.helpers import sol_format_amount, get_spl_token

        await confirm_sol_token_transfer(
            ctx,
            from_addr=str(params.source),
            to_addr=str(params.dest),
            amount=sol_format_amount(
                params.amount, decimals=params.decimals, token_mint=params.mint
            ),
            source_owner=str(params.owner),
            fee_payer=str(params.owner),
            token_mint=None if get_spl_token(str(params.mint)) else str(params.mint),
        )
    elif instruction_type == InstructionType.APPROVE2:
        parsed_data = AMOUNT2_LAYOUT.parse(data)
        params = Approve2Params(
            source=accounts[0],
            mint=accounts[1],
            delegate=accounts[2],
            owner=accounts[3],
            signers=accounts[4:] if len(accounts) > 4 else [],
            amount=parsed_data.amount,
            decimals=parsed_data.decimals,
        )
    elif instruction_type == InstructionType.MINT_TO2:
        parsed_data = AMOUNT2_LAYOUT.parse(data)
        params = MintTo2Params(
            mint=accounts[0],
            dest=accounts[1],
            mint_authority=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
            amount=parsed_data.amount,
            decimals=parsed_data.decimals,
        )
    elif instruction_type == InstructionType.BURN2:
        parsed_data = AMOUNT2_LAYOUT.parse(data)
        params = Burn2Params(
            mint=accounts[0],
            account=accounts[1],
            owner=accounts[2],
            signers=accounts[3:] if len(accounts) > 3 else [],
            amount=parsed_data.amount,
            decimals=parsed_data.decimals,
        )
    elif instruction_type == InstructionType.SYNC_NATIVE:
        # update wrapped SOL balance
        params = SyncNativeParams(
            account=accounts[0],
        )
    # pyright: on
    else:

        raise wire.DataError(
            f"Unknown spl-token program instruction type {instruction_type}"
        )
