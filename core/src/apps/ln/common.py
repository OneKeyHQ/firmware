import functools
import traceback
import sys

INVALID = 0
P2PKH = 1
P2WPKH = 3
P2SH_P2WPKH = 4

START_INDEX = 2**48 - 1

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
    if spend_type == P2PKH:
        return 'p2pkh'
    elif spend_type == P2WPKH:
        return 'p2wpkh'
    elif spend_type == P2SH_P2WPKH:
        return 'p2sh_p2wpkh'
    else:
        return 'unknown'

def stdout_exceptions(function):
    """
    A decorator that wraps the passed in function and logs
    exceptions to the debug stream.
    """

    @functools.wraps(function)
    def wrapper(*args, **kwargs):
        try:
            return function(*args, **kwargs)
        except Exception as e:
            traceback.print_exc(file=sys.stdout)
            sys.stdout.flush()
            raise e

    return wrapper

g_nodes = {}
g_newwork = "testnet"
