from trezor import utils
from trezor.lvglui.scrs.components.button import NormalButton
from trezor.lvglui.scrs.components.pageable import PageAbleMessage

from ..i18n import gettext as _, keys as i18n_keys
from ..lv_colors import lv_colors
from . import font_PJSBOLD36, font_PJSBOLD48, font_STATUS_BAR
from .common import FullSizeWindow, lv
from .components.container import ContainerFlexCol
from .components.listitem import DisplayItem
from .components.qrcode import QRCode
from .widgets.style import StyleWrapper


class Address(FullSizeWindow):
    class SHOW_TYPE:
        ADDRESS = 0
        QRCODE = 1

    def __init__(
        self,
        title,
        path,
        address,
        primary_color,
        icon_path: str,
        xpubs=None,
        multisig_index: int | None = 0,
    ):
        super().__init__(
            title,
            address,
            confirm_text=_(i18n_keys.BUTTON__DONE),
            cancel_text=_(i18n_keys.BUTTON__QRCODE),
            anim_dir=2,
            primary_color=primary_color,
        )
        self.path = path
        self.xpubs = xpubs
        self.multisig_index = multisig_index
        self.address = address
        self.icon = icon_path
        if primary_color:
            self.title.add_style(StyleWrapper().text_color(primary_color), 0)
        if __debug__:
            self.subtitle.add_style(
                StyleWrapper().text_font(font_PJSBOLD36).text_color(lv_colors.WHITE), 0
            )
        else:
            self.subtitle.add_style(
                StyleWrapper().text_font(font_PJSBOLD48).text_color(lv_colors.WHITE), 0
            )
        self.subtitle.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 48)
        self.show_address()

    def show_address(self):
        self.current = self.SHOW_TYPE.ADDRESS
        if hasattr(self, "qr"):
            self.qr.delete()
            del self.qr
        self.btn_no.label.set_text(_(i18n_keys.BUTTON__QRCODE))
        self.container = ContainerFlexCol(
            self.content_area, self.subtitle, pos=(0, 16), padding_row=8
        )
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__PATH__COLON), self.path
        )
        # self.item2 = DisplayItem(
        #     self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        # )
        # if xpubs:
        #     self.title.align(lv.ALIGN.TOP_MID, 0, 100)
        #     self.btn_yes.align(lv.ALIGN.BOTTOM_MID, 0, -30)
        self.current = 0
        for i, xpub in enumerate(self.xpubs or []):
            self.item3 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__XPUB_STR_MINE__COLON).format(i + 1)
                if i == self.multisig_index
                else _(i18n_keys.LIST_KEY__XPUB_STR_COSIGNER__COLON).format(i + 1),
                xpub,
            )

    def show_qr_code(self):
        self.current = self.SHOW_TYPE.QRCODE
        if hasattr(self, "container"):
            self.container.delete()
            del self.container
        self.btn_no.label.set_text(_(i18n_keys.BUTTON__ADDRESS))
        self.qr = QRCode(self.content_area, self.address, self.icon)
        self.qr.align_to(self.title, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            utils.lcd_resume()
            if target == self.btn_no:
                if self.current == self.SHOW_TYPE.ADDRESS:
                    self.show_qr_code()
                else:
                    self.show_address()
            elif target == self.btn_yes:
                self.show_unload_anim()
                self.channel.publish(1)


class XpubOrPub(FullSizeWindow):
    def __init__(
        self, title, path, primary_color, icon_path: str, xpub=None, pubkey=None
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__EXPORT),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
            icon_path=icon_path,
            primary_color=primary_color,
        )
        self.title.add_style(StyleWrapper().text_color(primary_color), 0)
        self.container = ContainerFlexCol(
            self.content_area, self.title, pos=(0, 40), padding_row=16
        )
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__PATH__COLON), path
        )
        self.item2 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__XPUB__COLON)
            if xpub
            else _(i18n_keys.LIST_KEY__PUBLIC_KEY__COLON),
            xpub or pubkey,
        )


