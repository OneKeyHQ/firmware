from trezor.messages.PingReply import PingReply
#from concurrent import futures
#import coincurve
#import time
#import grpc
# import binascii
# from .utils import sha256d, ecdh, shachain_derive, HKDF, derive_priv_key, ShachainElement, derive_blinded_privkey
# from .bip32 import BIP32
#import logging
# import hashlib
# import functools
# import traceback
# import sys
#
# from .bitcoinlib.transactions import Transaction
# from .bitcoinlib.keys import Key
# from . import remotesigner_pb2_grpc
# from . import remotesigner_pb2
#
# logging.basicConfig(
#     level=logging.INFO,
#     format='%(asctime)s - %(message)s')
# logger = logging.getLogger(__name__)
# logger.setLevel(logging.DEBUG)
#
# START_INDEX = 2**48 - 1
#
class Basepoints():
    def __init__(self):
        self.revocation = 1
        self.payment = 2
        self.htlc = 3
        self.delayed_payment = 4
        self.funding_pubkey = 5
        self.shaseed = None
        self.funding_pk = None
        self.funding_privkey = None
        self.revocation_basepoint_secret = None
        self.payment_basepoint_secret = None
        self.htlc_basepoint_secret = None
        self.delayed_payment_basepoint_secret = None
        self.keys = None


class Channel():
    def __init__(self):
        self.node_id = 1
        self.channel_nonce = 2
        self.is_outbound = None
        self.channel_value_sat = None
        self.local_to_self_delay = None
        self.remote_to_self_delay = None
        self.option_static_remotekey = None
        self.funding_privkey = None
        self.revocation_basepoint_secret = None
        self.payment_basepoint_secret = None
        self.htlc_basepoint_secret = None
        self.delayed_payment_basepoint_secret = None
        self.basepoints = None
        self.remote_basepoints = None
        self.remote_pubkey = None


class Keys():
    def __init__(self):
        self.funding_privkey = None
        self.revocation_basepoint_secret = None
        self.payment_basepoint_secret = None
        self.htlc_basepoint_secret = None
        self.delayed_payment_basepoint_secret = None
        self.shaseed = None


class Node():
    def __init__(self):
        self.chainparams = 1
        self.hsm_secret = None
        self.bip32_key = None
        self.node_pubkey = None
        self.node_privkey = None
        self.node_pk = None
        self.pubkey = None
        self.nodeid = None
        self.channels = {}
        self.channel_nonce = 0


def spend_type_to_string(spend_type):
    if spend_type == remotesigner_pb2.P2PKH:
        return 'p2pkh'
    elif spend_type == remotesigner_pb2.P2WPKH:
        return 'p2wpkh'
    elif spend_type == remotesigner_pb2.P2SH_P2WPKH:
        return 'p2sh_p2wpkh'
    else:
        return 'unknown'


# class Greeter(remotesigner_pb2_grpc.VersionServicer):
#     def Version(self, request, context):
#         return remotesigner_pb2.VersionReply(version_string='1')
#

# def stdout_exceptions(function):
#     """
#     A decorator that wraps the passed in function and logs
#     exceptions to the debug stream.
#     """
#
#     @functools.wraps(function)
#     def wrapper(*args, **kwargs):
#         try:
#             return function(*args, **kwargs)
#         except Exception as e:
#             traceback.print_exc(file=sys.stdout)
#             sys.stdout.flush()
#             raise e
#
#     return wrapper


