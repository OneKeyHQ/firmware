from trezor.messages.CheckFutureSecretReply import CheckFutureSecretReply
from .common import g_nodes, START_INDEX
from .utils import shachain_derive

async def CheckFutureSecret(self, request):
    """BOLT #2 - Message Retransmission
    Used to recover from local data loss by checking that our secret
    provided by the peer is correct.
    """
    node_id = request.node_id.data
    node = g_nodes.get(node_id)
    print("CheckFutureSecret node:%s" % node_id.hex())
    channel_nonce = request.channel_nonce.data
    channel = node.channels.get(channel_nonce)
    print("CheckFutureSecret channel:%s" % channel_nonce.hex())
    n = request.n
    suggested_secret = request.suggested.data

    element = shachain_derive(channel.shaelement, START_INDEX - n)

    reply = CheckFutureSecretReply()
    reply.correct = suggested_secret == element.secret
    return reply
