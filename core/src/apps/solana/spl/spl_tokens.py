from typing import Iterator


class SPLToken:
    def __init__(self, symbol: str, mint: str, decimals: int):
        self.symbol = symbol
        self.mint = mint
        self.decimals = decimals


def get_spl_token(mint: str) -> SPLToken | None:
    for symbol, _mint, decimals in _spl_tokens_iterator():
        if mint == _mint:
            return SPLToken(symbol, _mint, decimals)
    return None


def _spl_tokens_iterator() -> Iterator[tuple[str, str, int]]:
    # symbol, mint, decimals
    yield ("USDC", "EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v", 6)
    yield ("USDT", "Es9vMFrzaCERmJfrF4H2FYD4KCoNkY11McCe8BenwNYB", 6)
    yield ("JUP", "JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN", 6)
    yield ("RAY", "4k3Dyjzvzp8eMZWUXbBCjEvwSkkk59S5iCNLY3QrkX6R", 6)
    yield ("BONK", "DezXAZ8z7PnrnRJjz3wXBoRgixCa6xjnB7YaB1pPB263", 5)
