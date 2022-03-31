from trezor import log, wire, workflow
from trezor.enums import ButtonRequestType
from trezor.messages import ButtonAck, ButtonRequest
from typing import TYPE_CHECKING
from .scrs.common import Screen
if TYPE_CHECKING:
    from typing import (
        Any,
        Awaitable,
    )
def is_confirmed(x: Any) -> bool:
    return x

async def raise_if_cancelled(a: Awaitable[T], exc: Any = wire.ActionCancelled) -> T:
    result = await a
    if result is False:
        raise exc
    return result

async def button_request(
    ctx: wire.GenericContext,
    br_type: str,
    code: ButtonRequestType = ButtonRequestType.Other,
) -> None:
    if __debug__:
        log.debug(__name__, "ButtonRequest.type=%s", br_type)
    workflow.close_others()
    await ctx.call(ButtonRequest(code=code), ButtonAck)


async def interact(
    ctx: wire.GenericContext,
    screen: Screen,
    br_type: str,
    br_code: ButtonRequestType = ButtonRequestType.Other,
) -> Any:
    await button_request(ctx, br_type, br_code)
    return await ctx.wait(screen.request())
