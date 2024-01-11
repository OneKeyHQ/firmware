from trezor import utils
from trezor.lvglui.scrs.components.button import NormalButton
from trezor.lvglui.scrs.components.pageable import PageAbleMessage
from trezor.lvglui.scrs.components.transition import BtnClickTransition

from ..i18n import gettext as _, keys as i18n_keys
from ..lv_colors import lv_colors
from . import font_MONO24, font_PJSBOLD36, font_PJSBOLD48, font_STATUS_BAR
from .common import FullSizeWindow, lv
from .components.banner import Banner
from .components.container import ContainerFlexCol
from .components.listitem import DisplayItemNoBgc
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
        address_qr=None,
        multisig_index: int | None = 0,
        addr_type=None,
        evm_chain_id: int | None = None,
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
        self.address_qr = address_qr
        self.icon = icon_path
        self.addr_type = addr_type
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
        self.show_address(evm_chain_id=evm_chain_id)

    def show_address(self, evm_chain_id: int | None = None):
        self.current = self.SHOW_TYPE.ADDRESS
        if hasattr(self, "qr"):
            self.qr.delete()
            del self.qr
        self.btn_no.label.set_text(_(i18n_keys.BUTTON__QRCODE))
        self.container = ContainerFlexCol(
            self.content_area, self.subtitle, pos=(0, 16), padding_row=8
        )
        if self.addr_type:
            self.item1_0 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__TYPE__COLON), self.addr_type
            )
        if evm_chain_id:
            self.item2 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CHAIN_ID__COLON),
                str(evm_chain_id),
            )
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__PATH__COLON), self.path
        )
        # if xpubs:
        #     self.title.align(lv.ALIGN.TOP_MID, 0, 100)
        #     self.btn_yes.align(lv.ALIGN.BOTTOM_MID, 0, -30)
        self.current = 0
        for i, xpub in enumerate(self.xpubs or []):
            self.item3 = DisplayItemNoBgc(
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
        self.qr = QRCode(
            self.content_area,
            self.address if self.address_qr is None else self.address_qr,
            self.icon,
        )
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__PATH__COLON), path
        )
        self.item2 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__XPUB__COLON)
            if xpub
            else _(i18n_keys.LIST_KEY__PUBLIC_KEY__COLON),
            xpub or pubkey,
        )


