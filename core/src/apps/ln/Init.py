from trezor.messages.InitReply import InitReply
from trezor.crypto.curve import secp256k1
import ubinascii as binascii
from .utils import HKDF
from . import bip32
from .common import Node, g_nodes

async def Init(self, request):
    print("Init.......", request)
    node = Node()
    node.hsm_secret = request.hsm_secret.data
    # on chain wallet
    hkdf = HKDF(key=node.hsm_secret)
    r = hkdf.extract_key(info='bip32 seed'.encode(), length=32)
    #logger.debug("bip32_key seed: %s" % r.hex())
    node.bip32_key = bip32.from_seed(r, network=self.network)
    # node pubkey, node msg sign
    r = hkdf.extract_key(info='nodeid'.encode(), length=32)
    node.node_privkey = r
    node.node_pk = r

    # node_pk = coincurve.PrivateKey(secret=r)
    # node.pubkey = node_pk.public_key.format()
    node.pubkey = secp256k1.publickey(unhexlify(r), True)

    node.nodeid = binascii.hexlify(node.pubkey)
    print("new node id %s" % node.nodeid)

    # channel secret_base
    node.secret_base = hkdf.extract_key(
        info='peer seed'.encode(), length=32)
    print("new channel secret base %s" % node.secret_base.hex())

    if not g_nodes.get(node.pubkey) or request.coldstart:
        g_nodes[node.pubkey] = node

    reply = InitReply()
    reply.node_id.data = node.pubkey
    return reply
