# generated from tokens.py.mako
# (by running `make templates` in `core`)
# do not edit manually!
# fmt: off

# NOTE: returning a tuple instead of `TokenInfo` from the "data" function
# saves 5600 bytes of flash size. Implementing the `_token_iterator`
# instead of if-tree approach saves another 5600 bytes.

# NOTE: interestingly, it did not save much flash size to use smaller
# parts of the address, for example address length of 10 bytes saves
# 1 byte per entry, so 1887 bytes overall (and further decrease does not help).
# (The idea was not having to store the whole address, even a smaller part
# of it has enough collision-resistance.)
# (In the if-tree approach the address length did not have any effect whatsoever.)

from typing import Iterator

from trezor.messages import EthereumTokenInfo

UNKNOWN_TOKEN = EthereumTokenInfo(
    symbol="Wei UNKN",
    decimals=0,
    address=b"",
    chain_id=0,
    name="Unknown token",
)


def token_by_chain_address(chain_id: int, address: bytes) -> EthereumTokenInfo | None:
    for addr, symbol, decimal, name in _token_iterator(chain_id):
        if address == addr:
            return EthereumTokenInfo(
                symbol=symbol,
                decimals=decimal,
                address=address,
                chain_id=chain_id,
                name=name,
            )
    return UNKNOWN_TOKEN


