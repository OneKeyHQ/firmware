# Serialize TRON Format
from trezor.crypto import base58
from trezor.messages import TronSignTx

from apps.common.writers import write_bytes_fixed

# PROTOBUF3 types
TYPE_VARINT = 0
TYPE_DOUBLE = 1
TYPE_STRING = 2
TYPE_GROUPS = 3
TYPE_GROUPE = 4
TYPE_FLOAT = 5


def add_field(w, fnumber, ftype):
    if fnumber > 15:
        w.append(fnumber << 3 | ftype)
        w.append(0x01)
    else:
        w.append(fnumber << 3 | ftype)


def write_varint(w, value):
    """
    Implements Base 128 variant
    See: https://developers.google.com/protocol-buffers/docs/encoding#varints
    """
    while True:
        byte = value & 0x7F
        value = value >> 7
        if value == 0:
            w.append(byte)
            break
        else:
            w.append(byte | 0x80)


def write_bytes_with_length(w, buf: bytes):
    write_varint(w, len(buf))
    write_bytes_fixed(w, buf, len(buf))


def pack_contract(contract, owner_address):
    """
    Pack Tron Proto3 Contract
    See: https://github.com/tronprotocol/protocol/blob/master/core/Tron.proto
    and https://github.com/tronprotocol/protocol/blob/master/core/contract/smart_contract.proto
    """
    retc = bytearray()
    add_field(retc, 1, TYPE_VARINT)
    # contract message
    cmessage = bytearray()
    api = ""
    if contract.transfer_contract:
        write_varint(retc, 1)
        api = "TransferContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 2, TYPE_STRING)
        write_bytes_with_length(
            cmessage, base58.decode_check(contract.transfer_contract.to_address)
        )
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.transfer_contract.amount)

    if contract.trigger_smart_contract:
        write_varint(retc, 31)
        api = "TriggerSmartContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 2, TYPE_STRING)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.trigger_smart_contract.contract_address),
        )
        if contract.trigger_smart_contract.call_value:
            add_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.call_value)

        # Contract data
        add_field(cmessage, 4, TYPE_STRING)
        write_bytes_with_length(cmessage, contract.trigger_smart_contract.data)

        if contract.trigger_smart_contract.call_token_value:
            add_field(cmessage, 5, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.call_token_value)
            add_field(cmessage, 6, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.asset_id)

    if contract.freeze_balance_contract:
        write_varint(retc, 11)
        api = "FreezeBalanceContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_contract.frozen_balance)
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_contract.frozen_duration)
        if contract.freeze_balance_contract.resource is not None:
            add_field(cmessage, 4, TYPE_VARINT)
            write_varint(cmessage, contract.freeze_balance_contract.resource)
        if contract.freeze_balance_contract.receiver_address is not None:
            add_field(cmessage, 5, TYPE_STRING)
            write_bytes_with_length(
                cmessage,
                base58.decode_check(contract.freeze_balance_contract.receiver_address),
            )

    if contract.unfreeze_balance_contract:
        write_varint(retc, 12)
        api = "UnfreezeBalanceContract"

        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        if contract.unfreeze_balance_contract.resource is not None:
            add_field(cmessage, 2, TYPE_VARINT)
            write_varint(cmessage, contract.unfreeze_balance_contract.resource)
        if contract.unfreeze_balance_contract.receiver_address is not None:
            add_field(cmessage, 3, TYPE_STRING)
            write_bytes_with_length(
                cmessage,
                base58.decode_check(
                    contract.unfreeze_balance_contract.receiver_address
                ),
            )

    if contract.withdraw_balance_contract:
        write_varint(retc, 13)
        api = "WithdrawBalanceContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

    if contract.freeze_balance_v2_contract:
        write_varint(retc, 54)
        api = "FreezeBalanceV2Contract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_v2_contract.frozen_balance)
        if contract.freeze_balance_v2_contract.resource is not None:
            add_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.freeze_balance_v2_contract.resource)

    if contract.unfreeze_balance_v2_contract:
        write_varint(retc, 55)
        api = "UnfreezeBalanceV2Contract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.unfreeze_balance_v2_contract.unfreeze_balance)
        if contract.unfreeze_balance_v2_contract.resource is not None:
            add_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.unfreeze_balance_v2_contract.resource)

    if contract.withdraw_expire_unfreeze_contract:
        write_varint(retc, 56)
        api = "WithdrawExpireUnfreezeContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

    if contract.delegate_resource_contract:
        write_varint(retc, 57)
        api = "DelegateResourceContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.delegate_resource_contract.resource)
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.delegate_resource_contract.balance)
        add_field(cmessage, 4, TYPE_STRING)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.delegate_resource_contract.receiver_address),
        )
        if contract.delegate_resource_contract.lock is not None:
            add_field(cmessage, 5, TYPE_VARINT)
            write_varint(cmessage, contract.delegate_resource_contract.lock)

    if contract.undelegate_resource_contract:
        write_varint(retc, 58)
        api = "UnDelegateResourceContract"
        add_field(cmessage, 1, TYPE_STRING)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        add_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.undelegate_resource_contract.resource)
        add_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.undelegate_resource_contract.balance)
        add_field(cmessage, 4, TYPE_STRING)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.undelegate_resource_contract.receiver_address),
        )

    # write API
    capi = bytearray()
    add_field(capi, 1, TYPE_STRING)
    # write_bytes_with_length(capi, "type.googleapis.com/protocol." + api)
    write_bytes_with_length(capi, bytes("type.googleapis.com/protocol." + api, "ascii"))

    # extend to capi
    add_field(capi, 2, TYPE_STRING)
    write_bytes_with_length(capi, cmessage)

    # extend to contract
    add_field(retc, 2, TYPE_STRING)
    write_bytes_with_length(retc, capi)
    return retc


def serialize(transaction: TronSignTx, owner_address: str):
    # transaction parameters
    ret = bytearray()
    add_field(ret, 1, TYPE_STRING)
    write_bytes_with_length(ret, transaction.ref_block_bytes)
    add_field(ret, 4, TYPE_STRING)
    write_bytes_with_length(ret, transaction.ref_block_hash)
    add_field(ret, 8, TYPE_VARINT)
    write_varint(ret, transaction.expiration)
    if transaction.data is not None:
        add_field(ret, 10, TYPE_STRING)
        write_bytes_with_length(ret, bytes(transaction.data, "ascii"))

    # add Contract
    retc = pack_contract(transaction.contract, owner_address)

    add_field(ret, 11, TYPE_STRING)
    write_bytes_with_length(ret, retc)
    # add timestamp
    add_field(ret, 14, TYPE_VARINT)
    write_varint(ret, transaction.timestamp)
    # add fee_limit if any
    if transaction.fee_limit:
        add_field(ret, 18, TYPE_VARINT)
        write_varint(ret, transaction.fee_limit)

    return ret
