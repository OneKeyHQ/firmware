# This file is part of the Trezor project.
#
# Copyright (C) 2012-2019 SatoshiLabs and contributors
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the License along with this library.
# If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.

import hashlib
from enum import Enum
from typing import Callable, List, Tuple

import construct as c
import ecdsa

from . import cosi, messages, tools

try:
    from hashlib import blake2s
except ImportError:
    from pyblake2 import blake2s


V1_SIGNATURE_SLOTS = 3
V1_BOOTLOADER_KEYS = [
    bytes.fromhex(key)
    for key in (
        "04ad9035d67ac4795c913c452d25156f0b094c34f656a249b94d8d6619ab0d92b1e8bcc328bdc833b9b51ca31bfd0136615153f93aba46d02ab5d925f4f364c666",
        "0495b83da442c689bda82e9f9543811fec2f58334f5c76361f5a49fbb6634a81152c6da7b8a1782ccaa9287ec7a8efe9d8bba7d90180f7b719c017d804d61c3d5d",
        "04e0d5ef94cf950b9f855fb5526764dc28d2d96582c1ca1ad39cabab653e6198f80e6495e536b8bce37857dafc5a51952624ab083c33168ee3ed83e036defab85c",
        "04c9ec740cb3328165dc3fdb93a54e7075569b7a54cdeec0211fa9d952c6647960bd95f994c817458892d5db6eb34da76ce03c9c04a4325c2752647590b1a4f365",
        "04d208aba79b6f60f178608167ddb777860a8155022e28f120a41630869a4e0f0d1698a40eed2aedf74846e1e401cee6fdb1e81116d14cd2971270f2cb0c562c2e",
    )
]

V2_BOARDLOADER_KEYS = [
    bytes.fromhex(key)
    for key in (
        "154b8ab261cc8879483f689a2d41243ae7dbc4021672bbd25c338ae84d931154",
        "a9e65e07fe6d39a8a84e11a996a0283f881e175cba602eb5ac442fb75b39e8e0",
        "6c8805abb2df9d3679f1d28a40cd990399b99fc3ee4e0657d81d381ea1488a12",
        "3ed79779064d56571b29bcaa734cbb6db61d2e626566628ecf4c89e1db45eaec",
        "54a40633bfd9e60b8a391265b2e006374abe631d1e1107332bca56bf9f8c5c99",
        "4b71134f18e00787c583d40742cc188e17fc85ade4cb472dae5ef8e069f0fec5",
        "2ecf80c82b449848c000335092139551bfe47b3c7317b49950f65e1d82432024"
    )
]

V2_BOARDLOADER_DEV_KEYS = [
    bytes.fromhex(key)
    for key in (
        "db995fe25169d141cab9bbba92baa01f9f2e1ece7df4cb2ac05190f37fcc1f9d",
        "2152f8d19b791d24453242e15f2eab6cb7cffa7b6a5ed30097960e069881db12",
        "22fc297792f0b6ffc0bfcfdb7edb0c0aa14e025a365ec0e342e86e3829cb74b6",
    )
]

V2_BOOTLOADER_KEYS = [
    bytes.fromhex(key)
    for key in (
        "154b8ab261cc8879483f689a2d41243ae7dbc4021672bbd25c338ae84d931154",
        "a9e65e07fe6d39a8a84e11a996a0283f881e175cba602eb5ac442fb75b39e8e0",
        "6c8805abb2df9d3679f1d28a40cd990399b99fc3ee4e0657d81d381ea1488a12",
        "3ed79779064d56571b29bcaa734cbb6db61d2e626566628ecf4c89e1db45eaec",
        "54a40633bfd9e60b8a391265b2e006374abe631d1e1107332bca56bf9f8c5c99",
        "4b71134f18e00787c583d40742cc188e17fc85ade4cb472dae5ef8e069f0fec5",
        "2ecf80c82b449848c000335092139551bfe47b3c7317b49950f65e1d82432024"
    )
]

V2_SIGS_REQUIRED = 4

ONEV2_CHUNK_SIZE = 1024 * 64
V2_CHUNK_SIZE = 1024 * 256


def _transform_vendor_trust(data: bytes) -> bytes:
    """Byte-swap and bit-invert the VendorTrust field.

    Vendor trust is interpreted as a bitmask in a 16-bit little-endian integer,
    with the added twist that 0 means set and 1 means unset.
    We feed it to a `BitStruct` that expects a big-endian sequence where bits have
    the traditional meaning. We must therefore do a bitwise negation of each byte,
    and return them in reverse order. This is the same transformation both ways,
    fortunately, so we don't need two separate functions.
    """
    return bytes(~b & 0xFF for b in data)[::-1]


