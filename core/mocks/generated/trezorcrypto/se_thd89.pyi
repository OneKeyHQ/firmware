from typing import *


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def check(mnemonic: str) -> bool:
    """
    Check whether given mnemonic is valid.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def seed(
    passphrase: str,
    callback: Callable[[int, int], None] | None = None,
) -> bool:
    """
    Generate seed from mnemonic and passphrase.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def start_session(
    session_id: bytes,
) -> bytes:
    """
    start session.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def end_session() -> None:
    """
    end current session.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def get_session_state() -> bytes:
    """
    get current session secret state.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def nist256p1_sign(
    secret_key: bytes, digest: bytes, compressed: bool = True
) -> bytes:
    """
    Uses secret key to produce the signature of the digest.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def secp256k1_sign_digest(
    seckey: bytes,
    digest: bytes,
    compressed: bool = True,
    canonical: int | None = None,
) -> bytes:
    """
    Uses secret key to produce the signature of the digest.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def bip340_sign(
    secret_key: bytes,
    digest: bytes,
) -> bytes:
    """
    Uses secret key to produce the signature of the digest.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def ed25519_sign(secret_key: bytes, message: bytes, hasher: str = "") ->
bytes:
    """
    Uses secret key to produce the signature of message.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def ecdh(curve: str, public_key: bytes) -> bytes:
    """
    Multiplies point defined by public_key with scalar defined by
    secret_key. Useful for ECDH.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def bip340_ecdh(public_key: bytes) -> bytes:
    """
    Multiplies point defined by public_key with scalar defined by
    secret_key. Useful for ECDH.
    """


# extmod/modtrezorcrypto/modtrezorcrypto-se-thd89.h
def uncompress_pubkey(pubkey: bytes) -> bytes:
    """
    Uncompress public.
    """
