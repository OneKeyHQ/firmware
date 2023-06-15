# generated from networks.py.mako
# (by running `make templates` in `core`)
# do not edit manually!
from typing import Iterator

from apps.common.paths import HARDENED


def shortcut_by_chain_id(chain_id: int) -> str:
    n = by_chain_id(chain_id)
    return n.shortcut if n is not None else "UNKN"


def by_chain_id(chain_id: int) -> "NetworkInfo" | None:
    for n in _networks_iterator():
        if n.chain_id == chain_id:
            return n
    return None


def by_slip44(slip44: int) -> "NetworkInfo" | None:
    for n in _networks_iterator():
        if n.slip44 == slip44:
            return n
    return None


def all_slip44_ids_hardened() -> Iterator[int]:
    for n in _networks_iterator():
        yield n.slip44 | HARDENED


class NetworkInfo:
    def __init__(
        self,
        chain_id: int,
        slip44: int,
        shortcut: str,
        name: str,
        rskip60: bool,
        icon: str,
        primary_color: int,
    ) -> None:
        self.chain_id = chain_id
        self.slip44 = slip44
        self.shortcut = shortcut
        self.name = name
        self.rskip60 = rskip60
        self.icon = icon
        self.primary_color = primary_color


# fmt: off
def _networks_iterator() -> Iterator[NetworkInfo]:
    yield NetworkInfo(
        chain_id=1,
        slip44=60,
        shortcut="ETH",
        name="Ethereum",
        rskip60=False,
        icon="evm-eth.png",
        primary_color=0x637FFF,
    )
    yield NetworkInfo(
        chain_id=2,
        slip44=40,
        shortcut="EXP",
        name="Expanse Network",
        rskip60=False,
        icon="evm-exp.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=3,
        slip44=1,
        shortcut="tETH",
        name="Ropsten",
        rskip60=False,
        icon="evm-teth.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=4,
        slip44=1,
        shortcut="tETH",
        name="Rinkeby",
        rskip60=False,
        icon="evm-teth.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=5,
        slip44=1,
        shortcut="tETH",
        name="Goerli",
        rskip60=False,
        icon="evm-teth.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=6,
        slip44=1,
        shortcut="tKOT",
        name="Ethereum Classic Testnet Kotti",
        rskip60=False,
        icon="evm-tkot.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=7,
        slip44=60,
        shortcut="TCH",
        name="ThaiChain",
        rskip60=False,
        icon="evm-tch.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=8,
        slip44=108,
        shortcut="UBQ",
        name="Ubiq",
        rskip60=False,
        icon="evm-ubq.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=9,
        slip44=1,
        shortcut="TUBQ",
        name="Ubiq Network Testnet",
        rskip60=False,
        icon="evm-tubq.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=10,
        slip44=60,
        shortcut="ETH",
        name="Optimism",
        rskip60=False,
        icon="evm-oeth.png",
        primary_color=0xFF0420,
    )
    yield NetworkInfo(
        chain_id=11,
        slip44=916,
        shortcut="META",
        name="Metadium",
        rskip60=False,
        icon="evm-meta.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=12,
        slip44=1,
        shortcut="tKAL",
        name="Metadium Testnet",
        rskip60=False,
        icon="evm-tkal.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=13,
        slip44=1,
        shortcut="tsDIODE",
        name="Diode Testnet Staging",
        rskip60=False,
        icon="evm-tsdiode.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=14,
        slip44=60,
        shortcut="FLR",
        name="Flare",
        rskip60=False,
        icon="evm-flr.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=15,
        slip44=60,
        shortcut="DIODE",
        name="Diode Prenet",
        rskip60=False,
        icon="evm-diode.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=16,
        slip44=1,
        shortcut="tCFLR",
        name="Flare Testnet Coston",
        rskip60=False,
        icon="evm-tcflr.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=17,
        slip44=60,
        shortcut="TFI",
        name="ThaiChain 2.0 ThaiFi",
        rskip60=False,
        icon="evm-tfi.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=18,
        slip44=1,
        shortcut="TST",
        name="ThunderCore Testnet",
        rskip60=False,
        icon="evm-tst.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=19,
        slip44=60,
        shortcut="SGB",
        name="Songbird Canary-Network",
        rskip60=False,
        icon="evm-sgb.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=20,
        slip44=60,
        shortcut="ELA",
        name="Elastos Smart Chain",
        rskip60=False,
        icon="evm-ela.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=25,
        slip44=60,
        shortcut="CRO",
        name="Cronos",
        rskip60=False,
        icon="evm-cro.png",
        primary_color=0x1199FA,
    )
    yield NetworkInfo(
        chain_id=27,
        slip44=60,
        shortcut="SHIB",
        name="ShibaChain",
        rskip60=False,
        icon="evm-shib.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=29,
        slip44=60,
        shortcut="L1",
        name="Genesis L1",
        rskip60=False,
        icon="evm-l1.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=30,
        slip44=137,
        shortcut="RBTC",
        name="RSK",
        rskip60=True,
        icon="evm-rbtc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=31,
        slip44=1,
        shortcut="tRBTC",
        name="RSK Testnet",
        rskip60=False,
        icon="evm-trbtc.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=33,
        slip44=60,
        shortcut="GooD",
        name="GoodData",
        rskip60=False,
        icon="evm-good.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=35,
        slip44=60,
        shortcut="TBG",
        name="TBWG Chain",
        rskip60=False,
        icon="evm-tbg.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=38,
        slip44=538,
        shortcut="VAL",
        name="Valorbit",
        rskip60=False,
        icon="evm-val.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=40,
        slip44=60,
        shortcut="TLOS",
        name="Telos EVM",
        rskip60=False,
        icon="evm-tlos.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=44,
        slip44=60,
        shortcut="CRAB",
        name="Darwinia Crab Network",
        rskip60=False,
        icon="evm-crab.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=50,
        slip44=60,
        shortcut="XDC",
        name="XinFin XDC Network",
        rskip60=False,
        icon="evm-xdc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=51,
        slip44=60,
        shortcut="TXDC",
        name="XDC Apothem Network",
        rskip60=False,
        icon="evm-txdc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=52,
        slip44=60,
        shortcut="cet",
        name="CoinEx Smart Chain",
        rskip60=False,
        icon="evm-cet.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=55,
        slip44=60,
        shortcut="ZYX",
        name="Zyx",
        rskip60=False,
        icon="evm-zyx.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=56,
        slip44=714,
        shortcut="BNB",
        name="Binance Smart Chain",
        rskip60=False,
        icon="evm-bnb.png",
        primary_color=0xF0B90B,
    )
    yield NetworkInfo(
        chain_id=58,
        slip44=60,
        shortcut="ONG",
        name="Ontology",
        rskip60=False,
        icon="evm-ong.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=60,
        slip44=6060,
        shortcut="GO",
        name="GoChain",
        rskip60=False,
        icon="evm-go.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=61,
        slip44=61,
        shortcut="ETC",
        name="Ethereum Classic",
        rskip60=False,
        icon="evm-etc.png",
        primary_color=0x328332,
    )
    yield NetworkInfo(
        chain_id=62,
        slip44=1,
        shortcut="TETC",
        name="Ethereum Classic Testnet Morden",
        rskip60=False,
        icon="evm-tetc.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=63,
        slip44=1,
        shortcut="tMETC",
        name="Ethereum Classic Testnet Mordor",
        rskip60=False,
        icon="evm-tmetc.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=64,
        slip44=163,
        shortcut="ELLA",
        name="Ellaism",
        rskip60=False,
        icon="evm-ella.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=66,
        slip44=60,
        shortcut="OKT",
        name="OKXChain",
        rskip60=False,
        icon="evm-okt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=67,
        slip44=1,
        shortcut="tDBM",
        name="DBChain Testnet",
        rskip60=False,
        icon="evm-tdbm.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=74,
        slip44=60,
        shortcut="EIDI",
        name="IDChain",
        rskip60=False,
        icon="evm-eidi.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=76,
        slip44=76,
        shortcut="MIX",
        name="Mix",
        rskip60=False,
        icon="evm-mix.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=77,
        slip44=60,
        shortcut="SPOA",
        name="POA Network Sokol",
        rskip60=False,
        icon="evm-spoa.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=78,
        slip44=60,
        shortcut="PETH",
        name="PrimusChain",
        rskip60=False,
        icon="evm-peth.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=80,
        slip44=60,
        shortcut="RNA",
        name="GeneChain",
        rskip60=False,
        icon="evm-rna.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=82,
        slip44=60,
        shortcut="MTR",
        name="Meter",
        rskip60=False,
        icon="evm-mtr.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=87,
        slip44=60,
        shortcut="SNT",
        name="Nova Network",
        rskip60=False,
        icon="evm-snt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=88,
        slip44=889,
        shortcut="TOMO",
        name="TomoChain",
        rskip60=False,
        icon="evm-tomo.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=90,
        slip44=60,
        shortcut="GAR",
        name="Garizon Stage0",
        rskip60=False,
        icon="evm-gar.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=97,
        slip44=1,
        shortcut="tBNB",
        name="Binance Smart Chain Testnet",
        rskip60=False,
        icon="evm-tbnb.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=99,
        slip44=178,
        shortcut="POA",
        name="POA Network Core",
        rskip60=False,
        icon="evm-poa.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100,
        slip44=700,
        shortcut="XDAI",
        name="Gnosis",
        rskip60=False,
        icon="evm-xdai.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=101,
        slip44=464,
        shortcut="ETI",
        name="EtherInc",
        rskip60=False,
        icon="evm-eti.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=105,
        slip44=60,
        shortcut="W3G",
        name="Web3Games Devnet",
        rskip60=False,
        icon="evm-w3g.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=106,
        slip44=60,
        shortcut="VLX",
        name="Velas EVM",
        rskip60=False,
        icon="evm-vlx.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=108,
        slip44=1001,
        shortcut="TT",
        name="ThunderCore",
        rskip60=False,
        icon="evm-tt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=111,
        slip44=60,
        shortcut="ETL",
        name="EtherLite Chain",
        rskip60=False,
        icon="evm-etl.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=122,
        slip44=60,
        shortcut="FUSE",
        name="Fuse",
        rskip60=False,
        icon="evm-fuse.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=123,
        slip44=60,
        shortcut="SPARK",
        name="Fuse Sparknet",
        rskip60=False,
        icon="evm-spark.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=124,
        slip44=60,
        shortcut="DWU",
        name="Decentralized Web",
        rskip60=False,
        icon="evm-dwu.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=126,
        slip44=126,
        shortcut="OY",
        name="OYchain",
        rskip60=False,
        icon="evm-oy.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=127,
        slip44=127,
        shortcut="FETH",
        name="Factory 127",
        rskip60=False,
        icon="evm-feth.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=128,
        slip44=1010,
        shortcut="HT",
        name="Huobi ECO Chain",
        rskip60=False,
        icon="evm-ht.png",
        primary_color=0x01943F,
    )
    yield NetworkInfo(
        chain_id=137,
        slip44=966,
        shortcut="MATIC",
        name="Polygon",
        rskip60=False,
        icon="evm-matic.png",
        primary_color=0x8247E5,
    )
    yield NetworkInfo(
        chain_id=142,
        slip44=60,
        shortcut="DAX",
        name="DAX CHAIN",
        rskip60=False,
        icon="evm-dax.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=162,
        slip44=1,
        shortcut="tPHT",
        name="Lightstreams Testnet",
        rskip60=False,
        icon="evm-tpht.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=163,
        slip44=60,
        shortcut="PHT",
        name="Lightstreams",
        rskip60=False,
        icon="evm-pht.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=186,
        slip44=60,
        shortcut="Seele",
        name="Seele",
        rskip60=False,
        icon="evm-seele.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=188,
        slip44=60,
        shortcut="BTM",
        name="BMC",
        rskip60=False,
        icon="evm-btm.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=199,
        slip44=60,
        shortcut="BTT",
        name="BitTorrent Chain",
        rskip60=False,
        icon="evm-btt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=211,
        slip44=60,
        shortcut="0xF",
        name="Freight Trust Network",
        rskip60=False,
        icon="evm-0xf.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=222,
        slip44=2221,
        shortcut="ASK",
        name="Permission",
        rskip60=False,
        icon="evm-ask.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=225,
        slip44=60,
        shortcut="LA",
        name="LACHAIN",
        rskip60=False,
        icon="evm-la.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=246,
        slip44=246,
        shortcut="EWT",
        name="Energy Web Chain",
        rskip60=False,
        icon="evm-ewt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=250,
        slip44=60,
        shortcut="FTM",
        name="Fantom Opera",
        rskip60=False,
        icon="evm-ftm.png",
        primary_color=0x1969FF,
    )
    yield NetworkInfo(
        chain_id=256,
        slip44=1,
        shortcut="thtt",
        name="Huobi ECO Chain Testnet",
        rskip60=False,
        icon="evm-thtt.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=258,
        slip44=60,
        shortcut="SETM",
        name="Setheum",
        rskip60=False,
        icon="evm-setm.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=262,
        slip44=60,
        shortcut="SRN",
        name="SUR Blockchain Network",
        rskip60=False,
        icon="evm-srn.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=269,
        slip44=269,
        shortcut="HPB",
        name="High Performance Blockchain",
        rskip60=False,
        icon="evm-hpb.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=288,
        slip44=60,
        shortcut="ETH",
        name="Boba Network",
        rskip60=False,
        icon="evm-boba.png",
        primary_color=0xCCFF00,
    )
    yield NetworkInfo(
        chain_id=314,
        slip44=461,
        shortcut="FIL",
        name="Filecoin",
        rskip60=False,
        icon="evm-filecoin.png",
        primary_color=0x0090FF,
    )
    yield NetworkInfo(
        chain_id=321,
        slip44=641,
        shortcut="KCS",
        name="KCC",
        rskip60=False,
        icon="evm-kcs.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=324,
        slip44=60,
        shortcut="ETH",
        name="zkSync Era",
        rskip60=False,
        icon="evm-zksync-era.png",
        primary_color=0xFFFFFF,
    )
    yield NetworkInfo(
        chain_id=333,
        slip44=60,
        shortcut="W3Q",
        name="Web3Q",
        rskip60=False,
        icon="evm-w3q.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=336,
        slip44=60,
        shortcut="SDN",
        name="Shiden",
        rskip60=False,
        icon="evm-sdn.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=369,
        slip44=60,
        shortcut="PLS",
        name="PulseChain",
        rskip60=False,
        icon="evm-pls.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=420,
        slip44=1,
        shortcut="tETH",
        name="Optimism Goerli Testnet",
        rskip60=False,
        icon="evm-teth.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=499,
        slip44=499,
        shortcut="RUPX",
        name="Rupaya",
        rskip60=False,
        icon="evm-rupx.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=512,
        slip44=1512,
        shortcut="AAC",
        name="Double-A Chain",
        rskip60=False,
        icon="evm-aac.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=555,
        slip44=60,
        shortcut="CLASS",
        name="Vela1 Chain",
        rskip60=False,
        icon="evm-class.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=558,
        slip44=60,
        shortcut="TAO",
        name="Tao Network",
        rskip60=False,
        icon="evm-tao.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=595,
        slip44=1,
        shortcut="tmACA",
        name="Acala Mandala Testnet",
        rskip60=False,
        icon="evm-tmaca.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=686,
        slip44=686,
        shortcut="KAR",
        name="Karura Network",
        rskip60=False,
        icon="evm-kar.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=707,
        slip44=60,
        shortcut="BCS",
        name="BlockChain Station",
        rskip60=False,
        icon="evm-bcs.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=777,
        slip44=60,
        shortcut="cTH",
        name="cheapETH",
        rskip60=False,
        icon="evm-cth.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=787,
        slip44=787,
        shortcut="ACA",
        name="Acala Network",
        rskip60=False,
        icon="evm-aca.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=803,
        slip44=60,
        shortcut="HAIC",
        name="Haic",
        rskip60=False,
        icon="evm-haic.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=820,
        slip44=820,
        shortcut="CLO",
        name="Callisto",
        rskip60=False,
        icon="evm-clo.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=821,
        slip44=1,
        shortcut="TCLO",
        name="Callisto Testnet Deprecated",
        rskip60=False,
        icon="evm-tclo.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=888,
        slip44=5718350,
        shortcut="WAN",
        name="Wanchain",
        rskip60=False,
        icon="evm-wan.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=977,
        slip44=60,
        shortcut="YETI",
        name="Nepal Blockchain Network",
        rskip60=False,
        icon="evm-yeti.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=998,
        slip44=60,
        shortcut="L99",
        name="Lucky Network",
        rskip60=False,
        icon="evm-l99.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1001,
        slip44=1,
        shortcut="tKLAY",
        name="Klaytn Testnet Baobab",
        rskip60=False,
        icon="evm-tklay.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=1007,
        slip44=1,
        shortcut="tNEW",
        name="Newton Testnet",
        rskip60=False,
        icon="evm-tnew.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=1010,
        slip44=1020,
        shortcut="EVC",
        name="Evrice Network",
        rskip60=False,
        icon="evm-evc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1012,
        slip44=60,
        shortcut="NEW",
        name="Newton",
        rskip60=False,
        icon="evm-new.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1022,
        slip44=60,
        shortcut="SKU",
        name="Sakura",
        rskip60=False,
        icon="evm-sku.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1024,
        slip44=60,
        shortcut="CLV",
        name="CLV Parachain",
        rskip60=False,
        icon="evm-clv.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1030,
        slip44=60,
        shortcut="CFX",
        name="Conflux eSpace",
        rskip60=False,
        icon="evm-cfx.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1088,
        slip44=60,
        shortcut="METIS",
        name="Metis Andromeda",
        rskip60=False,
        icon="evm-metis.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1139,
        slip44=60,
        shortcut="MATH",
        name="MathChain",
        rskip60=False,
        icon="evm-math.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1140,
        slip44=1,
        shortcut="tMATH",
        name="MathChain Testnet",
        rskip60=False,
        icon="evm-tmath.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=1197,
        slip44=60,
        shortcut="IORA",
        name="Iora Chain",
        rskip60=False,
        icon="evm-iora.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1202,
        slip44=60,
        shortcut="WTT",
        name="World Trade Technical Chain",
        rskip60=False,
        icon="evm-wtt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1213,
        slip44=60,
        shortcut="POP",
        name="Popcateum",
        rskip60=False,
        icon="evm-pop.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1214,
        slip44=60,
        shortcut="ENTER",
        name="EnterChain",
        rskip60=False,
        icon="evm-enter.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1280,
        slip44=60,
        shortcut="HO",
        name="HALO",
        rskip60=False,
        icon="evm-ho.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1284,
        slip44=60,
        shortcut="GLMR",
        name="Moonbeam",
        rskip60=False,
        icon="evm-glmr.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1285,
        slip44=60,
        shortcut="MOVR",
        name="Moonriver",
        rskip60=False,
        icon="evm-movr.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1287,
        slip44=60,
        shortcut="DEV",
        name="Moonbase Alpha",
        rskip60=False,
        icon="evm-dev.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1618,
        slip44=60,
        shortcut="CATE",
        name="Catecoin Chain",
        rskip60=False,
        icon="evm-cate.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1657,
        slip44=60,
        shortcut="BTA",
        name="Btachain",
        rskip60=False,
        icon="evm-bta.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1856,
        slip44=60,
        shortcut="TSF",
        name="Teslafunds",
        rskip60=False,
        icon="evm-tsf.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1898,
        slip44=60,
        shortcut="BOY",
        name="BON Network",
        rskip60=False,
        icon="evm-boy.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1987,
        slip44=1987,
        shortcut="EGEM",
        name="EtherGem",
        rskip60=False,
        icon="evm-egem.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=2021,
        slip44=523,
        shortcut="EDG",
        name="Edgeware EdgeEVM",
        rskip60=False,
        icon="evm-edg.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=2025,
        slip44=1008,
        shortcut="RPG",
        name="Rangers Protocol",
        rskip60=False,
        icon="evm-rpg.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=2100,
        slip44=60,
        shortcut="ECO",
        name="Ecoball",
        rskip60=False,
        icon="evm-eco.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=2213,
        slip44=60,
        shortcut="EVA",
        name="Evanesco",
        rskip60=False,
        icon="evm-eva.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=2559,
        slip44=60,
        shortcut="KTO",
        name="Kortho",
        rskip60=False,
        icon="evm-kto.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=3400,
        slip44=60,
        shortcut="PRB",
        name="Paribu Net",
        rskip60=False,
        icon="evm-prb.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=3966,
        slip44=60,
        shortcut="DYNO",
        name="DYNO",
        rskip60=False,
        icon="evm-dyno.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=4689,
        slip44=60,
        shortcut="IOTX",
        name="IoTeX Network",
        rskip60=False,
        icon="evm-iotx.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=5197,
        slip44=60,
        shortcut="ES",
        name="EraSwap",
        rskip60=False,
        icon="evm-es.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=5315,
        slip44=60,
        shortcut="UZMI",
        name="Uzmi Network",
        rskip60=False,
        icon="evm-uzmi.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=5869,
        slip44=60,
        shortcut="RBD",
        name="Wegochain Rubidium",
        rskip60=False,
        icon="evm-rbd.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=6626,
        slip44=60,
        shortcut="PIX",
        name="Pixie Chain",
        rskip60=False,
        icon="evm-pix.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=8000,
        slip44=60,
        shortcut="TELE",
        name="Teleport",
        rskip60=False,
        icon="evm-tele.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=8217,
        slip44=8217,
        shortcut="KLAY",
        name="Klaytn",
        rskip60=False,
        icon="evm-klay.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=8723,
        slip44=479,
        shortcut="OLO",
        name="TOOL Global",
        rskip60=False,
        icon="evm-olo.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=8995,
        slip44=60,
        shortcut="U+25B3",
        name="bloxberg",
        rskip60=False,
        icon="evm-u+25b3.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=9001,
        slip44=60,
        shortcut="EVMOS",
        name="Evmos",
        rskip60=False,
        icon="evm-evmos.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=9100,
        slip44=60,
        shortcut="GNC",
        name="Genesis Coin",
        rskip60=False,
        icon="evm-gnc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=10101,
        slip44=60,
        shortcut="GEN",
        name="Blockchain Genesis",
        rskip60=False,
        icon="evm-gen.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=10823,
        slip44=60,
        shortcut="CCP",
        name="CryptoCoinPay",
        rskip60=False,
        icon="evm-ccp.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=11111,
        slip44=60,
        shortcut="WGM",
        name="WAGMI",
        rskip60=False,
        icon="evm-wgm.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=12052,
        slip44=621,
        shortcut="ZERO",
        name="Singularity ZERO",
        rskip60=False,
        icon="evm-zero.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=13381,
        slip44=60,
        shortcut="PHX",
        name="Phoenix",
        rskip60=False,
        icon="evm-phx.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=16000,
        slip44=60,
        shortcut="MTT",
        name="MetaDot",
        rskip60=False,
        icon="evm-mtt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=19845,
        slip44=60,
        shortcut="BTCIX",
        name="BTCIX Network",
        rskip60=False,
        icon="evm-btcix.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=24484,
        slip44=227,
        shortcut="WEB",
        name="Webchain",
        rskip60=False,
        icon="evm-web.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=24734,
        slip44=60,
        shortcut="MINTME",
        name="MintMe.com Coin",
        rskip60=False,
        icon="evm-mintme.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=31102,
        slip44=31102,
        shortcut="ESN",
        name="Ethersocial Network",
        rskip60=False,
        icon="evm-esn.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=32659,
        slip44=288,
        shortcut="FSN",
        name="Fusion",
        rskip60=False,
        icon="evm-fsn.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=39797,
        slip44=39797,
        shortcut="NRG",
        name="Energi",
        rskip60=False,
        icon="evm-nrg.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=42069,
        slip44=60,
        shortcut="peggle",
        name="pegglecoin",
        rskip60=False,
        icon="evm-peggle.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=42161,
        slip44=60,
        shortcut="ETH",
        name="Arbitrum One",
        rskip60=False,
        icon="evm-arb1.png",
        primary_color=0x28A0F0,
    )
    yield NetworkInfo(
        chain_id=42220,
        slip44=60,
        shortcut="CELO",
        name="Celo",
        rskip60=False,
        icon="evm-celo.png",
        primary_color=0x35D07F,
    )
    yield NetworkInfo(
        chain_id=43113,
        slip44=1,
        shortcut="tAVAX",
        name="Avalanche Fuji Testnet",
        rskip60=False,
        icon="evm-tavax.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=43114,
        slip44=9005,
        shortcut="AVAX",
        name="Avalanche C-Chain",
        rskip60=False,
        icon="evm-avax.png",
        primary_color=0xE84142,
    )
    yield NetworkInfo(
        chain_id=44787,
        slip44=1,
        shortcut="tCELO",
        name="Celo Alfajores Testnet",
        rskip60=False,
        icon="evm-tcelo.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=47805,
        slip44=60,
        shortcut="REI",
        name="REI Network",
        rskip60=False,
        icon="evm-rei.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=49797,
        slip44=1,
        shortcut="tNRG",
        name="Energi Testnet",
        rskip60=False,
        icon="evm-tnrg.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=53935,
        slip44=60,
        shortcut="JEWEL",
        name="DFK Chain",
        rskip60=False,
        icon="evm-jewel.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=62320,
        slip44=1,
        shortcut="tCELO",
        name="Celo Baklava Testnet",
        rskip60=False,
        icon="evm-tcelo.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=63000,
        slip44=60,
        shortcut="ECS",
        name="eCredits",
        rskip60=False,
        icon="evm-ecs.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=73799,
        slip44=1,
        shortcut="tVT",
        name="Energy Web Volta Testnet",
        rskip60=False,
        icon="evm-tvt.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=73927,
        slip44=60,
        shortcut="ETH",
        name="Mixin Virtual Machine",
        rskip60=False,
        icon="evm-mixin.png",
        primary_color=0x5959D8,
    )
    yield NetworkInfo(
        chain_id=78110,
        slip44=60,
        shortcut="FIN",
        name="Firenze test network",
        rskip60=False,
        icon="evm-fin.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=80001,
        slip44=1,
        shortcut="tMATIC",
        name="Mumbai",
        rskip60=False,
        icon="evm-tmatic.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=99999,
        slip44=60,
        shortcut="UBC",
        name="UB Smart Chain",
        rskip60=False,
        icon="evm-ubc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100000,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100001,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100002,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100003,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100004,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100005,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100006,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100007,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=100008,
        slip44=60,
        shortcut="QKC",
        name="QuarkChain",
        rskip60=False,
        icon="evm-qkc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=200625,
        slip44=200625,
        shortcut="AKA",
        name="Akroma",
        rskip60=False,
        icon="evm-aka.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=201018,
        slip44=60,
        shortcut="atp",
        name="Alaya",
        rskip60=False,
        icon="evm-atp.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=210425,
        slip44=60,
        shortcut="lat",
        name="PlatON",
        rskip60=False,
        icon="evm-lat.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=246529,
        slip44=246529,
        shortcut="ATS",
        name="ARTIS sigma1",
        rskip60=False,
        icon="evm-ats.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=246785,
        slip44=1,
        shortcut="tATS",
        name="ARTIS Testnet tau1",
        rskip60=False,
        icon="evm-tats.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=281121,
        slip44=60,
        shortcut="$OC",
        name="Social Smart Chain",
        rskip60=False,
        icon="evm-$oc.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=333999,
        slip44=60,
        shortcut="POLIS",
        name="Polis",
        rskip60=False,
        icon="evm-polis.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=888888,
        slip44=60,
        shortcut="VS",
        name="Vision",
        rskip60=False,
        icon="evm-vs.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=955305,
        slip44=1011,
        shortcut="ELV",
        name="Eluvio Content Fabric",
        rskip60=False,
        icon="evm-elv.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1313114,
        slip44=1313114,
        shortcut="ETHO",
        name="Etho Protocol",
        rskip60=False,
        icon="evm-etho.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1313500,
        slip44=60,
        shortcut="XERO",
        name="Xerom",
        rskip60=False,
        icon="evm-xero.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=7762959,
        slip44=184,
        shortcut="MUSIC",
        name="Musicoin",
        rskip60=False,
        icon="evm-music.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=13371337,
        slip44=60,
        shortcut="TPEP",
        name="PepChain Churchill",
        rskip60=False,
        icon="evm-tpep.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=18289463,
        slip44=60,
        shortcut="ILT",
        name="IOLite",
        rskip60=False,
        icon="evm-ilt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=20181205,
        slip44=60,
        shortcut="QKI",
        name="quarkblockchain",
        rskip60=False,
        icon="evm-qki.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=28945486,
        slip44=344,
        shortcut="AUX",
        name="Auxilium Network",
        rskip60=False,
        icon="evm-aux.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=35855456,
        slip44=60,
        shortcut="JOYS",
        name="Joys Digital",
        rskip60=False,
        icon="evm-joys.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=61717561,
        slip44=61717561,
        shortcut="AQUA",
        name="Aquachain",
        rskip60=False,
        icon="evm-aqua.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=99415706,
        slip44=1,
        shortcut="TOYS",
        name="Joys Digital TestNet",
        rskip60=False,
        icon="evm-toys.png",
        primary_color=0x969696,
    )
    yield NetworkInfo(
        chain_id=245022934,
        slip44=60,
        shortcut="NEON",
        name="Neon EVM",
        rskip60=False,
        icon="evm-neon.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=311752642,
        slip44=60,
        shortcut="OLT",
        name="OneLedger",
        rskip60=False,
        icon="evm-olt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1122334455,
        slip44=60,
        shortcut="IPOS",
        name="IPOS Network",
        rskip60=False,
        icon="evm-ipos.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1313161554,
        slip44=60,
        shortcut="ETH",
        name="Aurora",
        rskip60=False,
        icon="evm-aurora.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1666600000,
        slip44=60,
        shortcut="ONE",
        name="Harmony",
        rskip60=False,
        icon="evm-one.png",
        primary_color=0x33D3D5,
    )
    yield NetworkInfo(
        chain_id=1666600001,
        slip44=60,
        shortcut="ONE",
        name="Harmony",
        rskip60=False,
        icon="evm-one.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1666600002,
        slip44=60,
        shortcut="ONE",
        name="Harmony",
        rskip60=False,
        icon="evm-one.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=1666600003,
        slip44=60,
        shortcut="ONE",
        name="Harmony",
        rskip60=False,
        icon="evm-one.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=2021121117,
        slip44=60,
        shortcut="HOP",
        name="DataHopper",
        rskip60=False,
        icon="evm-hop.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=3125659152,
        slip44=164,
        shortcut="PIRL",
        name="Pirl",
        rskip60=False,
        icon="evm-pirl.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=11297108109,
        slip44=60,
        shortcut="PALM",
        name="Palm",
        rskip60=False,
        icon="evm-palm.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=197710212030,
        slip44=60,
        shortcut="NTT",
        name="Ntity",
        rskip60=False,
        icon="evm-ntt.png",
        primary_color=0xD2D2D2,
    )
    yield NetworkInfo(
        chain_id=6022140761023,
        slip44=60,
        shortcut="MOLE",
        name="Molereum Network",
        rskip60=False,
        icon="evm-mole.png",
        primary_color=0xD2D2D2,
    )
