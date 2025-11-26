from typing import Iterator


class ProviderInfo:
    def __init__(
        self,
        name: str,
        address: str,
        icon_path: str,
    ):
        self.name = name
        self.address = address
        self.icon_path = icon_path


def provider_by_address(address: str) -> ProviderInfo | None:
    for name, provider_addr, icon_path in _provider_iterator():
        if address == provider_addr:
            return ProviderInfo(name, provider_addr, icon_path)
    return None


def _provider_iterator() -> Iterator[tuple[str, str, str]]:
    # name, address, icon_path
    yield ("TrxRes", "TLgTYgG5bD9crpYqVED3MGvWuAUvdoFYEa", "A:/res/provider-trxres.png")
