# Serialize TRON Format
from trezor.crypto import base58
from trezor.messages import TronSignTx

from apps.common.writers import write_bytes_fixed

# PROTOBUF3 types
TYPE_VARINT = 0
TYPE_DOUBLE = 1
TYPE_LEN = 2
TYPE_FLOAT = 5


def write_field(w: bytearray, fnumber: int, ftype: int):
    tag = fnumber << 3 | ftype
    write_varint(w, tag)


def write_varint(w: bytearray, value: int):
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


def write_bytes_with_length(w, buf: bytes | bytearray):
    write_varint(w, len(buf))
    write_bytes_fixed(w, bytes(buf), len(buf))


def pack_contract(contract, owner_address):
    """
    Pack Tron Proto3 Contract
    See: https://github.com/tronprotocol/protocol/blob/master/core/Tron.proto
    and https://github.com/tronprotocol/protocol/blob/master/core/contract/smart_contract.proto
    """
    retc = bytearray()
    write_field(retc, 1, TYPE_VARINT)
    # contract message
    cmessage = bytearray()
    api = ""
    if contract.transfer_contract:
        write_varint(retc, 1)
        api = "TransferContract"

        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        write_field(cmessage, 2, TYPE_LEN)
        write_bytes_with_length(
            cmessage, base58.decode_check(contract.transfer_contract.to_address)
        )
        write_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.transfer_contract.amount)
    elif contract.vote_witness_contract:
        write_varint(retc, 4)
        api = "VoteWitnessContract"

        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        for vote in contract.vote_witness_contract.votes:
            v_message = bytearray()
            write_field(cmessage, 2, TYPE_LEN)
            write_field(v_message, 1, TYPE_LEN)
            write_bytes_with_length(v_message, base58.decode_check(vote.vote_address))
            write_field(v_message, 2, TYPE_VARINT)
            write_varint(v_message, vote.vote_count)
            write_bytes_with_length(cmessage, v_message)
        if contract.vote_witness_contract.support is not None:
            write_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, int(contract.vote_witness_contract.support))

    elif contract.trigger_smart_contract:
        write_varint(retc, 31)
        api = "TriggerSmartContract"

        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        write_field(cmessage, 2, TYPE_LEN)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.trigger_smart_contract.contract_address),
        )
        if contract.trigger_smart_contract.call_value:
            write_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.call_value)

        # Contract data
        write_field(cmessage, 4, TYPE_LEN)
        write_bytes_with_length(cmessage, contract.trigger_smart_contract.data)

        if contract.trigger_smart_contract.call_token_value:
            write_field(cmessage, 5, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.call_token_value)
            write_field(cmessage, 6, TYPE_VARINT)
            write_varint(cmessage, contract.trigger_smart_contract.asset_id)

    elif contract.freeze_balance_contract:
        write_varint(retc, 11)
        api = "FreezeBalanceContract"

        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        write_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_contract.frozen_balance)
        write_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_contract.frozen_duration)
        if contract.freeze_balance_contract.resource is not None:
            write_field(cmessage, 10, TYPE_VARINT)
            write_varint(cmessage, contract.freeze_balance_contract.resource)
        if contract.freeze_balance_contract.receiver_address is not None:
            write_field(cmessage, 15, TYPE_LEN)
            write_bytes_with_length(
                cmessage,
                base58.decode_check(contract.freeze_balance_contract.receiver_address),
            )

    elif contract.unfreeze_balance_contract:
        write_varint(retc, 12)
        api = "UnfreezeBalanceContract"

        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        if contract.unfreeze_balance_contract.resource is not None:
            write_field(cmessage, 10, TYPE_VARINT)
            write_varint(cmessage, contract.unfreeze_balance_contract.resource)
        if contract.unfreeze_balance_contract.receiver_address is not None:
            write_field(cmessage, 15, TYPE_LEN)
            write_bytes_with_length(
                cmessage,
                base58.decode_check(
                    contract.unfreeze_balance_contract.receiver_address
                ),
            )

    elif contract.withdraw_balance_contract:
        write_varint(retc, 13)
        api = "WithdrawBalanceContract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

    elif contract.freeze_balance_v2_contract:
        write_varint(retc, 54)
        api = "FreezeBalanceV2Contract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        write_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.freeze_balance_v2_contract.frozen_balance)
        if contract.freeze_balance_v2_contract.resource is not None:
            write_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.freeze_balance_v2_contract.resource)

    elif contract.unfreeze_balance_v2_contract:
        write_varint(retc, 55)
        api = "UnfreezeBalanceV2Contract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

        write_field(cmessage, 2, TYPE_VARINT)
        write_varint(cmessage, contract.unfreeze_balance_v2_contract.unfreeze_balance)
        if contract.unfreeze_balance_v2_contract.resource is not None:
            write_field(cmessage, 3, TYPE_VARINT)
            write_varint(cmessage, contract.unfreeze_balance_v2_contract.resource)

    elif contract.withdraw_expire_unfreeze_contract:
        write_varint(retc, 56)
        api = "WithdrawExpireUnfreezeContract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))

    elif contract.delegate_resource_contract:
        write_varint(retc, 57)
        api = "DelegateResourceContract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        if contract.delegate_resource_contract.resource is not None:
            write_field(cmessage, 2, TYPE_VARINT)
            write_varint(cmessage, contract.delegate_resource_contract.resource)
        write_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.delegate_resource_contract.balance)
        write_field(cmessage, 4, TYPE_LEN)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.delegate_resource_contract.receiver_address),
        )
        if contract.delegate_resource_contract.lock is not None:
            write_field(cmessage, 5, TYPE_VARINT)
            write_varint(cmessage, contract.delegate_resource_contract.lock)
        if contract.delegate_resource_contract.lock_period is not None:
            write_field(cmessage, 6, TYPE_VARINT)
            write_varint(cmessage, contract.delegate_resource_contract.lock_period)

    elif contract.undelegate_resource_contract:
        write_varint(retc, 58)
        api = "UnDelegateResourceContract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
        if contract.undelegate_resource_contract.resource is not None:
            write_field(cmessage, 2, TYPE_VARINT)
            write_varint(cmessage, contract.undelegate_resource_contract.resource)
        write_field(cmessage, 3, TYPE_VARINT)
        write_varint(cmessage, contract.undelegate_resource_contract.balance)
        write_field(cmessage, 4, TYPE_LEN)
        write_bytes_with_length(
            cmessage,
            base58.decode_check(contract.undelegate_resource_contract.receiver_address),
        )
    elif contract.cancel_all_unfreeze_v2_contract:
        write_varint(retc, 59)
        api = "CancelAllUnfreezeV2Contract"
        write_field(cmessage, 1, TYPE_LEN)
        write_bytes_with_length(cmessage, base58.decode_check(owner_address))
    else:
        raise ValueError("Unsupported contract type")

    # write API
    capi = bytearray()
    write_field(capi, 1, TYPE_LEN)
    # write_bytes_with_length(capi, "type.googleapis.com/protocol." + api)
    write_bytes_with_length(capi, bytes("type.googleapis.com/protocol." + api, "ascii"))

    # extend to capi
    write_field(capi, 2, TYPE_LEN)
    write_bytes_with_length(capi, cmessage)

    # extend to contract
    write_field(retc, 2, TYPE_LEN)
    write_bytes_with_length(retc, capi)

    if contract.provider:
        write_field(retc, 3, TYPE_LEN)
        write_bytes_with_length(retc, contract.provider)
    if contract.contract_name:
        write_field(retc, 4, TYPE_LEN)
        write_bytes_with_length(retc, contract.contract_name)
    if contract.permission_id is not None:
        write_field(retc, 5, TYPE_VARINT)
        write_varint(retc, contract.permission_id)
    return retc


def serialize(transaction: TronSignTx, owner_address: str):
    # transaction parameters
    ret = bytearray()
    write_field(ret, 1, TYPE_LEN)
    write_bytes_with_length(ret, transaction.ref_block_bytes)
    write_field(ret, 4, TYPE_LEN)
    write_bytes_with_length(ret, transaction.ref_block_hash)
    write_field(ret, 8, TYPE_VARINT)
    write_varint(ret, transaction.expiration)
    if transaction.data is not None:
        write_field(ret, 10, TYPE_LEN)
        write_bytes_with_length(ret, transaction.data)

    # add Contract
    retc = pack_contract(transaction.contract, owner_address)

    write_field(ret, 11, TYPE_LEN)
    write_bytes_with_length(ret, retc)
    # add timestamp
    write_field(ret, 14, TYPE_VARINT)
    write_varint(ret, transaction.timestamp)
    # add fee_limit if any
    if transaction.fee_limit:
        write_field(ret, 18, TYPE_VARINT)
        write_varint(ret, transaction.fee_limit)

    return ret
