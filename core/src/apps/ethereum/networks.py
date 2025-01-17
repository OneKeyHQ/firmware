# generated from networks.py.mako
# (by running `make templates` in `core`)
# do not edit manually!

# NOTE: using positional arguments saves 4400 bytes in flash size,
# returning tuples instead of classes saved 800 bytes

from typing import TYPE_CHECKING

from trezor.messages import EthereumNetworkInfo

from apps.common.paths import HARDENED

if TYPE_CHECKING:
    from typing import Iterator

    # Removing the necessity to construct object to save space
    # fmt: off
    NetworkInfoTuple = tuple[
        int,  # chain_id
        int,  # slip44
        str,  # symbol
        str,  # name
        str,  # icon
        int,  # primary_color
    ]
    # fmt: on

UNKNOWN_NETWORK = EthereumNetworkInfo(
    chain_id=0,
    slip44=0,
    symbol="UNKN",
    name="Unknown network",
)


def shortcut_by_chain_id(chain_id: int) -> str:
    n = by_chain_id(chain_id)
    return n.symbol if n is not None else "UNKN"


def all_slip44_ids_hardened() -> Iterator[int]:
    for n in _networks_iterator():
        yield n[1] | HARDENED


def by_chain_id(chain_id: int) -> EthereumNetworkInfo:
    for n in _networks_iterator():
        n_chain_id = n[0]
        if n_chain_id == chain_id:
            return EthereumNetworkInfo(
                chain_id=n[0],
                slip44=n[1],
                symbol=n[2],
                name=n[3],
                icon=n[4],
                primary_color=n[5],
            )
    return UNKNOWN_NETWORK


def by_slip44(slip44: int) -> EthereumNetworkInfo:
    for n in _networks_iterator():
        n_slip44 = n[1]
        if n_slip44 == slip44:
            return EthereumNetworkInfo(
                chain_id=n[0],
                slip44=n[1],
                symbol=n[2],
                name=n[3],
                icon=n[4],
                primary_color=n[5],
            )
    return UNKNOWN_NETWORK


