# generated from tokens.py.mako
# do not edit manually!
# flake8: noqa
# fmt: off

from typing import Iterator
<%
from collections import defaultdict

def group_tokens(tokens):
    r = defaultdict(list)
    for t in sorted(tokens, key=lambda t: t.type):
        r[t.type].append(t)
    return r
%>\


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
% for _, tokens in group_tokens(supported_on("trezor2", tron)).items():
        % for t in tokens:
        yield (  # address, symbol, decimals, name
            ${black_repr(t.address)},
            ${black_repr(t.symbol)},
            ${t.decimals},
            ${black_repr(t.name)}
        )
        % endfor
% endfor
