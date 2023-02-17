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
    if token_type == "ASA":
        if address == "692085161":
            return TokenInfo("ADAO", 6)
        if address == "463554836":
            return TokenInfo("ALGF", 6)
        if address == "724480511":
            return TokenInfo("ALGX", 6)
        if address == "163650":
            return TokenInfo("ARCC", 6)
        if address == "342889824":
            return TokenInfo("BOARD", 6)
        if address == "137020565":
            return TokenInfo("BUY", 2)
        if address == "657291910":
            return TokenInfo("CCT", 0)
        if address == "297995609":
            return TokenInfo("CHOICE", 2)
        if address == "571576867":
            return TokenInfo("COSG", 6)
        if address == "470842789":
            return TokenInfo("DEFLY", 6)
        if address == "793124631":
            return TokenInfo("GALGO", 6)
        if address == "684649988":
            return TokenInfo("GARD", 6)
        if address == "386192725":
            return TokenInfo("GOBTC", 8)
        if address == "386195940":
            return TokenInfo("GOETH", 8)
        if address == "441139422":
            return TokenInfo("GOMINT", 6)
        if address == "403499324":
            return TokenInfo("GP", 0)
        if address == "137594422":
            return TokenInfo("HDL", 6)
        if address == "559219992":
            return TokenInfo("OCTO", 6)
        if address == "287867876":
            return TokenInfo("OPUL", 10)
        if address == "744665252":
            return TokenInfo("PBTC", 8)
        if address == "2751733":
            return TokenInfo("RIO", 7)
        if address == "300208676":
            return TokenInfo("SMILE", 6)
        if address == "465865291":
            return TokenInfo("STBL", 6)
        if address == "511484048":
            return TokenInfo("STKE", 2)
        if address == "31566704":
            return TokenInfo("USDC", 6)
        if address == "312769":
            return TokenInfo("USDt", 6)
        if address == "700965019":
            return TokenInfo("VEST", 6)
        if address == "607591690":
            return TokenInfo("XGLI", 6)
        if address == "792313023":
            return TokenInfo("XSOL", 9)
        if address == "444035862":
            return TokenInfo("ZONE", 6)
    return UNKNOWN_TOKEN