class FirmwareIntegrityError(Exception):
    pass


class InvalidSignatureError(FirmwareIntegrityError):
    pass


class Unsigned(FirmwareIntegrityError):
    pass


class ToifMode(Enum):
    full_color = b"f"
    grayscale = b"g"


class HeaderType(Enum):
    FIRMWARE = b"OKTF"
    BOOTLOADER = b"OKTB"


class EnumAdapter(c.Adapter):
    def __init__(self, subcon, enum):
        self.enum = enum
        super().__init__(subcon)

    def _encode(self, obj, ctx, path):
        return obj.value

    def _decode(self, obj, ctx, path):
        try:
            return self.enum(obj)
        except ValueError:
            return obj


# fmt: off
Toif = c.Struct(
    "magic" / c.Const(b"TOI"),
    "format" / EnumAdapter(c.Bytes(1), ToifMode),
    "width" / c.Int16ul,
    "height" / c.Int16ul,
    "data" / c.Prefixed(c.Int32ul, c.GreedyBytes),
)


VendorTrust = c.Transformed(c.BitStruct(
    "_reserved" / c.Default(c.BitsInteger(9), 0),
    "show_vendor_string" / c.Flag,
    "require_user_click" / c.Flag,
    "red_background" / c.Flag,
    "delay" / c.BitsInteger(4),
), _transform_vendor_trust, 2, _transform_vendor_trust, 2)


VendorHeader = c.Struct(
    "_start_offset" / c.Tell,
    "magic" / c.Const(b"OKTV"),
    "header_len" / c.Int32ul,
    "expiry" / c.Int32ul,
    "version" / c.Struct(
        "major" / c.Int8ul,
        "minor" / c.Int8ul,
    ),
    "sig_m" / c.Int8ul,
    "sig_n" / c.Rebuild(c.Int8ul, c.len_(c.this.pubkeys)),
    "trust" / VendorTrust,
    "_reserved" / c.Padding(14),
    "pubkeys" / c.Bytes(32)[c.this.sig_n],
    "text" / c.Aligned(4, c.PascalString(c.Int8ul, "utf-8")),
    "image" / Toif,
    "_end_offset" / c.Tell,

    "_min_header_len" / c.Check(c.this.header_len > (c.this._end_offset - c.this._start_offset) + 65),
    "_header_len_aligned" / c.Check(c.this.header_len % 512 == 0),

    c.Padding(c.this.header_len - c.this._end_offset + c.this._start_offset - 65),
    "sigmask" / c.Byte,
    "signature" / c.Bytes(64),
)


VersionLong = c.Struct(
    "major" / c.Int8ul,
    "minor" / c.Int8ul,
    "patch" / c.Int8ul,
    "build" / c.Int8ul,
)


FirmwareHeader = c.Struct(
    "_start_offset" / c.Tell,
    "magic" / EnumAdapter(c.Bytes(4), HeaderType),
    "header_len" / c.Int32ul,
    "expiry" / c.Int32ul,
    "code_length" / c.Rebuild(
        c.Int32ul,
        lambda this:
            len(this._.code) if "code" in this._
            else (this.code_length or 0)
    ),
    "version" / VersionLong,
    "fix_version" / VersionLong,
    "onekey_version" / VersionLong,
    "_reserved" / c.Padding(4),
    "hashes" / c.Bytes(32)[16],

    "v1_signatures" / c.Bytes(64)[V1_SIGNATURE_SLOTS],
    "v1_key_indexes" / c.Int8ul[V1_SIGNATURE_SLOTS],  # pylint: disable=E1136

    "_reserved" / c.Padding(220),
    "sigmask" / c.Byte,
    "signature" / c.Bytes(64),

    "_end_offset" / c.Tell,

    "_rebuild_header_len" / c.If(
        c.this.version.major > 1,
        c.Pointer(
            c.this._start_offset + 4,
            c.Rebuild(c.Int32ul, c.this._end_offset - c.this._start_offset)
        ),
    ),
)


"""Raw firmware image.

Consists of firmware header and code block.
This is the expected format of firmware binaries for Trezor One, or bootloader images
for Trezor T."""
FirmwareImage = c.Struct(
    "header" / FirmwareHeader,
    "_code_offset" / c.Tell,
    "code" / c.Bytes(c.this.header.code_length),
    c.Terminated,
)


"""Firmware image prefixed by a vendor header.

This is the expected format of firmware binaries for Trezor T."""
VendorFirmware = c.Struct(
    "vendor_header" / VendorHeader,
    "image" / FirmwareImage,
    c.Terminated,
)


