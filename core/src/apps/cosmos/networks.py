from typing import Iterator

from trezor.strings import format_amount


class NetworkInfo:
    def __init__(
        self,
        chainId: str,
        chainName: str,
        coinDenom: str,
        coinMinimalDenom: str,
        coinDecimals: int,
    ) -> None:
        self.chainId = chainId
        self.chainName = chainName
        self.coinDenom = coinDenom
        self.coinMinimalDenom = coinMinimalDenom
        self.coinDecimals = coinDecimals


def getChainName(chainId: str) -> str | None:
    n = by_chain_id(chainId)
    if n is None:
        return None

    return n.chainName


def formatAmont(chainId: str, amount: str, denom: str) -> str:
    n = by_chain_id(chainId)
    if n is None:
        return amount + " " + denom

    if denom == n.coinMinimalDenom:
        sum = f"{format_amount(int(amount), n.coinDecimals)} {n.coinDenom}"
        return sum

    return amount + " " + denom


def by_chain_id(chainId: str) -> "NetworkInfo" | None:
    for n in _networks_iterator():
        if n.chainId == chainId:
            return n
    return None


def _networks_iterator() -> Iterator[NetworkInfo]:
    yield NetworkInfo(
        chainId="cosmoshub-4",
        chainName="Cosmos Hub",
        coinDenom="ATOM",
        coinMinimalDenom="uatom",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="osmosis-1",
        chainName="Osmosis",
        coinDenom="OSMO",
        coinMinimalDenom="uosmo",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="secret-4",
        chainName="Secret Network",
        coinDenom="SCRT",
        coinMinimalDenom="uscrt",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="akashnet-2",
        chainName="Akash",
        coinDenom="AKT",
        coinMinimalDenom="uakt",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="crypto-org-chain-mainnet-1",
        chainName="Crypto.org",
        coinDenom="CRO",
        coinMinimalDenom="basecro",
        coinDecimals=8,
    )
    yield NetworkInfo(
        chainId="iov-mainnet-ibc",
        chainName="Starname",
        coinDenom="IOV",
        coinMinimalDenom="uiov",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="sifchain-1",
        chainName="Sifchain",
        coinDenom="ROWAN",
        coinMinimalDenom="rowan",
        coinDecimals=18,
    )
    yield NetworkInfo(
        chainId="shentu-2.2",
        chainName="Shentu",
        coinDenom="CTK",
        coinMinimalDenom="uctk",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="irishub-1",
        chainName="IRISnet",
        coinDenom="IRIS",
        coinMinimalDenom="uiris",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="regen-1",
        chainName="Regen",
        coinDenom="REGEN",
        coinMinimalDenom="uregen",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="core-1",
        chainName="Persistence",
        coinDenom="XPRT",
        coinMinimalDenom="uxprt",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="sentinelhub-2",
        chainName="Sentinel",
        coinDenom="DVPN",
        coinMinimalDenom="udvpn",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="ixo-4",
        chainName="ixo",
        coinDenom="IXO",
        coinMinimalDenom="uixo",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="emoney-3",
        chainName="e-Money",
        coinDenom="NGM",
        coinMinimalDenom="ungm",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="agoric-3",
        chainName="Agoric",
        coinDenom="BLD",
        coinMinimalDenom="ubld",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="bostrom",
        chainName="Bostrom",
        coinDenom="BOOT",
        coinMinimalDenom="boot",
        coinDecimals=0,
    )
    yield NetworkInfo(
        chainId="juno-1",
        chainName="Juno",
        coinDenom="JUNO",
        coinMinimalDenom="ujuno",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="stargaze-1",
        chainName="Stargaze",
        coinDenom="STARS",
        coinMinimalDenom="ustars",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="axelar-dojo-1",
        chainName="Axelar",
        coinDenom="AXL",
        coinMinimalDenom="uaxl",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="sommelier-3",
        chainName="Sommelier",
        coinDenom="SOMM",
        coinMinimalDenom="usomm",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="umee-1",
        chainName="Umee",
        coinDenom="UMEE",
        coinMinimalDenom="uumee",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="gravity-bridge-3",
        chainName="Gravity Bridge",
        coinDenom="GRAV",
        coinMinimalDenom="ugraviton",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="tgrade-mainnet-1",
        chainName="Tgrade",
        coinDenom="TGD",
        coinMinimalDenom="utgd",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="stride-1",
        chainName="Stride",
        coinDenom="STRD",
        coinMinimalDenom="ustrd",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="evmos_9001-2",
        chainName="Evmos",
        coinDenom="EVMOS",
        coinMinimalDenom="aevmos",
        coinDecimals=18,
    )
    yield NetworkInfo(
        chainId="injective-1",
        chainName="Injective",
        coinDenom="INJ",
        coinMinimalDenom="inj",
        coinDecimals=18,
    )
    yield NetworkInfo(
        chainId="kava_2222-10",
        chainName="Kava",
        coinDenom="KAVA",
        coinMinimalDenom="ukava",
        coinDecimals=6,
    )
    yield NetworkInfo(
        chainId="quicksilver-1",
        chainName="Quicksilver",
        coinDenom="QCK",
        coinMinimalDenom="uqck",
        coinDecimals=6,
    )
