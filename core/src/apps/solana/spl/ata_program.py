import ustruct as struct
from collections import namedtuple

from trezor import wire

from apps.solana import constents
from apps.solana.publickey import PublicKey


class InstructionType:
    """Token instruction types."""

    Create = 0
    CreateIdempotent = 1
    RecoverNested = 2  # not supported yet


CreateAssociatedTokenAccountParams = namedtuple(
    "CreateAssociatedTokenAccountParams",
    [
        "funding_account",
        "associated_token_account",
        "wallet_address",
        "token_mint",
        "system_program_id",
        "spl_token_program_id",
    ],
)


async def parse(ctx: wire.Context, accounts: list[PublicKey], data: bytes) -> None:
    """Parse create associated token account transaction params."""
    if data:
        instruction_type = struct.unpack("<B", data[:1])[0]
        if instruction_type not in (
            InstructionType.Create,
            InstructionType.CreateIdempotent,
        ):
            raise wire.DataError(
                f"Instruction type {instruction_type} is not supported"
            )
    assert accounts[4] == constents.SYS_PROGRAM_ID
    assert accounts[5] == constents.SPL_TOKEN_PROGRAM_ID
    params = CreateAssociatedTokenAccountParams(
        funding_account=accounts[0],
        associated_token_account=accounts[1],
        wallet_address=accounts[2],
        token_mint=accounts[3],
        system_program_id=accounts[4],
        spl_token_program_id=accounts[5],
    )
    from trezor.ui.layouts.lvgl import confirm_sol_create_ata

    await confirm_sol_create_ata(
        ctx,
        fee_payer=str(params.funding_account),
        funding_account=str(params.funding_account),
        associated_token_account=str(params.associated_token_account),
        wallet_address=str(params.wallet_address),
        token_mint=str(params.token_mint),
    )
