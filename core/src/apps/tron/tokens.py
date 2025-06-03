# generated from tokens.py.mako
# do not edit manually!
# flake8: noqa
# fmt: off

from typing import Iterator


class TokenInfo:
    def __init__(self, address: str, symbol: str, decimals: int, name: str) -> None:
        self.address = address
        self.symbol = symbol
        self.decimals = decimals
        self.name = name


UNKNOWN_TOKEN = TokenInfo("", "UNKN", 0, "Unknown Token")


def token_by_address(address: str) -> TokenInfo:
    for addr, symbol, decimal, name in _token_iterator():
        if address == addr:
            return TokenInfo(
                symbol=symbol,
                decimals=decimal,
                address=addr,
                name=name
            )
    return UNKNOWN_TOKEN

def _token_iterator() -> Iterator[tuple[str, str, int, str]]:
        yield (  # address, symbol, decimals, name
            "TAFjULxiVgT4qWk6UZwjqwZXTSaGaqnVp4",
            "BTT",
            18,
            "BitTorrent"
        )
        yield (  # address, symbol, decimals, name
            "THbVQp8kMjStKNnf2iCY6NEzThKMK5aBHg",
            "DOGE",
            8,
            "Dogecoin"
        )
        yield (  # address, symbol, decimals, name
            "THb4CqiFdwNHsWsQCs4JhzwjMWys4aqCbF",
            "ETH",
            18,
            "Ethereum"
        )
        yield (  # address, symbol, decimals, name
            "TDyvndWuvX5xTBwHPYJi7J3Yq8pq8yh62h",
            "HT",
            18,
            "HuobiToken"
        )
        yield (  # address, symbol, decimals, name
            "TCFLL5dx5ZJdKnWuesXxi1VPwjLVmWZZy9",
            "JST",
            18,
            "JUST GOV v1.0"
        )
        yield (  # address, symbol, decimals, name
            "TFczxzPhnThNSqr5by8tvxsdCFRRz6cPNq",
            "NFT",
            6,
            "APENFT"
        )
        yield (  # address, symbol, decimals, name
            "TSSMHYeV2uE9qYH95DqyoCuNCzEL1NvU3S",
            "SUN",
            18,
            "SUN TOKEN"
        )
        yield (  # address, symbol, decimals, name
            "TUpMhErZL2fhh4sVNULAbNKLokS4GjC1F4",
            "TUSD",
            18,
            "TrueUSD"
        )
        yield (  # address, symbol, decimals, name
            "TEkxiTehnzSmSe2XqrBj4w32RUN966rdz8",
            "USDC",
            6,
            "USD Coin"
        )
        yield (  # address, symbol, decimals, name
            "TMwFHYXLJaRUPeW6421aqXL4ZEzPRFGkGT",
            "USDJ",
            18,
            "JUST Stablecoin v1.0"
        )
        yield (  # address, symbol, decimals, name
            "TR7NHqjeKQxGTCi8q8ZY4pL8otSzgjLj6t",
            "USDT",
            6,
            "Tether USD"
        )
        yield (  # address, symbol, decimals, name
            "TXpw8XeWYeTUd4quDskoUqeQPowRh4jY65",
            "WBTC",
            8,
            "Wrapped BTC"
        )
        yield (  # address, symbol, decimals, name
            "TKfjV9RNKJJCqPvBtK8L7Knykh7DNWvnYt",
            "WBTT",
            6,
            "Wrapped BitTorrent"
        )
        yield (  # address, symbol, decimals, name
            "TLa2f6VPqDgRE67v1736s7bJ8Ray5wYjU7",
            "WIN",
            6,
            "WINK"
        )
        yield (  # address, symbol, decimals, name
            "TNUC9Qb1rRpS5CbWLmNMxXBjyFoydXjWFR",
            "WTRX",
            6,
            "Wrapped TRX"
        )
