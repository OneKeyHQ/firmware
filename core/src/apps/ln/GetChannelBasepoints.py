from trezor.messages.GetChannelBasepointsReply import GetChannelBasepointsReply
from .common import g_nodes

async def GetChannelBasepoints(self, request):
    """BOLT #3 - Key Derivation
    Get our channel basepoints and funding pubkey
    """

    node_id = request.node_id.data
    node = g_nodes.get(node_id)
    print("GetChannelBasepoints node:%s" % node_id.hex())

    channel_nonce = request.channel_nonce.data
    channel = node.channels.get(channel_nonce)
    print("GetChannelBasepoints channel:%s" % channel_nonce.hex())

    if channel is None:
        raise ("channel is None")

    reply = GetChannelBasepointsReply()
    reply.basepoints.revocation.data = channel.basepoints.revocation
    reply.basepoints.payment.data = channel.basepoints.payment
    reply.basepoints.htlc.data = channel.basepoints.htlc
    reply.basepoints.delayed_payment.data = channel.basepoints.delayed_payment
    reply.basepoints.funding_pubkey.data = channel.basepoints.funding_pubkey

    return reply