"""Legacy firmware image.
Consists of a custom header and code block.
This is the expected format of firmware binaries for Trezor One pre-1.8.0.

The code block can optionally be interpreted as a new-style firmware image. That is the
expected format of firmware binary for Trezor One version 1.8.0, which can be installed
by both the older and the newer bootloader."""
LegacyFirmware = c.Struct(
    "magic" / c.Const(b"TRZR"),
    "code_length" / c.Rebuild(c.Int32ul, c.len_(c.this.code)),
    "key_indexes" / c.Int8ul[V1_SIGNATURE_SLOTS],  # pylint: disable=E1136
    "flags" / c.BitStruct(
        c.Padding(7),
        "restore_storage" / c.Flag,
    ),
    "_reserved" / c.Padding(52),
    "signatures" / c.Bytes(64)[V1_SIGNATURE_SLOTS],
    "code" / c.Bytes(c.this.code_length),
    c.Terminated,

    "embedded_onev2" / c.RestreamData(c.this.code, c.Optional(FirmwareImage)),
)

# fmt: on


class FirmwareFormat(Enum):
    TREZOR_ONE = 1
    TREZOR_T = 2
    TREZOR_ONE_V2 = 3


ParsedFirmware = Tuple[FirmwareFormat, c.Container]


def parse(data: bytes) -> ParsedFirmware:
    if data[:4] == b"TRZR":
        version = FirmwareFormat.TREZOR_ONE
        cls = LegacyFirmware
    elif data[:4] == b"OKTV":
        version = FirmwareFormat.TREZOR_T
        cls = VendorFirmware
    elif data[:4] == b"OKTF":
        version = FirmwareFormat.TREZOR_ONE_V2
        cls = FirmwareImage
    else:
        raise ValueError("Unrecognized firmware image type")

    try:
        fw = cls.parse(data)
    except Exception as e:
        raise FirmwareIntegrityError("Invalid firmware image") from e
    return version, fw


def digest_onev1(fw: c.Container) -> bytes:
    return hashlib.sha256(fw.code).digest()


def check_sig_v1(
    digest: bytes, key_indexes: List[int], signatures: List[bytes]
) -> None:
    distinct_key_indexes = set(i for i in key_indexes if i != 0)
    if not distinct_key_indexes:
        raise Unsigned

    if len(distinct_key_indexes) < len(key_indexes):
        raise InvalidSignatureError(
            "Not enough distinct signatures (found {}, need {})".format(
                len(distinct_key_indexes), len(key_indexes)
            )
        )

    for i in range(len(key_indexes)):
        key_idx = key_indexes[i] - 1
        signature = signatures[i]

        if key_idx >= len(V1_BOOTLOADER_KEYS):
            # unknown pubkey
            raise InvalidSignatureError("Unknown key in slot {}".format(i))

        pubkey = V1_BOOTLOADER_KEYS[key_idx][1:]
        verify = ecdsa.VerifyingKey.from_string(pubkey, curve=ecdsa.curves.SECP256k1)
        try:
            verify.verify_digest(signature, digest)
        except ecdsa.BadSignatureError as e:
            raise InvalidSignatureError("Invalid signature in slot {}".format(i)) from e


def header_digest(header: c.Container, hash_function: Callable = blake2s) -> bytes:
    stripped_header = header.copy()
    stripped_header.sigmask = 0
    stripped_header.signature = b"\0" * 64
    stripped_header.v1_key_indexes = [0, 0, 0]
    stripped_header.v1_signatures = [b"\0" * 64] * 3
    if header.magic == b"OKTV":
        header_type = VendorHeader
    else:
        header_type = FirmwareHeader
    header_bytes = header_type.build(stripped_header)
    return hash_function(header_bytes).digest()


def digest_v2(fw: c.Container) -> bytes:
    return header_digest(fw.image.header, blake2s)


def digest_onev2(fw: c.Container) -> bytes:
    return header_digest(fw.header, hashlib.sha256)


def calculate_code_hashes(
    code: bytes,
    code_offset: int,
    hash_function: Callable = blake2s,
    chunk_size: int = V2_CHUNK_SIZE,
    padding_byte: bytes = None,
) -> None:
    hashes = []
    # End offset for each chunk. Normally this would be (i+1)*chunk_size for i-th chunk,
    # but the first chunk is shorter by code_offset, so all end offsets are shifted.
    ends = [(i + 1) * chunk_size - code_offset for i in range(16)]
    start = 0
    for end in ends:
        chunk = code[start:end]
        # padding for last non-empty chunk
        if padding_byte is not None and start < len(code) and end > len(code):
            chunk += padding_byte[0:1] * (end - start - len(chunk))

        if not chunk:
            hashes.append(b"\0" * 32)
        else:
            hashes.append(hash_function(chunk).digest())

        start = end

    return hashes


