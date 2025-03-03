import ustruct as struct
from typing import TYPE_CHECKING

from .script import is_opreturn, is_p2pk, is_p2pkh, is_p2sh, is_p2wsh, is_witness
from .serialize import (
    deser_string,
    deser_string_vector,
    deser_uint256,
    deser_vector,
    ser_string,
    ser_string_vector,
    ser_uint256,
    ser_vector,
    uint256_from_str,
)

if TYPE_CHECKING:
    from .serialize import Readable


# Objects that map to bitcoind objects, which can be serialized/deserialized

MSG_WITNESS_FLAG = 1 << 30


def sha256d(s: bytes) -> bytes:
    """
    Double SHA-256 hash of the input bytes.
    """
    from trezor.crypto import hashlib

    return hashlib.sha256(hashlib.sha256(s).digest()).digest()


class COutPoint:
    def __init__(self, hash: int = 0, n: int = 0xFFFFFFFF):
        self.hash = hash
        self.n = n

    def deserialize(self, f: Readable) -> None:
        self.hash = deser_uint256(f)
        self.n = struct.unpack("<I", f.read(4))[0]

    def serialize(self) -> bytes:
        r = b""
        r += ser_uint256(self.hash)
        r += struct.pack("<I", self.n)
        return r

    if __debug__:

        def __repr__(self) -> str:
            return f"COutPoint(hash={self.hash:064x}, n={self.n:i})"


class CTxIn:
    def __init__(
        self,
        outpoint: COutPoint | None = None,
        scriptSig: bytes = b"",
        nSequence: int = 0,
    ):
        if outpoint is None:
            self.prevout = COutPoint()
        else:
            self.prevout = outpoint
        self.scriptSig = scriptSig
        self.nSequence = nSequence

    def deserialize(self, f: Readable) -> None:
        self.prevout = COutPoint()
        self.prevout.deserialize(f)
        self.scriptSig = deser_string(f)
        self.nSequence = struct.unpack("<I", f.read(4))[0]

    def serialize(self) -> bytes:
        r = b""
        r += self.prevout.serialize()
        r += ser_string(self.scriptSig)
        r += struct.pack("<I", self.nSequence)
        return r

    if __debug__:

        def __repr__(self) -> str:
            import binascii

            return f"CTxIn(prevout={repr(self.prevout)}, scriptSig={binascii.hexlify(self.scriptSig).decode()}, nSequence={self.nSequence:i})"


class CTxOut:
    def __init__(self, nValue: int = 0, scriptPubKey: bytes = b""):
        self.nValue = nValue
        self.scriptPubKey = scriptPubKey

    def deserialize(self, f: Readable) -> None:
        self.nValue = struct.unpack("<q", f.read(8))[0]
        self.scriptPubKey = deser_string(f)

    def serialize(self) -> bytes:
        r = b""
        r += struct.pack("<q", self.nValue)
        r += ser_string(self.scriptPubKey)
        return r

    def is_opreturn(self) -> bool:
        return is_opreturn(self.scriptPubKey)

    def is_p2sh(self) -> bool:
        return is_p2sh(self.scriptPubKey)

    def is_p2wsh(self) -> bool:
        return is_p2wsh(self.scriptPubKey)

    def is_p2pkh(self) -> bool:
        return is_p2pkh(self.scriptPubKey)

    def is_p2pk(self) -> bool:
        return is_p2pk(self.scriptPubKey)

    def is_witness(self) -> tuple[bool, int, bytes]:
        return is_witness(self.scriptPubKey)

    if __debug__:

        def __repr__(self) -> str:
            import binascii

            return f"CTxOut(nValue={self.nValue // 100_000_000:i}.{self.nValue % 100_000_000:08i} scriptPubKey={binascii.hexlify(self.scriptPubKey).decode()})"


class CScriptWitness:
    def __init__(self) -> None:
        # stack is a vector of strings
        self.stack: list[bytes] = []

    if __debug__:

        def __repr__(self) -> str:
            import binascii

            return f"CScriptWitness({(';'.join([binascii.hexlify(x).decode() for x in self.stack]))})"

    def is_null(self) -> bool:
        if self.stack:
            return False
        return True


class CTxInWitness:
    def __init__(self) -> None:
        self.scriptWitness = CScriptWitness()

    def deserialize(self, f: Readable) -> None:
        self.scriptWitness.stack = deser_string_vector(f)

    def serialize(self) -> bytes:
        return ser_string_vector(self.scriptWitness.stack)

    if __debug__:

        def __repr__(self) -> str:
            return repr(self.scriptWitness)

    def is_null(self) -> bool:
        return self.scriptWitness.is_null()