class Message(FullSizeWindow):
    def __init__(
        self,
        title,
        address,
        message,
        primary_color,
        icon_path,
        verify: bool = False,
        evm_chain_id: int | None = None,
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
        if evm_chain_id:
            self.item3 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CHAIN_ID__COLON),
                str(evm_chain_id),
            )
        self.item1 = DisplayItemNoBgc(
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
            self.item2 = DisplayItemNoBgc(
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
    def __init__(
        self, title, amount, address, primary_color, icon_path, has_details=None
    ):
        if __debug__:
            self.layout_address = address

        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            anim_dir=2,
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItemNoBgc(
            self.container,
            f"#FFFFFF {_(i18n_keys.INSERT__SEND)}#  {amount}  #FFFFFF {_(i18n_keys.INSERT__TO)}#",
            address,
        )
        self.item1.label_top.add_style(
            StyleWrapper().text_color(lv.color_hex(0xE1E1E1)), 0
        )
        if has_details:
            self.view_btn = NormalButton(
                self.content_area, _(i18n_keys.BUTTON__VIEW_DETAILS)
            )
            self.view_btn.add_style(
                StyleWrapper()
                .border_width(2)
                .border_color(lv_colors.ONEKEY_GRAY_1)
                .bg_color(lv_colors.ONEKEY_BLACK_3),
                0,
            )
            self.view_btn.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 16)
            self.view_btn.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.view_btn:
                self.destroy(400)
                self.channel.publish(2)

    if __debug__:

        def read_content(self) -> list[str]:
            return (
                [self.layout_title or ""]
                + [self.layout_subtitle or ""]
                + [self.layout_address or ""]
            )


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
        evm_chain_id=None,
        raw_data=None,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        if evm_chain_id:
            self.item0_1 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CHAIN_ID__COLON),
                str(evm_chain_id),
            )

        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__AMOUNT__COLON),
            amount,
        )
        self.item4 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__TO__COLON),
            address_to,
        )
        self.item5 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__FROM__COLON),
            address_from,
        )
        if contract_addr:
            self.item0 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CONTRACT_ADDRESS__COLON),
                contract_addr,
            )
            self.item1 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__TOKEN_ID__COLON),
                token_id,
            )
        self.item3 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON),
            fee_max,
        )
        if not is_eip1559:
            self.item2 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__GAS_PRICE__COLON),
                gas_price,
            )
        else:
            self.item2 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__PRIORITY_FEE_PER_GAS__COLON),
                max_priority_fee_per_gas,
            )
            self.item2_1 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__MAXIMUM_FEE_PER_GAS__COLON),
                max_fee_per_gas,
            )
        if total_amount is None:
            if not contract_addr:  # token transfer
                total_amount = f"{amount}\n{fee_max}"
            else:  # nft transfer
                total_amount = f"{fee_max}"
        self.item6 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__TOTAL_AMOUNT__COLON),
            total_amount,
        )
        if raw_data:
            self.item7 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__DATA__COLON),
                _(i18n_keys.SUBTITLE__STR_BYTES).format(len(raw_data)),
            )
            self.panel = lv.obj(self.content_area)
            self.panel.clear_flag(lv.obj.FLAG.SCROLLABLE)
            self.panel.add_style(
                StyleWrapper()
                .width(464)
                .height(lv.SIZE.CONTENT)
                .bg_color(lv_colors.ONEKEY_BLACK_3)
                .bg_opa()
                .border_width(1)
                .border_color(lv_colors.ONEKEY_GRAY_1)
                .pad_all(8)
                .radius(0)
                .max_height(256)
                .text_font(font_MONO24)
                .text_color(lv_colors.LIGHT_GRAY)
                .text_align_left()
                .text_letter_space(-1),
                0,
            )
            self.panel.align_to(self.container, lv.ALIGN.OUT_BOTTOM_MID, 0, 8)
            self.content = lv.label(self.panel)
            self.content.set_align(lv.ALIGN.TOP_LEFT)
            self.content.set_size(lv.pct(100), lv.SIZE.CONTENT)
            from binascii import hexlify

            self.data_str = f"0x{hexlify(raw_data).decode()}"
            if len(self.data_str) > 216:
                self.content.set_long_mode(lv.label.LONG.WRAP)
                self.content.set_text(self.data_str[:213] + "...")
                self.view_btn = NormalButton(self.panel, _(i18n_keys.BUTTON__VIEW))
                self.view_btn.set_width(246)
                self.view_btn.align(lv.ALIGN.CENTER, 0, 0)
                self.view_btn.add_style(
                    StyleWrapper()
                    .bg_color(lv_colors.ONEKEY_BLACK_3)
                    .border_width(2)
                    .border_color(lv_colors.ONEKEY_GRAY_1),
                    0,
                )
                self.view_btn.add_style(
                    StyleWrapper()
                    .bg_opa(lv.OPA.COVER)
                    .bg_color(lv_colors.ONEKEY_GRAY_3)
                    .transform_height(-2)
                    .transform_width(-2)
                    .transition(BtnClickTransition()),
                    lv.PART.MAIN | lv.STATE.PRESSED,
                )

                self.view_btn.add_event_cb(self.on_click, lv.EVENT.CLICKED, None)
            else:
                self.content.set_text(self.data_str)

    def on_click(self, event_obj):
        code = event_obj.code
        target = event_obj.get_target()
        if code == lv.EVENT.CLICKED:
            if target == self.view_btn:
                PageAbleMessage(
                    _(i18n_keys.TITLE__VIEW_DATA),
                    self.data_str,
                    channel=None,
                    cancel_text=_(i18n_keys.BUTTON__CLOSE),
                    confirm_text=None,
                    page_size=405,
                    font=font_MONO24,
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__SIZE__COLON), description
        )
        self.long_data = False
        if len(data) > 80:
            self.data = data
            self.long_data = True
            self.btn_yes.label.set_text(_(i18n_keys.BUTTON__VIEW))
        else:
            self.item2 = DisplayItemNoBgc(
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
                        font=font_MONO24,
                        page_size=405,
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
        self.primary_color = primary_color
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItemNoBgc(self.container, description, content)
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
                        primary_color=self.primary_color,
                    )
                    self.destroy()
                else:
                    self.show_unload_anim()
                    self.channel.publish(1)
            elif target == self.btn_no:
                self.show_dismiss_anim()
                self.channel.publish(0)