class Message(FullSizeWindow):
    def __init__(
        self, title, address, message, primary_color, icon_path, verify: bool = False
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__VERIFY) if verify else _(i18n_keys.BUTTON__SIGN),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.primary_color = primary_color
        self.container = ContainerFlexCol(
            self.content_area, self.title, pos=(0, 40), padding_row=8
        )
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        )
        self.long_message = False
        if len(message) > 80:
            # self.show_full_message = NormalButton(
            #     self, _(i18n_keys.BUTTON__VIEW_FULL_MESSAGE)
            # )
            # self.show_full_message.align_to(self.item2, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
            # self.show_full_message.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
            self.message = message
            self.long_message = True
            self.btn_yes.label.set_text(_(i18n_keys.BUTTON__VIEW))
        else:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__MESSAGE__COLON), message
            )

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                if self.long_message:
                    PageAbleMessage(
                        _(i18n_keys.LIST_KEY__MESSAGE__COLON)[:-1],
                        self.message,
                        self.channel,
                        primary_color=self.primary_color,
                        confirm_text=_(i18n_keys.BUTTON__SIGN),
                    )
                    self.destroy()
                else:
                    self.show_unload_anim()
                    self.channel.publish(1)
            elif target == self.btn_no:
                self.show_dismiss_anim()
                self.channel.publish(0)


class TransactionOverview(FullSizeWindow):
    def __init__(self, title, amount, address, primary_color, icon_path):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__VIEW),
            _(i18n_keys.BUTTON__REJECT),
            anim_dir=2,
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container,
            f"#878787 {_(i18n_keys.INSERT__SEND)}#  {amount}  #878787 {_(i18n_keys.INSERT__TO)}#",
            address,
        )
        self.item1.label_top.set_recolor(True)
        self.item1.set_style_bg_color(lv_colors.BLACK, 0)


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
        primary_color=lv_colors.ONEKEY_GREEN,
        contract_addr=None,
        token_id=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        if contract_addr:
            self.item0 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CONTRACT_ADDRESS__COLON),
                contract_addr,
            )
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__TOKEN_ID__COLON), token_id
            )
        else:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
            )
        if not is_eip1559:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__GAS_PRICE__COLON), gas_price
            )
        else:
            self.item2 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__PRIORITY_FEE_PER_GAS__COLON),
                max_priority_fee_per_gas,
            )
            self.item2_1 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__MAXIMUM_FEE_PER_GAS__COLON),
                max_fee_per_gas,
            )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if total_amount is None:
            if not contract_addr:
                total_amount = f"{amount}\n{fee_max}"
            else:
                total_amount = f"{fee_max}"
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total_amount
        )


class ContractDataOverview(FullSizeWindow):
    def __init__(self, title, description, data, primary_color):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.primary_color = primary_color
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIZE__COLON), description
        )
        self.long_data = False
        if len(data) > 80:
            self.data = data
            self.long_data = True
            self.btn_yes.label.set_text(_(i18n_keys.BUTTON__VIEW))
        else:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__DATA__COLON), data
            )

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                if self.long_data:
                    PageAbleMessage(
                        _(i18n_keys.TITLE__VIEW_DATA),
                        self.data,
                        self.channel,
                        primary_color=self.primary_color,
                    )
                    self.destroy()
                else:
                    self.show_unload_anim()
                    self.channel.publish(1)
            elif target == self.btn_no:
                self.show_dismiss_anim()
                self.channel.publish(0)


