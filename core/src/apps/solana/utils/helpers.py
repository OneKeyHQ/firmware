from trezor.strings import format_amount

from ..publickey import PublicKey
from ..spl.spl_tokens import get_spl_token


def from_uint8_bytes(uint8: bytes) -> int:
    """Convert from uint8 to python int."""
    return int.from_bytes(uint8, byteorder="little")


def to_uint8_bytes(val: int) -> bytes:
    """Convert an integer to uint8."""
    return val.to_bytes(1, byteorder="little")


def sol_format_amount(
    amount: int, decimals: int | None = None, token_mint: PublicKey | None = None
) -> str:
    """Format amount for display."""
    if not token_mint:
        return f"{format_amount(amount, decimals=9)} SOL"

    token = get_spl_token(str(token_mint))
    symbol = token.symbol if token else "Token"
    token_decimals = decimals or (token.decimals if token else 0)

    return f"{format_amount(amount, decimals=token_decimals)} {symbol}"
