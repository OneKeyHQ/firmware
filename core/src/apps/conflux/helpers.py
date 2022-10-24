import binascii
from micropython import const
from ubinascii import hexlify, unhexlify

from trezor import wire
from trezor.crypto import base32

STANDARD_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567="
CUSTOM_ALPHABET = "abcdefghjkmnprstuvwxyz0123456789="
PADDING_LETTER = "="

DECIMALS = const(18)

CONFLUX_MAIN_NET_NETWORK_ID = 1029
CONFLUX_TEST_NET_NETWORK_ID = 1

CONFLUX_TYPE_BUILTIN = "builtin"
CONFLUX_TYPE_USER = "user"
CONFLUX_TYPE_CONTRACT = "contract"
CONFLUX_TYPE_INVALID = "invalid"


def address_from_bytes(address_bytes: bytes, network: None = None) -> str:
    """
    Converts address in bytes to a checksummed string as defined
    in https://github.com/ethereum/EIPs/blob/master/EIPS/eip-55.md
    """
    from trezor.crypto.hashlib import sha3_256

    if network is not None and network.rskip60:
        prefix = str(network.chain_id) + "0x"
    else:
        prefix = ""

    address_hex = hexlify(address_bytes).decode()
    digest = sha3_256((prefix + address_hex).encode(), keccak=True).digest()

    def maybe_upper(i: int) -> str:
        """Uppercase i-th letter only if the corresponding nibble has high bit set."""
        digest_byte = digest[i // 2]
        hex_letter = address_hex[i]
        if i % 2 == 0:
            # even letter -> high nibble
            bit = 0x80
        else:
            # odd letter -> low nibble
            bit = 0x08
        if digest_byte & bit:
            return hex_letter.upper()
        else:
            return hex_letter

    return "0x" + "".join(maybe_upper(i) for i in range(len(address_hex)))


def eth_address_to_cfx(address: str):
    assert type(address) == str
    return "0x1" + address.lower()[3:]


def hex_address_bytes(hex_address: str):
    assert type(hex_address) == str
    return binascii.unhexlify(hex_address.lower().replace("0x", ""))


def decode_to_words(b32str):
    result = bytearray()
    for c in b32str:
        result.append(CUSTOM_ALPHABET.index(c))
    return result


def _prefix_to_words(prefix):
    words = bytearray()
    for v in bytes(prefix, "ascii"):
        words.append(v & 0x1F)
    return words


def _poly_mod(v):
    """
    :param v: bytes
    :return: int64
    """
    assert type(v) == bytes or type(v) == bytearray
    c = 1
    for d in v:
        c0 = c >> 35
        c = ((c & 0x07FFFFFFFF) << 5) ^ d
        if c0 & 0x01:
            c ^= 0x98F2BC8E61
        if c0 & 0x02:
            c ^= 0x79B76D99E2
        if c0 & 0x04:
            c ^= 0xF33E5FB3C4
        if c0 & 0x08:
            c ^= 0xAE2EABE2A8
        if c0 & 0x10:
            c ^= 0x1E4F43E470

    return c ^ 1


def _checksum_to_bytes(data):
    result = bytearray(0)
    result.append((data >> 32) & 0xFF)
    result.append((data >> 24) & 0xFF)
    result.append((data >> 16) & 0xFF)
    result.append((data >> 8) & 0xFF)
    result.append((data) & 0xFF)
    return result


def _create_checksum(prefix, payload):
    """
    create checksum from prefix and payload
    :param prefix: network prefix (string)
    :param payload: bytes
    :return: string
    """
    prefix = _prefix_to_words(prefix)
    delimiter = bytes(1)
    payload = decode_to_words(payload)
    template = bytes(8)
    mod = _poly_mod(prefix + delimiter + payload + template)

    return _encode(_checksum_to_bytes(mod))


def _encode(buffer):
    b32str = base32.encode(buffer).replace("=", "")
    payload = ""
    for c in b32str:
        index = STANDARD_ALPHABET.index(c)
        v = CUSTOM_ALPHABET[index]
        payload = "".join([payload, v])

    return payload


def address_from_hex(hex_address: str, chain_id: int) -> str:
    network_prefix = "cfx"
    if chain_id == 1:
        network_prefix = "cfxtest"
    # 0x8: contract address
    if hex_address.lower()[:3] == "0x8":
        eth_addr = hex_address
    else:
        eth_addr = eth_address_to_cfx(hex_address)
    address_bytes = hex_address_bytes(eth_addr)

    payload = _encode(bytes(1) + address_bytes)

    checksum = _create_checksum(network_prefix, payload)
    parts = [network_prefix]
    parts.append(payload + checksum)
    address = ":".join(parts)

    return address


def bytes_from_address(address: str) -> bytes:
    if len(address) == 40:
        return unhexlify(address)

    elif len(address) == 42:
        if address[0:2] not in ("0x", "0X"):
            raise wire.ProcessError("Ethereum: invalid beginning of an address")
        return unhexlify(address[2:])

    elif len(address) == 0:
        return bytes()

    raise wire.ProcessError("Ethereum: Invalid address length")


def decode_hex_address(base32_address):
    assert type(base32_address) == str
    parts = base32_address.split(":")
    assert len(parts) >= 2, "invalid base32 address"

    b32str = parts[-1]
    b32len = len(b32str)
    if b32len % 8 > 0:
        padded_len = b32len + (8 - b32len % 8)
        b32str = (
            ("{:" + "*" + "<" + str(padded_len) + "}").format(b32str).replace("*", "=")
        )

    payload = ""
    for c in b32str:
        index = CUSTOM_ALPHABET.index(c)
        v = STANDARD_ALPHABET[index]
        payload = "".join([payload, v])

    address_buf = base32.decode(payload)

    hex_buf = address_buf[1:21]
    return "0x" + hexlify(hex_buf).decode()


def get_address_type(to: str):
    hex_addr = decode_hex_address(to)
    first_byte = hex_addr[:3]
    if first_byte == "0x0":
        return CONFLUX_TYPE_BUILTIN
    elif first_byte == "0x1":
        return CONFLUX_TYPE_USER
    elif first_byte == "0x8":
        return CONFLUX_TYPE_CONTRACT
    else:
        return CONFLUX_TYPE_INVALID