class BlobDisPlay(FullSizeWindow):
    def __init__(
        self,
        title,
        description: str,
        content: str,
        icon_path: str = "A:/res/warning.png",
        anim_dir: int = 1,
        primary_color=lv_colors.ONEKEY_GREEN,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            icon_path=icon_path,
            anim_dir=anim_dir,
            primary_color=primary_color or lv_colors.ONEKEY_GREEN,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(self.container, description, content)
        self.long_message = False
        if len(content) > 240:
            self.long_message = True
            self.btn_yes.label.set_text(_(i18n_keys.BUTTON__VIEW))
            self.data = content

    def eventhandler(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.btn_yes:
                if self.long_message:
                    PageAbleMessage(
                        _(i18n_keys.LIST_KEY__MESSAGE__COLON)[:-1],
                        self.data,
                        self.channel,
                    )
                    self.destroy()
                else:
                    self.show_unload_anim()
                    self.channel.publish(1)
            elif target == self.btn_no:
                self.show_dismiss_anim()
                self.channel.publish(0)


class ConfirmMetaData(FullSizeWindow):
    def __init__(self, title, subtitle, description, data, primary_color):
        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        if description:
            self.container = ContainerFlexCol(
                self.content_area, self.subtitle, pos=(0, 40)
            )
            self.item1 = DisplayItem(self.container, description, data)


class TransactionDetailsBTC(FullSizeWindow):
    def __init__(self, title: str, amount: str, fee: str, total: str, primary_color):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee)
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total
        )


class JointTransactionDetailsBTC(FullSizeWindow):
    def __init__(self, title: str, amount: str, total: str, primary_color):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT_YOU_SPEND__COLON), amount
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total
        )


class ModifyFee(FullSizeWindow):
    def __init__(self, description: str, fee_change: str, fee_new: str, primary_color):
        super().__init__(
            _(i18n_keys.TITLE__MODIFY_FEE),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(self.container, description, fee_change)
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__NEW_FEE__COLON), fee_new
        )


class ModifyOutput(FullSizeWindow):
    def __init__(
        self,
        address: str,
        description: str,
        amount_change: str,
        amount_new: str,
        primary_Color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__MODIFY_AMOUNT),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_Color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        )
        self.item2 = DisplayItem(self.container, description, amount_change)
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__NEW_AMOUNT__COLON), amount_new
        )


class ConfirmReplacement(FullSizeWindow):
    def __init__(self, title: str, txid: str, primary_color):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TRANSACTION_ID__COLON), txid
        )


class ConfirmPaymentRequest(FullSizeWindow):
    def __init__(self, title: str, subtitle, amount: str, to_addr: str, primary_color):
        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )


class ConfirmDecredSstxSubmission(FullSizeWindow):
    def __init__(
        self, title: str, subtitle: str, amount: str, to_addr: str, primary_color
    ):
        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, -(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )


class ConfirmCoinJoin(FullSizeWindow):
    def __init__(
        self,
        title: str,
        coin_name: str,
        max_rounds: str,
        max_fee_per_vbyte: str,
        primary_color,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__COIN_NAME__COLON), coin_name
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_ROUNDS__COLON), max_rounds
        )
        self.item3 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__MAXIMUM_MINING_FEE__COLON),
            max_fee_per_vbyte,
        )


class ConfirmSignIdentity(FullSizeWindow):
    def __init__(self, title: str, identity: str, subtitle: str | None, primary_color):
        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        align_base = self.title if subtitle is None else self.subtitle
        self.container = ContainerFlexCol(self.content_area, align_base, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__IDENTITY__COLON), identity
        )


class ConfirmProperties(FullSizeWindow):
    def __init__(self, title: str, properties: list[tuple[str, str]], primary_color):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        for key, value in properties:
            self.item = DisplayItem(self.container, f"{key.upper()}", value)


class ConfirmTransferBinance(FullSizeWindow):
    def __init__(self, items: list[tuple[str, str, str]], primary_color, icon_path):
        super().__init__(
            _(i18n_keys.TITLE__CONFIRM_TRANSFER),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        for key, value, address in items:
            self.item1 = DisplayItem(self.container, key, "")
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), value
            )
            self.item3 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__TO__COLON), address
            )


