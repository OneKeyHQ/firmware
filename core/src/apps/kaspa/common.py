from trezor.crypto.hashlib import blake2b, sha256
from trezor.utils import HashWriter

ALLOWED_SCHEMES = ("schnorr", "ecdsa")
TRANSACTION_SIGNING_DOMAIN = b"TransactionSigningHash"
TRANSACTION_SIGNING_ECDSA_DOMAIN = b"TransactionSigningHashECDSA"
HASH_SIZE = 32


def hashwriter() -> HashWriter:
    ctx = blake2b(key=TRANSACTION_SIGNING_DOMAIN, outlen=HASH_SIZE)
    return HashWriter(ctx)


def hashwriter_ecdsa() -> HashWriter:
    ctx = sha256(sha256(TRANSACTION_SIGNING_ECDSA_DOMAIN).digest())
    return HashWriter(ctx)
