import base58
import hashlib
import hmac

from bip32_utils import (
    HARDENED_INDEX, _derive_hardened_private_child,
    _derive_unhardened_private_child, _derive_public_child,
    _serialize_extended_key, _unserialize_extended_key,
    _hardened_index_in_path, _privkey_to_pubkey, _deriv_path_str_to_list
)


class BIP32:
    def __init__(self, chaincode, privkey=None, pubkey=None, fingerprint=None,
                 depth=0, index=0, network="main"):
        """
        :param chaincode: The master chaincode, used to derive keys. As bytes.
        :param privkey: The master private key for this index (default 0).
                        Can be None for pubkey-only derivation.
                        As bytes.
        :param pubkey: The master public key for this index (default 0).
                       Can be None if private key is specified.
                       Compressed format. As bytes.
        :param fingeprint: If we are instanciated from an xpub/xpriv, we need
                           to remember the parent's pubkey fingerprint to
                           reserialize !
        :param depth: If we are instanciated from an existing extended key, we
                      need this for serialization.
        :param index: If we are instanciated from an existing extended key, we
                      need this for serialization.
        :param network: Either "main" or "test".
        """
        assert isinstance(chaincode, bytes)
        assert privkey is not None or pubkey is not None
        if privkey is not None:
            assert isinstance(privkey, bytes)
        if pubkey is not None:
            assert isinstance(pubkey, bytes)
        else:
            pubkey = _privkey_to_pubkey(privkey)
        self.master_chaincode = chaincode
        self.master_privkey = privkey
        self.master_pubkey = pubkey
        self.parent_fingerprint = fingerprint
        self.depth = depth
        self.index = index
        self.network = network

    def get_extended_privkey_from_path(self, path):
        """Get an extended privkey from a derivation path.

        :param path: A list of integers (index of each depth) or a string with
                     m/x/x'/x notation. (e.g. m/0'/1/2'/2 or m/0H/1/2H/2).
        :return: chaincode (bytes), privkey (bytes)
        """
        if isinstance(path, str):
            path = _deriv_path_str_to_list(path)
        chaincode, privkey = self.master_chaincode, self.master_privkey
        for index in path:
            if index & HARDENED_INDEX:
                privkey, chaincode = \
                    _derive_hardened_private_child(privkey, chaincode, index)
            else:
                privkey, chaincode = \
                    _derive_unhardened_private_child(privkey, chaincode, index)
        return chaincode, privkey

    def get_privkey_from_path(self, path):
        """Get a privkey from a derivation path.

        :param path: A list of integers (index of each depth) or a string with
                     m/x/x'/x notation. (e.g. m/0'/1/2'/2 or m/0H/1/2H/2).
        :return: privkey (bytes)
        """
        return self.get_extended_privkey_from_path(path)[1]

    def get_extended_pubkey_from_path(self, path):
        """Get an extended pubkey from a derivation path.

        :param path: A list of integers (index of each depth) or a string with
                     m/x/x'/x notation. (e.g. m/0'/1/2'/2 or m/0H/1/2H/2).
        :return: chaincode (bytes), pubkey (bytes)
        """
        if isinstance(path, str):
            path = _deriv_path_str_to_list(path)
        chaincode, key = self.master_chaincode, self.master_privkey
        # We'll need the private key at some point anyway, so let's derive
        # everything from private keys.
        if _hardened_index_in_path(path):
            for index in path:
                if index & HARDENED_INDEX:
                    key, chaincode = \
                        _derive_hardened_private_child(key, chaincode, index)
                else:
                    key, chaincode = \
                        _derive_unhardened_private_child(key, chaincode, index)
                pubkey = _privkey_to_pubkey(key)
        # We won't need private keys for the whole path, so let's only use
        # public key derivation.
        else:
            key = self.master_pubkey
            for index in path:
                key, chaincode = \
                    _derive_public_child(key, chaincode, index)
                pubkey = key
        return chaincode, pubkey

    def get_pubkey_from_path(self, path):
        """Get a privkey from a derivation path.

        :param path: A list of integers (index of each depth) or a string with
                     m/x/x'/x notation. (e.g. m/0'/1/2'/2 or m/0H/1/2H/2).
        :return: privkey (bytes)
        """
        return self.get_extended_pubkey_from_path(path)[1]

    def get_xpriv_from_path(self, path):
        """Get an encoded extended privkey from a derivation path.

        :param path: A list of integers (index of each depth) or a string with
                     m/x/x'/x notation. (e.g. m/0'/1/2'/2 or m/0H/1/2H/2).
        :return: The encoded extended pubkey as str.
        """
        if isinstance(path, str):
            path = _deriv_path_str_to_list(path)
        if len(path) == 0:
            return self.get_master_xpriv()
        elif len(path) == 1:
            parent_pubkey = self.master_pubkey
        else:
            parent_pubkey = self.get_pubkey_from_path(path[:-1])
        chaincode, privkey = self.get_extended_privkey_from_path(path)
        extended_key = _serialize_extended_key(privkey, self.depth + len(path),
                                               parent_pubkey,
                                               path[-1], chaincode,
                                               self.network)
        return base58.b58encode_check(extended_key).decode()

    def get_xpub_from_path(self, path):
        """Get an encoded extended pubkey from a derivation path.

        :param path: A list of integers (index of each depth) or a string with
                     m/x/x'/x notation. (e.g. m/0'/1/2'/2 or m/0H/1/2H/2).
        :return: The encoded extended pubkey as str.
        """
        if isinstance(path, str):
            path = _deriv_path_str_to_list(path)
        if len(path) == 0:
            return self.get_master_xpub()
        elif len(path) == 1:
            parent_pubkey = self.master_pubkey
        else:
            parent_pubkey = self.get_pubkey_from_path(path[:-1])
        chaincode, pubkey = self.get_extended_pubkey_from_path(path)
        extended_key = _serialize_extended_key(pubkey, self.depth + len(path),
                                               parent_pubkey,
                                               path[-1], chaincode,
                                               self.network)
        return base58.b58encode_check(extended_key).decode()

    def get_master_xpriv(self):
        """Get the encoded extended private key of the master private key"""
        extended_key = _serialize_extended_key(self.master_privkey, self.depth,
                                               self.parent_fingerprint,
                                               self.index,
                                               self.master_chaincode,
                                               self.network)
        return base58.b58encode_check(extended_key).decode()

    def get_master_xpub(self):
        """Get the encoded extended public key of the master public key"""
        extended_key = _serialize_extended_key(self.master_pubkey, self.depth,
                                               self.parent_fingerprint,
                                               self.index,
                                               self.master_chaincode,
                                               self.network)
        return base58.b58encode_check(extended_key).decode()

    @classmethod
    def from_xpriv(cls, xpriv):
        """Get a BIP32 "wallet" out of this xpriv

        :param xpriv: (str) The encoded serialized extended private key.
        """
        extended_key = base58.b58decode_check(xpriv)
        (network, depth, fingerprint,
         index, chaincode, key) = _unserialize_extended_key(extended_key)
        # We need to remove the trailing `0` before the actual private key !!
        return BIP32(chaincode, key[1:], None, fingerprint, depth, index,
                     network)

    @classmethod
    def from_xpub(cls, xpub):
        """Get a BIP32 "wallet" out of this xpub

        :param xpub: (str) The encoded serialized extended public key.
        """
        extended_key = base58.b58decode_check(xpub)
        (network, depth, fingerprint,
         index, chaincode, key) = _unserialize_extended_key(extended_key)
        return BIP32(chaincode, None, key, fingerprint, depth, index, network)

    @classmethod
    def from_seed(cls, seed, network="main"):
        """Get a BIP32 "wallet" out of this seed (maybe after BIP39?)

        :param seed: The seed as bytes.
        """
        secret = hmac.new("Bitcoin seed".encode(), seed,
                          hashlib.sha512).digest()
        return BIP32(secret[32:], secret[:32], network=network)
