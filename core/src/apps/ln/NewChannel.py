from trezor.messages.NewChannelReply import NewChannelReply
from .utils import HKDF
from trezor.crypto.curve import secp256k1
from .common import Node, Keys, Channel, Basepoints
from .utils import ShachainElement
from .common import g_nodes

async def NewChannel(self, request):
    print("")
    node_id = request.node_id.data
    node = g_nodes.get(node_id)
    print("NewChannel node %s" % node_id.hex())
    print("new channel nonce %s" % request.channel_nonce.data.hex())

    hkdf = HKDF(key=node.secret_base, salt=request.channel_nonce.data)
    node.channel_seed = hkdf.extract_key(
        info='per-peer seed'.encode(), length=32)
    print("new channel seed %s" % node.channel_seed.hex())

    channel_nonce = request.channel_nonce.data
    channel = node.channels.get(channel_nonce)
    if not channel:
        channel = Channel()
    keys = Keys()
    hkdf = HKDF(key=node.channel_seed)
    r = hkdf.extract_key(info='c-lightning'.encode(), length=32 * 6)
    keys.funding_privkey = r[:32]
    keys.revocation_basepoint_secret = r[32:64]
    keys.payment_basepoint_secret = r[64:96]
    keys.htlc_basepoint_secret = r[96:128]
    keys.delayed_payment_basepoint_secret = r[128:160]
    keys.shaseed = r[160:192]

    basepoints = Basepoints()
    basepoints.funding_pk = keys.funding_privkey
    basepoints.keys = keys

    basepoints.shaseed = keys.shaseed
    channel.shaelement = ShachainElement(index=0, secret=keys.shaseed)

    basepoints.revocation = secp256k1.publickey(unhexlify(keys.revocation_basepoint_secret), True)
    basepoints.payment = secp256k1.publickey(unhexlify(keys.payment_basepoint_secret), True)
    basepoints.htlc = secp256k1.publickey(unhexlify(keys.htlc_basepoint_secret), True)
    basepoints.delayed_payment = secp256k1.publickey(unhexlify(keys.delayed_payment_basepoint_secret), True)
    basepoints.funding_pubkey = secp256k1.publickey(unhexlify(keys.funding_privkey), True)
    channel.funding_privkey = keys.funding_privkey
    channel.basepoints = basepoints

    print("basepoints.revocation %s" %
                 keys.revocation_basepoint_secret.hex())
    print(
        "basepoints.payment %s" % keys.payment_basepoint_secret.hex())
    print("basepoints.htlc %s" % keys.htlc_basepoint_secret.hex())
    print("basepoints.delayed_payment %s" %
                 keys.delayed_payment_basepoint_secret.hex())
    print("basepoints.funding_pk %s" % keys.funding_privkey.hex())
    print("basepoints.shaseed %s" % keys.shaseed.hex())

    print(
        "basepoints.revocation %s" % channel.basepoints.revocation.hex())
    print(
        "basepoints.payment %s" % channel.basepoints.payment.hex())
    print("basepoints.htlc %s" % channel.basepoints.htlc.hex())
    print("basepoints.delayed_payment %s" %
                 channel.basepoints.delayed_payment.hex())
    print("basepoints.funding_pubkey %s" %
                 channel.basepoints.funding_pubkey.hex())

    node.channels[request.channel_nonce.data] = channel
    print("new node:channel %s:%s" %
                 (node_id.hex(), request.channel_nonce.data.hex()))

    reply = NewChannelReply()
    reply.channel_nonce.data = channel_nonce

    return reply
