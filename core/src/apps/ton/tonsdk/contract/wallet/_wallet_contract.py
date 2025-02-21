from typing import TYPE_CHECKING, List, Union

from ...boc import Cell
from ...utils import Address
from .. import Contract

if TYPE_CHECKING:
    from enum import IntEnum
else:
    IntEnum = int


class SendModeEnum(IntEnum):
    carry_all_remaining_balance = 128
    carry_all_remaining_incoming_value = 64
    destroy_account_if_zero = 32
    ignore_errors = 2
    pay_gas_separately = 1

    def __str__(self) -> str:
        return super().__str__()


class WalletContract(Contract):
    def __init__(self, **kwargs):
        if "public_key" not in kwargs:
            raise Exception("WalletContract required publicKey in options")
        super().__init__(**kwargs)

    def create_data_cell(self):
        cell = Cell()
        cell.bits.write_uint(0, 32)
        cell.bits.write_bytes(self.options["public_key"])
        return cell

    def create_signing_message(self, _expiration_time, seqno=None):
        seqno = seqno or 0
        cell = Cell()
        cell.bits.write_uint(seqno, 32)
        return cell

    def create_transaction_digest(
        self,
        to_addr: str,
        amount: int,
        seqno: int,
        expire_at: int,
        payload: Union[Cell, str, bytes, None] = None,
        is_raw_data: bool = False,
        send_mode=SendModeEnum.ignore_errors | SendModeEnum.pay_gas_separately,
        state_init=None,
        ext_to: List[str] = None,
        ext_amount: List[int] = None,
        ext_payload: List[Union[Cell, str, bytes, None]] = None,
    ):
        payload_cell = Cell()
        if payload:
            if isinstance(payload, str):
                # check payload type
                # if is_raw_data:
                if payload.startswith("b5ee9c72"):
                    payload_cell = Cell.one_from_boc(payload)
                else:
                    payload_cell.bits.write_uint(0, 32)
                    payload_cell.bits.write_string(payload)
            elif isinstance(payload, Cell):
                payload_cell = payload
            else:
                payload_cell.bits.write_bytes(payload)

        order_header = Contract.create_internal_message_header(
            dest=Address(to_addr), grams=amount
        )
        order = Contract.create_common_msg_info(order_header, state_init, payload_cell)
        signing_message = self.create_signing_message(expire_at, seqno)
        signing_message.bits.write_uint8(send_mode)
        signing_message.refs.append(order)

        if ext_to:
            if len(ext_to) > 3:
                raise ValueError(
                    "Number of extra messages exceeds the maximum limit of 3"
                )

            ext_payload_list = (
                ext_payload if ext_payload is not None else [None] * len(ext_to)
            )
            ext_amount_list = (
                ext_amount if ext_amount is not None else [0] * len(ext_to)
            )

            zipped_ext_data = zip(ext_to, ext_payload_list, ext_amount_list)

            for ext_addr, current_payload, ext_amt in zipped_ext_data:
                ext_payload_cell = Cell()
                if current_payload:
                    if isinstance(current_payload, str):
                        # check payload type
                        if current_payload.startswith("b5ee9c72"):
                            ext_payload_cell = Cell.one_from_boc(current_payload)
                        else:
                            ext_payload_cell.bits.write_uint(0, 32)
                            ext_payload_cell.bits.write_string(current_payload)
                    elif isinstance(current_payload, Cell):
                        ext_payload_cell = current_payload
                    else:
                        ext_payload_cell.bits.write_bytes(current_payload)

                ext_order_header = Contract.create_internal_message_header(
                    dest=Address(ext_addr), grams=ext_amt
                )
                ext_order = Contract.create_common_msg_info(
                    ext_order_header, state_init, ext_payload_cell
                )

                signing_message.bits.write_uint8(send_mode)
                signing_message.refs.append(ext_order)

        boc = bytes(signing_message.to_boc())

        return signing_message.bytes_hash(), boc
