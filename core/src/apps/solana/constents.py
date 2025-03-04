from .publickey import PublicKey

LEGACY_HEADER_LEN = 3
"""Constant for the length of the legacy Message header."""
VERSIONED_HEADER_LEN = 4
"""Constant for the length of the versioned Message header."""
VERSION_PREFIX_MASK = 0b10000000
"""Mask for the version prefix bit in a serialized Message."""
SIG_LENGTH = 64
"""Constant for standard length of a signature."""

MAX_DATA_SIZE = 1280 - 40 - 8 - SIG_LENGTH
"""Constant for maximum over-the-wire size of a Message."""

# Program IDs
SPL_TOKEN_PROGRAM_ID = PublicKey("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA")

STAKE_PROGRAM_ID = PublicKey("Stake11111111111111111111111111111111111111")

VOTE_PROGRAM_ID = PublicKey("Vote111111111111111111111111111111111111111")

SPL_ASSOCIATED_TOKEN_ACCOUNT_PROGRAM_ID = PublicKey(
    "ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL"
)

SPL_MEMO_PROGRAM_ID = PublicKey("MemoSq4gqABAXKb96qnH8TysNcWxMyWCqXgDLGmfcHr")  # v3

SYS_PROGRAM_ID = PublicKey("11111111111111111111111111111111")

SYSVAR_RENT_PUBKEY: PublicKey = PublicKey("SysvarRent111111111111111111111111111111111")
"""Public key of the synthetic account that serves the network fee resource consumption."""

WRAPPED_SOL_MINT: PublicKey = PublicKey("So11111111111111111111111111111111111111112")
"""Public key of the "Native Mint" for wrapping SOL to SPL token.

The Token Program can be used to wrap native SOL. Doing so allows native SOL to be treated like any
other Token program token type and can be useful when being called from other programs that interact
with the Token Program's interface.
"""

COMPUTE_BUDGET_PROGRAM_ID = PublicKey("ComputeBudget111111111111111111111111111111")
"""Public key of the Compute Budget Program."""