class CTxWitness:
    def __init__(self) -> None:
        self.vtxinwit: list[CTxInWitness] = []

    def deserialize(self, f: Readable) -> None:
        for i in range(len(self.vtxinwit)):
            self.vtxinwit[i].deserialize(f)

    def serialize(self) -> bytes:
        r = b""
        # This is different than the usual vector serialization --
        # we omit the length of the vector, which is required to be
        # the same length as the transaction's vin vector.
        for x in self.vtxinwit:
            r += x.serialize()
        return r

    if __debug__:

        def __repr__(self) -> str:
            return f"CTxWitness({(';'.join([repr(x) for x in self.vtxinwit]))})"

    def is_null(self) -> bool:
        for x in self.vtxinwit:
            if not x.is_null():
                return False
        return True


class CTransaction:
    def __init__(self, tx: "CTransaction" | None = None) -> None:
        if tx is None:
            self.nVersion = 1
            self.vin: list[CTxIn] = []
            self.vout: list[CTxOut] = []
            self.wit = CTxWitness()
            self.nLockTime = 0
            self.sha256: int | None = None
            self.hash: bytes | None = None
        else:
            self.nVersion = tx.nVersion
            self.vin = tx.vin[:]
            self.vout = tx.vout[:]
            self.nLockTime = tx.nLockTime
            self.sha256 = tx.sha256
            self.hash = tx.hash
            self.wit = tx.wit

    def deserialize(self, f: Readable) -> None:
        self.nVersion = struct.unpack("<i", f.read(4))[0]
        self.vin = deser_vector(f, CTxIn)
        flags = 0
        if len(self.vin) == 0:
            flags = struct.unpack("<B", f.read(1))[0]
            # Not sure why flags can't be zero, but this
            # matches the implementation in bitcoind
            if flags != 0:
                self.vin = deser_vector(f, CTxIn)
                self.vout = deser_vector(f, CTxOut)
        else:
            self.vout = deser_vector(f, CTxOut)
        if flags != 0:
            self.wit.vtxinwit = [CTxInWitness() for i in range(len(self.vin))]
            self.wit.deserialize(f)
        self.nLockTime = struct.unpack("<I", f.read(4))[0]
        self.sha256 = None
        self.hash = None

    def serialize_without_witness(self) -> bytes:
        r = b""
        r += struct.pack("<i", self.nVersion)
        r += ser_vector(self.vin)
        r += ser_vector(self.vout)
        r += struct.pack("<I", self.nLockTime)
        return r

    # Only serialize with witness when explicitly called for
    def serialize_with_witness(self) -> bytes:
        flags = 0
        if not self.wit.is_null():
            flags |= 1
        r = b""
        r += struct.pack("<i", self.nVersion)
        if flags:
            r += ser_vector([])
            r += struct.pack("<B", flags)
        r += ser_vector(self.vin)
        r += ser_vector(self.vout)
        if flags & 1:
            if len(self.wit.vtxinwit) != len(self.vin):
                # vtxinwit must have the same length as vin
                self.wit.vtxinwit = self.wit.vtxinwit[: len(self.vin)]
                for _ in range(len(self.wit.vtxinwit), len(self.vin)):
                    self.wit.vtxinwit.append(CTxInWitness())
            r += self.wit.serialize()
        r += struct.pack("<I", self.nLockTime)
        return r

    # Regular serialization is without witness -- must explicitly
    # call serialize_with_witness to include witness data.
    def serialize(self) -> bytes:
        return self.serialize_without_witness()

    # Recalculate the txid (transaction hash without witness)
    def rehash(self) -> None:
        self.sha256 = None
        self.calc_sha256()

    # We will only cache the serialization without witness in
    # self.sha256 and self.hash -- those are expected to be the txid.
    def calc_sha256(self, with_witness: bool = False) -> int | None:
        if with_witness:
            # Don't cache the result, just return it
            return uint256_from_str(sha256d(self.serialize_with_witness()))

        if self.sha256 is None:
            self.sha256 = uint256_from_str(sha256d(self.serialize_without_witness()))
        self.hash = sha256d(self.serialize())
        return None

    def is_null(self) -> bool:
        return len(self.vin) == 0 and len(self.vout) == 0

    if __debug__:

        def __repr__(self) -> str:
            return f"CTransaction(nVersion={self.nVersion:i} vin={repr(self.vin)} vout={repr(self.vout)} wit={repr(self.wit)} nLockTime={self.nLockTime:i})"