class ShouldShowMore(FullSizeWindow):
    def __init__(
        self, title: str, key: str, value: str, button_text: str, primary_color
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item = DisplayItem(self.container, f"{key}:", value)
        self.show_more = NormalButton(self.content_area, button_text)
        self.show_more.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
        self.show_more.add_event_cb(self.on_show_more, lv.EVENT.CLICKED, None)

    def on_show_more(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.show_more:
                # 2 means show more
                self.channel.publish(2)
                self.destroy()


class EIP712DOMAIN(FullSizeWindow):
    def __init__(self, title: str, primary_color, icon_path, **kwargs):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        if kwargs.get("name"):
            self.item1 = DisplayItem(
                self.container, "name (string):", kwargs.get("name")
            )
        if kwargs.get("version"):
            self.item2 = DisplayItem(
                self.container, "version (string):", kwargs.get("version")
            )
        if kwargs.get("chainId"):
            self.item3 = DisplayItem(
                self.container, "chainId (uint256):", kwargs.get("chainId")
            )
        if kwargs.get("verifyingContract"):
            self.item4 = DisplayItem(
                self.container,
                "verifyingContract (address):",
                kwargs.get("verifyingContract"),
            )
        if kwargs.get("salt"):
            self.item5 = DisplayItem(
                self.container, "salt (bytes32):", kwargs.get("salt")
            )


class TransactionDetailsTRON(FullSizeWindow):
    def __init__(
        self,
        title,
        address_from,
        address_to,
        amount,
        fee_max,
        primary_color,
        total_amount=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total_amount
        )


class SecurityCheck(FullSizeWindow):
    def __init__(self):
        super().__init__(
            title=_(i18n_keys.TITLE__SECURITY_CHECK),
            subtitle=_(i18n_keys.SUBTITLE__SECURITY_CHECK),
            confirm_text=_(i18n_keys.BUTTON__CONFIRM),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            icon_path="A:/res/security-check.png",
            anim_dir=2,
        )


class PassphraseDisplayConfirm(FullSizeWindow):
    def __init__(self, passphrase: str):
        super().__init__(
            title=_(i18n_keys.TITLE__USE_THIS_PASSPHRASE),
            subtitle=_(i18n_keys.SUBTITLE__USE_THIS_PASSPHRASE),
            confirm_text=_(i18n_keys.BUTTON__CONFIRM),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            anim_dir=0,
        )

        self.panel = lv.obj(self.content_area)
        self.panel.remove_style_all()
        self.panel.set_size(464, 264)
        self.panel.align_to(self.subtitle, lv.ALIGN.OUT_BOTTOM_MID, 0, 40)

        self.panel.add_style(
            StyleWrapper()
            .bg_color(lv_colors.ONEKEY_BLACK_3)
            .bg_opa()
            .border_width(0)
            .text_font(font_PJSBOLD36)
            .text_color(lv_colors.LIGHT_GRAY)
            .text_align_left()
            .pad_all(8)
            .radius(0),
            0,
        )
        self.content = lv.label(self.panel)
        self.content.set_size(lv.pct(100), lv.pct(100))
        self.content.set_text(passphrase)
        self.content.set_long_mode(lv.label.LONG.WRAP)
        self.input_count_tips = lv.label(self.content_area)
        self.input_count_tips.align_to(self.panel, lv.ALIGN.OUT_BOTTOM_LEFT, 8, 8)
        self.input_count_tips.add_style(
            StyleWrapper()
            .text_font(font_STATUS_BAR)
            .text_letter_space(-1)
            .text_align_left()
            .text_color(lv_colors.LIGHT_GRAY),
            0,
        )
        self.input_count_tips.set_text(f"{len(passphrase)}/50")


class SolBlindingSign(FullSizeWindow):
    def __init__(self, fee_payer: str, message_hex: str, primary_color, icon_path):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            _(i18n_keys.LIST_VALUE__UNKNOWN__COLON),
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MESSAGE_HASH__COLON), message_hex
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )


