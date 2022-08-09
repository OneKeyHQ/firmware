def from_uint8_bytes(uint8: bytes) -> int:
    """Convert from uint8 to python int."""
    return int.from_bytes(uint8, byteorder="little")


def to_uint8_bytes(val: int) -> bytes:
    """Convert an integer to uint8."""
    return val.to_bytes(1, byteorder="little")


def sol_format_amount(
    amount: int, is_token: bool = False, decimals: int | None = None
) -> str:
    """Format amount for display."""
    if is_token:
        return f"{amount if decimals is None else amount / 10 ** decimals} TOKEN"
    return f"{amount / 1_000_000_000} SOL"
