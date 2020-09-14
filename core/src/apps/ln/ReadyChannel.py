from trezor.messages.ReadyChannelReply import ReadyChannelReply
from .common import Node, Keys, Channel, Basepoints
from .common import Node, g_nodes

async def ReadyChannel(self, request):
    """BOLT #2 - Peer Protocol - memorize remote basepoints and funding outpoint
    Signatures can only be requested after this call.
    """

    node_id = request.node_id.data
    node = g_nodes.get(node_id)
    print("ReadyChannel requestnode:%s" % node_id.hex())

    channel_nonce = request.channel_nonce.data
    channel = node.channels.get(channel_nonce)
    new_channel = not channel
    if new_channel:
        channel = Channel()
    channel.channel_nonce = request.channel_nonce.data
    channel.is_outbound = request.is_outbound
    channel.channel_value_sat = request.channel_value_sat
    channel.local_to_self_delay = request.local_to_self_delay
    channel.remote_to_self_delay = request.remote_to_self_delay
    channel.option_static_remotekey = request.option_static_remotekey

    if not channel.remote_basepoints:
        basepoints = Basepoints()
    else:
        basepoints = channel.remote_basepoints

    basepoints.revocation = request.remote_basepoints.revocation.data
    basepoints.payment = request.remote_basepoints.payment.data
    basepoints.htlc = request.remote_basepoints.htlc.data
    basepoints.delayed_payment = request.remote_basepoints.delayed_payment.data
    basepoints.funding_pubkey = request.remote_basepoints.funding_pubkey.data

    channel.remote_basepoints = basepoints
    node.channels[channel.channel_nonce] = channel

    reply = ReadyChannelReply()
    return reply
