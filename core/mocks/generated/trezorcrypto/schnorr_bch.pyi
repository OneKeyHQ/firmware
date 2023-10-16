from typing import *


# extmod/modtrezorcrypto/modtrezorcrypto-schnorr-bch.h
def sign(
    secret_key: bytes,
    digest: bytes,
) -> bytes:
    """
    Uses secret key to produce the signature of the digest.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-schnorr-bch.h
def verify(public_key: bytes, signature: bytes, digest: bytes) -> bool:
    """
    Uses public key to verify the signature of the digest.
    Returns True on success.
    """
