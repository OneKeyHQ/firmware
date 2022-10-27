# generated from tokens.py.mako
# do not edit manually!
# flake8: noqa
# fmt: off
<%
from collections import defaultdict

def group_tokens(tokens):
    r = defaultdict(list)
    for t in sorted(tokens, key=lambda t: t.type):
        r[t.type].append(t)
    return r
%>\

class TokenInfo:
    def __init__(self, symbol: str, decimals: int) -> None:
        self.symbol = symbol
        self.decimals = decimals


UNKNOWN_TOKEN = TokenInfo("UNKN", 0)


def token_by_address(token_type, address) -> TokenInfo:
% for t_type, tokens in group_tokens(supported_on("trezor2", conflux)).items():
    if token_type == "${t_type}":
        % for t in tokens:
        if address == ${black_repr(t.address)}:
            return TokenInfo(${black_repr(t.symbol)}, ${t.decimals})
        % endfor
% endfor
    return UNKNOWN_TOKEN