class ConfirmMetaData(FullSizeWindow):
    def __init__(self, title, subtitle, description, data, primary_color, icon_path):
        if __debug__:
            self.layout_data = data

        super().__init__(
            title,
            subtitle,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
            icon_path=icon_path,
        )

        if description:
            self.container = ContainerFlexCol(
                self.content_area, self.subtitle, pos=(0, 40)
            )
            self.item1 = DisplayItemNoBgc(self.container, description, data)

    if __debug__:

        def read_content(self) -> list[str]:
            return (
                [self.layout_title or ""]
                + [self.layout_subtitle or ""]
                + [self.layout_data or ""]
            )


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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee
        )
        self.item3 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT_YOU_SPEND__COLON), amount
        )
        self.item3 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(self.container, description, fee_change)
        self.item2 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__ADDRESS__COLON), address
        )
        self.item2 = DisplayItemNoBgc(self.container, description, amount_change)
        self.item3 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )
        self.item2 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__COIN_NAME__COLON), coin_name
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_ROUNDS__COLON), max_rounds
        )
        self.item3 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
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
            self.item = DisplayItemNoBgc(self.container, f"{key.upper()}", value)


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
            self.item1 = DisplayItemNoBgc(self.container, key, "")
            self.item2 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), value
            )
            self.item3 = DisplayItemNoBgc(
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
        self.item = DisplayItemNoBgc(self.container, f"{key}:", value)
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
            self.item1 = DisplayItemNoBgc(
                self.container, "name (string):", kwargs.get("name")
            )
        if kwargs.get("version"):
            self.item2 = DisplayItemNoBgc(
                self.container, "version (string):", kwargs.get("version")
            )
        if kwargs.get("chainId"):
            self.item3 = DisplayItemNoBgc(
                self.container, "chainId (uint256):", kwargs.get("chainId")
            )
        if kwargs.get("verifyingContract"):
            self.item4 = DisplayItemNoBgc(
                self.container,
                "verifyingContract (address):",
                kwargs.get("verifyingContract"),
            )
        if kwargs.get("salt"):
            self.item5 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item5 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            _(i18n_keys.LIST_VALUE__UNKNOWN__COLON),
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MESSAGE_HASH__COLON), message_hex
        )
        self.item3 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), to_addr
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), from_addr
        )
        self.item4 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__NEW_TOKEN_ACCOUNT),
            associated_token_account,
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__OWNER), wallet_address
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MINT_ADDRESS), token_mint
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FUNDED_BY__COLON), funding_account
        )
        self.item5 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO_TOKEN_ACCOUNT__COLON), to
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM_TOKEN_ACCOUNT__COLON), from_addr
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), source_owner
        )
        self.item5 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FEE_PAYER__COLON), fee_payer
        )
        if token_mint:
            self.item6 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            _(i18n_keys.LIST_VALUE__UNKNOWN__COLON),
        )
        self.item2 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__TYPE__COLON),
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), receiver
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item5 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if close_to is not None:
            self.item6 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CLOSE_REMAINDER_TO__COLON),
                close_to,
            )
        if rekey_to is not None:
            self.item7 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItemNoBgc(self.container, "GENESIS ID:", genesis_id)


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

        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FREEZE_ACCOUNT__COLON), target
        )
        self.item3 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__FREEZE_ASSET_ID__COLON),
            _(i18n_keys.LIST_VALUE__TRUE)
            if new_freeze_state is True
            else _(i18n_keys.LIST_VALUE__FALSE),
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
        )
        self.item5 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item6 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item7 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItemNoBgc(self.container, "GENESIS ID:", genesis_id)


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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), receiver
        )
        if revocation_target is not None:
            self.item4 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__REVOCATION_ADDRESS__COLON),
                revocation_target,
            )
        self.item5 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
        )
        self.item6 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item7 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item8 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if close_assets_to is not None:
            self.item9 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CLOSE_ASSET_TO__COLON),
                close_assets_to,
            )
        if genesis_id is not None:
            self.item10 = DisplayItemNoBgc(self.container, "GENESIS ID:", genesis_id)


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

        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        if asset_name is not None:
            self.item2 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__ASSET_NAME__COLON), asset_name
            )
        if index is not None and index != "0":
            self.item3 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__ASSET_ID__COLON), index
            )
        if url is not None:
            self.item14 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__URL__COLON), url
            )
        if manager is not None:
            self.item5 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__MANAGER_ADDRESS__COLON), manager
            )
        if reserve is not None:
            self.item6 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__RESERVE_ADDRESS__COLON), reserve
            )
        if clawback is not None:
            self.item7 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__CLAW_BACK_ADDRESS__COLON),
                clawback,
            )
        if default_frozen is not None:
            self.item8 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__FREEZE_ADDRESS__QUESTION),
                _(i18n_keys.LIST_VALUE__TRUE)
                if default_frozen is True
                else _(i18n_keys.LIST_VALUE__FALSE),
            )
        if freeze is not None:
            self.item9 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__FREEZE_ADDRESS__COLON), freeze
            )
        if total is not None:
            self.item10 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__TOTAL__COLON), total
            )
        if decimals is not None and decimals != "0":
            self.item11 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__DECIMALS__COLON), decimals
            )
        if unit_name is not None:
            self.item12 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__UNIT_NAME__COLON), unit_name
            )
        if metadata_hash is not None:
            self.item13 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__METADATA_HASH__COLON),
                metadata_hash,
            )
        self.item14 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item15 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item16 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item17 = DisplayItemNoBgc(self.container, "GENESIS ID:", genesis_id)


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

        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__VRF_PUBLIC_KEY__COLON), selkey
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__VOTE_PUBLIC_KEY__COLON), votekey
        )
        if sprfkey is not None:
            self.item4 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__STATE_PROOF_PUBLIC_KEY__COLON),
                sprfkey,
            )
        self.item5 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item6 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        if rekey_to is not None:
            self.item7 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__REKEY_TO__COLON), rekey_to
            )
        if genesis_id is not None:
            self.item8 = DisplayItemNoBgc(self.container, "GENESIS ID:", genesis_id)


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

        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee
        )
        if note is not None:
            self.item3 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__NOTE__COLON), note
            )
        self.item4 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__NONPARTICIPATION__COLON),
            _(i18n_keys.LIST_VALUE__FALSE)
            if nonpart is True
            else _(i18n_keys.LIST_VALUE__TRUE),
        )
        if rekey_to is not None:
            self.item5 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__FORMAT__COLON),
            "Application",
        )
        self.item2 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        if tag is not None:
            self.item4 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__DESTINATION_TAG__COLON), tag
            )
        self.item5 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MAXIMUM_FEE__COLON), fee_max
        )
        if total_amount is None:
            total_amount = f"{amount}\n{fee_max}"
        self.item6 = DisplayItemNoBgc(
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

    def destroy(self, _delay_ms=400):
        self.del_delayed(200)


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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__GAS_LIMIT__COLON), gaslimit
        )
        self.item5 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__GAS_FEE_CAP__COLON), gasfeecap
        )
        self.item6 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__GAS_PREMIUM__COLON), gaspremium
        )
        self.item6 = DisplayItemNoBgc(
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
            self.item1 = DisplayItemNoBgc(
                self.container,
                f"#878787 {_(i18n_keys.INSERT__SEND)}#  {amount}  #878787 {_(i18n_keys.INSERT__TO)}#",
                address,
            )
            self.item1.label_top.set_recolor(True)
            self.item1.set_style_bg_color(lv_colors.BLACK, 0)
        else:
            self.item1 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), address_to
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), address_from
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee
        )
        if chain_name is not None:
            self.item1 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
            )
        else:
            self.item1 = DisplayItemNoBgc(
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
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), amount
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__DELEGATOR__COLON), delegator
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__VALIDATOR__COLON), validator
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee
        )
        if chain_name is not None:
            self.item1 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
            )
        else:
            self.item1 = DisplayItemNoBgc(
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
            self.item1 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
            )
        else:
            self.item1 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__CHAIN_ID__COLON), chain_id
            )
        if signer:
            self.item2 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
            )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TYPE__COLON), value
        )
        if fee is not None:
            self.item4 = DisplayItemNoBgc(
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
                self.item2 = DisplayItemNoBgc(self.container, key, value)


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
        self.primary_color = primary_color
        PageAbleMessage(
            title,
            content,
            self.channel,
            primary_color=self.primary_color,
        )
        self.destroy()


class CosmosSignCombined(FullSizeWindow):
    def __init__(self, chain_id: str, signer: str, fee: str, msgs: str, primary_color):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            primary_color=primary_color,
        )
        self.primary_color = primary_color
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__CHAIN_ID__COLON), chain_id
        )
        if signer:
            self.item2 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), signer
            )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FEE__COLON), fee
        )
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
                    primary_color=self.primary_color,
                )