# fmt: off
def _networks_iterator() -> Iterator[NetworkInfoTuple]:
    yield (
        1,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Ethereum",  # name
        "evm-eth.png",  # name
        0x637FFF,  # primary_color
    )
    yield (
        2,  # chain_id
        40,  # slip44
        "EXP",  # symbol
        "Expanse Network",  # name
        "evm-exp.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        3,  # chain_id
        1,  # slip44
        "tETH",  # symbol
        "Ropsten",  # name
        "evm-teth.png",  # name
        0x969696,  # primary_color
    )
    yield (
        4,  # chain_id
        1,  # slip44
        "tETH",  # symbol
        "Rinkeby",  # name
        "evm-teth.png",  # name
        0x969696,  # primary_color
    )
    yield (
        5,  # chain_id
        1,  # slip44
        "tETH",  # symbol
        "Goerli",  # name
        "evm-teth.png",  # name
        0x969696,  # primary_color
    )
    yield (
        6,  # chain_id
        1,  # slip44
        "tKOT",  # symbol
        "Kotti Testnet",  # name
        "evm-tkot.png",  # name
        0x969696,  # primary_color
    )
    yield (
        7,  # chain_id
        60,  # slip44
        "TCH",  # symbol
        "ThaiChain",  # name
        "evm-tch.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        8,  # chain_id
        108,  # slip44
        "UBQ",  # symbol
        "Ubiq",  # name
        "evm-ubq.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        9,  # chain_id
        1,  # slip44
        "TUBQ",  # symbol
        "Ubiq Network Testnet",  # name
        "evm-tubq.png",  # name
        0x969696,  # primary_color
    )
    yield (
        10,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "OP",  # name
        "evm-oeth.png",  # name
        0xFF0420,  # primary_color
    )
    yield (
        11,  # chain_id
        916,  # slip44
        "META",  # symbol
        "Metadium",  # name
        "evm-meta.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        12,  # chain_id
        1,  # slip44
        "tKAL",  # symbol
        "Metadium Testnet",  # name
        "evm-tkal.png",  # name
        0x969696,  # primary_color
    )
    yield (
        13,  # chain_id
        1,  # slip44
        "tsDIODE",  # symbol
        "Diode Testnet Staging",  # name
        "evm-tsdiode.png",  # name
        0x969696,  # primary_color
    )
    yield (
        14,  # chain_id
        60,  # slip44
        "FLR",  # symbol
        "Flare",  # name
        "evm-flr.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        15,  # chain_id
        60,  # slip44
        "DIODE",  # symbol
        "Diode Prenet",  # name
        "evm-diode.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        16,  # chain_id
        1,  # slip44
        "tCFLR",  # symbol
        "Songbird Testnet Coston",  # name
        "evm-tcflr.png",  # name
        0x969696,  # primary_color
    )
    yield (
        17,  # chain_id
        60,  # slip44
        "TFI",  # symbol
        "ThaiChain 2.0 ThaiFi",  # name
        "evm-tfi.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        18,  # chain_id
        1,  # slip44
        "TST",  # symbol
        "ThunderCore Testnet",  # name
        "evm-tst.png",  # name
        0x969696,  # primary_color
    )
    yield (
        19,  # chain_id
        60,  # slip44
        "SGB",  # symbol
        "Songbird Canary-Network",  # name
        "evm-sgb.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        20,  # chain_id
        60,  # slip44
        "ELA",  # symbol
        "Elastos Smart Chain",  # name
        "evm-ela.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        25,  # chain_id
        60,  # slip44
        "CRO",  # symbol
        "Cronos",  # name
        "evm-cro.png",  # name
        0x1199FA,  # primary_color
    )
    yield (
        27,  # chain_id
        60,  # slip44
        "SHIB",  # symbol
        "ShibaChain",  # name
        "evm-shib.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        29,  # chain_id
        60,  # slip44
        "L1",  # symbol
        "Genesis L1",  # name
        "evm-l1.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        30,  # chain_id
        137,  # slip44
        "RBTC",  # symbol
        "Rootstock",  # name
        "evm-rbtc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        31,  # chain_id
        1,  # slip44
        "tRBTC",  # symbol
        "Rootstock Testnet",  # name
        "evm-trbtc.png",  # name
        0x969696,  # primary_color
    )
    yield (
        33,  # chain_id
        60,  # slip44
        "GooD",  # symbol
        "GoodData",  # name
        "evm-good.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        35,  # chain_id
        60,  # slip44
        "TBG",  # symbol
        "TBWG Chain",  # name
        "evm-tbg.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        38,  # chain_id
        538,  # slip44
        "VAL",  # symbol
        "Valorbit",  # name
        "evm-val.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        40,  # chain_id
        60,  # slip44
        "TLOS",  # symbol
        "Telos EVM",  # name
        "evm-tlos.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        44,  # chain_id
        60,  # slip44
        "CRAB",  # symbol
        "Crab Network",  # name
        "evm-crab.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        50,  # chain_id
        60,  # slip44
        "XDC",  # symbol
        "XDC Network",  # name
        "evm-xdc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        51,  # chain_id
        60,  # slip44
        "TXDC",  # symbol
        "XDC Apothem Network",  # name
        "evm-txdc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        52,  # chain_id
        60,  # slip44
        "cet",  # symbol
        "CoinEx Smart Chain",  # name
        "evm-cet.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        55,  # chain_id
        60,  # slip44
        "ZYX",  # symbol
        "Zyx",  # name
        "evm-zyx.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        56,  # chain_id
        714,  # slip44
        "BNB",  # symbol
        "BNB Smart Chain",  # name
        "evm-bnb.png",  # name
        0xF0B90B,  # primary_color
    )
    yield (
        58,  # chain_id
        60,  # slip44
        "ONG",  # symbol
        "Ontology",  # name
        "evm-ong.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        60,  # chain_id
        6060,  # slip44
        "GO",  # symbol
        "GoChain",  # name
        "evm-go.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        61,  # chain_id
        61,  # slip44
        "ETC",  # symbol
        "Ethereum Classic",  # name
        "evm-etc.png",  # name
        0x328332,  # primary_color
    )
    yield (
        62,  # chain_id
        1,  # slip44
        "TETC",  # symbol
        "Morden Testnet",  # name
        "evm-tetc.png",  # name
        0x969696,  # primary_color
    )
    yield (
        63,  # chain_id
        1,  # slip44
        "tMETC",  # symbol
        "Mordor Testnet",  # name
        "evm-tmetc.png",  # name
        0x969696,  # primary_color
    )
    yield (
        64,  # chain_id
        163,  # slip44
        "ELLA",  # symbol
        "Ellaism",  # name
        "evm-ella.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        66,  # chain_id
        60,  # slip44
        "OKT",  # symbol
        "OKXChain",  # name
        "evm-okt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        67,  # chain_id
        1,  # slip44
        "tDBM",  # symbol
        "DBChain Testnet",  # name
        "evm-tdbm.png",  # name
        0x969696,  # primary_color
    )
    yield (
        74,  # chain_id
        60,  # slip44
        "EIDI",  # symbol
        "IDChain",  # name
        "evm-eidi.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        76,  # chain_id
        76,  # slip44
        "MIX",  # symbol
        "Mix",  # name
        "evm-mix.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        77,  # chain_id
        60,  # slip44
        "SPOA",  # symbol
        "POA Network Sokol",  # name
        "evm-spoa.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        78,  # chain_id
        60,  # slip44
        "PETH",  # symbol
        "PrimusChain",  # name
        "evm-peth.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        80,  # chain_id
        60,  # slip44
        "RNA",  # symbol
        "GeneChain",  # name
        "evm-rna.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        82,  # chain_id
        60,  # slip44
        "MTR",  # symbol
        "Meter",  # name
        "evm-mtr.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        87,  # chain_id
        60,  # slip44
        "SNT",  # symbol
        "Nova Network",  # name
        "evm-snt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        88,  # chain_id
        889,  # slip44
        "VIC",  # symbol
        "Viction",  # name
        "evm-vic.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        90,  # chain_id
        60,  # slip44
        "GAR",  # symbol
        "Garizon Stage0",  # name
        "evm-gar.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        97,  # chain_id
        1,  # slip44
        "tBNB",  # symbol
        "BNB Smart Chain Testnet",  # name
        "evm-tbnb.png",  # name
        0x969696,  # primary_color
    )
    yield (
        99,  # chain_id
        178,  # slip44
        "POA",  # symbol
        "POA Network Core",  # name
        "evm-poa.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100,  # chain_id
        700,  # slip44
        "XDAI",  # symbol
        "Gnosis",  # name
        "evm-xdai.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        101,  # chain_id
        464,  # slip44
        "ETI",  # symbol
        "EtherInc",  # name
        "evm-eti.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        105,  # chain_id
        60,  # slip44
        "W3G",  # symbol
        "Web3Games Devnet",  # name
        "evm-w3g.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        106,  # chain_id
        60,  # slip44
        "VLX",  # symbol
        "Velas EVM",  # name
        "evm-vlx.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        108,  # chain_id
        1001,  # slip44
        "TT",  # symbol
        "ThunderCore",  # name
        "evm-tt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        111,  # chain_id
        60,  # slip44
        "ETL",  # symbol
        "EtherLite Chain",  # name
        "evm-etl.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        122,  # chain_id
        60,  # slip44
        "FUSE",  # symbol
        "Fuse",  # name
        "evm-fuse.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        123,  # chain_id
        60,  # slip44
        "SPARK",  # symbol
        "Fuse Sparknet",  # name
        "evm-spark.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        124,  # chain_id
        60,  # slip44
        "DWU",  # symbol
        "Decentralized Web",  # name
        "evm-dwu.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        126,  # chain_id
        126,  # slip44
        "OY",  # symbol
        "OYchain",  # name
        "evm-oy.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        127,  # chain_id
        127,  # slip44
        "FETH",  # symbol
        "Factory 127",  # name
        "evm-feth.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        128,  # chain_id
        1010,  # slip44
        "HT",  # symbol
        "Huobi ECO Chain",  # name
        "evm-ht.png",  # name
        0x01943F,  # primary_color
    )
    yield (
        137,  # chain_id
        966,  # slip44
        "POL",  # symbol
        "Polygon",  # name
        "evm-matic.png",  # name
        0x8247E5,  # primary_color
    )
    yield (
        142,  # chain_id
        60,  # slip44
        "DAX",  # symbol
        "DAX CHAIN",  # name
        "evm-dax.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        146,  # chain_id
        60,  # slip44
        "S",  # symbol
        "Sonic",  # name
        "evm-s.png",  # name
        0xFFFFFF,  # primary_color
    )
    yield (
        162,  # chain_id
        1,  # slip44
        "tPHT",  # symbol
        "Lightstreams Testnet",  # name
        "evm-tpht.png",  # name
        0x969696,  # primary_color
    )
    yield (
        163,  # chain_id
        60,  # slip44
        "PHT",  # symbol
        "Lightstreams",  # name
        "evm-pht.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        177,  # chain_id
        60,  # slip44
        "HSK",  # symbol
        "HashKey Chain",  # name
        "evm-hsk.png",  # name
        0xFFFFFF,  # primary_color
    )
    yield (
        186,  # chain_id
        60,  # slip44
        "Seele",  # symbol
        "Seele",  # name
        "evm-seele.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        188,  # chain_id
        60,  # slip44
        "BTM",  # symbol
        "BMC",  # name
        "evm-btm.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        199,  # chain_id
        60,  # slip44
        "BTT",  # symbol
        "BitTorrent Chain",  # name
        "evm-btt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        211,  # chain_id
        60,  # slip44
        "0xF",  # symbol
        "Freight Trust Network",  # name
        "evm-0xf.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        222,  # chain_id
        2221,  # slip44
        "ASK",  # symbol
        "Permission",  # name
        "evm-ask.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        225,  # chain_id
        60,  # slip44
        "LA",  # symbol
        "LACHAIN",  # name
        "evm-la.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        246,  # chain_id
        246,  # slip44
        "EWT",  # symbol
        "Energy Web Chain",  # name
        "evm-ewt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        250,  # chain_id
        60,  # slip44
        "FTM",  # symbol
        "Fantom Opera",  # name
        "evm-ftm.png",  # name
        0x1969FF,  # primary_color
    )
    yield (
        256,  # chain_id
        1,  # slip44
        "thtt",  # symbol
        "Huobi ECO Chain Testnet",  # name
        "evm-thtt.png",  # name
        0x969696,  # primary_color
    )
    yield (
        258,  # chain_id
        60,  # slip44
        "SETM",  # symbol
        "Setheum",  # name
        "evm-setm.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        262,  # chain_id
        60,  # slip44
        "SRN",  # symbol
        "SUR Blockchain Network",  # name
        "evm-srn.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        269,  # chain_id
        269,  # slip44
        "HPB",  # symbol
        "High Performance Blockchain",  # name
        "evm-hpb.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        288,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Boba Network",  # name
        "evm-boba.png",  # name
        0xCCFF00,  # primary_color
    )
    yield (
        314,  # chain_id
        461,  # slip44
        "FIL",  # symbol
        "Filecoin",  # name
        "evm-filecoin.png",  # name
        0x0090FF,  # primary_color
    )
    yield (
        321,  # chain_id
        641,  # slip44
        "KCS",  # symbol
        "KCC",  # name
        "evm-kcs.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        324,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "zkSync",  # name
        "evm-zksync-era.png",  # name
        0xFFFFFF,  # primary_color
    )
    yield (
        333,  # chain_id
        60,  # slip44
        "W3Q",  # symbol
        "Web3Q",  # name
        "evm-w3q.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        336,  # chain_id
        60,  # slip44
        "SDN",  # symbol
        "Shiden",  # name
        "evm-sdn.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        369,  # chain_id
        60,  # slip44
        "PLS",  # symbol
        "PulseChain",  # name
        "evm-pls.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        420,  # chain_id
        1,  # slip44
        "tETH",  # symbol
        "Optimism Goerli Testnet",  # name
        "evm-teth.png",  # name
        0x969696,  # primary_color
    )
    yield (
        499,  # chain_id
        499,  # slip44
        "RUPX",  # symbol
        "Rupaya",  # name
        "evm-rupx.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        512,  # chain_id
        1512,  # slip44
        "AAC",  # symbol
        "Double-A Chain",  # name
        "evm-aac.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        555,  # chain_id
        60,  # slip44
        "CLASS",  # symbol
        "Vela1 Chain",  # name
        "evm-class.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        558,  # chain_id
        60,  # slip44
        "TAO",  # symbol
        "Tao Network",  # name
        "evm-tao.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        595,  # chain_id
        1,  # slip44
        "tmACA",  # symbol
        "Acala Mandala Testnet TC9",  # name
        "evm-tmaca.png",  # name
        0x969696,  # primary_color
    )
    yield (
        686,  # chain_id
        686,  # slip44
        "KAR",  # symbol
        "Karura Network",  # name
        "evm-kar.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        707,  # chain_id
        60,  # slip44
        "BCS",  # symbol
        "BlockChain Station",  # name
        "evm-bcs.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        777,  # chain_id
        60,  # slip44
        "cTH",  # symbol
        "cheapETH",  # name
        "evm-cth.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        787,  # chain_id
        787,  # slip44
        "ACA",  # symbol
        "Acala Network",  # name
        "evm-aca.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        803,  # chain_id
        60,  # slip44
        "HAIC",  # symbol
        "Haic",  # name
        "evm-haic.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        820,  # chain_id
        820,  # slip44
        "CLO",  # symbol
        "Callisto",  # name
        "evm-clo.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        821,  # chain_id
        1,  # slip44
        "TCLO",  # symbol
        "Callisto Testnet Deprecated",  # name
        "evm-tclo.png",  # name
        0x969696,  # primary_color
    )
    yield (
        888,  # chain_id
        5718350,  # slip44
        "WAN",  # symbol
        "Wanchain",  # name
        "evm-wan.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        977,  # chain_id
        60,  # slip44
        "YETI",  # symbol
        "Nepal Blockchain Network",  # name
        "evm-yeti.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        998,  # chain_id
        60,  # slip44
        "L99",  # symbol
        "Lucky Network",  # name
        "evm-l99.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1001,  # chain_id
        1,  # slip44
        "tKAIA",  # symbol
        "Kaia Kairos Testnet",  # name
        "evm-tkaia.png",  # name
        0x969696,  # primary_color
    )
    yield (
        1007,  # chain_id
        1,  # slip44
        "tNEW",  # symbol
        "Newton Testnet",  # name
        "evm-tnew.png",  # name
        0x969696,  # primary_color
    )
    yield (
        1010,  # chain_id
        1020,  # slip44
        "EVC",  # symbol
        "Evrice Network",  # name
        "evm-evc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1012,  # chain_id
        60,  # slip44
        "NEW",  # symbol
        "Newton",  # name
        "evm-new.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1022,  # chain_id
        60,  # slip44
        "SKU",  # symbol
        "Sakura",  # name
        "evm-sku.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1024,  # chain_id
        60,  # slip44
        "CLV",  # symbol
        "CLV Parachain",  # name
        "evm-clv.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1030,  # chain_id
        60,  # slip44
        "CFX",  # symbol
        "Conflux eSpace",  # name
        "evm-cfx.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1088,  # chain_id
        60,  # slip44
        "METIS",  # symbol
        "Metis Andromeda",  # name
        "evm-metis.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1139,  # chain_id
        60,  # slip44
        "MATH",  # symbol
        "MathChain",  # name
        "evm-math.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1140,  # chain_id
        1,  # slip44
        "tMATH",  # symbol
        "MathChain Testnet",  # name
        "evm-tmath.png",  # name
        0x969696,  # primary_color
    )
    yield (
        1197,  # chain_id
        60,  # slip44
        "IORA",  # symbol
        "Iora Chain",  # name
        "evm-iora.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1202,  # chain_id
        60,  # slip44
        "WTT",  # symbol
        "World Trade Technical Chain",  # name
        "evm-wtt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1213,  # chain_id
        60,  # slip44
        "POP",  # symbol
        "Popcateum",  # name
        "evm-pop.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1214,  # chain_id
        60,  # slip44
        "ENTER",  # symbol
        "EnterChain",  # name
        "evm-enter.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1280,  # chain_id
        60,  # slip44
        "HO",  # symbol
        "HALO",  # name
        "evm-ho.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1284,  # chain_id
        60,  # slip44
        "GLMR",  # symbol
        "Moonbeam",  # name
        "evm-glmr.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1285,  # chain_id
        60,  # slip44
        "MOVR",  # symbol
        "Moonriver",  # name
        "evm-movr.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1287,  # chain_id
        1,  # slip44
        "DEV",  # symbol
        "Moonbase Alpha",  # name
        "evm-dev.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1618,  # chain_id
        60,  # slip44
        "CATE",  # symbol
        "Catecoin Chain",  # name
        "evm-cate.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1657,  # chain_id
        60,  # slip44
        "BTA",  # symbol
        "Btachain",  # name
        "evm-bta.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1856,  # chain_id
        60,  # slip44
        "TSF",  # symbol
        "Teslafunds",  # name
        "evm-tsf.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1898,  # chain_id
        60,  # slip44
        "BOY",  # symbol
        "BON Network",  # name
        "evm-boy.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1987,  # chain_id
        1987,  # slip44
        "EGEM",  # symbol
        "EtherGem",  # name
        "evm-egem.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        2021,  # chain_id
        523,  # slip44
        "EDG",  # symbol
        "Edgeware EdgeEVM",  # name
        "evm-edg.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        2025,  # chain_id
        1008,  # slip44
        "RPG",  # symbol
        "Rangers Protocol",  # name
        "evm-rpg.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        2100,  # chain_id
        60,  # slip44
        "ECO",  # symbol
        "Ecoball",  # name
        "evm-eco.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        2213,  # chain_id
        60,  # slip44
        "EVA",  # symbol
        "Evanesco",  # name
        "evm-eva.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        2559,  # chain_id
        60,  # slip44
        "KTO",  # symbol
        "Kortho",  # name
        "evm-kto.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        3400,  # chain_id
        60,  # slip44
        "PRB",  # symbol
        "Paribu Net",  # name
        "evm-prb.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        3966,  # chain_id
        60,  # slip44
        "DYNO",  # symbol
        "DYNO",  # name
        "evm-dyno.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        4689,  # chain_id
        60,  # slip44
        "IOTX",  # symbol
        "IoTeX Network",  # name
        "evm-iotx.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        5000,  # chain_id
        60,  # slip44
        "MNT",  # symbol
        "Mantle",  # name
        "evm-mnt.png",  # name
        0xFFFFFF,  # primary_color
    )
    yield (
        5197,  # chain_id
        60,  # slip44
        "ES",  # symbol
        "EraSwap",  # name
        "evm-es.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        5315,  # chain_id
        60,  # slip44
        "UZMI",  # symbol
        "Uzmi Network",  # name
        "evm-uzmi.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        5869,  # chain_id
        60,  # slip44
        "RBD",  # symbol
        "Wegochain Rubidium",  # name
        "evm-rbd.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        6626,  # chain_id
        60,  # slip44
        "PIX",  # symbol
        "Pixie Chain",  # name
        "evm-pix.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        8000,  # chain_id
        60,  # slip44
        "TELE",  # symbol
        "Teleport",  # name
        "evm-tele.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        8217,  # chain_id
        8217,  # slip44
        "KAIA",  # symbol
        "Kaia",  # name
        "evm-kaia.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        8453,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Base",  # name
        "evm-base.png",  # name
        0x0052FF,  # primary_color
    )
    yield (
        8723,  # chain_id
        479,  # slip44
        "OLO",  # symbol
        "TOOL Global",  # name
        "evm-olo.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        8995,  # chain_id
        60,  # slip44
        "U+25B3",  # symbol
        "bloxberg",  # name
        "evm-u+25b3.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        9001,  # chain_id
        60,  # slip44
        "EVMOS",  # symbol
        "Evmos",  # name
        "evm-evmos.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        9100,  # chain_id
        60,  # slip44
        "GNC",  # symbol
        "Genesis Coin",  # name
        "evm-gnc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        10101,  # chain_id
        60,  # slip44
        "GEN",  # symbol
        "Blockchain Genesis",  # name
        "evm-gen.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        10823,  # chain_id
        60,  # slip44
        "CCP",  # symbol
        "CryptoCoinPay",  # name
        "evm-ccp.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        11111,  # chain_id
        60,  # slip44
        "WGM",  # symbol
        "WAGMI",  # name
        "evm-wgm.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        12052,  # chain_id
        621,  # slip44
        "ZERO",  # symbol
        "Singularity ZERO",  # name
        "evm-zero.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        13381,  # chain_id
        60,  # slip44
        "PHX",  # symbol
        "Phoenix",  # name
        "evm-phx.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        16000,  # chain_id
        60,  # slip44
        "MTT",  # symbol
        "MetaDot",  # name
        "evm-mtt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        19515,  # chain_id
        1,  # slip44
        "tSEP",  # symbol
        "SEC Testnet",  # name
        "evm-tsep.png",  # name
        0x969696,  # primary_color
    )
    yield (
        19845,  # chain_id
        60,  # slip44
        "BTCIX",  # symbol
        "BTCIX Network",  # name
        "evm-btcix.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        24484,  # chain_id
        227,  # slip44
        "WEB",  # symbol
        "Webchain",  # name
        "evm-web.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        24734,  # chain_id
        60,  # slip44
        "MINTME",  # symbol
        "MintMe.com Coin",  # name
        "evm-mintme.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        31102,  # chain_id
        31102,  # slip44
        "ESN",  # symbol
        "Ethersocial Network",  # name
        "evm-esn.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        32659,  # chain_id
        288,  # slip44
        "FSN",  # symbol
        "Fusion",  # name
        "evm-fsn.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        39797,  # chain_id
        39797,  # slip44
        "NRG",  # symbol
        "Energi",  # name
        "evm-nrg.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        42069,  # chain_id
        60,  # slip44
        "peggle",  # symbol
        "pegglecoin",  # name
        "evm-peggle.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        42161,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Arbitrum One",  # name
        "evm-arb1.png",  # name
        0x28A0F0,  # primary_color
    )
    yield (
        42220,  # chain_id
        60,  # slip44
        "CELO",  # symbol
        "Celo",  # name
        "evm-celo.png",  # name
        0x35D07F,  # primary_color
    )
    yield (
        43113,  # chain_id
        1,  # slip44
        "tAVAX",  # symbol
        "Avalanche Fuji Testnet",  # name
        "evm-tavax.png",  # name
        0x969696,  # primary_color
    )
    yield (
        43114,  # chain_id
        9005,  # slip44
        "AVAX",  # symbol
        "Avalanche C-Chain",  # name
        "evm-avax.png",  # name
        0xE84142,  # primary_color
    )
    yield (
        44787,  # chain_id
        1,  # slip44
        "tCELO",  # symbol
        "Celo Alfajores Testnet",  # name
        "evm-tcelo.png",  # name
        0x969696,  # primary_color
    )
    yield (
        47805,  # chain_id
        60,  # slip44
        "REI",  # symbol
        "REI Network",  # name
        "evm-rei.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        49797,  # chain_id
        1,  # slip44
        "tNRG",  # symbol
        "Energi Testnet",  # name
        "evm-tnrg.png",  # name
        0x969696,  # primary_color
    )
    yield (
        53935,  # chain_id
        60,  # slip44
        "JEWEL",  # symbol
        "DFK Chain",  # name
        "evm-jewel.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        59144,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Linea",  # name
        "evm-linea.png",  # name
        0xFFFFFF,  # primary_color
    )
    yield (
        62320,  # chain_id
        1,  # slip44
        "tCELO",  # symbol
        "Celo Baklava Testnet",  # name
        "evm-tcelo.png",  # name
        0x969696,  # primary_color
    )
    yield (
        63000,  # chain_id
        60,  # slip44
        "ECS",  # symbol
        "eSync Network",  # name
        "evm-ecs.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        73799,  # chain_id
        1,  # slip44
        "tVT",  # symbol
        "Energy Web Volta Testnet",  # name
        "evm-tvt.png",  # name
        0x969696,  # primary_color
    )
    yield (
        73927,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Mixin Virtual Machine",  # name
        "evm-mixin.png",  # name
        0x5959D8,  # primary_color
    )
    yield (
        78110,  # chain_id
        1,  # slip44
        "FIN",  # symbol
        "Firenze test network",  # name
        "evm-fin.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        80001,  # chain_id
        1,  # slip44
        "tMATIC",  # symbol
        "Mumbai",  # name
        "evm-tmatic.png",  # name
        0x969696,  # primary_color
    )
    yield (
        99999,  # chain_id
        60,  # slip44
        "UBC",  # symbol
        "UB Smart Chain",  # name
        "evm-ubc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100000,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100001,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100002,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100003,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100004,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100005,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100006,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100007,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        100008,  # chain_id
        60,  # slip44
        "QKC",  # symbol
        "QuarkChain",  # name
        "evm-qkc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        200625,  # chain_id
        200625,  # slip44
        "AKA",  # symbol
        "Akroma",  # name
        "evm-aka.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        201018,  # chain_id
        60,  # slip44
        "atp",  # symbol
        "Alaya",  # name
        "evm-atp.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        210425,  # chain_id
        60,  # slip44
        "lat",  # symbol
        "PlatON",  # name
        "evm-lat.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        246529,  # chain_id
        246529,  # slip44
        "ATS",  # symbol
        "ARTIS sigma1",  # name
        "evm-ats.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        246785,  # chain_id
        1,  # slip44
        "tATS",  # symbol
        "ARTIS Testnet tau1",  # name
        "evm-tats.png",  # name
        0x969696,  # primary_color
    )
    yield (
        281121,  # chain_id
        60,  # slip44
        "$OC",  # symbol
        "Social Smart Chain",  # name
        "evm-$oc.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        333999,  # chain_id
        60,  # slip44
        "POLIS",  # symbol
        "Polis",  # name
        "evm-polis.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        888888,  # chain_id
        60,  # slip44
        "VS",  # symbol
        "Vision",  # name
        "evm-vs.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        955305,  # chain_id
        1011,  # slip44
        "ELV",  # symbol
        "Eluvio Content Fabric",  # name
        "evm-elv.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1313114,  # chain_id
        1313114,  # slip44
        "ETHO",  # symbol
        "Etho Protocol",  # name
        "evm-etho.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1313500,  # chain_id
        60,  # slip44
        "XERO",  # symbol
        "Xerom",  # name
        "evm-xero.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        7762959,  # chain_id
        184,  # slip44
        "MUSIC",  # symbol
        "Musicoin",  # name
        "evm-music.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        11155111,  # chain_id
        1,  # slip44
        "tETH",  # symbol
        "Sepolia",  # name
        "evm-teth.png",  # name
        0x969696,  # primary_color
    )
    yield (
        13371337,  # chain_id
        60,  # slip44
        "TPEP",  # symbol
        "PepChain Churchill",  # name
        "evm-tpep.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        18289463,  # chain_id
        60,  # slip44
        "ILT",  # symbol
        "IOLite",  # name
        "evm-ilt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        20181205,  # chain_id
        60,  # slip44
        "QKI",  # symbol
        "quarkblockchain",  # name
        "evm-qki.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        28945486,  # chain_id
        344,  # slip44
        "AUX",  # symbol
        "Auxilium Network",  # name
        "evm-aux.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        35855456,  # chain_id
        60,  # slip44
        "JOYS",  # symbol
        "Joys Digital",  # name
        "evm-joys.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        61717561,  # chain_id
        61717561,  # slip44
        "AQUA",  # symbol
        "Aquachain",  # name
        "evm-aqua.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        99415706,  # chain_id
        1,  # slip44
        "TOYS",  # symbol
        "Joys Digital TestNet",  # name
        "evm-toys.png",  # name
        0x969696,  # primary_color
    )
    yield (
        245022934,  # chain_id
        60,  # slip44
        "NEON",  # symbol
        "Neon EVM",  # name
        "evm-neon.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        311752642,  # chain_id
        60,  # slip44
        "OLT",  # symbol
        "OneLedger",  # name
        "evm-olt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1122334455,  # chain_id
        60,  # slip44
        "IPOS",  # symbol
        "IPOS Network",  # name
        "evm-ipos.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1313161554,  # chain_id
        60,  # slip44
        "ETH",  # symbol
        "Aurora",  # name
        "evm-aurora.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1666600000,  # chain_id
        1023,  # slip44
        "ONE",  # symbol
        "Harmony",  # name
        "evm-one.png",  # name
        0x33D3D5,  # primary_color
    )
    yield (
        1666600001,  # chain_id
        1023,  # slip44
        "ONE",  # symbol
        "Harmony",  # name
        "evm-one.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1666600002,  # chain_id
        1023,  # slip44
        "ONE",  # symbol
        "Harmony",  # name
        "evm-one.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        1666600003,  # chain_id
        1023,  # slip44
        "ONE",  # symbol
        "Harmony",  # name
        "evm-one.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        2021121117,  # chain_id
        60,  # slip44
        "HOP",  # symbol
        "DataHopper",  # name
        "evm-hop.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        3125659152,  # chain_id
        164,  # slip44
        "PIRL",  # symbol
        "Pirl",  # name
        "evm-pirl.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        11297108109,  # chain_id
        60,  # slip44
        "PALM",  # symbol
        "Palm",  # name
        "evm-palm.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        197710212030,  # chain_id
        60,  # slip44
        "NTT",  # symbol
        "Ntity",  # name
        "evm-ntt.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        6022140761023,  # chain_id
        60,  # slip44
        "MOLE",  # symbol
        "Molereum Network",  # name
        "evm-mole.png",  # name
        0xD2D2D2,  # primary_color
    )
    yield (
        9798,  # chain_id
        9798,  # slip44
        "DTT",  # symbol
        "Data Trade Chain",  # name
        "evm-dtt.png",  # icon
        0x1A2A5F,  # primary_color
    )