def validate_code_hashes(fw: c.Container, version: FirmwareFormat) -> None:
    if version == FirmwareFormat.TREZOR_ONE_V2:
        image = fw
        hash_function = hashlib.sha256
        chunk_size = ONEV2_CHUNK_SIZE
        padding_byte = b"\xff"
    else:
        image = fw.image
        hash_function = blake2s
        chunk_size = V2_CHUNK_SIZE
        padding_byte = None

    expected_hashes = calculate_code_hashes(
        image.code, image._code_offset, hash_function, chunk_size, padding_byte
    )
    if expected_hashes != image.header.hashes:
        raise FirmwareIntegrityError("Invalid firmware data.")


def validate_onev2(fw: c.Container, allow_unsigned: bool = False) -> None:
    try:
        check_sig_v1(
            digest_onev2(fw),
            fw.header.v1_key_indexes,
            fw.header.v1_signatures,
        )
    except Unsigned:
        if not allow_unsigned:
            raise

    validate_code_hashes(fw, FirmwareFormat.TREZOR_ONE_V2)


def validate_onev1(fw: c.Container, allow_unsigned: bool = False) -> None:
    try:
        check_sig_v1(digest_onev1(fw), fw.key_indexes, fw.signatures)
    except Unsigned:
        if not allow_unsigned:
            raise
    if fw.embedded_onev2:
        validate_onev2(fw.embedded_onev2, allow_unsigned)


def validate_v2(fw: c.Container, skip_vendor_header: bool = False) -> None:
    vendor_fingerprint = header_digest(fw.vendor_header)
    fingerprint = digest_v2(fw)

    if not skip_vendor_header:
        try:
            # if you want to validate a custom vendor header, you can modify
            # the global variables to match your keys and m-of-n scheme
            cosi.verify(
                fw.vendor_header.signature,
                vendor_fingerprint,
                V2_SIGS_REQUIRED,
                V2_BOOTLOADER_KEYS,
                fw.vendor_header.sigmask,
            )
        except Exception:
            raise InvalidSignatureError("Invalid vendor header signature.")

        # XXX expiry is not used now
        # now = time.gmtime()
        # if time.gmtime(fw.vendor_header.expiry) < now:
        #     raise ValueError("Vendor header expired.")

    try:
        cosi.verify(
            fw.image.header.signature,
            fingerprint,
            fw.vendor_header.sig_m,
            fw.vendor_header.pubkeys,
            fw.image.header.sigmask,
        )
    except Exception:
        raise InvalidSignatureError("Invalid firmware signature.")

    # XXX expiry is not used now
    # if time.gmtime(fw.image.header.expiry) < now:
    #     raise ValueError("Firmware header expired.")
    validate_code_hashes(fw, FirmwareFormat.TREZOR_T)


def digest(version: FirmwareFormat, fw: c.Container) -> bytes:
    if version == FirmwareFormat.TREZOR_ONE:
        return digest_onev1(fw)
    elif version == FirmwareFormat.TREZOR_ONE_V2:
        return digest_onev2(fw)
    elif version == FirmwareFormat.TREZOR_T:
        return digest_v2(fw)
    else:
        raise ValueError("Unrecognized firmware version")


def validate(
    version: FirmwareFormat, fw: c.Container, allow_unsigned: bool = False
) -> None:
    if version == FirmwareFormat.TREZOR_ONE:
        return validate_onev1(fw, allow_unsigned)
    elif version == FirmwareFormat.TREZOR_ONE_V2:
        return validate_onev2(fw, allow_unsigned)
    elif version == FirmwareFormat.TREZOR_T:
        return validate_v2(fw)
    else:
        raise ValueError("Unrecognized firmware version")


# ====== Client functions ====== #


@tools.session
def update(client, data):
    if client.features.bootloader_mode is False:
        raise RuntimeError("Device must be in bootloader mode")

    if data[:4] == b"5283":
        resp = client.call(messages.FirmwareErase_ex(length=len(data)))
    else:
        resp = client.call(messages.FirmwareErase(length=len(data)))

    # TREZORv1 method
    if isinstance(resp, messages.Success):
        resp = client.call(messages.FirmwareUpload(payload=data))
        if isinstance(resp, messages.Success):
            return
        else:
            raise RuntimeError("Unexpected result %s" % resp)

    # TREZORv2 method
    while isinstance(resp, messages.FirmwareRequest):
        payload = data[resp.offset : resp.offset + resp.length]
        digest = blake2s(payload).digest()
        resp = client.call(messages.FirmwareUpload(payload=payload, hash=digest))

    if isinstance(resp, messages.Success):
        return
    else:
        raise RuntimeError("Unexpected message %s" % resp)
