from typing import TYPE_CHECKING
from ubinascii import hexlify

from trezor import ui
from trezor.enums import ButtonRequestType
from trezor.strings import format_amount
from trezor.ui.layouts import confirm_output

from . import codec, helper

if TYPE_CHECKING:
    from trezor.wire import Context


class Transaction:
    def __init__(self) -> None:
        self.addressType = 0
        self.era_period = 0
        self.era_phase = 0
        self.nonce = 0
        self.tip = 0

    async def layout(
        self, ctx: Context, sender: str, chain_name: str, symbol: str, decimal: int
    ) -> None:
        return None

    @staticmethod
    def _readAccountIdLookupOfT_V15(
        rawtx: codec.base.ScaleBytes, address_type, skip_type_lookup: bool = False
    ) -> str:
        if not skip_type_lookup:
            value = rawtx.get_next_bytes(1)[0]
        else:
            value = 0
        if value == 0:
            accountid = helper.ss58_encode(rawtx.get_next_bytes(32), address_type)
        elif value == 1:
            obj = codec.types.Compact(rawtx)
            accountid = str(obj.decode(check_remaining=False))
        elif value == 2:
            obj = codec.types.Compact(rawtx)
            value = obj.decode(check_remaining=False)
            clen = int(0 if value is None else value)
            accountid = hexlify(rawtx.get_next_bytes(clen)).decode()
        elif value == 3:
            accountid = hexlify(rawtx.get_next_bytes(32)).decode()
        elif value == 4:
            accountid = hexlify(rawtx.get_next_bytes(20)).decode()
        else:
            raise Exception("Unexpected value")

        return accountid

    @staticmethod
    def deserialize_polkadot(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx in (1287, 1280, 2560):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 0)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(desc, balance)
        elif callPrivIdx == (1282, 2562):
            source = Transaction._readAccountIdLookupOfT_V15(rawtx, 0)
            dest = Transaction._readAccountIdLookupOfT_V15(rawtx, 0)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == (1283, 2563):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 0)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(desc, balance)
        elif callPrivIdx == (1284, 2564):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 0)
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(desc, keep_alive)

        return tx

    @staticmethod
    def deserialize_kusama(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx in (1031, 1024, 2560):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 2)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(desc, balance)
        elif callPrivIdx == (1026, 2562):
            source = Transaction._readAccountIdLookupOfT_V15(rawtx, 2)
            dest = Transaction._readAccountIdLookupOfT_V15(rawtx, 2)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == (1027, 2563):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 2)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(desc, balance)
        elif callPrivIdx == (1028, 2564):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 2)
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(desc, keep_alive)

        return tx

    @staticmethod
    def deserialize_westend(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx == 1024:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 42)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(desc, balance)
        elif callPrivIdx == 1026:
            source = Transaction._readAccountIdLookupOfT_V15(rawtx, 42)
            dest = Transaction._readAccountIdLookupOfT_V15(rawtx, 42)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == 1027:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 42)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(desc, balance)
        elif callPrivIdx == 1028:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 42)
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(desc, keep_alive)

        return tx

    @staticmethod
    def deserialize_astar(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx in (7943, 7936):
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 5)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(desc, balance)
        elif callPrivIdx == 7938:
            source = Transaction._readAccountIdLookupOfT_V15(rawtx, 5)
            dest = Transaction._readAccountIdLookupOfT_V15(rawtx, 5)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == 7939:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 5)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(desc, balance)
        elif callPrivIdx == 7940:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, 5)
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(desc, keep_alive)

        return tx

    @staticmethod
    def deserialize_joy(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx == 1280:
            dest = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 126, skip_type_lookup=True
            )
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(dest, balance)
        elif callPrivIdx == 1282:
            source = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 126, skip_type_lookup=True
            )
            dest = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 126, skip_type_lookup=True
            )
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == 1283:
            dest = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 126, skip_type_lookup=True
            )
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(dest, balance)
        elif callPrivIdx == 1284:
            dest = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 126, skip_type_lookup=True
            )
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(dest, keep_alive)

        return tx

    @staticmethod
    def deserialize_manta(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int, address_type: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx == 2560:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, address_type)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(desc, balance)
        elif callPrivIdx == 2562:
            source = Transaction._readAccountIdLookupOfT_V15(rawtx, address_type)
            dest = Transaction._readAccountIdLookupOfT_V15(rawtx, address_type)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == 2563:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, address_type)
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(desc, balance)
        elif callPrivIdx == 2564:
            desc = Transaction._readAccountIdLookupOfT_V15(rawtx, address_type)
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(desc, keep_alive)

        return tx

    @staticmethod
    def deserialize_hydration(
        rawtx: codec.base.ScaleBytes, callPrivIdx: int
    ) -> "Transaction":
        tx = TransactionUnknown(rawtx)
        if callPrivIdx == 1792:
            desc = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 0, skip_type_lookup=True
            )
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransfer(desc, balance)
        elif callPrivIdx == 1794:
            source = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 0, skip_type_lookup=True
            )
            dest = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 0, skip_type_lookup=True
            )
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesForceTransfer(source, dest, balance)
        elif callPrivIdx == 1795:
            desc = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 0, skip_type_lookup=True
            )
            obj = codec.types.Compact(rawtx)
            balance = obj.decode(check_remaining=False)
            tx = BalancesTransferKeepAlive(desc, balance)
        elif callPrivIdx == 1796:
            desc = Transaction._readAccountIdLookupOfT_V15(
                rawtx, 0, skip_type_lookup=True
            )
            keep_alive = rawtx.get_next_bytes(1)[0]
            tx = BalancesTransferAll(desc, keep_alive)

        return tx

    @staticmethod
    def deserialize(raw_tx: bytes, network: str) -> "Transaction":
        rawtx = codec.base.ScaleBytes(raw_tx)
        moduleIdx = rawtx.get_next_bytes(1)
        idx = rawtx.get_next_bytes(1)
        callPrivIdx = (moduleIdx[0] << 8) + idx[0]
        tx = Transaction()
        if network == "polkadot":
            tx = Transaction.deserialize_polkadot(rawtx, callPrivIdx)
        elif network == "kusama":
            tx = Transaction.deserialize_kusama(rawtx, callPrivIdx)
        elif network == "westend":
            tx = Transaction.deserialize_westend(rawtx, callPrivIdx)
        elif network == "astar":
            tx = Transaction.deserialize_astar(rawtx, callPrivIdx)
        elif network == "joystream":
            tx = Transaction.deserialize_joy(rawtx, callPrivIdx)
        elif network in ("manta", "bifrost", "bifrost-ksm"):
            if network == "manta":
                address_type = 77
            else:
                address_type = 0
            tx = Transaction.deserialize_manta(rawtx, callPrivIdx, address_type)
        elif network == "hydration":
            tx = Transaction.deserialize_hydration(rawtx, callPrivIdx)
        else:
            tx = TransactionUnknown(rawtx)

        if type(tx) == TransactionUnknown:
            return tx

        # Era
        obj = codec.types.Era(rawtx)
        obj.decode(check_remaining=False)
        tx.era_period = obj.period if obj.period is not None else 0
        tx.era_phase = obj.phase if obj.phase is not None else 0

        # None
        obj = codec.types.Compact(rawtx)
        nonce = obj.decode(check_remaining=False)
        tx.nonce = nonce if nonce is not None else 0

        # Tip
        obj = codec.types.Compact(rawtx)
        tip = obj.decode(check_remaining=False)
        tx.tip = tip if tip is not None else 0

        # optional: assetId(asset location if assetId is not equal to 0)
        # optional: mode

        return tx


