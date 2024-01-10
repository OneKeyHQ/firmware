from typing import TYPE_CHECKING

from ._wallet_contract import SendModeEnum, WalletContract
from ._wallet_contract_v3 import WalletV3ContractR1, WalletV3ContractR2
from ._wallet_contract_v4 import WalletV4ContractR1, WalletV4ContractR2

if TYPE_CHECKING:
    from enum import Enum
else:
    Enum = object


class WalletVersionEnum(str, Enum):
    v3r1 = "v3r1"
    v3r2 = "v3r2"
    v4r1 = "v4r1"
    v4r2 = "v4r2"


class Wallets:
    default_version = WalletVersionEnum.v3r2
    ALL = {
        WalletVersionEnum.v3r1: WalletV3ContractR1,
        WalletVersionEnum.v3r2: WalletV3ContractR2,
        WalletVersionEnum.v4r1: WalletV4ContractR1,
        WalletVersionEnum.v4r2: WalletV4ContractR2,
    }


__all__ = [
    "WalletV3ContractR1",
    "WalletV3ContractR2",
    "WalletV4ContractR1",
    "WalletV4ContractR2",
    "WalletContract",
    "SendModeEnum",
    "WalletVersionEnum",
    "Wallets",
]