class ConfirmTypedHash(FullSizeWindow):
    def __init__(self, title, icon, domain_hash, message_hash, primary_color):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            icon,
            primary_color=primary_color,
            anim_dir=0,
        )
        self.banner = Banner(
            self.content_area, 2, _(i18n_keys.MSG__SIGNING_MSG_MAY_HAVE_RISK)
        )
        self.banner.align(lv.ALIGN.TOP_LEFT, 8, 8)
        self.icon.align_to(self.banner, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.title.align_to(self.icon, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 16)
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))
        self.item1 = DisplayItemNoBgc(
            self.container,
            _(i18n_keys.LIST_KEY__DOMAIN_SEPARATOR_HASH__COLON),
            domain_hash,
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__MESSAGE_HASH__COLON), message_hash
        )


class PolkadotBalances(FullSizeWindow):
    def __init__(
        self,
        chain_name,
        module,
        method,
        sender,
        dest,
        source,
        balance,
        tip,
        keep_alive,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__VIEW_TRANSACTION),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__CANCEL),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 48))
        if balance:
            self.item1 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), balance
            )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__TO__COLON), dest
        )
        self.item3 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__SIGNER__COLON), sender
        )
        self.item4 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__CHAIN_NAME__COLON), chain_name
        )
        # self.item5 = DisplayItemNoBgc(self.container, module, method)
        if source:
            self.item6 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__SOURCE_COLON), source
            )
        if tip is not None:
            self.item7 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__TIP_COLON), tip
            )
        if keep_alive is not None:
            self.item8 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__KEEP_ALIVE_COLON), keep_alive
            )