def _token_iterator(chain_id: int) -> Iterator[tuple[bytes, str, int, str]]:
    if chain_id == 1:  # ETH
        yield (  # address, symbol, decimals, name
            b"\x11\x11\x11\x11\x11\x17\xdc\x0a\xa7\x8b\x77\x0f\xa6\xa7\x38\x03\x41\x20\xc3\x02",
            "1INCH",
            18,
            "1INCH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\xc6\x65\x00\xc8\x4a\x76\xad\x7e\x9c\x93\x43\x7b\xfc\x5a\xc3\x3e\x2d\xda\xe9",
            "AAVE",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\x90\x33\x3c\xef\x9e\x6d\x52\x8d\xd5\x61\x8f\xb9\x7a\x76\xf2\x68\xf3\xed\xd4",
            "ANKR",
            18,
            "Ankr Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\x22\x44\x52\x80\x1a\xce\xd8\xb2\xf0\xae\xbe\x15\x53\x79\xbb\x5d\x59\x43\x81",
            "APE",
            18,
            "ApeCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\x0e\x17\xef\x65\xf8\x2a\xb0\x18\xd8\xed\xd7\x76\xe8\xdd\x94\x03\x27\xb2\x8b",
            "AXS",
            18,
            "Axie Infinity Shard",
        )
        yield (  # address, symbol, decimals, name
            b"\xba\x10\x00\x00\x62\x5a\x37\x54\x42\x39\x78\xa6\x0c\x93\x17\xc5\x8a\x42\x4e\x3d",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x87\x75\xf6\x48\x43\x06\x79\xa7\x09\xe9\x8d\x2b\x0c\xb6\x25\x0d\x28\x87\xef",
            "BAT",
            18,
            "Basic Attention Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\x4b\x46\x69\x6b\x2b\xb4\x79\x4e\xb3\xd4\xc2\x6f\x1c\x55\xf9\x17\x0f\xa4\xc5",
            "BIT",
            18,
            "BitDAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\xab\xb1\x45\xd6\x46\x52\xa9\x48\xd7\x25\x33\x02\x3f\x6e\x7a\x62\x3c\x7c\x53",
            "BUSD",
            18,
            "Binance USD",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x0e\x94\xcb\x66\x2c\x35\x20\x28\x2e\x6f\x57\x17\x21\x40\x04\xa7\xf2\x68\x88",
            "COMP",
            18,
            "Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\x33\xa9\x49\x74\x0b\xb3\x30\x6d\x11\x9c\xc7\x77\xfa\x90\x0b\xa0\x34\xcd\x52",
            "CRV",
            18,
            "Curve DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x17\x54\x74\xe8\x90\x94\xc4\x4d\xa9\x8b\x95\x4e\xed\xea\xc4\x95\x27\x1d\x0f",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x0f\x51\xbb\x10\x11\x97\x27\xa7\xe5\xea\x35\x38\x07\x4f\xb3\x41\xf5\x6b\x09\xad",
            "DAO",
            18,
            "DAO Maker",
        )
        yield (  # address, symbol, decimals, name
            b"\x92\xd6\xc1\xe3\x1e\x14\x52\x0e\x67\x6a\x68\x7f\x0a\x93\x78\x8b\x71\x6b\xef\xf5",
            "DYDX",
            18,
            "dYdX",
        )
        yield (  # address, symbol, decimals, name
            b"\xf6\x29\xcb\xd9\x4d\x37\x91\xc9\x25\x01\x52\xbd\x8d\xfb\xdf\x38\x0e\x2a\x3b\x9c",
            "ENJ",
            18,
            "Enjin Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xc1\x83\x60\x21\x7d\x8f\x7a\xb5\xe7\xc5\x16\x56\x67\x61\xea\x12\xce\x7f\x9d\x72",
            "ENS",
            18,
            "Ethereum Name Service",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\x15\x36\x1f\xd6\xb4\xbb\x60\x9f\xa6\x3c\x81\xa2\xbe\x19\xd8\x73\x71\x78\x70",
            "FTM",
            18,
            "Fantom",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xd1\xc9\x77\x19\x02\x47\x60\x76\xec\xfc\x8b\x2a\x83\xad\x6b\x93\x55\xa4\xc9",
            "FTX Token",
            18,
            "FTT",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\x10\xe7\x76\x88\x0c\x02\x93\x3d\x47\xdb\x1b\x9f\xc0\x59\x08\xe5\x38\x6b\x96",
            "GNO",
            18,
            "Gnosis Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x44\xe9\x0c\x64\xb2\xc0\x76\x62\xa2\x92\xbe\x62\x44\xbd\xf0\x5c\xda\x44\xa7",
            "GRT",
            18,
            "Graph Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x05\x6f\xd4\x09\xe1\xd7\xa1\x24\xbd\x70\x17\x45\x9d\xfe\xa2\xf3\x87\xb6\xd5\xcd",
            "GUSD",
            2,
            "Gemini dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x03\x16\xeb\x71\x48\x5b\x0a\xb1\x41\x03\x30\x7b\xf6\x5a\x02\x10\x42\xc6\xd3\x80",
            "HBTC",
            18,
            "Huobi BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xe7\xc6\xbf\x46\x9e\x97\xee\xb0\xbf\xb7\x4c\x8d\xbf\xf5\xbd\x47\xd4\xc1\xc9\x8a",
            "HSK",
            18,
            "HashKey Platform Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\x25\x96\x37\xdc\xd7\x4c\x76\x77\x81\xe3\x7b\xc6\x13\x3c\xd6\xa6\x8a\xa1\x61",
            "HT",
            18,
            "HuobiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xb3\xe0\xa2\x17\x40\x7e\xff\xf7\xca\x06\x2d\x46\xc2\x6e\x5d\x60\xa1\x4d\x69",
            "IOTX",
            18,
            "IoTeX Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x5a\x98\xfc\xbe\xa5\x16\xcf\x06\x85\x72\x15\x77\x9f\xd8\x12\xca\x3b\xef\x1b\x32",
            "LDO",
            18,
            "Lido DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x49\x10\x77\x1a\xf9\xca\x65\x6a\xf8\x40\xdf\xf8\x3e\x82\x64\xec\xf9\x86\xca",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xbb\xbb\xca\x6a\x90\x1c\x92\x6f\x24\x0b\x89\xea\xcb\x64\x1d\x8a\xec\x7a\xea\xfd",
            "LRC",
            18,
            "LoopringCoin V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x1a\xfa\x7b\x71\x8f\xb8\x93\xdb\x30\xa3\xab\xc0\xcf\xc6\x08\xaa\xcf\xeb\xb0",
            "MATIC",
            18,
            "Polygon",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xd8\xa9\xc4\x5b\x2e\xca\x88\x64\x37\x3a\x26\xd1\x45\x9e\x3d\xff\x1e\x17\xf3",
            "MIM",
            18,
            "Magic Internet Money",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x8f\x72\xaa\x93\x04\xc8\xb5\x93\xd5\x55\xf1\x2e\xf6\x58\x9c\xc3\xa5\x79\xa2",
            "MKR",
            18,
            "Maker",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\x21\x32\xe3\x5a\x6c\x13\xee\x1e\xe0\xf8\x4d\xc5\xd4\x0b\xad\x8d\x81\x52\x06",
            "NEXO",
            18,
            "Nexo",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\xe8\x32\x13\xd5\x63\x08\x33\x0e\xc3\x02\xa8\xbd\x64\x1f\x1d\x01\x13\xa4\xcc",
            "NU",
            18,
            "NuCypher",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xaa\x33\x64\xf1\x7a\x4d\x01\xc6\xf1\x75\x1f\xd9\x7c\x2b\xd3\xd7\xe7\xf1\xd5",
            "OHM",
            9,
            "Olympus",
        )
        yield (  # address, symbol, decimals, name
            b"\x75\x23\x1f\x58\xb4\x32\x40\xc9\x71\x8d\xd5\x8b\x49\x67\xc5\x11\x43\x42\xa8\x6c",
            "OKB",
            18,
            "OKB",
        )
        yield (  # address, symbol, decimals, name
            b"\xd2\x61\x14\xcd\x6e\xe2\x89\xac\xcf\x82\x35\x0c\x8d\x84\x87\xfe\xdb\x8a\x0c\x07",
            "OMG",
            18,
            "OMGToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\x5e\x53\xcb\xb8\x60\x18\xac\x2b\x80\x92\xfd\xcd\x39\xd8\x44\x4a\xff\xc3\xf6",
            "POL",
            18,
            "Polygon",
        )
        yield (  # address, symbol, decimals, name
            b"\x95\xad\x61\xb0\xa1\x50\xd7\x92\x19\xdc\xf6\x4e\x1e\x6c\xc0\x1f\x0b\x64\xc4\xce",
            "SHIB",
            18,
            "SHIBA INU",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x11\xa7\x3e\xe8\x57\x6f\xb4\x6f\x5e\x1c\x57\x51\xca\x3b\x9f\xe0\xaf\x2a\x6f",
            "SNX",
            18,
            "Synthetix Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6b\x35\x95\x06\x87\x78\xdd\x59\x2e\x39\xa1\x22\xf4\xf5\xa5\xcf\x09\xc9\x0f\xe2",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x8c\xe9\x13\x7d\x39\x32\x6a\xd0\xcd\x64\x91\xfb\x5c\xc0\xcb\xa0\xe0\x89\xb6\xa9",
            "SXP",
            18,
            "Swipe",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x00\x00\x00\x00\x08\x5d\x47\x80\xb7\x31\x19\xb6\x44\xae\x5e\xcd\x22\xb3\x76",
            "TUSD",
            18,
            "TrueUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x1f\x98\x40\xa8\x5d\x5a\xf5\xbf\x1d\x17\x62\xf9\x25\xbd\xad\xdc\x42\x01\xf9\x84",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xb8\x69\x91\xc6\x21\x8b\x36\xc1\xd1\x9d\x4a\x2e\x9e\xb0\xce\x36\x06\xeb\x48",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\x4c\x6a\xd9\x2f\xd0\x80\xe4\x00\x4b\x23\x12\xb4\x5f\x79\x6a\x19\x2d\x27\xa0",
            "USDN",
            18,
            "Neutrino USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\x87\x0d\x67\xf6\x60\xd9\x5d\x5b\xe5\x30\x38\x0d\x0e\xc0\xbd\x38\x82\x89\xe1",
            "USDP",
            18,
            "Pax Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\xc1\x7f\x95\x8d\x2e\xe5\x23\xa2\x20\x62\x06\x99\x45\x97\xc1\x3d\x83\x1e\xc7",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x1c\xf4\x59\x2e\xbf\xfd\x73\x0c\x7d\xc9\x2c\x1b\xdf\xfd\xfc\x3b\x9e\xfc\xf2\x9a",
            "WAVES",
            18,
            "WAVES",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x60\xfa\xc5\xe5\x54\x2a\x77\x3a\xa4\x4f\xbc\xfe\xdf\x7c\x19\x3b\xc2\xc5\x99",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x91\x93\x7a\x75\x08\x86\x0f\x87\x6c\x9c\x0a\x2a\x61\x7e\x7d\x9e\x94\x5d\x4b",
            "WOO",
            18,
            "Wootrade Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\xc5\x29\xc0\x0c\x64\x01\xae\xf6\xd2\x20\xbe\x8c\x6e\xa1\x66\x7f\x6a\xd9\x3e",
            "YFI",
            18,
            "yearn.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x1d\x24\x89\x57\x1d\x32\x21\x89\x24\x6d\xaf\xa5\xeb\xde\x1f\x46\x99\xf4\x98",
            "ZRX",
            18,
            "0x Protocol Token",
        )
    if chain_id == 10:  # OPT
        yield (  # address, symbol, decimals, name
            b"\x3e\x7e\xf8\xf5\x02\x46\xf7\x25\x88\x51\x02\xe8\x23\x8c\xbb\xa3\x3f\x27\x67\x47",
            "BOND",
            18,
            "BarnBridge Governance Token (Optimism)",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x10\x00\x9c\xbd\x5d\x07\xdd\x0c\xec\xc6\x61\x61\xfc\x93\xd7\xc9\x00\x0d\xa1",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x0a\x79\x1b\xfc\x2c\x21\xf9\xed\x5d\x10\x98\x0d\xad\x2e\x26\x38\xff\xa7\xf6",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x10\x28\xf5\xf1\xd5\xed\xe5\x97\x48\xff\xce\xe5\x53\x25\x09\x97\x68\x40\xe0",
            "PERP",
            18,
            "Perpetual",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\xb6\x88\xcc\xf6\x82\xd5\x8f\x86\xd7\xe3\x8e\x03\xf9\xd2\x2e\x77\x05\x44\x8b",
            "RAI",
            18,
            "Rai Reflex Index",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x00\xda\xec\x35\xaf\x8f\xf8\x8c\x16\xbd\xf0\x41\x87\x74\xcb\x3d\x75\x99\xb4",
            "SNX",
            18,
            "Synthetix Network Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x6f\xd9\xd7\xad\x17\x24\x2c\x41\xf7\x13\x1d\x25\x72\x12\xc5\x4a\x0e\x81\x66\x91",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\xb0\x08\xaa\x00\x57\x9c\x13\x07\xb0\xef\x2c\x49\x9a\xd9\x8a\x8c\xe5\x8e\x58",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x68\xf1\x80\xfc\xce\x68\x36\x68\x8e\x90\x84\xf0\x35\x30\x9e\x29\xbf\x0a\x20\x95",
            "WBTC",
            8,
            "Wrapped BTC",
        )
    if chain_id == 25:  # CRO
        yield (  # address, symbol, decimals, name
            b"\x98\x93\x6b\xde\x1c\xf1\xbf\xf1\xe7\xa8\x01\x2c\xee\x5e\x25\x83\x85\x1f\x20\x67",
            "ANN",
            18,
            "Annex",
        )
        yield (  # address, symbol, decimals, name
            b"\xad\xbd\x12\x31\xfb\x36\x00\x47\x52\x5b\xed\xf9\x62\x58\x1f\x3e\xee\x7b\x49\xfe",
            "CRONA",
            18,
            "CronaSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\x43\xcc\xab\x9e\x66\xe6\xcf\x12\x15\x37\x69\x80\x81\x1d\xdf\x1e\xb7\xf6\x89",
            "CRX",
            18,
            "Crodex Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf2\x00\x1b\x14\x5b\x43\x03\x2a\xaf\x5e\xe2\x88\x4e\x45\x6c\xcd\x80\x5f\x67\x7d",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x4b\xac\x3e\xc7\x7d\x6d\xb4\x97\x89\x24\x78\xf8\x54\xcf\x6e\x82\x45\xdc\xa9",
            "SVN",
            18,
            "Savanna Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\x12\x23\x24\x9c\xa2\x83\x97\xb4\xb6\x54\x1d\xff\xae\xcc\x53\x9b\xff\x0c\x59",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\xe4\x28\xc3\xf6\x7a\x68\x87\x85\x62\xe7\x9a\x02\x34\xc1\xf8\x3c\x20\x87\x70",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\x03\xbe\xce\x67\x47\xad\xc0\x0e\x1a\x13\x1b\xba\x14\x69\xc1\x5f\xd1\x1e\x03",
            "VVS",
            18,
            "VVSToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x06\x2e\x66\x47\x7f\xaf\x21\x9f\x25\xd2\x7d\xce\xd6\x47\xbf\x57\xc3\x10\x7d\x52",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\x7f\x8a\x57\x0d\x57\x8e\xd8\x4e\x63\xfd\xfa\x7b\x1e\xe7\x2d\xea\xe1\xae\x23",
            "WCRO",
            18,
            "Wrapped CRO",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x4f\xd7\xfc\xb2\xb1\x58\x18\x22\xd0\xc8\x62\xb6\x82\x22\x99\x8a\x0c\x29\x9a",
            "WETH",
            18,
            "Wrapped Ether",
        )
    if chain_id == 56:  # BSC
        yield (  # address, symbol, decimals, name
            b"\xfe\xea\x0b\xdd\x3d\x07\xeb\x6f\xe3\x05\x93\x88\x78\xc0\xca\xdb\xfa\x16\x90\x42",
            "8PAY",
            18,
            "8PAY Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xe2\x20\x0e\xfb\x34\x00\xfa\xbb\x9a\xac\xf3\x12\x97\xcb\xdd\x1d\x43\x5d\x47",
            "ADA",
            18,
            "Cardano Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x05\x28\xce\x5e\xf7\xb5\x11\x52\xa5\x97\x45\xbe\xfd\xd9\x1d\x97\x09\x1d\x2f",
            "ALPACA",
            18,
            "AlpacaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\xfa\xa1\x13\xcb\xe5\x34\x36\xdf\x28\xff\x0a\xee\x54\x27\x5c\x13\xb4\x09\x75",
            "ALPHA",
            18,
            "AlphaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\xb3\xa7\x05\xfc\x54\x72\x50\x37\xcc\x9e\x00\x8b\xde\xde\x69\x7f\x62\xf3\x35",
            "ATOM",
            18,
            "Cosmos Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x5d\x40\x0f\x88\xc1\x67\x88\x4b\xbc\xc4\x1c\x5f\xea\x40\x7e\xd4\xd2\xf8\xa0",
            "AXS",
            18,
            "Axie Infinity Shard",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x2d\xf9\xe3\xe6\x22\xde\xbd\xd6\x9f\xb8\x38\xbb\x79\x9e\x3f\x16\x89\x02\xc5",
            "BAKE",
            18,
            "BakeryToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\xf7\x95\xa6\xf4\xd9\x7e\x78\x87\xc7\x9b\xea\x79\xab\xa5\xcc\x76\x44\x4a\xdf",
            "BCH",
            18,
            "Bitcoin Cash Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\x59\xbf\x77\xcb\xe8\x69\x90\x13\xd6\xdb\xc7\xc2\xb9\x26\xaa\xf3\x07\xf8\x30",
            "BRY",
            18,
            "Berry Tributes",
        )
        yield (  # address, symbol, decimals, name
            b"\x71\x30\xd2\xa1\x2b\x9b\xcb\xfa\xe4\xf2\x63\x4d\x86\x4a\x1e\xe1\xce\x3e\xad\x9c",
            "BTCB",
            18,
            "BTCB Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x84\x9e\x6f\xdb\x74\x3d\x08\xfa\xee\x3e\x34\xdd\x2d\x1b\xc6\x9e\xa1\x1a\x51",
            "BUNNY",
            18,
            "Bunny Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe9\xe7\xce\xa3\xde\xdc\xa5\x98\x47\x80\xba\xfc\x59\x9b\xd6\x9a\xdd\x08\x7d\x56",
            "BUSD",
            18,
            "BUSD Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\x09\xfa\xbb\x73\xbd\x3a\xde\x0a\x17\xec\xc3\x21\xfd\x13\xa1\x9e\x81\xce\x82",
            "Cake",
            18,
            "PancakeSwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\x7e\xa5\xc0\xea\x75\xa8\xdf\x45\xd2\x88\xa4\xde\xbd\xd5\xbb\x63\x3f\x9e\x56",
            "CAN",
            18,
            "CanYaCoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x20\xde\x22\x02\x9a\xb6\x3c\xf9\xa7\xcf\x5f\xeb\x2b\x73\x7c\xa1\xee\x4c\x82\xa6",
            "CHESS",
            18,
            "Chess",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\xce\x07\x1b\xd9\xb1\xc4\xb0\x0a\x0b\x92\xd2\x98\xc5\x12\x47\x8c\xad\x67\xe8",
            "COMP",
            18,
            "Compound Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x1a\xf3\xf3\x29\xe8\xbe\x15\x40\x74\xd8\x76\x9d\x1f\xfa\x4e\xe0\x58\xb1\xdb\xc3",
            "DAI",
            18,
            "Dai Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\x3d\x91\xa0\x71\x31\x55\x00\x3f\xc4\xdc\xe0\xaf\xa8\x71\xb5\x08\xb3\xb7\x15",
            "DITTO",
            9,
            "Ditto",
        )
        yield (  # address, symbol, decimals, name
            b"\x67\xee\x3c\xb0\x86\xf8\xa1\x6f\x34\xbe\xe3\xca\x72\xfa\xd3\x6f\x7d\xb9\x29\xe2",
            "DODO",
            18,
            "DODO bird",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\xb6\xfb\x70\x8f\xc5\x73\x2d\xec\x1a\xfc\x8d\x85\x56\x42\x3a\x2e\xdc\xcb\xd6",
            "EOS",
            18,
            "EOS Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa7\xf5\x52\x07\x8d\xcc\x24\x7c\x26\x84\x33\x60\x20\xc0\x36\x48\x50\x0c\x6d\x9f",
            "EPS",
            18,
            "Ellipsis",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x70\xed\x08\x80\xac\x9a\x75\x5f\xd2\x9b\x26\x88\x95\x6b\xd9\x59\xf9\x33\xf8",
            "ETH",
            18,
            "Ethereum Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x4e\x64\x15\xa5\x72\x7e\xa0\x8a\xae\x45\x80\x05\x71\x87\x92\x3a\xec\x33\x12\x27",
            "FINE",
            18,
            "Refinable",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x8a\x10\x9c\x59\x00\xbc\x6d\x23\x57\xc8\x75\x49\xb6\x51\x67\x0e\x5b\x05\x39",
            "FOR",
            18,
            "The Force Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\x69\xfe\xc2\x33\x80\xf9\xf1\x4f\xfd\x07\xa1\xdc\x4b\x7c\x1a\x9f\xdd\x81\xc9",
            "Froyo",
            18,
            "Froyo",
        )
        yield (  # address, symbol, decimals, name
            b"\xf5\xd8\xa0\x96\xcc\xcb\x31\xb9\xd7\xbc\xe5\xaf\xe8\x12\xbe\x23\xe3\xd4\x69\x0d",
            "HAPPY",
            18,
            "HappyFans",
        )
        yield (  # address, symbol, decimals, name
            b"\x94\x8d\x2a\x81\x08\x6a\x07\x5b\x31\x30\xba\xc1\x9e\x4c\x6d\xee\x1d\x2e\x3f\xe8",
            "Helmet",
            18,
            "Helmet.insure Governance Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\xef\xf7\x74\x9b\x12\x54\x44\x95\x3e\xf8\x96\x82\x20\x1f\xb8\xc6\xa9\x17\xcd",
            "HZN",
            18,
            "Horizon Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xf8\xa0\xbf\x9c\xf5\x4b\xb9\x2f\x17\x37\x4d\x9e\x9a\x32\x1e\x6a\x11\x1a\x51\xbd",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x43\x38\x66\x5c\xbb\x7b\x24\x85\xa8\x85\x5a\x13\x9b\x75\xd5\xe3\x4a\xb0\xdb\x94",
            "LTC",
            18,
            "Litecoin Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x7b\x22\x2f\xc7\x9e\x1c\xbb\xf8\xca\x5f\x78\xcb\x13\x3d\x1b\x7c\xf3\x4b\xbd",
            "LTO",
            18,
            "LTO Network",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x98\x01\x66\x9c\x61\x38\xe8\x4b\xd5\x0d\xeb\x50\x08\x27\xb7\x76\x77\x7d\x28",
            "O3",
            18,
            "O3 Swap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x7b\x3a\x77\x84\x8f\x1c\x2d\x67\xe0\x5e\x54\xd7\x8d\x17\x4a\x0c\x85\x03\x35",
            "ONT",
            18,
            "Ontology Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x28\xa4\x57\xa5\xa7\x65\x96\xac\x48\xd8\x7c\x0f\x57\x70\x20\xf6\xea\x1c\x4c",
            "pBTC",
            18,
            "pTokens BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xaf\x53\xd5\x6f\xf9\x9f\x13\x22\x51\x5e\x54\xfd\xde\x93\xff\x8b\x3b\x7d\xaf\xd5",
            "PROM",
            18,
            "Prometeus",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x8c\x8a\xa8\x29\x9c\x10\xf0\x67\x49\x6b\xb6\x6f\x8c\xc7\xfb\x33\x8a\x34\x05",
            "PROS",
            18,
            "Prosper",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\x1f\xdb\x03\xba\x84\x76\x2d\xd6\x6a\x0a\xf1\xa6\xc8\x54\x0f\xf1\xba\x5d\xfb",
            "SFP",
            18,
            "SafePal Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\x7b\xc8\xd2\x3c\x63\x4c\x15\x40\x61\x86\x94\x78\xbc\xe9\x6b\xe6\x04\x5d\x12",
            "SFUND",
            18,
            "SeedifyFund",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x22\x22\x7e\x22\x10\x2f\xe3\x32\x20\x98\xe4\xcb\xfe\x18\xcf\xeb\xd5\x7c\x95",
            "TLM",
            4,
            "Alien Worlds Trilium",
        )
        yield (  # address, symbol, decimals, name
            b"\x14\x01\x6e\x85\xa2\x5a\xeb\x13\x06\x56\x88\xca\xfb\x43\x04\x4c\x2e\xf8\x67\x84",
            "TUSD",
            18,
            "TrueUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\x4b\x0f\x18\x12\xe5\xdf\x2a\x09\x79\x64\x81\xff\x14\x01\x7e\x60\x05\x50\x80\x03",
            "TWT",
            18,
            "Trust Wallet",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\x51\x40\xa2\x25\x78\x16\x8f\xd5\x62\xdc\xcf\x23\x5e\x5d\x43\xa0\x2c\xe9\xb1",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\xc7\x6a\x51\xcc\x95\x0d\x98\x22\xd6\x8b\x83\xfe\x1a\xd9\x7b\x32\xcd\x58\x0d",
            "USDC",
            18,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\xd3\x98\x32\x6f\x99\x05\x9f\xf7\x75\x48\x52\x46\x99\x90\x27\xb3\x19\x79\x55",
            "USDT",
            18,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x4b\xd1\x70\x03\x47\x33\x89\xa4\x2d\xaf\x6a\x0a\x72\x9f\x6f\xdb\x32\x8b\xbb\xd7",
            "VAI",
            18,
            "VAI Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\x30\x3d\x5b\xbf\xbb\xf4\x6f\x1a\x4d\x97\x41\xe4\x91\xe0\x69\x86\x89\x4e\x18",
            "WOOP",
            18,
            "Woonkly Power",
        )
        yield (  # address, symbol, decimals, name
            b"\x4d\xa9\x96\xc5\xfe\x84\x75\x5c\x80\xe1\x08\xcf\x96\xfe\x70\x51\x74\xc5\xe3\x6a",
            "WOW",
            18,
            "WOWswap",
        )
        yield (  # address, symbol, decimals, name
            b"\x78\x59\xb0\x1b\xbf\x67\x5d\x67\xda\x8c\xd1\x28\xa5\x0d\x15\x5c\xd8\x81\xb5\x76",
            "XMS",
            18,
            "Mars Ecosystem Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1d\x2f\x0d\xa1\x69\xce\xb9\xfc\x7b\x31\x44\x62\x8d\xb1\x56\xf3\xf6\xc6\x0d\xbe",
            "XRP",
            18,
            "XRP Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xcf\x6b\xb5\x38\x9c\x92\xbd\xda\x8a\x37\x47\xdd\xb4\x54\xcb\x7a\x64\x62\x6c\x63",
            "XVS",
            18,
            "Venus",
        )
    if chain_id == 66:  # okexchain
        yield (  # address, symbol, decimals, name
            b"\x77\xdf\x6e\xbe\xc3\x31\x67\x92\xd4\xea\x5b\xc0\xf8\x28\x6c\x27\x90\x5a\xa8\xe8",
            "AUCTIONK",
            18,
            "AUCTIONK",
        )
        yield (  # address, symbol, decimals, name
            b"\x18\xd1\x03\xb7\x06\x6a\xee\xdb\x60\x05\xb7\x8a\x46\x2e\xf9\x02\x73\x29\xb1\xea",
            "BCHK",
            18,
            "BCHK",
        )
        yield (  # address, symbol, decimals, name
            b"\x54\xe4\x62\x2d\xc5\x04\x17\x6b\x3b\xb4\x32\xdc\xca\xf5\x04\x56\x96\x99\xa7\xff",
            "BTCK",
            18,
            "BTCK",
        )
        yield (  # address, symbol, decimals, name
            b"\x33\x27\x30\xa4\xf6\xe0\x3d\x9c\x55\x82\x94\x35\xf1\x03\x60\xe1\x3c\xfa\x41\xff",
            "BUSD",
            18,
            "Binance-Peg BUSD Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x81\x79\xd9\x7e\xb6\x48\x88\x60\xd8\x16\xe3\xec\xaf\xe6\x94\xa4\x15\x3f\x21\x6c",
            "CHE",
            18,
            "CherrySwap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\xcd\xe7\xe3\x2a\x6c\xaf\x47\x42\xd0\x0d\x44\xb0\x72\x79\xe7\x59\x6d\x26\xb9",
            "DAIK",
            18,
            "DAIK",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\x97\x07\x78\xe2\x71\x5b\xbc\x9c\xf8\xfb\x83\xd1\x0d\xcb\xc2\xd2\xd5\x51\xa3",
            "ETCK",
            18,
            "ETCK",
        )
        yield (  # address, symbol, decimals, name
            b"\xef\x71\xca\x2e\xe6\x8f\x45\xb9\xad\x6f\x72\xfb\xdb\x33\xd7\x07\xb8\x72\x31\x5c",
            "ETHK",
            18,
            "ETHK",
        )
        yield (  # address, symbol, decimals, name
            b"\x3f\x89\x69\xbe\x2f\xc0\x77\x0d\xcc\x17\x49\x68\xe4\xb4\xff\x14\x6e\x0a\xcd\xaf",
            "FILK",
            18,
            "FILK",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\xc6\x82\x1a\xba\x4f\xcc\x65\xe8\xf1\x54\x25\x89\xe6\x4b\xae\x9d\xe1\x12\x28",
            "FLUXK",
            18,
            "Flux Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xc0\x57\x60\xd7\x5e\x7f\x5a\xd4\x28\xa9\x06\x67\x4c\xe7\xc7\xc8\x2d\x00\x3d\x01",
            "KINEK",
            18,
            "KINEK",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x52\x0e\xfc\x34\xc8\x1b\xfc\x1e\x3d\xd4\x8b\x7f\xe9\xff\x32\x60\x49\xf9\x86",
            "LTCK",
            18,
            "LTCK",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x54\xb6\xc6\x19\x5e\xa4\xd9\x48\xd0\x3b\xfd\x81\x8d\x36\x5c\xf1\x75\xcf\xc2",
            "OKB",
            18,
            "OKB",
        )
        yield (  # address, symbol, decimals, name
            b"\x32\x12\x60\x6f\x74\xcc\x59\x65\x6e\x1e\xc6\xf5\x87\xfc\xa6\x1b\xa3\xb8\x5e\xb0",
            "SFGK",
            18,
            "SFGK",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\x27\xba\xda\xa3\xc9\xec\x90\x81\xb8\xf6\xc9\xcd\xd2\xbf\x37\x5f\x14\x37\x80",
            "SHIBK",
            18,
            "SHIBK",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\x18\xe0\xd5\xe0\x17\x37\x69\xf5\xb4\x93\x9a\x3a\xe4\x23\xf7\xe5\xe4\xea\xb7",
            "SUSHIK",
            18,
            "SUSHIK",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\xd2\x26\xbb\x0a\x4d\x74\x27\x4d\x43\x54\xeb\xb6\xa0\xe1\xa1\xaa\x51\x75\xb6",
            "UNIK",
            18,
            "UNIK",
        )
        yield (  # address, symbol, decimals, name
            b"\xc9\x46\xda\xf8\x1b\x08\x14\x6b\x1c\x7a\x8d\xa2\xa8\x51\xdd\xf2\xb3\xea\xaf\x85",
            "USDC",
            18,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\xac\x52\xe0\x01\xf5\xbd\x41\x3a\xa6\xea\x83\x95\x64\x38\xf2\x90\x98\x16\x6b",
            "USDK",
            18,
            "USDK",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\x2b\xb3\x69\xd3\x43\x12\x5b\xfb\x21\x17\xaf\x9c\x14\x97\x95\xc6\xc6\x5c\x50",
            "USDT",
            18,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x85\x26\xdb\xfd\x6e\x38\xe3\xd8\x30\x77\x02\xca\x84\x69\xba\xe6\xc5\x6c\x15",
            "WOKT",
            18,
            "Wrapped OKT",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\x08\xd3\x21\xf6\xbc\x10\xa1\x0f\x09\x4e\x4b\x2e\x6c\x9b\x8b\xf9\x90\x64\x01",
            "ZKSK",
            18,
            "ZKSK",
        )
    if chain_id == 100:  # Gnosis
        yield (  # address, symbol, decimals, name
            b"\xd3\xd4\x7d\x55\x78\xe5\x5c\x88\x05\x05\xdc\x40\x64\x8f\x7f\x93\x07\xc3\xe7\xa8",
            "DPI",
            18,
            "DefiPulse Index on xDai",
        )
        yield (  # address, symbol, decimals, name
            b"\x9c\x58\xba\xcc\x33\x1c\x9a\xa8\x71\xaf\xd8\x02\xdb\x63\x79\xa9\x8e\x80\xce\xdb",
            "GNO",
            18,
            "Gnosis Token on xDai",
        )
        yield (  # address, symbol, decimals, name
            b"\xe2\xe7\x3a\x1c\x69\xec\xf8\x3f\x46\x4e\xfc\xe6\xa5\xbe\x35\x3a\x37\xca\x09\xb2",
            "LINK",
            18,
            "ChainLink Token on xDai",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x57\xaa\x7b\xee\xd6\x3d\x03\xaa\xf8\x5f\xfd\x17\x53\xf5\xf6\x24\x25\x88\xfb",
            "MPS",
            0,
            "MtPelerin Shares",
        )
        yield (  # address, symbol, decimals, name
            b"\xb7\xd3\x11\xe2\xeb\x55\xf2\xf6\x8a\x94\x40\xda\x38\xe7\x98\x92\x10\xb9\xa0\x5e",
            "STAKE",
            18,
            "STAKE on xDai",
        )
        yield (  # address, symbol, decimals, name
            b"\xdd\xaf\xbb\x50\x5a\xd2\x14\xd7\xb8\x0b\x1f\x83\x0f\xcc\xc8\x9b\x60\xfb\x7a\x83",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x8e\x5b\xbb\xb0\x9e\xd1\xeb\xde\x86\x74\xcd\xa3\x9a\x0c\x16\x94\x01\xdb\x42\x52",
            "WBTC",
            8,
            "Wrapped BTC on xDai",
        )
        yield (  # address, symbol, decimals, name
            b"\x6a\x02\x3c\xcd\x1f\xf6\xf2\x04\x5c\x33\x09\x76\x8e\xad\x9e\x68\xf9\x78\xf6\xe1",
            "WETH",
            18,
            "Wrapped Ether on xDai",
        )
        yield (  # address, symbol, decimals, name
            b"\xe9\x1d\x15\x3e\x0b\x41\x51\x8a\x2c\xe8\xdd\x3d\x79\x44\xfa\x86\x34\x63\xa9\x7d",
            "WXDAI",
            18,
            "Wrapped XDAI",
        )
    if chain_id == 128:  # Heco
        yield (  # address, symbol, decimals, name
            b"\x20\x2b\x49\x36\xfe\x1a\x82\xa4\x96\x52\x20\x86\x0a\xe4\x6d\x7d\x39\x39\xbb\x25",
            "AAVE",
            18,
            "Heco-Peg AAVE Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\x42\xfb\x0e\x60\x12\x5a\x40\x22\x67\x00\x14\xac\x12\x19\x31\xe7\x50\x1a\xf4",
            "BAG",
            18,
            "BAG",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x5d\xe1\x5c\xa7\x6e\x76\x42\x6e\x8f\xc7\xcb\xa8\x39\x2a\x31\x38\x07\x8d\x0f",
            "BAL",
            18,
            "Heco-Peg BAL Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\xf8\x08\x44\xa1\xb8\x4c\x61\xab\x80\xca\xfd\x88\xb1\xf8\xc0\x9f\x93\x42\xe1",
            "BEE",
            8,
            "BEE",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\xf4\xc4\x18\x51\x4d\xd4\x68\x0f\x76\xd5\xca\xa3\xbb\x42\xdb\x4a\x89\x3a\xcb",
            "BETH",
            18,
            "Heco-Peg BETH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x4f\x99\xd1\x0e\x16\x97\x2f\xf2\xfe\x31\x5e\xee\x53\xa9\x5f\xc5\xa5\x87\x0c\xe3",
            "BNB",
            18,
            "Poly-Peg BNB",
        )
        yield (  # address, symbol, decimals, name
            b"\x1e\x63\x95\xe6\xb0\x59\xfc\x97\xa4\xdd\xa9\x25\xb6\xc5\xeb\xf1\x9e\x05\xc6\x9f",
            "CAN",
            18,
            "Channels",
        )
        yield (  # address, symbol, decimals, name
            b"\x24\xab\x27\xa7\x27\x4d\xe0\xba\x57\x60\xba\xb8\x04\xfe\x87\x0b\xb5\x72\xc5\x10",
            "CETF",
            18,
            "CellETF",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\xf1\x32\x43\x43\xca\x7b\xf6\xe7\x43\xe2\x1d\xd9\xe9\x2d\xfa\x4e\xfc\x3a\x56",
            "CON",
            18,
            "CON",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x18\x98\x62\xb0\x69\xe2\xbe\x5f\x7c\x8e\x6f\xf0\x8e\xa8\xe1\xb1\x94\x85\x19",
            "COOK",
            18,
            "Poly-Peg COOK",
        )
        yield (  # address, symbol, decimals, name
            b"\x3d\x76\x0a\x45\xd0\x88\x7d\xfd\x89\xa2\xf5\x38\x5a\x23\x6b\x29\xcb\x46\xed\x2a",
            "DAI-HECO",
            18,
            "Heco-Peg DAIHECO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x48\xc8\x59\x53\x12\x54\xf2\x5e\x57\xd1\xc1\xa8\xe0\x30\xef\x0b\x1c\x89\x5c\x27",
            "DEP",
            18,
            "Depth Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x57\xa7\xbc\xdf\xab\x16\x31\xac\xa9\xd6\xe0\xf3\x99\x59\x47\x71\x82\xcf\xae\x12",
            "DMT",
            18,
            "DMT token",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\x96\x26\x78\x38\x42\xd3\x5c\x22\x1e\x5d\x01\x69\x4c\x2b\x92\x8e\xb3\xb0\xad",
            "DOG",
            18,
            "DOG",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\xec\xfc\x2b\xec\x06\xe4\xb4\x3d\xdd\x42\x3b\x94\xfe\xf8\x4d\x0d\xbc\x8f\x5c",
            "ELA",
            18,
            "ELA on HuobiChain",
        )
        yield (  # address, symbol, decimals, name
            b"\x64\xff\x63\x7f\xb4\x78\x86\x3b\x74\x68\xbc\x97\xd3\x0a\x5b\xf3\xa4\x28\xa1\xfd",
            "ETH",
            18,
            "Heco-Peg ETH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\x4b\x63\x30\x38\xf3\x6d\x03\xfe\xf5\xaf\x7f\x12\xe5\x19\x87\x95\x76\x77\x1a",
            "FCN",
            18,
            "FEICHANG NIU",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\x6f\xfd\x17\xb2\x66\x1e\xb5\x71\x44\xce\xae\xf9\x42\xd9\x52\x95\xe6\x37\xf0",
            "FILDA",
            18,
            "FilDA on Heco",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\xaf\xe3\xc9\x11\x8d\xb3\x6a\x20\xdd\x4a\x94\x2b\x6f\xf3\xe7\x89\x81\xdc\xe1",
            "GOF",
            18,
            "Heco-Peg GOF Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x89\x4b\x29\x17\xc7\x83\x51\x4c\x9e\x4c\x24\x22\x9b\xf6\x0f\x3c\xb4\xc9\xc9\x05",
            "HBC",
            18,
            "Heco-Peg HBC Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xef\x3c\xeb\xd7\x7e\x0c\x52\xcb\x6f\x60\x87\x5d\x93\x06\x39\x7b\x5c\xac\xa3\x75",
            "HBCH",
            18,
            "Heco-Peg HBCH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x64\xbd\x4f\xcc\x02\x7f\xaf\x72\xba\x83\xc0\xb2\x02\x8a\x3b\xae\x0d\x2d\x57",
            "HBO",
            18,
            "Hash Bridge Oracle",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\xcb\x6b\x53\x57\xcc\xce\x1b\x99\xcd\x22\x23\x29\x42\xd9\xa2\x25\xea\x4e\xb1",
            "HBSV",
            18,
            "Heco-Peg HBSV Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x66\xa7\x9d\x23\xe5\x84\x75\xd2\x73\x81\x79\xca\x52\xcd\x0b\x41\xd7\x3f\x0b\xea",
            "HBTC",
            18,
            "Heco-Peg HBTC Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\xc4\x9c\xee\x16\xa5\xe5\xbd\xef\xde\x93\x11\x07\xdc\x1f\xae\x9f\x77\x73\xe3",
            "HDOT",
            18,
            "Heco-Peg HDOT Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x3a\x76\x8f\x9a\xb1\x04\xc6\x9a\x7c\xd6\x04\x1f\xe1\x6f\xfa\x23\x5d\x18\x10",
            "HFIL",
            18,
            "Heco-Peg HFIL Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\x08\x52\xdf\x03\xea\x8b\x8a\xf0\xcc\x0b\x09\xca\xc3\xf7\xdb\xb1\x5e\x04\x33",
            "hFLUX",
            18,
            "Flux Protocol",
        )
        yield (  # address, symbol, decimals, name
            b"\xec\xb5\x6c\xf7\x72\xb5\xc9\xa6\x90\x7f\xb7\xd3\x23\x87\xda\x2f\xcb\xfb\x63\xb4",
            "HLTC",
            18,
            "Heco-Peg HLTC Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\x99\xef\x46\x16\x99\x37\x30\xce\xd0\xf3\x1f\xa2\x70\x3b\x92\xb5\x0b\xb5\x36",
            "HPT",
            18,
            "Heco-Peg HPT Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\xc6\x6d\x46\x0e\x2b\xb9\xd3\x1a\x8d\xe5\x4b\x40\x16\xfc\xa9\x86\xd0\x81\x1f",
            "HTM",
            18,
            "\u706b\u5e01\u751f\u6001\u9690\u79c1\u62d3\u5c55\u94fe",
        )
        yield (  # address, symbol, decimals, name
            b"\x02\x98\xc2\xb3\x2e\xae\x4d\xa0\x02\xa1\x5f\x36\xfd\xf7\x61\x5b\xea\x3d\xa0\x47",
            "HUSD",
            8,
            "Heco-Peg HUSD Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x45\xe9\x7d\xad\x82\x8a\xd7\x35\xaf\x1d\xf0\x47\x3f\xc2\x73\x5f\x0f\xd5\x33\x0c",
            "HXTZ",
            18,
            "Heco-Peg HXTZ Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x31\xf0\x48\xd8\x5f\x03\x91\xa2\x44\x35\xee\xfb\x77\x63\x19\x9b\x58\x7d\x0e",
            "LAMB",
            18,
            "Heco-Peg LAMB Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x67\x85\x44\x97\x21\x80\x43\xe1\xf7\x29\x08\xff\xe3\x8d\x0e\xd7\xf2\x47\x21",
            "LHB",
            18,
            "LendHub",
        )
        yield (  # address, symbol, decimals, name
            b"\x9e\x00\x45\x45\xc5\x9d\x35\x9f\x6b\x7b\xfb\x06\xa2\x63\x90\xb0\x87\x71\x7b\x42",
            "LINK",
            18,
            "Heco-Peg LINK Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\x70\x51\x2b\x52\x48\xe7\xc1\xf0\xf6\x99\x6e\x2f\xde\x2e\x95\x27\x08\xc4\xc9",
            "NT",
            18,
            "NEXTYPE",
        )
        yield (  # address, symbol, decimals, name
            b"\xaa\xae\x74\x6b\x5e\x55\xd1\x43\x98\x87\x93\x12\x66\x0e\x9f\xde\x07\xfb\xc1\xdc",
            "PIPI",
            18,
            "Pippi Shrimp Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x52\xee\x54\xdd\x7a\x68\xe9\xcf\x13\x1b\x0a\x57\xfd\x60\x15\xc7\x4d\x71\x40\xe2",
            "PTD",
            18,
            "P.TD",
        )
        yield (  # address, symbol, decimals, name
            b"\xb6\xcc\xfa\x7e\xf3\xa2\x95\x93\x25\x36\xe0\x98\x8c\xff\xd8\x52\x28\xcb\x17\x7c",
            "sCASH",
            18,
            "sCASH",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x78\x50\x28\x17\x19\xd5\xa9\x6c\x29\x81\x2a\xb7\x2f\x82\x2e\x0e\x09\xf3\xda",
            "SNX",
            18,
            "Heco-Peg SNX Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\xe1\x65\x63\xa2\xba\x84\xe5\x60\x78\x09\x46\xf0\xfb\x73\xa8\xb3\x2c\x84\x1e",
            "SOVI",
            18,
            "Sovi Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x32\x9d\xda\x64\xcb\xc4\xdf\xd5\xfa\x50\x72\xb4\x47\xb3\x94\x1c\xe0\x54\xeb\xb3",
            "SWFTC",
            8,
            "Heco-Peg SWFTC Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x88\xbe\xdc\xed\xac\xa5\x6a\x3d\x9a\x56\xb6\x56\x80\x44\x60\xca\x16\xad\x86",
            "TLOD",
            18,
            "The Legend of Deification",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\xc5\x4c\xe8\x32\x1a\x40\x15\x74\x0e\xe1\x10\x9d\x9c\xbc\x25\x81\x5c\x46\xe6",
            "UNI",
            18,
            "Heco-Peg UNI Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x93\x62\xbb\xef\x4b\x83\x13\xa8\xaa\x9f\x0c\x98\x08\xb8\x05\x77\xaa\x26\xb7\x3b",
            "USDC-HECO",
            6,
            "Heco-Peg USDCHECO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa7\x1e\xdc\x38\xd1\x89\x76\x75\x82\xc3\x8a\x31\x45\xb5\x87\x30\x52\xc3\xe4\x7a",
            "USDT",
            18,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x55\x45\x15\x3c\xcf\xca\x01\xfb\xd7\xdd\x11\xc0\xb2\x3b\xa6\x94\xd9\x50\x9a\x6f",
            "WHT",
            18,
            "Wrapped HT",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\xfe\x25\xee\xfc\xfc\xad\xde\xf8\x44\xfe\x30\xb8\xbe\x1d\x68\xac\x6b\x7a\xf3",
            "XF",
            18,
            "xFarmer",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\x94\x4b\x50\xdf\x84\x00\x1a\x36\xc7\xde\x0d\x5c\xb4\xda\x7a\xb2\x14\x07\xd2",
            "XNFT",
            18,
            "XNFT",
        )
        yield (  # address, symbol, decimals, name
            b"\xb4\xf0\x19\xbe\xac\x75\x8a\xbb\xee\x2f\x90\x60\x33\xaa\xa2\xf0\xf6\xda\xcb\x35",
            "YFI",
            18,
            "Heco-Peg YFI Token",
        )
    if chain_id == 137:  # Polygon
        yield (  # address, symbol, decimals, name
            b"\x9c\x2c\x5f\xd7\xb0\x7e\x95\xee\x04\x4d\xde\xba\x0e\x97\xa6\x65\xf1\x42\x39\x4f",
            "1INCH",
            18,
            "1Inch (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xd6\xdf\x93\x2a\x45\xc0\xf2\x55\xf8\x51\x45\xf2\x86\xea\x0b\x29\x2b\x21\xc9\x0b",
            "AAVE",
            18,
            "Aave (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\xce\x1e\x99\xa3\x1c\x45\x97\xa3\xf8\xa8\x94\x5c\xbf\xa9\x03\x76\x55\xe3\x35",
            "ASTRAFER",
            18,
            "Astrafer",
        )
        yield (  # address, symbol, decimals, name
            b"\x9a\x71\x01\x2b\x13\xca\x4d\x3d\x0c\xdc\x72\xa1\x77\xdf\x3e\xf0\x3b\x0e\x76\xa3",
            "BAL",
            18,
            "Balancer (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x3c\xef\x98\xbb\x43\xd7\x32\xe2\xf2\x85\xee\x60\x5a\x81\x58\xcd\xe9\x67\xd2\x19",
            "BAT",
            18,
            "Basic Attention Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x92\x46\xa5\xf1\x0a\x79\xa5\xa9\x39\xb0\xc2\xa7\x5a\x3a\xd1\x96\xaa\xfd\xb4\x3b",
            "BETS",
            18,
            "BetSwirl Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\xb5\x29\xf4\x0e\x67\x1a\x1d\x4b\xf9\x13\x61\xc2\x1b\xf9\xf0\xc9\x71\x2a\xb7",
            "BUSD",
            18,
            "Binance-Peg BUSD Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x85\x05\xb9\xd2\x25\x4a\x7a\xe4\x68\xc0\xe9\xdd\x10\xcc\xea\x3a\x83\x7a\xef\x5c",
            "COMP",
            18,
            "(PoS) Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\x23\x70\xd5\xcd\x63\x27\x9e\xfa\x6d\x50\x2d\xab\x29\x17\x19\x33\xa6\x10\xaf",
            "CRV",
            18,
            "CRV (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x3c\xf7\xad\x23\xcd\x3c\xad\xbd\x97\x35\xaf\xf9\x58\x02\x32\x39\xc6\xa0\x63",
            "DAI",
            18,
            "(PoS) Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xe3\x90\xe9\xce\xa0\xd0\xe8\xcd\x40\x04\x8c\xed\x9f\xa9\xea\x10\xd7\x16\x39",
            "DSLA",
            18,
            "DSLA (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\xde\x72\xb1\x7c\x7b\xd1\x2e\x9e\x6d\x69\xeb\x50\x6a\x66\x9e\xb6\xb5\x24\x9e",
            "EGG",
            18,
            "Waves Ducks",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\xc1\xb2\x99\xa7\x45\x88\xd6\xab\xdc\x1b\x85\xa5\x33\x45\xa4\x84\x28\xa5\x21",
            "EZ",
            18,
            "EASY V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x22\xe3\xf0\x2f\x86\xbc\x8e\xa0\xd7\x37\x18\xa2\xae\x88\x51\x85\x4e\x62\xad\xc5",
            "FLAME",
            18,
            "FireStarter",
        )
        yield (  # address, symbol, decimals, name
            b"\x3b\x56\xa7\x04\xc0\x1d\x65\x01\x47\xad\xe2\xb8\xce\xe5\x94\x06\x6b\x3f\x94\x21",
            "FYN",
            18,
            "Affyn",
        )
        yield (  # address, symbol, decimals, name
            b"\x72\x3b\x17\x71\x82\x89\xa9\x1a\xf2\x52\xd6\x16\xde\x2c\x77\x94\x49\x62\xd1\x22",
            "GAIA",
            18,
            "GAIA Everworld",
        )
        yield (  # address, symbol, decimals, name
            b"\x38\x01\xc3\xb3\xb5\xc9\x8f\x88\xa9\xc9\x00\x59\x66\xaa\x96\xaa\x44\x0b\x9a\xfc",
            "GLTR",
            18,
            "GAX Liquidity Token Reward",
        )
        yield (  # address, symbol, decimals, name
            b"\xdd\x2a\xf2\xe7\x23\x54\x70\x88\xd3\x84\x68\x41\xfb\xdc\xc6\xa8\x09\x33\x13\xd6",
            "GOGO",
            18,
            "GOGOcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5f\xe2\xb5\x8c\x01\x3d\x76\x01\x14\x7d\xcd\xd6\x8c\x14\x3a\x77\x49\x9f\x55\x31",
            "GRT",
            18,
            "Graph Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x0e\x8b\x4b\x09\x03\x92\x9f\x47\xc3\xea\x30\x97\x39\x40\xd4\xa9\x70\x20\x67",
            "INSUR",
            18,
            "InsurAce (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xe0\x6b\xd4\xf5\xaa\xc8\xd0\xaa\x33\x7d\x13\xec\x88\xdb\x6d\xef\xc6\xea\xee\xfe",
            "IXT",
            18,
            "PlanetIX",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x74\x8d\x54\x8d\x95\xd7\x8a\x3c\x83\xfe\x3f\x32\x60\x4b\x47\x96\xcf\xfa\x23",
            "KOGECOIN",
            9,
            "kogecoin.io",
        )
        yield (  # address, symbol, decimals, name
            b"\x53\xe0\xbc\xa3\x5e\xc3\x56\xbd\x5d\xdd\xfe\xbb\xd1\xfc\x0f\xd0\x3f\xab\xad\x39",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa1\xc5\x7f\x48\xf0\xde\xb8\x9f\x56\x9d\xfb\xe6\xe2\xb7\xf4\x6d\x33\x60\x6f\xd4",
            "MANA",
            18,
            "(PoS) Decentraland MANA",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\x9e\x7c\xcd\xf0\xf4\xe5\xb2\x47\x57\xc1\xe1\xa8\x0e\x31\x1e\x34\xcb\x10\xc7",
            "MASK",
            18,
            "Mask Network (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xa2\x56\x10\xa7\x70\x77\x39\x0a\x75\xad\x90\x72\xa0\x84\xc5\xfb\xc7\xd4\x3a\x0d",
            "MCASH",
            18,
            "Monsoon Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\xc9\x49\xb4\xeb\x85\xa9\x00\x71\xf2\xc0\xbe\xf8\x61\xbd\xde\xe1\xa7\x47\x9d",
            "mSHEESHA",
            18,
            "SHEESHA POLYGON",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x00\x6d\x1e\x1d\x9d\xc6\xc9\x89\x96\xf5\x0a\x4c\x81\x0f\x17\xa4\x7f\xbf\x19",
            "NSFW",
            18,
            "Pleasure Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xee\x98\x01\x66\x9c\x61\x38\xe8\x4b\xd5\x0d\xeb\x50\x08\x27\xb7\x76\x77\x7d\x28",
            "O3",
            18,
            "O3 Swap Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x8a\x95\x3c\xfe\x44\x2c\x5e\x88\x55\xcc\x6c\x61\xb1\x29\x3f\xa6\x48\xba\xe4\x72",
            "PolyDoge",
            18,
            "PolyDoge",
        )
        yield (  # address, symbol, decimals, name
            b"\x58\x0a\x84\xc7\x38\x11\xe1\x83\x9f\x75\xd8\x6d\x75\xd8\x8c\xca\x0c\x24\x1f\xf4",
            "QI",
            18,
            "Qi Dao",
        )
        yield (  # address, symbol, decimals, name
            b"\x83\x17\x53\xdd\x70\x87\xca\xc6\x1a\xb5\x64\x4b\x30\x86\x42\xcc\x1c\x33\xdc\x13",
            "QUICK",
            18,
            "Quickswap",
        )
        yield (  # address, symbol, decimals, name
            b"\xb5\xc0\x64\xf9\x55\xd8\xe7\xf3\x8f\xe0\x46\x0c\x55\x6a\x72\x98\x74\x94\xee\x17",
            "QUICK",
            18,
            "QuickSwap",
        )
        yield (  # address, symbol, decimals, name
            b"\xcd\x73\x61\xac\x33\x07\xd1\xc5\xa4\x6b\x63\x08\x6a\x90\x74\x2f\xf4\x4c\x63\xb3",
            "RAIDER",
            18,
            "RaiderToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\xce\xbf\xcf\x60\x4a\xd2\x45\xea\xf0\xd5\xbd\x68\x45\x9c\x3a\x7a\x63\x99\xc2",
            "RAMP",
            18,
            "RAMP (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xdb\xf3\x1d\xf1\x4b\x66\x53\x5a\xf6\x5a\xac\x99\xc3\x2e\x9e\xa8\x44\xe1\x45\x01",
            "renBTC",
            8,
            "renBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xce\x82\x9a\x89\xd4\xa5\x5a\x63\x41\x8b\xcc\x43\xf0\x01\x45\xad\xef\x0e\xdb\x8e",
            "renDOGE",
            8,
            "renDOGE",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xb7\x28\xd8\xd9\x64\xfd\x00\xc2\xd0\xaa\xd8\x17\x18\xb7\x13\x11\xfe\xf6\x8a",
            "SNX",
            18,
            "Synthetix Network Token (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x0b\x3f\x86\x8e\x0b\xe5\x59\x7d\x5d\xb7\xfe\xb5\x9e\x1c\xad\xbb\x0f\xdd\xa5\x0a",
            "SUSHI",
            18,
            "SushiToken (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xdf\x78\x37\xde\x1f\x2f\xa4\x63\x1d\x71\x6c\xf2\x50\x2f\x8b\x23\x0f\x1d\xcc\x32",
            "TEL",
            2,
            "Telcoin (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x2e\x1a\xd1\x08\xff\x1d\x8c\x78\x2f\xcb\xbb\x89\xaa\xd7\x83\xac\x49\x58\x67\x56",
            "TUSD",
            18,
            "TrueUSD (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x3e\xaa\xd8\xd9\x22\xb1\x08\x34\x46\xdc\x23\xf6\x10\xc2\x56\x7f\xb5\x18\x0f",
            "UNI",
            18,
            "Uniswap (PoS)",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\x91\xbc\xa1\xf2\xde\x46\x61\xed\x88\xa3\x0c\x99\xa7\xa9\x44\x9a\xa8\x41\x74",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xc2\x13\x2d\x05\xd3\x1c\x91\x4a\x87\xc6\x61\x1c\x10\x74\x8a\xeb\x04\xb5\x8e\x8f",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\xd0\x25\x8a\x3f\xd0\x0f\x38\xaa\x80\x90\xdf\xee\x34\x3f\x10\xa9\xd4\xd3\x0d\x3f",
            "VOXEL",
            18,
            "VOXEL Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x1b\xfd\x67\x03\x7b\x42\xcf\x73\xac\xf2\x04\x70\x67\xbd\x4f\x2c\x47\xd9\xbf\xd6",
            "WBTC",
            8,
            "(PoS) Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x7c\xeb\x23\xfd\x6b\xc0\xad\xd5\x9e\x62\xac\x25\x57\x82\x70\xcf\xf1\xb9\xf6\x19",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\x0d\x50\x0b\x1d\x8e\x8e\xf3\x1e\x21\xc9\x9d\x1d\xb9\xa6\x44\x4d\x3a\xdf\x12\x70",
            "WMATIC",
            18,
            "Wrapped Matic",
        )
        yield (  # address, symbol, decimals, name
            b"\x8f\x18\xdc\x39\x95\x94\xb4\x51\xed\xa8\xc5\xda\x02\xd0\x56\x3c\x0b\x2d\x0f\x16",
            "WOLF",
            9,
            "moonwolf.io",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x53\x71\x04\xd6\xa5\xed\xd5\x3c\x6f\xbb\xa9\xa8\x98\x70\x8e\x46\x52\x60\xb6",
            "YFI",
            18,
            "(PoS) yearn.finance",
        )
    if chain_id == 177:  # HashKey Chain
        yield (  # address, symbol, decimals, name
            b"\xf1\xb5\x0e\xd6\x7a\x9e\x2c\xc9\x4a\xd3\xc4\x77\x77\x9e\x2d\x4c\xbf\xff\x90\x29",
            "USDT",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x19\xca\x49\xa7\x9f\x58\x25\xc8\xb3\x45\xf8\xd7\xac\x36\xb2\x72\x56\x5b\x14",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xef\xd4\xbc\x9a\xfd\x21\x05\x17\x80\x3f\x29\x3a\xba\xbd\x70\x1c\xae\xec\xdf\xd0",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\x10\xd2\x12\x0d\x57\xb7\x58\xee\x16\x3c\xff\xb4\x3e\x73\x72\x8c\x47\x1c\xf1",
            "WHSK",
            18,
            "Wrapped HSK",
        )
    if chain_id == 250:  # FTM
        yield (  # address, symbol, decimals, name
            b"\x6a\x07\xa7\x92\xab\x29\x65\xc7\x2a\x5b\x80\x88\xd3\xa0\x69\xa7\xac\x3a\x99\x3b",
            "AAVE",
            18,
            "Aave",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\xe7\x62\x8e\x8b\x43\x50\xb2\x71\x6a\xb4\x70\xee\x0b\xa1\xfa\x9e\x76\xc6\xc5",
            "BAND",
            18,
            "Band",
        )
        yield (  # address, symbol, decimals, name
            b"\x32\x11\x62\xcd\x93\x3e\x2b\xe4\x98\xcd\x22\x67\xa9\x05\x34\xa8\x04\x05\x1b\x11",
            "BTC",
            8,
            "Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xb0\x1e\x84\x19\xd8\x42\xbe\xeb\xf1\xb7\x0a\x7b\x5f\x71\x42\xab\xba\xf7\x15\x9d",
            "COVER",
            18,
            "Cover Protocol Governance",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x7a\x18\x61\xc1\x5a\x3d\xed\x9a\xf0\xb6\x79\x9a\x19\x5a\x24\x9e\xbd\xcb\xc6",
            "CREAM",
            18,
            "Cream",
        )
        yield (  # address, symbol, decimals, name
            b"\x1e\x4f\x97\xb9\xf9\xf9\x13\xc4\x6f\x16\x32\x78\x17\x32\x92\x7b\x90\x19\xc6\x8b",
            "CRV",
            18,
            "Curve DAO",
        )
        yield (  # address, symbol, decimals, name
            b"\x8d\x11\xec\x38\xa3\xeb\x5e\x95\x6b\x05\x2f\x67\xda\x8b\xdc\x9b\xef\x8a\xbf\x3e",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\xb2\x38\x82\xa3\x02\x90\x45\x1a\x17\xc4\x4f\x4f\x05\x24\x3b\x6b\x58\xc7\x6d",
            "ETH",
            18,
            "Ethereum",
        )
        yield (  # address, symbol, decimals, name
            b"\x07\x8e\xef\x5a\x2f\xb5\x33\xe1\xa4\xd4\x87\xef\x64\xb2\x7d\xf1\x13\xd1\x2c\x32",
            "FBAND",
            18,
            "fBAND",
        )
        yield (  # address, symbol, decimals, name
            b"\x27\xf2\x6f\x00\xe1\x60\x59\x03\x64\x5b\xba\xbc\x0a\x73\xe3\x50\x27\xdc\xcd\x45",
            "FBNB",
            18,
            "fBNB",
        )
        yield (  # address, symbol, decimals, name
            b"\xe1\x14\x6b\x9a\xc4\x56\xfc\xbb\x60\x64\x4c\x36\xfd\x3f\x86\x8a\x90\x72\xfc\x6e",
            "FBTC",
            18,
            "fBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x8b\x0c\x76\x13\xe8\x90\xee\x50\xb8\xc4\xbc\x6a\x3f\x41\xef\x41\x12\x08\xad",
            "FETH",
            18,
            "fETH",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x49\xa1\x94\x23\x05\x2d\xc6\xb3\x20\x36\x0b\x3c\x76\x08\x84\xe0\x95\xac\x57",
            "FLINK",
            18,
            "fLINK",
        )
        yield (  # address, symbol, decimals, name
            b"\xdc\x30\x16\x22\xe6\x21\x16\x6b\xd8\xe8\x2f\x2c\xa0\xa2\x6c\x13\xad\x0b\xe3\x55",
            "FRAX",
            18,
            "Frax",
        )
        yield (  # address, symbol, decimals, name
            b"\x7d\x01\x6e\xec\x9c\x25\x23\x2b\x01\xf2\x3e\xf9\x92\xd9\x8c\xa9\x7f\xc2\xaf\x5a",
            "FXS",
            18,
            "Frax Share",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\xb2\x6e\x83\x9e\xb3\x57\x2c\x5e\x95\x9f\x99\x48\x04\xa5\xde\x66\x60\x03\x49",
            "HEGIC",
            18,
            "Hegic",
        )
        yield (  # address, symbol, decimals, name
            b"\x10\x01\x00\x78\xa5\x43\x96\xf6\x2c\x96\xdf\x85\x32\xdc\x2b\x48\x47\xd4\x7e\xd3",
            "HND",
            18,
            "Hundred Finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xf1\x6e\x81\xdc\xe1\x5b\x08\xf3\x26\x22\x07\x42\x02\x03\x79\xb8\x55\xb8\x7d\xf9",
            "ICE",
            18,
            "IceToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\x50\x62\xd2\x2a\xdc\xfa\xaf\xbd\x5c\x54\x1d\x4d\xa8\x2e\x4b\x45\x0d\x42\x12",
            "KP3R",
            18,
            "Keep3r",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x65\x4d\xc3\xd1\x0e\xa7\x64\x5f\x83\x19\x66\x8e\x8f\x54\xd2\x57\x4f\xbd\xc8",
            "LINK",
            18,
            "ChainLink",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xf0\xb8\xb4\x56\xc1\xa4\x51\x37\x84\x67\x39\x89\x82\xd4\x83\x4b\x68\x29\xc1",
            "MIM",
            18,
            "Magic Internet Money",
        )
        yield (  # address, symbol, decimals, name
            b"\x56\xee\x92\x6b\xd8\xc7\x2b\x2d\x5f\xa1\xaf\x4d\x9e\x4c\xbb\x51\x5a\x1e\x3a\xdc",
            "SNX",
            18,
            "Synthetix Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\x80\x03\xb6\x88\x94\x39\x77\xe6\x13\x0f\x4f\x68\xf2\x3a\xad\x93\x9a\x10\x40",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x0e\x16\x94\x48\x3e\xbb\x3b\x74\xd3\x05\x4e\x38\x38\x40\xc6\xcf\x01\x1e\x51\x8e",
            "sUSD",
            18,
            "Synth sUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xae\x75\xa4\x38\xb2\xe0\xcb\x8b\xb0\x1e\xc1\xe1\xe3\x76\xde\x11\xd4\x44\x77\xcc",
            "SUSHI",
            18,
            "Sushi",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x06\x8d\xa6\xc8\x3a\xfc\xfa\x0e\x13\xba\x15\xa6\x69\x66\x62\x33\x5d\x5b\x75",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\xbe\x37\x0d\x53\x12\xf4\x4c\xb4\x2c\xe3\x77\xbc\x9b\x8a\x0c\xef\x1a\x4c\x83",
            "WFTM",
            18,
            "Wrapped Fantom",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\xb0\xda\x86\xe4\x84\xe1\xc0\x02\x9b\x56\xe8\x17\x91\x2d\x77\x8a\xc0\xec\x69",
            "YFI",
            18,
            "yearn.finance",
        )
    if chain_id == 288:  # Boba
        yield (  # address, symbol, decimals, name
            b"\x12\x16\x36\xc4\x3e\x96\xd9\x7a\xb0\x0b\x6c\x69\x94\xcd\xde\xbe\xf2\x7d\xe1\xc7",
            "BDoge",
            18,
            "BobaDoge",
        )
        yield (  # address, symbol, decimals, name
            b"\x3a\x93\xbd\x0f\xa1\x00\x50\xd2\x06\x37\x0e\xea\x53\x27\x65\x42\xa1\x05\xc8\x85",
            "BRE",
            18,
            "Brewery",
        )
        yield (  # address, symbol, decimals, name
            b"\x61\x8c\xc6\x54\x9d\xdf\x12\xde\x63\x7d\x46\xcd\xda\xda\xfc\x0c\x29\x51\x13\x1c",
            "KYO",
            18,
            "K\u014dy\u014d Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf0\x8a\xd7\xc3\xf6\xb1\xc6\x84\x3b\xa0\x27\xad\x54\xed\x8d\xdb\x6d\x71\x16\x9b",
            "SB",
            18,
            "Shibui",
        )
    if chain_id == 9798:  # Data Trade Chain
        yield (  # address, symbol, decimals, name
            b"\x8e\x79\x85\x0c\x50\xe5\x25\xeb\x6b\xa6\x3e\x60\x1e\x7b\x41\x88\x8a\x1c\x91\x02",
            "BV",
            2,
            "BV",
        )
        yield (  # address, symbol, decimals, name
            b"\x89\x9f\x0b\x9d\x67\xdd\x1b\x83\x3f\xda\xa9\x0c\x8b\x09\xea\x61\x6d\x0e\x9e\x98",
            "CNV",
            2,
            "CNV",
        )
        yield (  # address, symbol, decimals, name
            b"\xe8\x95\xc5\x77\xd7\x47\xbb\x5d\xbb\xc1\xf0\x6c\xb4\x4d\x60\x67\x68\x0b\xe4\xbe",
            "dBTC",
            8,
            "dBTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\x71\x60\xc1\xe9\xfd\xb6\x89\xa0\x60\xff\x09\x19\xe8\x49\x15\xb0\xdf\xa0\x4a",
            "dETH",
            18,
            "dETH",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\x5c\x11\xfb\x47\x83\xbd\x00\xa8\x8a\x0b\x99\x42\x02\x62\xf4\x09\xfa\x8b\xb8",
            "DOS",
            2,
            "DOS",
        )
        yield (  # address, symbol, decimals, name
            b"\x36\xe6\x50\x4c\x96\x8f\x5c\x2a\x31\x0b\x6a\xf7\xb9\x7b\xc2\x2c\xdd\x34\x02\xcc",
            "dUSDT",
            6,
            "dUSDT",
        )
        yield (  # address, symbol, decimals, name
            b"\xb8\x8a\xd7\x67\xb4\x16\x19\x7e\x62\x93\x9d\xec\x20\x74\x31\xb5\x61\xa9\x38\x3b",
            "FEC",
            4,
            "FEC",
        )
        yield (  # address, symbol, decimals, name
            b"\xe5\x2a\x73\x68\x28\xc7\x82\xc2\xa4\xa3\x45\xbb\xe8\x05\x2a\xed\x01\x0f\xc8\x2d",
            "HLT",
            2,
            "HLT",
        )
        yield (  # address, symbol, decimals, name
            b"\x6d\x88\x5b\x0b\x37\xc6\x2b\xe0\xc7\x2e\xcd\x6a\x61\xaf\x2b\xff\xf6\x81\x41\x9e",
            "STC08375",
            0,
            "STC08375",
        )
    if chain_id == 42161:  # Arbitrum
        yield (  # address, symbol, decimals, name
            b"\x15\x5f\x0d\xd0\x44\x24\x93\x93\x68\x97\x2f\x4e\x18\x38\x68\x7d\x6a\x83\x11\x51",
            "ADoge",
            18,
            "ArbiDoge",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x20\xde\x1f\xc9\xb1\x61\xb3\x40\x89\xcb\xea\xe8\x88\x16\x8b\x44\xb0\x34\x61",
            "ARBIS",
            18,
            "ARBIS | We have the yields",
        )
        yield (  # address, symbol, decimals, name
            b"\x86\xa1\x01\x2d\x43\x7b\xbf\xf8\x4f\xbd\xf6\x25\x69\xd1\x2d\x4f\xd3\x39\x6f\x8c",
            "ARBYS",
            18,
            "Arbys",
        )
        yield (  # address, symbol, decimals, name
            b"\xbf\xa6\x41\x05\x1b\xa0\xa0\xad\x1b\x0a\xcf\x54\x9a\x89\x53\x6a\x0d\x76\x47\x2e",
            "BADGER",
            18,
            "Badger",
        )
        yield (  # address, symbol, decimals, name
            b"\x04\x0d\x1e\xdc\x95\x69\xd4\xba\xb2\xd1\x52\x87\xdc\x5a\x4f\x10\xf5\x6a\x56\xb8",
            "BAL",
            18,
            "Balancer",
        )
        yield (  # address, symbol, decimals, name
            b"\x3a\x8b\x78\x7f\x78\xd7\x75\xae\xcf\xee\xa1\x57\x06\xd4\x22\x1b\x40\xf3\x45\xab",
            "CELR",
            18,
            "CelerToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x35\x4a\x6d\xa3\xfc\xde\x09\x8f\x83\x89\xca\xd8\x4b\x01\x82\x72\x5c\x6c\x91\xde",
            "COMP",
            18,
            "Compound",
        )
        yield (  # address, symbol, decimals, name
            b"\xf4\xd4\x8c\xe3\xee\x1a\xc3\x65\x19\x98\x97\x15\x41\xba\xdb\xb9\xa1\x4d\x72\x34",
            "CREAM",
            18,
            "Cream",
        )
        yield (  # address, symbol, decimals, name
            b"\x11\xcd\xb4\x2b\x0e\xb4\x6d\x95\xf9\x90\xbe\xdd\x46\x95\xa6\xe3\xfa\x03\x49\x78",
            "CRV",
            18,
            "Curve DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xda\x10\x00\x9c\xbd\x5d\x07\xdd\x0c\xec\xc6\x61\x61\xfc\x93\xd7\xc9\x00\x0d\xa1",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x69\xeb\x4f\xa4\xa2\xfb\xd4\x98\xc2\x57\xc5\x7e\xa8\xb7\x65\x5a\x25\x59\xa5\x81",
            "DODO",
            18,
            "DODO bird",
        )
        yield (  # address, symbol, decimals, name
            b"\x6c\x2c\x06\x79\x0b\x3e\x3e\x3c\x38\xe1\x2e\xe2\x2f\x81\x83\xb3\x7a\x13\xee\x55",
            "DPX",
            18,
            "Dopex Governance Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa0\xb8\x62\xf6\x0e\xde\xf4\x45\x2f\x25\xb4\x16\x0f\x17\x7d\xb4\x4d\xeb\x6c\xf1",
            "GNO",
            18,
            "Gnosis Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x23\xa9\x41\x03\x6a\xe7\x78\xac\x51\xab\x04\xce\xa0\x8e\xd6\xe2\xfe\x10\x36\x14",
            "GRT",
            18,
            "Graph Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb4\x1b\xd4\xc9\x9d\xa7\x35\x10\xd9\xe0\x81\xc5\xfa\xdb\xe7\xa2\x7a\xc1\xf8\x14",
            "IMO",
            18,
            "Ideamarket",
        )
        yield (  # address, symbol, decimals, name
            b"\xf9\x7f\x4d\xf7\x51\x17\xa7\x8c\x1a\x5a\x0d\xbb\x81\x4a\xf9\x24\x58\x53\x9f\xb4",
            "LINK",
            18,
            "ChainLink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\xd0\xce\x7d\xe6\x24\x7b\x0a\x95\xf6\x7b\x43\xb5\x89\xb4\x04\x1b\xae\x7f\xbe",
            "LRC",
            18,
            "LoopringCoin V2",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\xf4\x0b\x01\xba\x9c\x46\x91\x93\xb3\x60\xf7\x27\x40\xe4\x16\xb1\x7a\xc3\x32",
            "MATH",
            18,
            "MATH Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfe\xa7\xa6\xa0\xb3\x46\x36\x2b\xf8\x8a\x9e\x4a\x88\x41\x6b\x77\xa5\x7d\x6c\x2a",
            "MIM",
            18,
            "Magic Internet Money",
        )
        yield (  # address, symbol, decimals, name
            b"\xed\x3f\xb7\x61\x41\x4d\xa7\x4b\x74\xf3\x3e\x5c\x5a\x1f\x78\x10\x4b\x18\x8d\xfc",
            "NYAN",
            18,
            "ArbiNYAN",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\x66\x48\xc5\xa7\x0a\x15\x0a\x88\xbc\xe6\x5f\x4a\xd4\xd5\x06\xfe\x15\xd2\xaf",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xa9\x70\xaf\x1a\x58\x45\x79\xb6\x18\xbe\x4d\x69\xad\x6f\x73\x45\x9d\x11\x2f\x95",
            "sUSD",
            18,
            "Synth sUSD",
        )
        yield (  # address, symbol, decimals, name
            b"\xd4\xd4\x2f\x0b\x6d\xef\x4c\xe0\x38\x36\x36\x77\x0e\xf7\x73\x39\x0d\x85\xc6\x1a",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xa7\x21\x59\xfc\x39\x0f\x0e\x3c\x6d\x41\x5e\x65\x82\x64\xc7\xc4\x05\x1e\x9b\x87",
            "TCR",
            18,
            "Tracer",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x7f\x89\x80\xb0\xf1\xe6\x4a\x20\x62\x79\x1c\xc3\xb0\x87\x15\x72\xf1\xf7\xf0",
            "UNI",
            18,
            "Uniswap",
        )
        yield (  # address, symbol, decimals, name
            b"\xff\x97\x0a\x61\xa0\x4b\x1c\xa1\x48\x34\xa4\x3f\x5d\xe4\x53\x3e\xbd\xdb\x5c\xc8",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xfd\x08\x6b\xc7\xcd\x5c\x48\x1d\xcc\x9c\x85\xeb\xe4\x78\xa1\xc0\xb6\x9f\xcb\xb9",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x99\x5c\x23\x55\x21\x82\x0f\x26\x37\x30\x3c\xa1\x97\x0c\x7c\x04\x45\x83\xdf\x44",
            "VISR",
            18,
            "VISOR",
        )
        yield (  # address, symbol, decimals, name
            b"\x2f\x2a\x25\x43\xb7\x6a\x41\x66\x54\x9f\x7a\xab\x2e\x75\xbe\xf0\xae\xfc\x5b\x0f",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xaf\x49\x44\x7d\x8a\x07\xe3\xbd\x95\xbd\x0d\x56\xf3\x52\x41\x52\x3f\xba\xb1",
            "WETH",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xca\xfc\xd8\x5d\x8c\xa7\xad\x1e\x1c\x6f\x82\xf6\x51\xfa\x15\xe3\x3a\xef\xd0\x7b",
            "WOO",
            18,
            "Wootrade Network",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xe3\xa8\xf0\x66\xa6\x98\x96\x66\xb0\x31\xd9\x16\xc4\x36\x72\x08\x5b\x15\x82",
            "YFI",
            18,
            "yearn.finance",
        )
    if chain_id == 42220:  # Celo
        yield (  # address, symbol, decimals, name
            b"\x63\x9a\x64\x7f\xbe\x20\xb6\xc8\xac\x19\xe4\x8e\x2d\xe4\x4e\xa7\x92\xc6\x2c\x5c",
            "BIFI",
            18,
            "beefy.finance",
        )
        yield (  # address, symbol, decimals, name
            b"\xd6\x29\xeb\x00\xde\xce\xd2\xa0\x80\xb7\xec\x63\x0e\xf6\xac\x11\x7e\x61\x4f\x1b",
            "BTC",
            18,
            "Wrapped Bitcoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\x1e\xce\x37\x50\xda\x23\x7f\x93\xb8\xe3\x39\xc5\x36\x98\x9b\x89\x78\xa4\x38",
            "CELO",
            18,
            "Celo native asset",
        )
        yield (  # address, symbol, decimals, name
            b"\x2d\xef\x42\x85\x78\x7d\x58\xa2\xf8\x11\xaf\x24\x75\x5a\x81\x50\x62\x2f\x43\x61",
            "cETH",
            18,
            "Wrapped Ethereum",
        )
        yield (  # address, symbol, decimals, name
            b"\xd8\x76\x3c\xba\x27\x6a\x37\x38\xe6\xde\x85\xb4\xb3\xbf\x5f\xde\xd6\xd6\xca\x73",
            "cEUR",
            18,
            "Celo Euro",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x27\xfd\x24\x4e\x11\xdb\x1c\x7b\x12\x15\x61\x91\x44\xd2\xaa\xba\xc8\x0a\x4f",
            "cLA",
            18,
            "celoLaunch",
        )
        yield (  # address, symbol, decimals, name
            b"\xf3\x60\x8f\x84\x6c\xa7\x31\x47\xf0\x8f\xde\x8d\x57\xf4\x5e\x27\xce\xea\x4d\x61",
            "cMETA",
            18,
            "metaCelo Game NFT",
        )
        yield (  # address, symbol, decimals, name
            b"\x76\x5d\xe8\x16\x84\x58\x61\xe7\x5a\x25\xfc\xa1\x22\xbb\x68\x98\xb8\xb1\x28\x2a",
            "cUSD",
            18,
            "Celo Dollar",
        )
        yield (  # address, symbol, decimals, name
            b"\x70\x37\xf7\x29\x6b\x2f\xc7\x90\x8d\xe7\xb5\x7a\x89\xef\xaa\x83\x19\xf0\xc5\x00",
            "mCELO",
            18,
            "Moola CELO AToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x73\xa2\x10\x63\x7f\x6f\x6b\x70\x05\x51\x26\x77\xba\x6b\x3c\x96\xbb\x4a\xa4\x4b",
            "MOBI",
            18,
            "Mobius DAO Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x46\xc9\x75\x7c\x54\x97\xc5\xb1\xf2\xeb\x73\xae\x79\xb6\xb6\x7d\x11\x9b\x0b\x58",
            "PACT",
            18,
            "impactMarket",
        )
        yield (  # address, symbol, decimals, name
            b"\x74\xc0\xc5\x8b\x99\xb6\x8c\xf1\x6a\x71\x72\x79\xac\x2d\x05\x6a\x34\xba\x2b\xfe",
            "SOURCE",
            18,
            "Source",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\x5e\xc7\x21\xc2\xa8\x96\x51\x2a\xd2\x9c\x67\x19\x97\xdd\x68\xf9\x59\x32\x26",
            "SUSHI",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x00\xbe\x91\x5b\x9d\xcf\x56\xa3\xcb\xe7\x39\xd9\xb9\xc2\x02\xca\x69\x24\x09\xec",
            "UBE",
            18,
            "Ubeswap",
        )
    if chain_id == 43114:  # AVAX
        yield (  # address, symbol, decimals, name
            b"\x63\xa7\x28\x06\x09\x8b\xd3\xd9\x52\x0c\xc4\x33\x56\xdd\x78\xaf\xe5\xd3\x86\xd9",
            "AAVE.e",
            18,
            "Aave Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x21\x47\xef\xff\x67\x5e\x4a\x4e\xe1\xc2\xf9\x18\xd1\x81\xcd\xbd\x7a\x8e\x20\x8f",
            "ALPHA.e",
            18,
            "AlphaToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x48\xf8\x8a\x3f\xe8\x43\xcc\xb0\xb5\x00\x3e\x70\xb4\x19\x2c\x1d\x74\x48\xbe\xf0",
            "CAI",
            18,
            "Colony Avalanche Index",
        )
        yield (  # address, symbol, decimals, name
            b"\x63\x7a\xfe\xff\x75\xca\x66\x9f\xf9\x2e\x45\x70\xb1\x4d\x63\x99\xa6\x58\x90\x2f",
            "COOK",
            18,
            "Poly-Peg COOK",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\x86\xe7\xf8\x44\xce\xa2\xf8\x7f\x50\x15\x26\x65\xbc\xbc\x2c\x27\x9d\x8d\x70",
            "DAI.e",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\x85\x43\x42\x27\xab\x73\x15\x1f\xad\x2d\xe5\x46\x21\x0c\xbc\x86\x63\xdf\x96",
            "DBY",
            18,
            "Metaderby token",
        )
        yield (  # address, symbol, decimals, name
            b"\xfc\xc6\xce\x74\xf4\xcd\x7e\xde\xf0\xc5\x42\x9b\xb9\x9d\x38\xa3\x60\x80\x43\xa5",
            "FIRE",
            18,
            "FIRE",
        )
        yield (  # address, symbol, decimals, name
            b"\x02\x61\x87\xbd\xbc\x6b\x75\x10\x03\x51\x7b\xcb\x30\xac\x78\x17\xd5\xb7\x66\xf8",
            "H2O",
            18,
            "Defrost Finance H2O",
        )
        yield (  # address, symbol, decimals, name
            b"\x82\xfe\x03\x8e\xa4\xb5\x0f\x9c\x95\x7d\xa3\x26\xc4\x12\xeb\xd7\x34\x62\x07\x7c",
            "HAT",
            18,
            "Joe Hat Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x65\x37\x8b\x69\x78\x53\x56\x8d\xa9\xff\x8e\xab\x60\xc1\x3e\x1e\xe9\xf4\xa6\x54",
            "HUSKY",
            18,
            "Husky",
        )
        yield (  # address, symbol, decimals, name
            b"\x3e\xef\xb1\x80\x03\xd0\x33\x66\x1f\x84\xe4\x83\x60\xeb\xec\xd1\x81\xa8\x47\x09",
            "ISA",
            18,
            "Islander",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x47\xbb\x27\x5c\x52\x10\x40\x05\x1d\x82\x39\x61\x92\x18\x1b\x41\x32\x27\xa3",
            "LINK.e",
            18,
            "Chainlink Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x44\x96\x74\xb8\x2f\x05\xd4\x98\xe1\x26\xdd\x66\x15\xa1\x05\x7a\x9c\x08\x8f\x2c",
            "LOST",
            18,
            "LostToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x47\xeb\x6f\x75\x25\xc1\xaa\x99\x9f\xbc\x9e\xe9\x27\x15\xf5\x23\x1e\xb1\x24\x1d",
            "MELT",
            18,
            "Defrost Finance Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x13\x09\x66\x62\x88\x46\xbf\xd3\x6f\xf3\x1a\x82\x27\x05\x79\x6e\x8c\xb8\xc1\x8d",
            "MIM",
            18,
            "Magic Internet Money",
        )
        yield (  # address, symbol, decimals, name
            b"\xd9\xd9\x0f\x88\x2c\xdd\xd6\x06\x39\x59\xa9\xd8\x37\xb0\x5c\xb7\x48\x71\x8a\x05",
            "MORE",
            18,
            "More Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\x78\x1c\x25\x86\xd6\x82\x29\xfd\xe4\x75\x64\x54\x67\x84\xab\x3f\xac\xa9\x82",
            "PNG",
            18,
            "Pangolin",
        )
        yield (  # address, symbol, decimals, name
            b"\x87\x29\x43\x8e\xb1\x5e\x2c\x8b\x57\x6f\xcc\x6a\xec\xda\x6a\x14\x87\x76\xc0\xf5",
            "QI",
            18,
            "BENQI",
        )
        yield (  # address, symbol, decimals, name
            b"\xce\x1b\xff\xbd\x53\x74\xda\xc8\x6a\x28\x93\x11\x96\x83\xf4\x91\x1a\x2f\x78\x14",
            "SPELL",
            18,
            "Spell Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xb2\x79\xf8\xdd\x15\x2b\x99\xec\x1d\x84\xa4\x89\xd3\x2c\x35\xbc\x0c\x7f\x56\x74",
            "STEAK",
            18,
            "STEAK",
        )
        yield (  # address, symbol, decimals, name
            b"\x37\xb6\x08\x51\x9f\x91\xf7\x0f\x2e\xeb\x0e\x5e\xd9\xaf\x40\x61\x72\x2e\x4f\x76",
            "SUSHI.e",
            18,
            "SushiToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xb9\x7e\xf9\xef\x87\x34\xc7\x19\x04\xd8\x00\x2f\x8b\x6b\xc6\x6d\xd9\xc4\x8a\x6e",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\xa7\xd7\x07\x9b\x0f\xea\xd9\x1f\x3e\x65\xf8\x6e\x89\x15\xcb\x59\xc1\xa4\xc6\x64",
            "USDC.e",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x97\x02\x23\x0a\x8e\xa5\x36\x01\xf5\xcd\x2d\xc0\x0f\xdb\xc1\x3d\x4d\xf4\xa8\xc7",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\xc7\x19\x84\x37\x98\x0c\x04\x1c\x80\x5a\x1e\xdc\xba\x50\xc1\xce\x5d\xb9\x51\x18",
            "USDT.e",
            6,
            "Tether USD",
        )
        yield (  # address, symbol, decimals, name
            b"\x84\x6d\x50\x24\x8b\xaf\x8b\x7c\xea\xa9\xd9\xb5\x3b\xfd\x12\xd7\xd7\xfb\xb2\x5a",
            "VSO",
            18,
            "VersoToken",
        )
        yield (  # address, symbol, decimals, name
            b"\xb3\x1f\x66\xaa\x3c\x1e\x78\x53\x63\xf0\x87\x5a\x1b\x74\xe2\x7b\x85\xfd\x66\xc7",
            "WAVAX",
            18,
            "Wrapped AVAX",
        )
        yield (  # address, symbol, decimals, name
            b"\x50\xb7\x54\x56\x27\xa5\x16\x2f\x82\xa9\x92\xc3\x3b\x87\xad\xc7\x51\x87\xb2\x18",
            "WBTC.e",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\xd5\xc2\xbd\xff\xac\x6c\xe2\xbf\xdb\x66\x40\xf4\xf8\x0f\x22\x6b\xc1\x0b\xab",
            "WETH.e",
            18,
            "Wrapped Ether",
        )
        yield (  # address, symbol, decimals, name
            b"\xd1\xc3\xf9\x4d\xe7\xe5\xb4\x5f\xa4\xed\xbb\xa4\x72\x49\x1a\x9f\x4b\x16\x6f\xc4",
            "XAVA",
            18,
            "Avalaunch",
        )
        yield (  # address, symbol, decimals, name
            b"\x59\x41\x4b\x30\x89\xce\x2a\xf0\x01\x0e\x75\x23\xde\xa7\xe2\xb3\x5d\x77\x6e\xc7",
            "YAK",
            18,
            "Yak Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x77\x77\x77\x77\x77\x7d\x45\x54\xc3\x92\x23\xc3\x54\xa0\x58\x25\xb2\xe8\xfa\xa3",
            "YETI",
            18,
            "Yeti Finance",
        )
    if chain_id == 1313161554:  # NEAR
        yield (  # address, symbol, decimals, name
            b"\xc2\x1f\xf0\x12\x29\xe9\x82\xd7\xc8\xb8\x69\x11\x63\xb0\xa3\xcb\x8f\x35\x74\x53",
            "$META",
            24,
            "Meta Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x2b\xae\x00\xc8\xbc\x18\x68\xa5\xf7\xa2\x16\xe8\x81\xba\xe9\xe6\x62\x63\x01\x11",
            "ABR",
            18,
            "Allbridge - Allbridge",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\xbd\xd2\x7c\x33\xec\x7d\xaa\x6f\xcf\xd8\x53\x2d\xdb\x52\x4b\xf4\x03\x80\x96",
            "atLUNA",
            18,
            "Luna Terra - Allbridge",
        )
        yield (  # address, symbol, decimals, name
            b"\x2a\xb9\x8d\x9e\xa8\x1a\xf2\x00\x37\xaf\x1a\x4f\x43\xcc\x3e\x69\x77\x54\x58\x40",
            "ATO",
            18,
            "Atocha Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x5c\xe9\xf0\xb6\xaf\xb3\x61\x35\xb5\xdd\xbf\x11\x70\x5c\xeb\x65\xe6\x34\xa9\xdc",
            "atUST",
            18,
            "UST Terra - Allbridge",
        )
        yield (  # address, symbol, decimals, name
            b"\x8b\xec\x47\x86\x5a\xde\x3b\x17\x2a\x92\x8d\xf8\xf9\x90\xbc\x7f\x2a\x3b\x9f\x79",
            "AURORA",
            18,
            "Aurora",
        )
        yield (  # address, symbol, decimals, name
            b"\x89\x73\xc9\xec\x7b\x79\xfe\x88\x06\x97\xcd\xbc\xa7\x44\x89\x26\x82\x76\x4c\x37",
            "BAKED",
            18,
            "BakedToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x41\x48\xd2\xce\x78\x16\xf0\xae\x37\x8d\x98\xb4\x0e\xb3\xa7\x21\x1e\x1f\xcf\x0d",
            "BBT",
            18,
            "BlueBit Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x9f\x1f\x93\x3c\x66\x0a\x1d\xc8\x56\xf0\xe0\xfe\x05\x84\x35\x87\x9c\x5c\xce\xf0",
            "BSTN",
            18,
            "Bastion",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\x52\x03\x49\xf4\x77\xa5\xf6\xeb\x06\x10\x70\x66\x04\x85\x08\x49\x8a\x29\x1b",
            "DAI",
            18,
            "Dai Stablecoin",
        )
        yield (  # address, symbol, decimals, name
            b"\xe3\x01\xed\x8c\x76\x30\xc9\x67\x8c\x39\xe4\xe4\x51\x93\xd1\xe7\xdf\xb9\x14\xf7",
            "DODO",
            18,
            "DODO bird",
        )
        yield (  # address, symbol, decimals, name
            b"\x17\xcb\xd9\xc2\x74\xe9\x0c\x53\x77\x90\xc5\x1b\x40\x15\xa6\x5c\xd0\x15\x49\x7e",
            "ETHERNAL",
            18,
            "ETHERNAL",
        )
        yield (  # address, symbol, decimals, name
            b"\xd5\xc9\x97\x72\x4e\x4b\x57\x56\xd0\x8e\x64\x64\xc0\x1a\xfb\xc5\xf6\x39\x72\x36",
            "FAME",
            18,
            "FAME",
        )
        yield (  # address, symbol, decimals, name
            b"\xea\x62\x79\x1a\xa6\x82\xd4\x55\x61\x4e\xaa\x2a\x12\xba\x3d\x9a\x2f\xd1\x97\xaf",
            "FLX",
            18,
            "Flux Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xe4\xeb\x03\x59\x8f\x4d\xca\xb7\x40\x33\x1f\xa4\x32\xf4\xb8\x5f\xf5\x8a\xa9\x7e",
            "KSW",
            18,
            "KillSwitchToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x91\x8d\xbe\x08\x70\x40\xa4\x1b\x78\x6f\x0d\xa8\x31\x90\xc2\x93\xda\xe2\x47\x49",
            "LINEAR",
            24,
            "LiNEAR",
        )
        yield (  # address, symbol, decimals, name
            b"\x25\xe8\x01\xeb\x75\x85\x9b\xa4\x05\x2c\x4a\xc4\x23\x3c\xec\x02\x64\xea\xdf\x8c",
            "LUNAR",
            18,
            "LUNAR",
        )
        yield (  # address, symbol, decimals, name
            b"\xa3\x3c\x3b\x53\x69\x44\x19\x82\x47\x22\xc1\x0d\x99\xad\x7c\xb1\x6e\xa6\x27\x54",
            "MECHA",
            18,
            "Mecha",
        )
        yield (  # address, symbol, decimals, name
            b"\x3a\xc5\x5e\xa8\xd2\x08\x2f\xab\xda\x67\x42\x70\xcd\x23\x67\xda\x96\x09\x28\x89",
            "ORBITAL",
            18,
            "ORBITAL",
        )
        yield (  # address, symbol, decimals, name
            b"\x34\xf2\x91\x93\x4b\x88\xc7\x87\x0b\x7a\x17\x83\x5b\x92\x6b\x26\x4f\xc1\x3a\x81",
            "PAD",
            18,
            "SmartPad token",
        )
        yield (  # address, symbol, decimals, name
            b"\x88\x5f\x8c\xf6\xe4\x5b\xdd\x3f\xdc\xdc\x64\x4e\xfd\xcd\x0a\xc9\x38\x80\xc7\x81",
            "PAD",
            18,
            "NearPad Token",
        )
        yield (  # address, symbol, decimals, name
            b"\x29\x1c\x8f\xce\xac\xa3\x34\x2b\x29\xcc\x36\x17\x1d\xeb\x98\x10\x6f\x71\x2c\x66",
            "PICKLE",
            18,
            "PickleToken",
        )
        yield (  # address, symbol, decimals, name
            b"\x09\xc9\xd4\x64\xb5\x8d\x96\x83\x7f\x8d\x8b\x6f\x4d\x9f\xe4\xad\x40\x8d\x3a\x4f",
            "PLY",
            18,
            "Aurigami Token",
        )
        yield (  # address, symbol, decimals, name
            b"\xf0\xf3\xb9\xee\xe3\x2b\x1f\x49\x0a\x4b\x87\x20\xcf\x6f\x00\x5d\x4a\xe9\xea\x86",
            "POLAR",
            18,
            "POLAR",
        )
        yield (  # address, symbol, decimals, name
            b"\x9d\x6f\xc9\x0b\x25\x97\x6e\x40\xad\xad\x5a\x3e\xdd\x08\xaf\x9e\xd7\xa2\x17\x29",
            "SPOLAR",
            18,
            "SPOLAR",
        )
        yield (  # address, symbol, decimals, name
            b"\x07\xf9\xf7\xf9\x63\xc5\xcd\x2b\xbf\xfd\x30\xcc\xfb\x96\x4b\xe1\x14\x33\x2e\x30",
            "STNEAR",
            24,
            "Staked NEAR",
        )
        yield (  # address, symbol, decimals, name
            b"\xfa\x94\x34\x84\x67\xf6\x4d\x5a\x45\x7f\x75\xf8\xbc\x40\x49\x5d\x33\xc6\x5a\xbb",
            "TRI",
            18,
            "Trisolaris",
        )
        yield (  # address, symbol, decimals, name
            b"\x60\x52\x7a\x27\x51\xa8\x27\xec\x0a\xdf\x86\x1e\xfc\xac\xbf\x11\x15\x87\xd7\x48",
            "TRIPOLAR",
            18,
            "TRIPOLAR",
        )
        yield (  # address, symbol, decimals, name
            b"\x98\x4c\x25\x05\xa1\x4d\xa7\x32\xd7\x27\x14\x16\x35\x6f\x53\x59\x53\x61\x03\x40",
            "UMINT",
            18,
            "YouMinter",
        )
        yield (  # address, symbol, decimals, name
            b"\xb1\x2b\xfc\xa5\xa5\x58\x06\xaa\xf6\x4e\x99\x52\x19\x18\xa4\xbf\x0f\xc4\x08\x02",
            "USDC",
            6,
            "USD Coin",
        )
        yield (  # address, symbol, decimals, name
            b"\x49\x88\xa8\x96\xb1\x22\x72\x18\xe4\xa6\x86\xfd\xe5\xea\xbd\xca\xbd\x91\x57\x1f",
            "USDT",
            6,
            "Tether",
        )
        yield (  # address, symbol, decimals, name
            b"\x51\x83\xe1\xb1\x09\x18\x04\xbc\x26\x02\x58\x69\x19\xe6\x88\x0a\xc1\xcf\x28\x96",
            "USN",
            18,
            "USN",
        )
        yield (  # address, symbol, decimals, name
            b"\xa6\x9d\x9b\xa0\x86\xd4\x14\x25\xf3\x59\x88\x61\x3c\x15\x6d\xb9\xa8\x8a\x1a\x96",
            "USP",
            18,
            "USP",
        )
        yield (  # address, symbol, decimals, name
            b"\x7f\xaa\x64\xfa\xf5\x47\x50\xa2\xe3\xee\x62\x11\x66\x63\x5f\xea\xf4\x06\xab\x22",
            "WANNA",
            18,
            "WannaSwap",
        )
        yield (  # address, symbol, decimals, name
            b"\xf4\xeb\x21\x7b\xa2\x45\x46\x13\xb1\x5d\xbd\xea\x6e\x5f\x22\x27\x64\x10\xe8\x9e",
            "WBTC",
            8,
            "Wrapped BTC",
        )
        yield (  # address, symbol, decimals, name
            b"\xc4\x2c\x30\xac\x6c\xc1\x5f\xac\x9b\xd9\x38\x61\x8b\xca\xa1\xa1\xfa\xe8\x50\x1d",
            "wNEAR",
            24,
            "Wrapped NEAR fungible token",
        )
        yield (  # address, symbol, decimals, name
            b"\x7c\xa1\xc2\x86\x63\xb7\x6c\xfd\xe4\x24\xa9\x49\x45\x55\xb9\x48\x46\x20\x55\x85",
            "XNL",
            18,
            "Chronicle",
        )
        yield (  # address, symbol, decimals, name
            b"\x80\x21\x19\xe4\xe2\x53\xd5\xc1\x9a\xa0\x6a\x5d\x56\x7c\x5a\x41\x59\x6d\x68\x03",
            "xTRI",
            18,
            "TriBar",
        )
