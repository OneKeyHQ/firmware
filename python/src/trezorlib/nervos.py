import re
from typing import TYPE_CHECKING, Any, AnyStr, Dict, List, Optional, Tuple

from . import exceptions, messages
from .tools import expect, prepare_message_bytes, session

if TYPE_CHECKING:
    from .client import TrezorClient
    from .tools import Address
    from .protobuf import MessageType

@expect(messages.NervosAddress, field="address", ret_type=str)
def get_nervos_address(
    client: "TrezorClient", n: "Address", show_display: bool = False
) -> "MessageType":
    # 请求硬件钱包返回一个 Nervos 网络地址
    # show_display: 是否在钱包屏幕上展示地址
    return client.call(
        messages.NervosGetAddress(address_n=n, show_display=show_display)
    )