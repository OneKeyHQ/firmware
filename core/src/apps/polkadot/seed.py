from typing import TYPE_CHECKING

from trezor import wire
from trezor.crypto import bip32, cardano

from apps.common.seed import get_seed

from . import paths

if TYPE_CHECKING:
    from typing import Callable, Awaitable, TypeVar

    from apps.common.paths import Bip32Path
    from apps.common.keychain import MsgOut, Handler

    from trezor.messages import (
        PolkadotGetAddress,
        PolkadotSignTx,
    )

    PolkadotMessages = PolkadotGetAddress | PolkadotSignTx
    MsgIn = TypeVar("MsgIn", bound=PolkadotMessages)

    HandlerWithKeychain = Callable[[wire.Context, MsgIn, "Keychain"], Awaitable[MsgOut]]


class Keychain:
    def __init__(self, root: bip32.HDNode) -> None:
        self.byron_root = self._derive_path(root, paths.BYRON_ROOT)
        root.__del__()

    @staticmethod
    def _derive_path(root: bip32.HDNode, path: Bip32Path) -> bip32.HDNode:
        """Clone and derive path from the root."""
        node = root.clone()
        node.derive_path(path)
        return node

    def verify_path(self, path: Bip32Path) -> None:
        if not self.is_in_keychain(path):
            raise wire.DataError("Forbidden key path")

    def _get_path_root(self, path: Bip32Path) -> bip32.HDNode:
        if is_byron_path(path):
            return self.byron_root
        else:
            raise wire.DataError("Forbidden key path")

    def is_in_keychain(self, path: Bip32Path) -> bool:
        return is_byron_path(path)

    def derive(self, node_path: Bip32Path) -> bip32.HDNode:
        self.verify_path(node_path)
        path_root = self._get_path_root(node_path)
        suffix = node_path[len(paths.BYRON_ROOT) :]
        # derive child node from the root
        return self._derive_path(path_root, suffix)

    # XXX the root node remains in session cache so we should not delete it
    # def __del__(self) -> None:
    #     self.root.__del__()


def is_byron_path(path: Bip32Path) -> bool:
    return path[: len(paths.BYRON_ROOT)] == paths.BYRON_ROOT


async def get_keychain(ctx: wire.Context) -> Keychain:
    seed = await get_seed(ctx)
    return Keychain(cardano.from_seed_ledger(seed))


def with_keychain(func: HandlerWithKeychain[MsgIn, MsgOut]) -> Handler[MsgIn, MsgOut]:
    async def wrapper(ctx: wire.Context, msg: MsgIn) -> MsgOut:
        keychain = await get_keychain(ctx)
        return await func(ctx, msg, keychain)

    return wrapper
