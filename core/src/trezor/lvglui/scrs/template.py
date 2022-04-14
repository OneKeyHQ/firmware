from . import *
from .common import FullSizeWindow
from .components.container import ContainerFlexCol
from .components.listitem import DisplayItem
from .components.qrcode import QRCode


class Address(FullSizeWindow):
    def __init__(self, title, path, address, xpubs=[], multisig_index=0):
        super().__init__(title, None, confirm_text="Done")
        self.qr = QRCode(self, address)
        self.qr.align_to(self.title, lv.ALIGN.OUT_BOTTOM_MID, 0, 48)
        self.container = ContainerFlexCol(self, self.qr)
        self.item1 = DisplayItem(self.container, "PATH:", path)
        self.item2 = DisplayItem(self.container, "ADDRESS:", address)
        for i, xpub in enumerate(xpubs):
            self.item3 = DisplayItem(
                self.container,
                f"XPUB #{i+1}{'mine' if i == multisig_index else 'cosigner'}",
                xpub,
            )
        self.container.set_style_max_height(480, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.container.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)


class XpubOrPub(FullSizeWindow):
    def __init__(self, title, path, xpub=None, pubkey=None):
        super().__init__(title, None, "Export", "Cancel")
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, "PATH:", path)
        self.item2 = DisplayItem(
            self.container, "XPUB:" if xpub else "PUBLIC KEY:", xpub or pubkey
        )


class Message(FullSizeWindow):
    def __init__(self, title, address, message):
        super().__init__(title, None, "Sign", "Cancel")
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, "ADDRESS:", address)
        self.item2 = DisplayItem(self.container, "MESSAGE:", message)
        self.item2.set_style_max_height(300, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.item2.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)


class TransactionOverview(FullSizeWindow):
    def __init__(self, title, amount, address):
        super().__init__(title, None, "Continue", "Cancel")
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, f"SEND #ffffff {amount}# TO:", address)
        self.item1.label_top.set_recolor(True)


class TransactionDetailsETH(FullSizeWindow):
    def __init__(
        self,
        title,
        address_from,
        address_to,
        amount,
        fee_max,
        is_eip1559=False,
        gas_price=None,
        max_priority_fee_per_gas=None,
        max_fee_per_gas=None,
        total_amount=None,
    ):
        super().__init__(title, None, "Continue", "Cancel")
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, "AMOUNT:", amount)
        if not is_eip1559:
            self.item2 = DisplayItem(self.container, "GAS PRICE:", gas_price)
        else:
            self.item2 = DisplayItem(
                self.container, "PRIORITY FEE PER GAS:", max_priority_fee_per_gas
            )
            self.item2_1 = DisplayItem(
                self.container, "MAXIMUM FEE PER GAS:", max_fee_per_gas
            )
        self.item3 = DisplayItem(self.container, "MAXIMUM FEE:", fee_max)
        self.item4 = DisplayItem(self.container, "TO:", address_to)
        self.item5 = DisplayItem(self.container, "FROM:", address_from)
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item6 = DisplayItem(self.container, "TOTAL AMOUNT:", total_amount)
        self.container.set_style_max_height(480, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.container.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)


class ContractDataOverview(FullSizeWindow):
    def __init__(self, title, description, data):
        super().__init__(title, None, "Continue", "Cancel")
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, "SIZE:", description)
        self.item2 = DisplayItem(self.container, "DATA:", data)
        self.item2.set_style_max_height(400, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.item2.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)


class BlobDisPlay(FullSizeWindow):
    def __init__(
        self,
        title,
        description: str,
        content: str,
        icon_path: str = "images/shriek.png",
    ):
        super().__init__(title, None, "Continue", "Cancel", icon_path=icon_path)
        self.container = ContainerFlexCol(self, self.title, pos=(0, 48))
        self.item1 = DisplayItem(self.container, description, content)
        self.item1.set_style_max_height(400, lv.PART.MAIN | lv.STATE.DEFAULT)
        self.item1.set_scrollbar_mode(lv.SCROLLBAR_MODE.AUTO)
