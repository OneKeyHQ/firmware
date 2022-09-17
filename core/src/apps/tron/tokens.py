# generated from tokens.py.mako
# do not edit manually!
# flake8: noqa
# fmt: off

class TokenInfo:
    def __init__(self, symbol: str, decimals: int) -> None:
        self.symbol = symbol
        self.decimals = decimals


UNKNOWN_TOKEN = TokenInfo("UNKN", 0)


def token_by_address(token_type, address) -> TokenInfo:
    if token_type == "TRC20":
        if address == "THbVQp8kMjStKNnf2iCY6NEzThKMK5aBHg":
            return TokenInfo("DOGE", 8)
        if address == "THb4CqiFdwNHsWsQCs4JhzwjMWys4aqCbF":
            return TokenInfo("ETH", 18)
        if address == "TDyvndWuvX5xTBwHPYJi7J3Yq8pq8yh62h":
            return TokenInfo("HT", 18)
        if address == "TCFLL5dx5ZJdKnWuesXxi1VPwjLVmWZZy9":
            return TokenInfo("JST", 18)
        if address == "TFczxzPhnThNSqr5by8tvxsdCFRRz6cPNq":
            return TokenInfo("NFT", 6)
        if address == "TSSMHYeV2uE9qYH95DqyoCuNCzEL1NvU3S":
            return TokenInfo("SUN", 18)
        if address == "TUpMhErZL2fhh4sVNULAbNKLokS4GjC1F4":
            return TokenInfo("TUSD", 18)
        if address == "TEkxiTehnzSmSe2XqrBj4w32RUN966rdz8":
            return TokenInfo("USDC", 6)
        if address == "TMwFHYXLJaRUPeW6421aqXL4ZEzPRFGkGT":
            return TokenInfo("USDJ", 18)
        if address == "TR7NHqjeKQxGTCi8q8ZY4pL8otSzgjLj6t":
            return TokenInfo("USDT", 6)
        if address == "TXpw8XeWYeTUd4quDskoUqeQPowRh4jY65":
            return TokenInfo("WBTC", 8)
        if address == "TKfjV9RNKJJCqPvBtK8L7Knykh7DNWvnYt":
            return TokenInfo("WBTT", 6)
        if address == "TLa2f6VPqDgRE67v1736s7bJ8Ray5wYjU7":
            return TokenInfo("WIN", 6)
        if address == "TNUC9Qb1rRpS5CbWLmNMxXBjyFoydXjWFR":
            return TokenInfo("WTRX", 6)
    return UNKNOWN_TOKEN
