import binascii
import ustruct as struct
from micropython import const

from trezor.crypto import base58

HARDENED_FLAG = const(0x8000_0000)


def H_(x: int) -> int:
    return x | HARDENED_FLAG


def is_hardened(i: int) -> bool:
    return i & HARDENED_FLAG != 0


class ExtendedPubKey:
    """
    A BIP 32 extended public key.
    """

    MAINNET_PUBLIC = b"\x04\x88\xB2\x1E"
    TESTNET_PUBLIC = b"\x04\x35\x87\xCF"

    def __init__(
        self,
        version: bytes,
        depth: int,
        parent_fingerprint: bytes,
        child_num: int,
        chaincode: bytes,
        pubkey: bytes,
    ) -> None:
        self.version: bytes = version
        self.is_testnet: bool = version == ExtendedPubKey.TESTNET_PUBLIC
        self.depth: int = depth
        self.parent_fingerprint: bytes = parent_fingerprint
        self.child_num: int = child_num
        self.chaincode: bytes = chaincode
        self.pubkey: bytes = pubkey

    @classmethod
    def deserialize(cls, xpub: str) -> "ExtendedPubKey":
        data = base58.decode_check(xpub)
        return cls.from_bytes(data)

    @classmethod
    def from_bytes(cls, data: bytes) -> "ExtendedPubKey":
        version = data[0:4]
        if version not in [
            ExtendedPubKey.MAINNET_PUBLIC,
            ExtendedPubKey.TESTNET_PUBLIC,
        ]:
            raise Exception(f"Extended key magic of {version.hex()} is invalid")
        depth = data[4]
        parent_fingerprint = data[5:9]
        child_num = struct.unpack(">I", data[9:13])[0]
        chaincode = data[13:45]
        pubkey = data[45:78]
        return cls(version, depth, parent_fingerprint, child_num, chaincode, pubkey)

    def serialize(self) -> bytes:
        return (
            self.version
            + struct.pack("B", self.depth)
            + self.parent_fingerprint
            + struct.pack(">I", self.child_num)
            + self.chaincode
            + self.pubkey
        )


class KeyOriginInfo:
    """
    Object representing the origin of a key.
    """

    def __init__(self, fingerprint: bytes, path: list[int]) -> None:
        """
        :param fingerprint: The 4 byte BIP 32 fingerprint of a parent key from which this key is derived from
        :param path: The derivation path to reach this key from the key at ``fingerprint``
        """
        self.fingerprint: bytes = fingerprint
        self.path: list[int] = path

    @classmethod
    def deserialize(cls, s: bytes) -> "KeyOriginInfo":
        """
        Deserialize a serialized KeyOriginInfo.
        They will be serialized in the same way that PSBTs serialize derivation paths
        """
        fingerprint = s[0:4]
        s = s[4:]
        path = list(struct.unpack("<" + "I" * (len(s) // 4), s))
        return cls(fingerprint, path)

    def serialize(self) -> bytes:
        """
        Serializes the KeyOriginInfo in the same way that derivation paths are stored in PSBTs
        """
        r = self.fingerprint
        r += struct.pack("<" + "I" * len(self.path), *self.path)
        return r

    def _path_string(self, hardened_char: str = "h") -> str:
        s = ""
        for i in self.path:
            hardened = is_hardened(i)
            i &= ~HARDENED_FLAG
            s += "/" + str(i)
            if hardened:
                s += hardened_char
        return s

    def to_string(self, hardened_char: str = "h") -> str:
        """
        Return the KeyOriginInfo as a string in the form <fingerprint>/<index>/<index>/...
        This is the same way that KeyOriginInfo is shown in descriptors
        """
        s = binascii.hexlify(self.fingerprint).decode()
        s += self._path_string(hardened_char)
        return s

    @classmethod
    def from_string(cls, s: str) -> "KeyOriginInfo":
        """
        Create a KeyOriginInfo from the string

        :param s: The string to parse
        """
        s = s.lower()
        entries = s.split("/")
        fingerprint = binascii.unhexlify(s[0:8])
        path: list[int] = []
        if len(entries) > 1:
            path = parse_path(s[9:])
        return cls(fingerprint, path)

    def get_derivation_path(self) -> str:
        """
        Return the string for just the path
        """
        return "m" + self._path_string()

    def get_full_int_list(self) -> list[int]:
        """
        Return a list of ints representing this KeyOriginInfo.
        The first int is the fingerprint, followed by the path
        """
        xfp = [struct.unpack("<I", self.fingerprint)[0]]
        xfp.extend(self.path)
        return xfp


def parse_path(nstr: str) -> list[int]:
    """
    Convert BIP32 path string to list of uint32 integers with hardened flags.
    Several conventions are supported to set the hardened flag: -1, 1', 1h

    e.g.: "0/1h/1" -> [0, 0x80000001, 1]

    :param nstr: path string
    :return: list of integers
    """
    if not nstr:
        return []

    n = nstr.split("/")

    # m/a/b/c => a/b/c
    if n[0] == "m":
        n = n[1:]

    def str_to_harden(x: str) -> int:
        if x.startswith("-"):
            return H_(abs(int(x)))
        elif x.endswith(("h", "'")):
            return H_(int(x[:-1]))
        else:
            return int(x)

    try:
        return [str_to_harden(x) for x in n]
    except Exception:
        raise ValueError("Invalid BIP32 path", nstr)