class SolTransfer(FullSizeWindow):
    def __init__(
        self, from_addr: str, fee_payer: str, to_addr: str, amount: str, primary_color
    ):
        super().__init__(
            title=_(i18n_keys.TITLE__STR_TRANSFER).format("SOL"),
            subtitle=None,
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), from_addr
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )


class SolCreateAssociatedTokenAccount(FullSizeWindow):
    def __init__(
        self,
        fee_payer: str,
        funding_account: str,
        associated_token_account: str,
        wallet_address: str,
        token_mint: str,
        primary_color,
    ):
        super().__init__(
            title=_(i18n_keys.TITLE__CREATE_TOKEN_ACCOUNT),
            subtitle=None,
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__NEW_TOKEN_ACCOUNT),
            associated_token_account,
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__OWNER), wallet_address
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MINT_ADDRESS), token_mint
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FUNDED_BY__COLON), funding_account
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )  # _(i18n_keys.LIST_KEY__FEE_PAYER__COLON)


class SolTokenTransfer(FullSizeWindow):
    def __init__(
        self,
        from_addr: str,
        to: str,
        amount: str,
        source_owner: str,
        fee_payer: str,
        primary_color,
        token_mint: str | None = None,
    ):
        super().__init__(
            title=_(i18n_keys.TITLE__TOKEN_TRANSFER),
            subtitle=None,
            confirm_text=_(i18n_keys.BUTTON__CONTINUE),
            cancel_text=_(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO_TOKEN_ACCOUNT__COLON), to
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM_TOKEN_ACCOUNT__COLON), from_addr
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), source_owner
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )
        if token_mint:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__MINT_ADDRESS), token_mint
            )


class BlindingSignCommon(FullSizeWindow):
    def __init__(self, signer: str, primary_color, icon_path):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            _(i18n_keys.LIST_VALUE__UNKNOWN__COLON),
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
        )


class Modal(FullSizeWindow):
    def __init__(
        self,
        title: str | None,
        subtitle: str | None,
        confirm_text: str = "",
        cancel_text: str = "",
        icon_path: str | None = None,
        anim_dir: int = 1,
    ):
        super().__init__(
            title, subtitle, confirm_text, cancel_text, icon_path, anim_dir=anim_dir
        )


class AlgoCommon(FullSizeWindow):
    def __init__(self, type: str, primary_color, icon_path):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            type,
        )