class TransactionUnknown(Transaction):
    def __init__(
        self,
        data,
    ):
        Transaction.__init__(self)
        self.data = data

    async def layout(
        self, ctx: Context, sender: str, chain_name: str, symbol: str, decimal: int
    ) -> None:
        from trezor.ui.layouts.lvgl import confirm_blind_sign_common

        await confirm_blind_sign_common(ctx, sender, self.data.data)


class BalancesTransfer(Transaction):
    def __init__(
        self,
        dest,
        balance,
    ):
        Transaction.__init__(self)
        self.dest = dest
        self.balance = balance

    async def layout(
        self, ctx: Context, sender: str, chain_name: str, symbol: str, decimal: int
    ) -> None:
        from trezor.ui.layouts import confirm_polkadot_balances

        balance = self.balance if self.balance is not None else 0
        await confirm_output(
            ctx,
            address=self.dest,
            amount=f"{format_amount(balance, decimal)} {symbol}",
            font_amount=ui.BOLD,
            color_to=ui.GREY,
            br_code=ButtonRequestType.SignTx,
        )
        await confirm_polkadot_balances(
            ctx,
            chain_name=chain_name,
            module="Balances",
            method="Transfer",
            sender=sender,
            dest=self.dest,
            source=None,
            balance=f"{format_amount(balance, decimal)} {symbol}",
            tip=f"{format_amount(self.tip, decimal)} {symbol}"
            if self.tip != 0
            else None,
            keep_alive=None,
        )


