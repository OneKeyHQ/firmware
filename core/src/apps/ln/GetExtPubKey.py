from trezor.messages.ExtPubKey import ExtPubKey
from trezor.messages.GetExtPubKeyReply import GetExtPubKeyReply

async def GetExtPubKey(self, request):
    """Get signing XPUB for a node.
    """

    node_id = request.node_id.data
    print("GetExtPubKey node:%s" % node_id.hex())
    node = self.nodes.get(node_id)
    if node:
        #xpub = remotesigner_pb2.ExtPubKey()
        xpub = ExtPubKey()
        xpub.encoded = node.bip32_key.get_xpub_from_path('m/0/0')
        #logger.info(xpub)

        return GetExtPubKeyReply(xpub=xpub)