class AlgoPayment(FullSizeWindow):
    def __init__(
        self,
        sender,
        receiver,
        close_to,
        rekey_to,
        genesis_id,
        note,
        fee,
        amount,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), receiver
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item5 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if close_to is not None:
            self.item6 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CLOSE_REMAINDER_TO__COLON),
                close_to,
            )
        if rekey_to is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoAssetFreeze(FullSizeWindow):
    def __init__(
        self,
        sender,
        rekey_to,
        fee,
        index,
        target,
        new_freeze_state,
        genesis_id,
        note,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FREEZE_ACCOUNT__COLON), target
        )
        self.item3 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FREEZE_ASSET_ID__COLON),
            _(i18n_keys.LIST_VALUE__TRUE)
            if new_freeze_state is True
            else _(i18n_keys.LIST_VALUE__FALSE),
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoAssetXfer(FullSizeWindow):
    def __init__(
        self,
        sender,
        receiver,
        index,
        fee,
        amount,
        close_assets_to,
        revocation_target,
        rekey_to,
        genesis_id,
        note,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), receiver
        )
        if revocation_target is not None:
            self.item4 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__REVOCATION_ADDRESS__COLON),
                revocation_target,
            )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
        )
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item8 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if close_assets_to is not None:
            self.item9 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CLOSE_ASSET_TO__COLON),
                close_assets_to,
            )
        if genesis_id is not None:
            self.item10 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoAssetCfg(FullSizeWindow):
    def __init__(
        self,
        fee,
        sender,
        index,
        total,
        default_frozen,
        unit_name,
        asset_name,
        decimals,
        manager,
        reserve,
        freeze,
        clawback,
        url,
        metadata_hash,
        rekey_to,
        genesis_id,
        note,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        if asset_name is not None:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__ASSET_NAME__COLON), asset_name
            )
        if index is not None and index != "0":
            self.item3 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
            )
        if url is not None:
            self.item14 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__URL__COLON), url
            )
        if manager is not None:
            self.item5 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__MANAGER_ADDRESS__COLON), manager
            )
        if reserve is not None:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__RESERVE_ADDRESS__COLON), reserve
            )
        if clawback is not None:
            self.item7 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__CLAW_BACK_ADDRESS__COLON),
                clawback,
            )
        if default_frozen is not None:
            self.item8 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__FREEZE_ADDRESS__QUESTION),
                _(i18n_keys.LIST_VALUE__TRUE)
                if default_frozen is True
                else _(i18n_keys.LIST_VALUE__FALSE),
            )
        if freeze is not None:
            self.item9 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__FREEZE_ADDRESS__COLON), freeze
            )
        if total is not None:
            self.item10 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__TOTAL__COLON), total
            )
        if decimals is not None and decimals != "0":
            self.item11 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__DECIMALS__COLON), decimals
            )
        if unit_name is not None:
            self.item12 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__UNIT_NAME__COLON), unit_name
            )
        if metadata_hash is not None:
            self.item13 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__METADATA_HASH__COLON),
                metadata_hash,
            )
        self.item14 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item15 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item16 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item17 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoKeyregOnline(FullSizeWindow):
    def __init__(
        self,
        sender,
        fee,
        votekey,
        selkey,
        sprfkey,
        rekey_to,
        genesis_id,
        note,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__VRF_PUBLIC_KEY__COLON), selkey
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__VOTE_PUBLIC_KEY__COLON), votekey
        )
        if sprfkey is not None:
            self.item4 = DisplayItem(
                self.container,
                _(i18n_keys.LIST_KEY__STATE_PROOF_PUBLIC_KEY__COLON),
                sprfkey,
            )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item6 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item7 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItem(self.container, "GENESIS ID:", genesis_id)


class AlgoKeyregNonp(FullSizeWindow):
    def __init__(self, sender, fee, nonpart, rekey_to, genesis_id, note, primary_color):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("ALGO"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))

        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item3 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        self.item4 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__NONPARTICIPATION__COLON),
            _(i18n_keys.LIST_VALUE__FALSE)
            if nonpart is True
            else _(i18n_keys.LIST_VALUE__TRUE),
        )
        if rekey_to is not None:
            self.item5 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )


class AlgoApplication(FullSizeWindow):
    def __init__(self, signer: str, primary_color, icon_path):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            "Application",
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
        )


class RipplePayment(FullSizeWindow):
    def __init__(
        self,
        title,
        address_from,
        address_to,
        amount,
        fee_max,
        total_amount=None,
        tag=None,
        primary_color=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if tag is not None:
            self.item4 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__DESTINATION_TAG__COLON), tag
            )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total_amount
        )


class NftRemoveConfirm(FullSizeWindow):
    def __init__(self, icon_path):
        super().__init__(
            title=_(i18n_keys.TITLE__REMOVE_NFT),
            subtitle=_(i18n_keys.SUBTITLE__REMOVE_NFT),
            confirm_text=_(i18n_keys.BUTTON__REMOVE),
            cancel_text=_(i18n_keys.BUTTON__CANCEL),
            icon_path=icon_path,
            anim_dir=0,
        )
        self.btn_yes.enable(bg_color=lv_colors.ONEKEY_RED_1, text_color=lv_colors.BLACK)

    def destroy(self, _delay_ms):
        self.delete()


class FilecoinPayment(FullSizeWindow):
    def __init__(
        self,
        title,
        address_from,
        address_to,
        amount,
        gaslimit,
        gasfeecap=None,
        gaspremium=None,
        total_amount=None,
        primary_color=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        self.item4 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__GAS_LIMIT__COLON), gaslimit
        )
        self.item5 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__GAS_FEE_CAP__COLON), gasfeecap
        )
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__GAS_PREMIUM__COLON), gaspremium
        )
        self.item6 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON), total_amount
        )


