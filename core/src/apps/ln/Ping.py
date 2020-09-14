from trezor.messages.PingReply import PingReply

async def Ping(self, request):
    return PingReply(message="ping reply")