class TronAssetFreeze(FullSizeWindow):
    def __init__(
        self,
        is_freeze,
        sender,
        resource,
        balance,
        duration,
        receiver,
        lock,
        primary_color,
    ):
        super().__init__(
            _(i18n_keys.TITLE__SIGN_STR_TRANSACTION).format("Tron"),
            None,
            _(i18n_keys.BUTTON__CONTINUE),
            _(i18n_keys.BUTTON__REJECT),
            primary_color=primary_color,
        )
        self.container = ContainerFlexCol(self.content_area, self.title, pos=(0, 40))

        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__FROM__COLON), sender
        )
        if resource is not None:
            self.item2 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__RESOURCE_COLON), resource
            )
        if balance:
            if is_freeze:
                self.item3 = DisplayItemNoBgc(
                    self.container, _(i18n_keys.LIST_KEY__FROZEN_BALANCE_COLON), balance
                )
            else:
                self.item3 = DisplayItemNoBgc(
                    self.container, _(i18n_keys.LIST_KEY__AMOUNT__COLON), balance
                )
        if duration:
            self.item4 = DisplayItemNoBgc(
                self.container,
                _(i18n_keys.LIST_KEY__FROZEN_DURATION_COLON),
                duration,
            )
        if receiver is not None:
            self.item5 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__RECEIVER_ADDRESS_COLON), receiver
            )
        if lock is not None:
            self.item6 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__LOCK_COLON), lock
            )


class MessageNostr(FullSizeWindow):
    def __init__(
        self,
        title,
        address,
        message,
        encrypt,
        primary_color,
        icon_path,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.ACTION__ENCRYPT) if encrypt else _(i18n_keys.ACTION__DECRYPT),
            _(i18n_keys.BUTTON__CANCEL),
            anim_dir=2,
            primary_color=primary_color,
            icon_path=icon_path,
        )
        self.primary_color = primary_color
        self.container = ContainerFlexCol(
            self.content_area, self.title, pos=(0, 40), padding_row=8
        )
        self.long_message = False
        if len(message) > 80:
            self.message = message
            self.long_message = True
            self.btn_yes.label.set_text(_(i18n_keys.BUTTON__VIEW))
        else:
            self.item2 = DisplayItemNoBgc(
                self.container, _(i18n_keys.LIST_KEY__MESSAGE__COLON), message
            )
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__PUBLIC_KEY__COLON), address
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


class LnurlAuth(FullSizeWindow):
    def __init__(
        self,
        title,
        domain,
        data,
        primary_color,
    ):
        super().__init__(
            title,
            None,
            _(i18n_keys.BUTTON__SIGN),
            _(i18n_keys.BUTTON__REJECT),
            anim_dir=2,
            primary_color=primary_color,
        )
        self.primary_color = primary_color
        self.container = ContainerFlexCol(
            self.content_area, self.title, pos=(0, 40), padding_row=8
        )
        self.item1 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__DOMAIN__COLON), domain
        )
        self.item2 = DisplayItemNoBgc(
            self.container, _(i18n_keys.LIST_KEY__DATA__COLON), data
        )
