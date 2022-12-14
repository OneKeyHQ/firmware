from typing import TYPE_CHECKING

from trezor import log, loop, wire, workflow
from trezor.enums import ButtonRequestType
from trezor.messages import ButtonAck, ButtonRequest

if TYPE_CHECKING:
    from typing import Any, Awaitable, TypeVar

    T = TypeVar("T")
    LayoutType = Awaitable[Any]
    from ....lvglui.scrs.common import Screen, FullSizeWindow

CANCEL = 0
CONFIRM = 1
SHOW_MORE = 2


async def button_request(
    ctx: wire.GenericContext,
    br_type: str,
    code: ButtonRequestType = ButtonRequestType.Other,
) -> None:
    if __debug__:
        log.debug(__name__, "ButtonRequest.type=%s", br_type)
    workflow.close_others()
    await ctx.call(ButtonRequest(code=code), ButtonAck)


async def raise_if_cancelled(a: Awaitable[T], exc: Any = wire.ActionCancelled) -> T:
    result = await a
    if not result:
        await loop.sleep(300)
        raise exc
    return result


async def interact(
    ctx: wire.GenericContext,
    screen: Screen | FullSizeWindow,
    br_type: str,
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> Any:
    await button_request(ctx, br_type, br_code)
    return await ctx.wait(screen.request())
