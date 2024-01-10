from typing import TYPE_CHECKING, Union

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

    def get_transfer_message_hash(
        self,
        to_addr: str,
        amount: int,
        seqno: int,
        expiration_time: int,
        payload: Union[Cell, str, bytes, None] = None,
        send_mode=SendModeEnum.ignore_errors | SendModeEnum.pay_gas_separately,
        state_init=None,
    ) -> bytes:
        payload_cell = Cell()
        if payload:
            if isinstance(payload, str):
                payload_cell.bits.write_uint(0, 32)
                payload_cell.bits.write_string(payload)
            elif isinstance(payload, Cell):
                payload_cell = payload
            else:
                payload_cell.bits.write_bytes(payload)

        order_header = Contract.create_internal_message_header(Address(to_addr), amount)
        order = Contract.create_common_msg_info(order_header, state_init, payload_cell)
        signing_message = self.create_signing_message(expiration_time, seqno)
        signing_message.bits.write_uint8(send_mode)
        signing_message.refs.append(order)

        return signing_message.bytes_hash()
