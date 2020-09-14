from trezor.crypto import hmac
from trezor.crypto.hashlib import sha256
import coincurve
#from typing import Union
from ucollections import namedtuple


def ecdh(k, rk):
    k = coincurve.PrivateKey(secret=k)
    rk = coincurve.PublicKey(data=rk)
    a = k.ecdh(rk.public_key)
    return a


def count_trailing_zeros(index):
    """ BOLT-03 (where_to_put_secret) """
    try:
        return list(reversed(bin(index)[2:])).index("1")
    except ValueError:
        return 48


class LightningError(Exception):
    pass


class UnableToDeriveSecret(LightningError):
    pass


def shachain_derive(element, to_index):
    def get_prefix(index, pos):
        mask = (1 << 64) - 1 - ((1 << pos) - 1)
        return index & mask

    from_index = element.index
    zeros = count_trailing_zeros(from_index)
    if from_index != get_prefix(to_index, zeros):
        raise UnableToDeriveSecret(
            "prefixes are different; index not derivable")
    return ShachainElement(
        get_per_commitment_secret_from_seed(element.secret, to_index, zeros),
        to_index)


def bh2u(x: bytes) -> str:
    """
    str with hex representation of a bytes-like object

    >>> x = bytes((1, 2, 10))
    >>> bh2u(x)
    '01020A'
    """
    return x.hex()


ShachainElement = namedtuple("ShachainElement", ["secret", "index"])
ShachainElement.__str__ = lambda self: "ShachainElement(" + bh2u(self.secret) + "," + str(self.index) + ")"

CURVE_ORDER = 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141


def get_per_commitment_secret_from_seed(seed: bytes, i: int,
                                        bits: int = 48) -> bytes:
    """Generate per commitment secret."""
    per_commitment_secret = bytearray(seed)
    for bitindex in range(bits - 1, -1, -1):
        mask = 1 << bitindex
        if i & mask:
            per_commitment_secret[bitindex // 8] ^= 1 << (bitindex % 8)
            per_commitment_secret = bytearray(
                sha256(per_commitment_secret).digest())
    return per_commitment_secret


def derive_public_key(per_commitment_point, base_point):
    k = sha256(per_commitment_point + base_point).digest()

    pub1 = coincurve.PrivateKey(secret=k).public_key
    pub2 = coincurve.PublicKey(data=base_point)
    pub = pub2.combine([pub1])

    #pub1 = secp256k1.PrivateKey(privkey=k, raw=True).pubkey
    #pub2 = secp256k1.PublicKey(base_point, raw=True)
    #pub = pub2.combine([pub1.public_key])

    return pub.format()


def derive_priv_key(per_commitment_point, basepoint, basepoint_secret):
    k = sha256(per_commitment_point + basepoint).digest()

    prv = int.from_bytes(
        basepoint_secret, 'big', signed=False) + int.from_bytes(
            k, 'big', signed=False)
    prv %= CURVE_ORDER

    return prv
    #return prv.to_bytes(32, "big")


def to_bytes(something, encoding='utf8') -> bytes:
    """
    cast string to bytes() like object, but for python2 support it's bytearray copy
    """
    if isinstance(something, bytes):
        return something
    if isinstance(something, str):
        return something.encode(encoding)
    elif isinstance(something, bytearray):
        return bytes(something)
    else:
        raise TypeError("Not a string or bytes like object")


# def sha256d(x: Union[bytes, str]) -> bytes:
#     x = to_bytes(x, 'utf8')
#     out = bytes(sha256(sha256(x).digest()).digest())
#     return out


def secret_to_pubkey(secret: int) -> bytes:
    return coincurve.PrivateKey(secret=secret).public_key.format()


def string_to_number(b: bytes) -> int:
    return int.from_bytes(b, byteorder='big', signed=False)


def derive_privkey(secret, per_commitment_point: bytes) -> int:
    basepoint_bytes = secret_to_pubkey(secret)
    basepoint = int.from_bytes(
        secret, 'little', signed=False) + string_to_number(
            sha256(per_commitment_point + basepoint_bytes).digest())
    basepoint %= CURVE_ORDER
    return basepoint


# revocationprivkey = revocation_basepoint_secret * SHA256(revocation_basepoint || per_commitment_point) + per_commitment_secret * SHA256(per_commitment_point || revocation_basepoint)
def derive_blinded_privkey(basepoint_secret: bytes,
                           per_commitment_secret: bytes) -> bytes:
    basepoint = secret_to_pubkey(basepoint_secret)
    per_commitment_point = secret_to_pubkey(per_commitment_secret)
    k1 = string_to_number(basepoint_secret) * string_to_number(
        sha256(basepoint + per_commitment_point).digest())
    k2 = string_to_number(per_commitment_secret) * string_to_number(
        sha256(per_commitment_point + basepoint).digest())
    sum = (k1 + k2) % CURVE_ORDER
    return int.to_bytes(sum, length=32, byteorder='big', signed=False)


class HKDF(object):
    def __init__(self, key, salt="", digestmod=sha256):
        self.__digest = digestmod
        self.__digest_len = 32

        if not salt:
            salt = b'\0' * self.__digest_len

        self.__expanded_key = hmac.new(
            salt, key, digestmod=self.__digest).digest()

    def extract_key(self, info, length):
        assert length <= self.__digest_len * 255

        # generate key stream, stop when we have enought bytes

        keystream = b""
        key_block = b""
        block_index = 1

        while len(keystream) < length:
            key_block = hmac.new(
                self.__expanded_key,
                key_block + info + chr(block_index).encode(),
                digestmod=self.__digest).digest()
            block_index += 1
            keystream += key_block

        return keystream[:length]
