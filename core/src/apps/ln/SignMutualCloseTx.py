from trezor.messages.SignatureReply import SignatureReply
from .bitcoinlib.transactions import Transaction
from .bitcoinlib.keys import Key
from .common import g_nodes

async def SignMutualCloseTx(self, request):
    """BOLT #2 - Channel Close - phase 1
    """
    node_id = request.node_id.data
    node = g_nodes.get(node_id)
    print("SignMutualCloseTx node:%s" % node_id.hex())

    channel_nonce = request.channel_nonce.data
    channel = node.channels.get(channel_nonce)

    tx = request.tx
    amount = tx.input_descs[0].prev_output.value_sat

    tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    local_key = Key(import_key=channel.funding_privkey, is_private=True)
    remote_pub = channel.remote_basepoints.funding_pubkey
    remote_pubkey = Key(import_key=remote_pub)
    pubs = sorted([remote_pub, local_key.public_compressed_byte])
    if remote_pub == pubs[0]:
        keys = [remote_pubkey, local_key]
    else:
        keys = [local_key, remote_pubkey]

    tx.inputs[0].script_type = "p2wsh"
    tx.inputs[0].witness_type = 'segwit'
    tx.inputs[0].sigs_required = 2
    tx.inputs[0].value = amount
    tx.witness_type = 'segwit'
    tx.sign(keys=keys, tid=0)

    reply = SignatureReply()
    print("channel.basepoints.funding_pk %s" %
                 channel.basepoints.funding_pk.hex())
    reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    ) + b'\x01'
    return reply
