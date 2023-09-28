from typing import Iterable

import storage.cache
from trezor import protobuf, utils
from trezor.enums import MessageType
from trezor.utils import ensure
from trezor.crypto import se_thd89

WIRE_TYPES: dict[int, tuple[int, ...]] = {
    MessageType.AuthorizeCoinJoin: (MessageType.SignTx, MessageType.GetOwnershipProof),
}


def is_set() -> bool:
    if utils.USE_THD89:
        auth_type = se_thd89.authorization_get_type()
        if auth_type is None:
            return False
        return True
    else:
        return bool(storage.cache.get(storage.cache.APP_COMMON_AUTHORIZATION_TYPE))


def set(auth_message: protobuf.MessageType) -> None:
    buffer = protobuf.dump_message_buffer(auth_message)

    # only wire-level messages can be stored as authorization
    # (because only wire-level messages have wire_type, which we use as identifier)
    ensure(auth_message.MESSAGE_WIRE_TYPE is not None)
    assert auth_message.MESSAGE_WIRE_TYPE is not None  # so that typechecker knows too
    if utils.USE_THD89:
        se_thd89.authorization_set(auth_message.MESSAGE_WIRE_TYPE, buffer)
    else:
        storage.cache.set(
            storage.cache.APP_COMMON_AUTHORIZATION_TYPE,
            auth_message.MESSAGE_WIRE_TYPE.to_bytes(2, "big"),
        )
        storage.cache.set(storage.cache.APP_COMMON_AUTHORIZATION_DATA, buffer)


def get() -> protobuf.MessageType | None:
    if utils.USE_THD89:        
        auth_type = se_thd89.authorization_get_type()
        if auth_type is None:
            return None
        msg_wire_type = auth_type
        buffer = se_thd89.authorization_get_data()
    else:    
        stored_auth_type = storage.cache.get(storage.cache.APP_COMMON_AUTHORIZATION_TYPE)
        if not stored_auth_type:
            return None

        msg_wire_type = int.from_bytes(stored_auth_type, "big")
        buffer = storage.cache.get(storage.cache.APP_COMMON_AUTHORIZATION_DATA, b"")
    return protobuf.load_message_buffer(buffer, msg_wire_type)


def get_wire_types() -> Iterable[int]:
    if utils.USE_THD89:
        auth_type = se_thd89.authorization_get_type()
        if auth_type is None:
            return ()
        msg_wire_type = auth_type
    else:
        stored_auth_type = storage.cache.get(storage.cache.APP_COMMON_AUTHORIZATION_TYPE)
        if stored_auth_type is None:
            return ()

        msg_wire_type = int.from_bytes(stored_auth_type, "big")
    return WIRE_TYPES.get(msg_wire_type, ())


def clear() -> None:
    if utils.USE_THD89:
        se_thd89.authorization_clear()
    else:
        storage.cache.delete(storage.cache.APP_COMMON_AUTHORIZATION_TYPE)
        storage.cache.delete(storage.cache.APP_COMMON_AUTHORIZATION_DATA)