class BalancesForceTransfer(Transaction):
    def __init__(
        self,
        source,
        dest,
        balance,
    ):
        Transaction.__init__(self)
        self.source = source
        self.dest = dest
        self.balance = balance

    async def layout(
        self, ctx: Context, sender: str, chain_name: str, symbol: str, decimal: int
    ) -> None:
        from trezor.ui.layouts import confirm_polkadot_balances

        balance = self.balance if self.balance is not None else 0
        await confirm_output(
            ctx,
            address=self.dest,
            amount=f"{format_amount(balance, decimal)} {symbol}",
            font_amount=ui.BOLD,
            color_to=ui.GREY,
            br_code=ButtonRequestType.SignTx,
        )
        await confirm_polkadot_balances(
            ctx,
            chain_name=chain_name,
            module="Balances",
            method="ForceTransfer",
            sender=sender,
            dest=self.dest,
            source=self.source,
            balance=f"{format_amount(balance, decimal)} {symbol}",
            tip=f"{format_amount(self.tip, decimal)} {symbol}"
            if self.tip != 0
            else None,
            keep_alive=None,
        )


class BalancesTransferKeepAlive(Transaction):
    def __init__(
        self,
        dest,
        balance,
    ):
        Transaction.__init__(self)
        self.dest = dest
        self.balance = balance

    async def layout(
        self, ctx: Context, sender: str, chain_name: str, symbol: str, decimal: int
    ) -> None:
        from trezor.ui.layouts import confirm_polkadot_balances

        balance = self.balance if self.balance is not None else 0
        await confirm_output(
            ctx,
            address=self.dest,
            amount=f"{format_amount(self.balance, decimal)} {symbol}",
            font_amount=ui.BOLD,
            color_to=ui.GREY,
            br_code=ButtonRequestType.SignTx,
        )
        await confirm_polkadot_balances(
            ctx,
            chain_name=chain_name,
            module="Balances",
            method="TransferKeepAlive",
            sender=sender,
            dest=self.dest,
            source=None,
            balance=f"{format_amount(balance, decimal)} {symbol}",
            tip=f"{format_amount(self.tip, decimal)} {symbol}"
            if self.tip != 0
            else None,
            keep_alive=None,
        )


class BalancesTransferAll(Transaction):
    def __init__(
        self,
        dest,
        keepAlive,
    ):
        Transaction.__init__(self)
        self.dest = dest
        self.keepAlive = keepAlive

    async def layout(
        self, ctx: Context, sender: str, chain_name: str, symbol: str, decimal: int
    ) -> None:
        from trezor.ui.layouts import confirm_polkadot_balances

        if self.keepAlive == 0:
            keep_alive = "False"
        else:
            keep_alive = "True"
        await confirm_output(
            ctx,
            address=self.dest,
            amount="all",
            font_amount=ui.BOLD,
            color_to=ui.GREY,
            br_code=ButtonRequestType.SignTx,
        )
        await confirm_polkadot_balances(
            ctx,
            chain_name=chain_name,
            module="Balances",
            method="TransferAll",
            sender=sender,
            dest=self.dest,
            source=None,
            balance=None,
            tip=f"{format_amount(self.tip, decimal)} {symbol}"
            if self.tip != 0
            else None,
            keep_alive=keep_alive,
        )