class CosmosTransactionOverview(FullSizeWindow):
    def __init__(self, title, type, value, amount, address, primary_color, icon_path):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__VIEW),
            _(i18n_keys.BUTTON__REJECT),
            anim_dir=2,
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        if type == _(i18n_keys.TITLE__SEND):
            self.item1 = DisplayItem(
                self.container,
                f"#878787 {_(i18n_keys.INSERT__SEND)}#  {amount}  #878787 {_(i18n_keys.INSERT__TO)}#",
                address,
            )
            self.item1.label_top.set_recolor(True)
            self.item1.set_style_bg_color(lv_colors.BLACK, 0)
        else:
            self.item1 = DisplayItem(
                self.container,
                f"#878787 {_(i18n_keys.LIST_KEY__TYPE__COLON)}#",
                value,
            )
            self.item1.label_top.set_recolor(True)
            self.item1.set_style_bg_color(lv_colors.BLACK, 0)


class CosmosSend(FullSizeWindow):
    def __init__(
        self,
        title,
        chain_id,
        chain_name,
        address_from,
        address_to,
        amount,
        fee,
        primary_color=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        self.item4 = DisplayItem(self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee)
        if chain_name is not None:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
            )
        else:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_ID__COLON), chain_id
            )


class CosmosDelegate(FullSizeWindow):
    def __init__(
        self,
        title,
        chain_id,
        chain_name,
        delegator,
        validator,
        amount,
        fee,
        primary_color=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__DELEGATOR__COLON), delegator
        )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__VALIDATOR__COLON), validator
        )
        self.item4 = DisplayItem(self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee)
        if chain_name is not None:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
            )
        else:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_ID__COLON), chain_id
            )


class CosmosSignCommon(FullSizeWindow):
    def __init__(
        self,
        chain_id: str,
        chain_name: str,
        signer: str,
        fee: str,
        title: str,
        value: str,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        if chain_name is not None:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
            )
        else:
            self.item1 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_ID__COLON), chain_id
            )
        if signer:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
            )
        self.item3 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__TYPE__COLON), value
        )
        if fee is not None:
            self.item4 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee
            )


class CosmosSignContent(FullSizeWindow):
    def __init__(
        self,
        msgs_item: dict,
    ):
        super().__init__(
            _(i18n_keys.TITLE__CONTENT),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        for key, value in msgs_item.items():
            if len(value) <= 80:
                self.item2 = DisplayItem(self.container, key, value)


class CosmosLongValue(FullSizeWindow):
    def __init__(
        self,
        title,
        content: str,
        primary_color,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            primary_color=primary_color or lv_colors.ONEKEY_GREEN,
        )

        PageAbleMessage(
            title,
            content,
            self.channel,
            primary_color=lv_colors.ONEKEY_GREEN,
        )
        self.destroy()


class CosmosSignCombined(FullSizeWindow):
    def __init__(self, chain_id: str, signer: str, fee: str, msgs: str):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItem(
            self.container, _(i18n_keys.LIST_KEY__CHAIN_ID__COLON), chain_id
        )
        if signer:
            self.item2 = DisplayItem(
                self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
            )
        self.item3 = DisplayItem(self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee)
        self.show_full_message = NormalButton(
            self, _(i18n_keys.BUTTON__VIEW_FULL_MESSAGE)
        )
        self.show_full_message.align_to(self.item3, lv.ALIGN.OUT_BOTTOM_MID, 0, 32)
        self.show_full_message.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
        self.message = msgs

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.show_full_message:
                PageAbleMessage(
                    _(i18n_keys.BUTTON__VIEW_FULL_MESSAGE),
                    self.message,
                    self.channel,
                    primary_color=lv_colors.ONEKEY_GREEN,
                )