class hsmd():
    # 实现 proto 文件中定义的 rpc 调用
    def __init__(self):
        self.nodes = {}
        self.network = "testnet"

    # def Init(self, request):
    #     print("Init.......", request)
    #     node = Node()
    #     node.hsm_secret = request.hsm_secret.data
    #     print("Init.......11111", context)
    #     # on chain wallet
    #     hkdf = HKDF(key=node.hsm_secret)
    #     r = hkdf.extract_key(info='bip32 seed'.encode(), length=32)
    #     logger.debug("bip32_key seed: %s" % r.hex())
    #     node.bip32_key = BIP32.from_seed(r, network=self.network)
    #     print("Init.......11111", context)
    #     # node pubkey, node msg sign
    #     r = hkdf.extract_key(info='nodeid'.encode(), length=32)
    #     node.node_privkey = r
    #     node.node_pk = r
    #     logger.info("new node privkey %s" % r.hex())
    #
    #     node_pk = coincurve.PrivateKey(secret=r)
    #     node.pubkey = node_pk.public_key.format()
    #     node.nodeid = binascii.hexlify(node.pubkey)
    #     logger.info("new node id %s" % node.nodeid)
    #
    #     # channel secret_base
    #     node.secret_base = hkdf.extract_key(
    #         info='peer seed'.encode(), length=32)
    #     logger.debug("new channel secret base %s" % node.secret_base.hex())
    #
    #     if not self.nodes.get(node.pubkey) or request.coldstart:
    #         self.nodes[node.pubkey] = node
    #
    #     reply = remotesigner_pb2.InitReply()
    #     reply.node_id.data = node.pubkey
    #     return reply
    #
    def Ping(self, request):
        """Trivial call to test connectivity
        """
        return PingReply(message="ping reply")
        return remotesigner_pb2.PingReply(message="ping reply")

    # def GetExtPubKey(self, request, context):
    #     """Get signing XPUB for a node.
    #     """
    #
    #     node_id = request.node_id.data
    #     logger.debug("GetExtPubKey node:%s" % node_id.hex())
    #     node = self.nodes.get(node_id)
    #     if node:
    #         xpub = remotesigner_pb2.ExtPubKey()
    #         xpub.encoded = node.bip32_key.get_xpub_from_path('m/0/0')
    #         logger.info(xpub)
    #
    #         return remotesigner_pb2.GetExtPubKeyReply(xpub=xpub)

    def NewChannel(self, request, context):

        print("")
        node_id = request.node_id.data
        node = self.nodes.get(node_id)
        logger.debug("NewChannel node %s" % node_id.hex())
        logger.debug("new channel nonce %s" % request.channel_nonce.data.hex())

        hkdf = HKDF(key=node.secret_base, salt=request.channel_nonce.data)
        node.channel_seed = hkdf.extract_key(
            info='per-peer seed'.encode(), length=32)
        logger.debug("new channel seed %s" % node.channel_seed.hex())

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

        revocation_basepoint_secret = coincurve.PrivateKey(
            secret=keys.revocation_basepoint_secret)
        basepoints.revocation = revocation_basepoint_secret.public_key.format()
        payment_basepoint_secret = coincurve.PrivateKey(
            secret=keys.payment_basepoint_secret)
        basepoints.payment = payment_basepoint_secret.public_key.format()
        htlc_basepoint_secret = coincurve.PrivateKey(
            secret=keys.htlc_basepoint_secret)
        basepoints.htlc = htlc_basepoint_secret.public_key.format()
        delayed_payment_basepoint_secret = coincurve.PrivateKey(
            secret=keys.delayed_payment_basepoint_secret)
        basepoints.delayed_payment = delayed_payment_basepoint_secret.public_key.format(
        )
        funding_privkey = coincurve.PrivateKey(secret=keys.funding_privkey)
        basepoints.funding_pubkey = funding_privkey.public_key.format()
        channel.funding_privkey = keys.funding_privkey
        channel.basepoints = basepoints

        logger.debug("basepoints.revocation %s" %
                     keys.revocation_basepoint_secret.hex())
        logger.debug(
            "basepoints.payment %s" % keys.payment_basepoint_secret.hex())
        logger.debug("basepoints.htlc %s" % keys.htlc_basepoint_secret.hex())
        logger.debug("basepoints.delayed_payment %s" %
                     keys.delayed_payment_basepoint_secret.hex())
        logger.debug("basepoints.funding_pk %s" % keys.funding_privkey.hex())
        logger.debug("basepoints.shaseed %s" % keys.shaseed.hex())

        logger.debug(
            "basepoints.revocation %s" % channel.basepoints.revocation.hex())
        logger.debug(
            "basepoints.payment %s" % channel.basepoints.payment.hex())
        logger.debug("basepoints.htlc %s" % channel.basepoints.htlc.hex())
        logger.debug("basepoints.delayed_payment %s" %
                     channel.basepoints.delayed_payment.hex())
        logger.debug("basepoints.funding_pubkey %s" %
                     channel.basepoints.funding_pubkey.hex())

        node.channels[request.channel_nonce.data] = channel
        logger.debug("new node:channel %s:%s" %
                     (node_id.hex(), request.channel_nonce.data.hex()))

        reply = remotesigner_pb2.NewChannelReply()
        reply.channel_nonce.data = channel_nonce

        return reply

    # def ReadyChannel(self, request, context):
    #     """BOLT #2 - Peer Protocol - memorize remote basepoints and funding outpoint
    #     Signatures can only be requested after this call.
    #     """
    #
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("ReadyChannel requestnode:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #     new_channel = not channel
    #     if new_channel:
    #         channel = Channel()
    #     channel.channel_nonce = request.channel_nonce.data
    #     channel.is_outbound = request.is_outbound
    #     channel.channel_value_sat = request.channel_value_sat
    #     channel.local_to_self_delay = request.local_to_self_delay
    #     channel.remote_to_self_delay = request.remote_to_self_delay
    #     channel.option_static_remotekey = request.option_static_remotekey
    #
    #     if not channel.remote_basepoints:
    #         basepoints = Basepoints()
    #     else:
    #         basepoints = channel.remote_basepoints
    #
    #     basepoints.revocation = request.remote_basepoints.revocation.data
    #     basepoints.payment = request.remote_basepoints.payment.data
    #     basepoints.htlc = request.remote_basepoints.htlc.data
    #     basepoints.delayed_payment = request.remote_basepoints.delayed_payment.data
    #     basepoints.funding_pubkey = request.remote_basepoints.funding_pubkey.data
    #
    #     channel.remote_basepoints = basepoints
    #     node.channels[channel.channel_nonce] = channel
    #
    #     reply = remotesigner_pb2.ReadyChannelReply()
    #     return reply
    #
    def SignMutualCloseTx(self, request, context):
        """BOLT #2 - Channel Close - phase 1
        """
        node_id = request.node_id.data
        node = self.nodes.get(node_id)
        logger.debug("SignMutualCloseTx node:%s" % node_id.hex())

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

        reply = remotesigner_pb2.SignatureReply()
        logger.debug("channel.basepoints.funding_pk %s" %
                     channel.basepoints.funding_pk.hex())
        reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
        ) + b'\x01'
        return reply

    # def SignMutualCloseTxPhase2(self, request, context):
    #     """BOLT #2 - Channel Close - phase 2
    #     """
    #     node_id = request.node_id.data
    #     logger.debug("SignMutualCloseTxPhase2 node:%s" % node_id.hex())
    #     reply = remotesigner_pb2.CloseTxSignatureReply()
    #     return reply
    #
    def CheckFutureSecret(self, request, context):
        """BOLT #2 - Message Retransmission
        Used to recover from local data loss by checking that our secret
        provided by the peer is correct.
        """
        node_id = request.node_id.data
        node = self.nodes.get(node_id)
        logger.debug("CheckFutureSecret node:%s" % node_id.hex())
        channel_nonce = request.channel_nonce.data
        channel = node.channels.get(channel_nonce)
        logger.debug("CheckFutureSecret channel:%s" % channel_nonce.hex())
        n = request.n
        suggested_secret = request.suggested.data

        element = shachain_derive(channel.shaelement, START_INDEX - n)

        reply = remotesigner_pb2.CheckFutureSecretReply()
        reply.correct = suggested_secret == element.secret
        return reply

    # def GetChannelBasepoints(self, request, context):
    #     """BOLT #3 - Key Derivation
    #     Get our channel basepoints and funding pubkey
    #     """
    #
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("GetChannelBasepoints node:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #     logger.debug("GetChannelBasepoints channel:%s" % channel_nonce.hex())
    #
    #     if channel is None:
    #         raise ("channel is None")
    #
    #     reply = remotesigner_pb2.GetChannelBasepointsReply()
    #     reply.basepoints.revocation.data = channel.basepoints.revocation
    #     reply.basepoints.payment.data = channel.basepoints.payment
    #     reply.basepoints.htlc.data = channel.basepoints.htlc
    #     reply.basepoints.delayed_payment.data = channel.basepoints.delayed_payment
    #     reply.basepoints.funding_pubkey.data = channel.basepoints.funding_pubkey
    #
    #     return reply
    #
    # def GetPerCommitmentPoint(self, request, context):
    #     """BOLT #3 - Per-commitment Secret Requirements
    #     Get our current per-commitment point and the secret for the point
    #     at commitment n-2
    #     """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("GetPerCommitmentPoint node:%s" % node_id.hex())
    #     #
    #     # channel_nonce = request.channel_nonce.data
    #     # logger.debug(
    #     #     "GetPerCommitmentPoint channel_nonce:%s" % channel_nonce.hex())
    #     # channel = node.channels.get(channel_nonce)
    #     #
    #     # n = request.n
    #     #
    #     # reply = remotesigner_pb2.GetPerCommitmentPointReply()
    #     #
    #     # if n >= 2:
    #     #     old_element = shachain_derive(channel.shaelement,
    #     #                                   START_INDEX - n + 2)
    #     #     reply.old_secret.data = bytes(old_element.secret)
    #     #     logger.debug("GetPerCommitmentPoint old_secret:%s, %d" %
    #     #                  (reply.old_secret.data.hex(), old_element.index))
    #     # else:
    #     #     reply.old_secret.data = b''
    #     #
    #     # element = shachain_derive(channel.shaelement, START_INDEX - n)
    #     # logger.debug("GetPerCommitmentPoint n:%s" % request.n)
    #     # logger.debug("GetPerCommitmentPoint secret:%s, %d" % (bytes(
    #     #     element.secret).hex(), element.index))
    #     # per_commitment_point_prv = coincurve.PrivateKey(
    #     #     secret=bytes(element.secret))
    #     # reply.per_commitment_point.data = per_commitment_point_prv.public_key.format(
    #     # )
    #     #
    #     # return reply
    #
    # def SignFundingTx(self, request, context):
    #     """BOLT #3 - Funding Transaction
    #     Sign the funding transaction
    #     TODO this must be done after we know the remote signature on the
    #     commitment tx - need an API call that provides this to the
    #     signer.
    #     """
    #
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("SignFundingTx node:%s" % node_id)
    #
    #     xpub = node.bip32_key.get_xpriv_from_path('m/0/0')
    #     onchain_bip32_key = BIP32.from_xpriv(xpub)
    #
    #     tx = request.tx
    #     tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #
    #     reply = remotesigner_pb2.SignFundingTxReply()
    #     witnesses = remotesigner_pb2.Witness()
    #
    #     local_prv = None
    #     local_key = None
    #     logger.debug(request)
    #     logger.debug(request.tx.raw_tx_bytes.hex())
    #     for i, input_desc in enumerate(request.tx.input_descs):
    #         key_index = input_desc.key_loc.key_index
    #         spend_type = input_desc.spend_type
    #         amount = input_desc.prev_output.value_sat
    #         channel_nonce = input_desc.close_info.channel_nonce.data
    #         commitment_point = input_desc.close_info.commitment_point.data
    #         channel = node.channels.get(channel_nonce)
    #
    #         if key_index != 0:
    #             local_prv = onchain_bip32_key.get_privkey_from_path(
    #                 "m/%d" % key_index)
    #             local_key = Key(import_key=local_prv, is_private=True)
    #         elif channel:
    #             if commitment_point:
    #                 local_priv_key = derive_priv_key(
    #                     commitment_point, payment_basepoint,
    #                     channel.basepoints.keys.payment_basepoint_secret)
    #                 local_key = Key(import_key=local_priv_key, is_private=True)
    #             else:
    #                 local_key = Key(
    #                     import_key=channel.basepoints.keys.
    #                     payment_basepoint_secret,
    #                     is_private=True)
    #         else:
    #             witnesses.signature.data = b''
    #             witnesses.pubkey.data = b''
    #             reply.witnesses.extend([witnesses])
    #             continue
    #
    #         spend_type = spend_type_to_string(spend_type)
    #         tx.inputs[i].script_type = spend_type
    #         logger.debug("funding signature: %s" % spend_type)
    #         tx.inputs[i].witness_type = "segwit"
    #         tx.inputs[i].sigs_required = 1
    #         tx.inputs[i].value = amount
    #         if spend_type == 'p2pkh':
    #             tx.witness_type = 'legacy'
    #         else:
    #             tx.witness_type = 'segwit'
    #         tx.sign(keys=[local_key], tid=i)
    #
    #         witnesses.signature.data = tx.inputs[i].witnesses[0]
    #         witnesses.pubkey.data = tx.inputs[i].witnesses[1]
    #
    #         logger.debug("funding signature: %s" %
    #                      tx.inputs[i].signatures[0].as_der_encoded().hex())
    #         logger.debug("funding public key %s" %
    #                      tx.inputs[i].as_dict()['public_keys'])
    #         reply.witnesses.extend([witnesses])
    #     logger.debug(reply)
    #     return reply
    #
    # def SignRemoteCommitmentTx(self, request, context):
    #     """BOLT #3 - Commitment Transaction, phase 1
    #     Sign the remote commitment tx, at commitment time
    #     """
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("SignRemoteCommitmentTx node:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #
    #     tx = request.tx
    #     amount = tx.input_descs[0].prev_output.value_sat
    #
    #     reply = remotesigner_pb2.SignatureReply()
    #
    #     local_key = Key(import_key=channel.funding_privkey, is_private=True)
    #     remote_pub = channel.remote_basepoints.funding_pubkey
    #     remote_pubkey = Key(
    #         import_key=channel.remote_basepoints.funding_pubkey)
    #
    #     pubs = sorted([remote_pub, local_key.public_compressed_byte])
    #     if remote_pub == pubs[0]:
    #         keys = [remote_pubkey, local_key]
    #     else:
    #         keys = [local_key, remote_pubkey]
    #
    #     tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     tx.inputs[0].script_type = "p2wsh"
    #     tx.inputs[0].witness_type = 'segwit'
    #     tx.inputs[0].sigs_required = 2
    #     tx.inputs[0].value = amount
    #     tx.witness_type = 'segwit'
    #     tx.sign(keys=keys, tid=0)
    #
    #     logger.debug("channel.basepoints.funding_pk %s" %
    #                  channel.basepoints.funding_pk.hex())
    #     reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    #     ) + b'\x01'
    #     return reply
    #
    # def SignRemoteCommitmentTxPhase2(self, request, context):
    #     """BOLT #3 - Commitment Transaction and attached HTLCs, phase 2
    #     Sign the remote commitment tx and attached HTLCs, at commitment time
    #     """
    #     reply = remotesigner_pb2.SignFundingTxReply()
    #     return reply
    #
    # def SignCommitmentTx(self, request, context):
    #     """BOLT #3 - Commitment Transaction, , phase 1
    #     Sign the local commitment tx, at force-close time
    #     """
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("SignCommitmentTx node:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #
    #     remote_pub = channel.remote_basepoints.funding_pubkey
    #
    #     tx = request.tx
    #     amount = tx.input_descs[0].prev_output.value_sat
    #
    #     reply = remotesigner_pb2.SignatureReply()
    #
    #     local_key = Key(import_key=channel.funding_privkey, is_private=True)
    #     remote_pubkey = Key(import_key=remote_pub)
    #     pubs = sorted([remote_pub, local_key.public_compressed_byte])
    #     if remote_pub == pubs[0]:
    #         keys = [remote_pubkey, local_key]
    #     else:
    #         keys = [local_key, remote_pubkey]
    #
    #     tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     tx.inputs[0].script_type = "p2wsh"
    #     tx.inputs[0].witness_type = 'segwit'
    #     tx.inputs[0].sigs_required = 2
    #     tx.inputs[0].value = amount
    #     tx.witness_type = 'segwit'
    #     tx.sign(keys=keys, tid=0)
    #
    #     logger.debug("channel.basepoints.funding_pk %s" %
    #                  channel.basepoints.funding_pk.hex())
    #     reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    #     ) + b'\x01'
    #     return reply
    #
    # def SignLocalCommitmentTxPhase2(self, request, context):
    #     """BOLT #3 - Commitment Transaction and attached HTLCs, phase 2
    #     Sign the local commitment tx and attached HTLCs, at force-close time.
    #     No further commitments can be signed on this channel.
    #     """
    #     context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    #     context.set_details('Method not implemented!')
    #     raise NotImplementedError('Method not implemented!')
    #
    # def SignLocalHTLCTx(self, request, context):
    #     """BOLT #3 - HTLC Outputs, phase 1
    #     Sign an htlc-success tx spending a local HTLC output, assuming we
    #     know the preimage, at force-close time
    #     """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignLocalHTLCTx node:%s" % node_id.hex())
    #     #
    #     # channel_nonce = request.channel_nonce.data
    #     # channel = node.channels.get(channel_nonce)
    #     #
    #     # n = request.n
    #     #
    #     # tx = request.tx
    #     # amount = tx.input_descs[0].prev_output.value_sat
    #     # redeemscript = tx.input_descs[0].redeem_script
    #     #
    #     # reply = remotesigner_pb2.SignatureReply()
    #     #
    #     # htlc_basepoint = channel.basepoints.htlc
    #     #
    #     # element = shachain_derive(channel.shaelement, START_INDEX - n)
    #     # per_commitment_point_prv = coincurve.PrivateKey(
    #     #     secret=bytes(element.secret))
    #     # local_per_commit_point = per_commitment_point_prv.public_key.format(
    #     # )
    #     #
    #     # local_priv_key = derive_priv_key(
    #     #     local_per_commit_point, htlc_basepoint,
    #     #     channel.basepoints.keys.htlc_basepoint_secret)
    #     #
    #     # local_key = Key(import_key=local_priv_key, is_private=True)
    #     # keys = [local_key]
    #     #
    #     # tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     # tx.inputs[0].script_type = "p2wsh"
    #     # tx.inputs[0].witness_type = 'segwit'
    #     # tx.inputs[0].redeemscript = redeemscript
    #     # tx.inputs[0].value = amount
    #     # tx.witness_type = 'segwit'
    #     # tx.sign(keys=keys, tid=0)
    #     #
    #     # reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    #     # ) + b'\x01'
    #     # return reply
    #
    # def SignDelayedPaymentToUs(self, request, context):
    #     """BOLT #5 - Unilateral Close Handling, phase 1
    #     Sign a delayed to-local output - either from the commitment tx or
    #     from an HTLC, at force-close time
    #     """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignDelayedPaymentToUs node:%s" % node_id.hex())
    #     #
    #     # channel_nonce = request.channel_nonce.data
    #     # channel = node.channels.get(channel_nonce)
    #     #
    #     # n = request.n
    #     #
    #     # tx = request.tx
    #     # amount = tx.input_descs[0].prev_output.value_sat
    #     # redeemscript = tx.input_descs[0].redeem_script
    #     #
    #     # reply = remotesigner_pb2.SignatureReply()
    #     #
    #     # delayed_payment_basepoint = channel.basepoints.delayed_payment
    #     #
    #     # element = shachain_derive(channel.shaelement, START_INDEX - n)
    #     # per_commitment_point_prv = coincurve.PrivateKey(
    #     #     secret=bytes(element.secret))
    #     # local_per_commit_point = per_commitment_point_prv.public_key.format(
    #     # )
    #     #
    #     # local_priv_key = derive_priv_key(
    #     #     local_per_commit_point, delayed_payment_basepoint,
    #     #     channel.basepoints.keys.delayed_payment_basepoint_secret)
    #     #
    #     # local_key = Key(import_key=local_priv_key, is_private=True)
    #     # keys = [local_key]
    #     #
    #     # tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     # tx.inputs[0].script_type = "p2wsh"
    #     # tx.inputs[0].witness_type = 'segwit'
    #     # tx.inputs[0].redeemscript = redeemscript
    #     # tx.inputs[0].value = amount
    #     # tx.witness_type = 'segwit'
    #     # tx.sign(keys=keys, tid=0)
    #     #
    #     # reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    #     # ) + b'\x01'
    #     # return reply
    #
    # def SignRemoteHTLCTx(self, request, context):
    #     """BOLT #3 - HTLC Outputs, phase 1
    #     Sign a remote HTLC tx, at commitment time
    #     """
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("SignRemoteCommitmentTx node:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #
    #     remote_per_commit_point = request.remote_per_commit_point.data
    #
    #     tx = request.tx
    #     amount = tx.input_descs[0].prev_output.value_sat
    #     redeemscript = tx.input_descs[0].redeem_script
    #     index = request.input
    #
    #     reply = remotesigner_pb2.SignatureReply()
    #
    #     htlc_key = Key(
    #         import_key=channel.basepoints.keys.htlc_basepoint_secret,
    #         is_private=True)
    #     htlc_basepoint = htlc_key.public_compressed_byte
    #
    #     local_priv_key = derive_priv_key(
    #         remote_per_commit_point, htlc_basepoint,
    #         channel.basepoints.keys.htlc_basepoint_secret)
    #
    #     local_key = Key(import_key=local_priv_key, is_private=True)
    #     keys = [local_key]
    #
    #     tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     tx.inputs[index].script_type = "p2wsh"
    #     tx.inputs[index].witness_type = 'segwit'
    #     tx.inputs[index].redeemscript = redeemscript
    #     tx.inputs[index].value = amount
    #     tx.witness_type = 'segwit'
    #     tx.sign(keys=keys, tid=index)
    #
    #     logger.debug(
    #         "remote_per_commit_point %s" % remote_per_commit_point.hex())
    #     logger.debug("channel.basepoints.funding_pk %s" %
    #                  channel.basepoints.funding_pk.hex())
    #     reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    #     ) + b'\x01'
    #     return reply
    #
    # def SignRemoteHTLCToUs(self, request, context):
    #     """BOLT #3 - HTLC Outputs, phase 1 Sign an htlc-success tx on a
    #     remote HTLC output offered to us, assuming we know the preimage,
    #     at force-close time
    #     """
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("SignRemoteHTLCToUs node:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #
    #     remote_per_commit_point = request.remote_per_commit_point.data
    #
    #     tx = request.tx
    #     amount = tx.input_descs[0].prev_output.value_sat
    #     redeemscript = tx.input_descs[0].redeem_script
    #
    #     reply = remotesigner_pb2.SignatureReply()
    #
    #     htlc_key = Key(
    #         import_key=channel.basepoints.keys.htlc_basepoint_secret,
    #         is_private=True)
    #     htlc_basepoint = htlc_key.public_compressed_byte
    #
    #     local_priv_key = derive_priv_key(
    #         remote_per_commit_point, htlc_basepoint,
    #         channel.basepoints.keys.htlc_basepoint_secret)
    #
    #     local_key = Key(import_key=local_priv_key, is_private=True)
    #     keys = [local_key]
    #
    #     tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     tx.inputs[0].script_type = "p2wsh"
    #     tx.inputs[0].witness_type = 'segwit'
    #     tx.inputs[0].redeemscript = redeemscript
    #     tx.inputs[0].value = amount
    #     tx.witness_type = 'segwit'
    #     tx.sign(keys=keys, tid=0)
    #
    #     logger.debug(
    #         "remote_per_commit_point %s" % remote_per_commit_point.hex())
    #     logger.debug("channel.basepoints.funding_pk %s" %
    #                  channel.basepoints.funding_pk.hex())
    #     reply.signature.data = tx.inputs[0].signatures[0].as_der_encoded(
    #     ) + b'\x01'
    #     return reply
    #
    # def SignPenaltyToUs(self, request, context):
    #     """BOLT #5 - Unilateral Close Handling, phase 1
    #     Sign a penalty tx to us - either sweeping the to-local commitment
    #     tx output or sweeping an HTLC tx output, after the remote
    #     broadcast a revoked commitment transaction.
    #     """
    #     node_id = request.node_id.data
    #     node = self.nodes.get(node_id)
    #     logger.debug("SignPenaltyToUs node:%s" % node_id.hex())
    #
    #     channel_nonce = request.channel_nonce.data
    #     channel = node.channels.get(channel_nonce)
    #
    #     revocation_secret = request.revocation_secret.data
    #
    #     tx = request.tx
    #     amount = tx.input_descs[0].prev_output.value_sat
    #     redeemscript = tx.input_descs[0].redeem_script
    #     index = request.input
    #
    #     revocationprivkey = derive_blinded_privkey(
    #         revocation_secret,
    #         channel.basepoints.keys.revocation_basepoint_secret)
    #
    #     local_key = Key(import_key=revocationprivkey, is_private=True)
    #     keys = [local_key]
    #
    #     tx = Transaction.import_raw(tx.raw_tx_bytes, self.network)
    #     tx.inputs[index].script_type = "p2wsh"
    #     tx.inputs[index].witness_type = 'segwit'
    #     tx.inputs[index].redeemscript = redeemscript
    #     tx.inputs[index].value = amount
    #     tx.witness_type = 'segwit'
    #     tx.sign(keys=keys, tid=index)
    #
    #     reply = remotesigner_pb2.SignatureReply()
    #     reply.signature.data = tx.inputs[index].signatures[0].as_der_encoded(
    #     ) + b'\x01'
    #
    #     return reply
    #
    # def SignChannelAnnouncement(self, request, context):
    #     """BOLT #7 - channel_announcement
    #   """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignChannelAnnouncement node:%s" % node_id)
    #     #
    #     # channel_nonce = request.channel_nonce.data
    #     # channel = node.channels.get(channel_nonce)
    #     #
    #     # msg = request.channel_announcement
    #     # node_key = coincurve.PrivateKey(secret=node.node_privkey)
    #     # node_signature = node_key.sign(message=msg, hasher=sha256d)
    #     #
    #     # bitcoin_key = coincurve.PrivateKey(secret=channel.funding_privkey)
    #     # bitcoin_signature = bitcoin_key.sign(message=msg, hasher=sha256d)
    #     #
    #     # reply = remotesigner_pb2.SignChannelAnnouncementReply()
    #     # reply.node_signature.data = node_signature
    #     # reply.bitcoin_signature.data = bitcoin_signature
    #     # return reply
    #
    # def SignNodeAnnouncement(self, request, context):
    #     """BOLT #7 - node_announcement
    #     """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignNodeAnnouncement node:%s" % node_id)
    #     #
    #     # msg = request.node_announcement
    #     # node_key = coincurve.PrivateKey(secret=node.node_privkey)
    #     # signature = node_key.sign(message=msg, hasher=sha256d)
    #     #
    #     # reply = remotesigner_pb2.NodeSignatureReply()
    #     # reply.signature.data = signature
    #     # return reply
    #
    # def SignChannelUpdate(self, request, context):
    #     """BOLT #7 - channel_update
    #   """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignChannelUpdate node:%s" % node_id.hex())
    #     #
    #     # msg = request.channel_update
    #     # logger.debug("SignChannelUpdate msg:%s" % msg.hex())
    #     # node_key = coincurve.PrivateKey(secret=node.node_privkey)
    #     # signature = node_key.sign(message=msg, hasher=sha256d)
    #     #
    #     # reply = remotesigner_pb2.NodeSignatureReply()
    #     # reply.signature.data = signature
    #     # return reply
    #
    # def ECDH(self, request, context):
    #     """BOLT #8 - Authenticated Key Agreement Handshake
    #     """
    #
    #     node_id = request.node_id.data
    #     logger.debug("ECDH node:%s" % node_id.hex())
    #     logger.debug("ECDH point:%s" % request.point.data.hex())
    #     node = self.nodes.get(node_id)
    #     if node:
    #         compress_pub = request.point.data
    #         reply = remotesigner_pb2.ECDHReply()
    #         reply.shared_secret.data = ecdh(node.node_privkey, compress_pub)
    #         return reply
    #
    # def SignInvoice(self, request, context):
    #     """BOLT #11 - Invoice Protocol
    #     https://github.com/lightningnetwork/lightning-rfc/blob/master/11-payment-encoding.md
    #     """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignInvoice node:%s" % node_id.hex())
    #     #
    #     # data = request.data_part
    #     # hrp = request.human_readable_part
    #     #
    #     # import lnaddr
    #     # lnaddr.lndecode(hrp, data, verbose=True)
    #     #
    #     # node_key = coincurve.PrivateKey(secret=node.node_privkey)
    #     # logger.debug(
    #     #     "SignInvoice node pubkey:%s" % node_key.public_key.format().hex())
    #     # signature = node_key.sign_recoverable(
    #     #     message=lnaddr.data_cov(hrp, data))
    #     #
    #     # reply = remotesigner_pb2.RecoverableNodeSignatureReply()
    #     # reply.signature.data = signature
    #     # return reply
    #
    # def SignMessage(self, request, context):
    #     """BOLT #?? - Sign Message
    #   """
    #     print("")
    #     # node_id = request.node_id.data
    #     # node = self.nodes.get(node_id)
    #     # logger.debug("SignMessage node:%s" % node_id.hex())
    #     #
    #     # # From https://twitter.com/rusty_twit/status/1182102005914800128:
    #     # # @roasbeef & @bitconner point out that #lnd algo is:
    #     # #   zbase32(SigRec(SHA256(SHA256("Lightning Signed Message:" + msg)))).
    #     # # zbase32 from https://philzimmermann.com/docs/human-oriented-base-32-encoding.txt
    #     # # and SigRec has first byte 31 + recovery id, followed by 64 byte sig.
    #     # # #specinatweet */
    #     # msg = b'Lightning Signed Message:' + request.message
    #     #
    #     # node_key = coincurve.PrivateKey(secret=node.node_privkey)
    #     # signature = node_key.sign_recoverable(message=msg, hasher=sha256d)
    #     #
    #     # reply = remotesigner_pb2.RecoverableNodeSignatureReply()
    #     # reply.signature.data = signature
    #     #
    #     # return reply
    #

# def serve():
#     # create grpc server
#     server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
#
#     # version server
#     remotesigner_pb2_grpc.add_VersionServicer_to_server(Greeter(), server)
#
#     # signer server
#     remotesigner_pb2_grpc.add_SignerServicer_to_server(SignGreeter(), server)
#
#     server.add_insecure_port('[::]:50051')
#     server.start()
#     try:
#         while True:
#             time.sleep(60 * 60 * 24)  # one day in seconds
#     except KeyboardInterrupt:
#         server.stop(0)
#
#
# if __name__ == '__main__':
#     serve()
