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
    if token_type == "CRC20":
        if address == "cfx:acbyc3ahvctpx5cabfw6n1s9fv40trur6ydbu1zr4x":
            return TokenInfo("cBAND", 18)
        if address == "cfx:acczy0zs2fu03pnx0w1u19pkprsm6r50spkz7eg4c3":
            return TokenInfo("cBNB", 18)
        if address == "cfx:acbb225r9wc7a2kt1dz9gw0tuv5v1kgdjuh5akdh3t":
            return TokenInfo("cBTC", 18)
        if address == "cfx:accedvremfhmym60f9u4nghb8utxcgbtb2acewunwh":
            return TokenInfo("cCOMP", 18)
        if address == "cfx:acd3fhs4u0yzx7kpzrujhj15yg63st2z6athmtka95":
            return TokenInfo("cDAI", 18)
        if address == "cfx:acdrf821t59y12b4guyzckyuw2xf1gfpj2ba0x4sj6":
            return TokenInfo("cETH", 18)
        if address == "cfx:acgbjtsmfpex2mbn97dsygtkfrt952sp0psmh8pnvz":
            return TokenInfo("cFLUX", 18)
        if address == "cfx:accuj4mt4kmnhzr1b3xe653n63694tc0cjuzkj5t94":
            return TokenInfo("cLINK", 18)
        if address == "cfx:achcuvuasx3t8zcumtwuf35y51sksewvca0h0hj71a":
            return TokenInfo("cMOON", 18)
        if address == "cfx:aca13suyk7mbgxw9y3wbjn9vd136swu6s21tg67xmb":
            return TokenInfo("cUSDC", 18)
        if address == "cfx:acf2rcsh8payyxpg6xj7b0ztswwh81ute60tsw35j7":
            return TokenInfo("cUSDT", 18)
        if address == "cfx:accxdrf7c3vntwyyhj8ws8mcatd433k8sjvrjbx39r":
            return TokenInfo("cYFII", 18)
        if address == "cfx:achc8nxj7r451c223m18w2dwjnmhkd6rxawrvkvsy2":
            return TokenInfo("FC", 18)
        if address == "cfx:acg158kvr8zanb1bs048ryb6rtrhr283ma70vz70tx":
            return TokenInfo("WCFX", 18)
    return UNKNOWN_TOKEN
