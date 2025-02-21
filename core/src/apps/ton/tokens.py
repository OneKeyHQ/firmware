class TokenInfo:
    def __init__(self, symbol: str, decimals: int) -> None:
        self.symbol = symbol
        self.decimals = decimals


UNKNOWN_TOKEN = TokenInfo("UNKN", 0)


def token_by_address(token_type, address) -> TokenInfo:
    if token_type == "TON_TOKEN":
        if address == "EQBynBO23ywHy_CgarY9NK9FTz0yDsG82PtcbSTQgGoXwiuA":
            return TokenInfo("jUSDT", 6)
        if address == "EQCxE6mUtQJKFnGfaROTKOt1lZbDiiX1kCixRv7Nw2Id_sDs":
            return TokenInfo("USDT", 6)
        if address == "EQAvlWFDxGF2lXm67y4yzC17wYKD9A0guwPkMs1gOsM__NOT":
            return TokenInfo("NOT", 9)
    return UNKNOWN_TOKEN
