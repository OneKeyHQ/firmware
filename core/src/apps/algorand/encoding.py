from ubinascii import b2a_base64

from trezor.crypto import base32, sha512_256

from . import transactions, umsgpack


def msgpack_encode(obj):
    """
    Encode the object using canonical msgpack.

    Args:
        obj (Transaction, SignedTransaction, MultisigTransaction, Multisig,\
            Bid, or SignedBid): object to be encoded

    Returns:
        str: msgpack encoded object

    Note:
        Canonical Msgpack: maps must contain keys in lexicographic order; maps
        must omit key-value pairs where the value is a zero-value; positive
        integer values must be encoded as "unsigned" in msgpack, regardless of
        whether the value space is semantically signed or unsigned; integer
        values must be represented in the shortest possible encoding; binary
        arrays must be represented using the "bin" format family (that is, use
        the most recent version of msgpack rather than the older msgpack
        version that had no "bin" family).
    """
    d = obj
    if not isinstance(obj, dict):
        d = obj.dictify()
    # only for estimate_size
    # od = _sort_dict(d)
    return b2a_base64(umsgpack.dumps(d))[:-1].decode()


# def _sort_dict(d):
#     """
#     Sorts a dictionary recursively and removes all zero values.

#     Args:
#         d (dict): dictionary to be sorted

#     Returns:
#         OrderedDict: sorted dictionary with no zero values
#     """
#     od = OrderedDict()
#     for k, v in sorted(d.items()):
#         if isinstance(v, dict):
#             od[k] = _sort_dict(v)
#         elif v:
#             od[k] = v
#     return od


def future_msgpack_decode(enc):
    """
    Decode a msgpack encoded object from a string.

    Args:
        enc (str): string to be decoded

    Returns:
        Transaction, SignedTransaction, Multisig, Bid, or SignedBid:\
            decoded object
    """
    decoded = umsgpack.loads(enc)
    if type(decoded) is dict:
        if "type" in decoded:
            return transactions.transaction.Transaction.undictify(decoded)
    return None


def encode_address(addr_bytes):
    """
    Encode a byte address into a string composed of the encoded bytes and the
    checksum.

    Args:
        addr_bytes (bytes): address in bytes

    Returns:
        str: base32 encoded address
    """
    digest = checksum(addr_bytes)
    chksum = digest[28:32]
    return base32.encode(addr_bytes + chksum).replace("=", "")


def decode_address(addr):
    """
    Decode a string address into its address bytes and checksum.

    Args:
        addr (str): base32 address

    Returns:
        bytes: address decoded into bytes

    """
    if not addr:
        return addr
    if not len(addr) == transactions.constants.address_len:
        raise transactions.error.WrongKeyLengthError
    decoded = base32.decode(addr)
    addr = decoded[: -transactions.constants.check_sum_len_bytes]
    expected_checksum = decoded[-transactions.constants.check_sum_len_bytes :]

    digest = checksum(addr)
    chksum = digest[28:32]

    if chksum == expected_checksum:
        return addr
    else:
        raise transactions.error.WrongChecksumError


def checksum(data):
    """
    Compute the checksum of arbitrary binary input.

    Args:
        data (bytes): data as bytes

    Returns:
        bytes: checksum of the data
    """
    digest = sha512_256.digest(data)
    return digest[:32]
