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
% for n in sorted(supported_on("trezor2", eth), key=lambda network: (int(network.chain_id), network.name)):
    yield (
        ${n.chain_id},  # chain_id
        ${n.slip44},  # slip44
        "${n.shortcut}",  # symbol
        "${n.name}",  # name
        "${n.icon}",  # name
        ${n.primary_color},  # primary_color
    )
% endfor
    yield (
        9798,  # chain_id
        9798,  # slip44
        "DTT",  # symbol
        "Data Trade Chain",  # name
        "evm-dtt.png",  # icon
        0x1A2A5F,  # primary_color
    )
