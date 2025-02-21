from binascii import a2b_base64, b2a_base64, unhexlify

from ._utils import crc16, string_to_bytes


def parse_friendly_address(addr_str):
    if len(addr_str) != 48:
        raise Exception("User-friendly address should contain strictly 48 characters")

    # avoid padding error (https://gist.github.com/perrygeo/ee7c65bb1541ff6ac770)
    data = string_to_bytes(a2b_base64(addr_str + "=="))

    if len(data) != 36:
        raise Exception("Unknown address type: byte length is not equal to 36")

    addr = data[:34]
    crc = data[34:36]
    calced_crc = crc16(addr)
    if not (calced_crc[0] == crc[0] and calced_crc[1] == crc[1]):
        raise Exception("Wrong crc16 hashsum")

    tag = addr[0]
    is_test_only = False
    is_bounceable = False
    if tag & Address.TEST_FLAG:
        is_test_only = True
        tag ^= Address.TEST_FLAG
    if tag not in [Address.BOUNCEABLE_TAG, Address.NON_BOUNCEABLE_TAG]:
        raise Exception("Unknown address tag")

    is_bounceable = tag == Address.BOUNCEABLE_TAG

    if addr[1] == 0xFF:
        workchain = -1
    else:
        workchain = addr[1]
    if workchain not in [0, -1]:
        raise Exception(f"Invalid address wc {workchain}")

    hash_part = bytearray(addr[2:34])
    return {
        "is_test_only": is_test_only,
        "is_bounceable": is_bounceable,
        "workchain": workchain,
        "hash_part": hash_part,
    }


class Address:
    BOUNCEABLE_TAG = 0x11
    NON_BOUNCEABLE_TAG = 0x51
    TEST_FLAG = 0x80

    def __init__(self, any_form):
        if any_form is None:
            raise Exception("Invalid address")

        if isinstance(any_form, Address):
            self.wc = any_form.wc
            self.hash_part = any_form.hash_part
            self.is_test_only = any_form.is_test_only
            self.is_user_friendly = any_form.is_user_friendly
            self.is_bounceable = any_form.is_bounceable
            self.is_url_safe = any_form.is_url_safe
            return

        if any_form.find("-") > 0 or any_form.find("_") > 0:
            any_form = any_form.replace("-", "+").replace("_", "/")
            self.is_url_safe = True
        else:
            self.is_url_safe = False

        try:
            colon_index = any_form.index(":")
        except ValueError:
            colon_index = -1

        if colon_index > -1:
            arr = any_form.split(":")
            if len(arr) != 2:
                raise Exception(f"Invalid address {any_form}")

            wc = int(arr[0])
            if wc not in [0, -1]:
                raise Exception(f"Invalid address wc {wc}")

            address_hex = arr[1]
            if len(address_hex) != 64:
                raise Exception(f"Invalid address hex {any_form}")

            self.is_user_friendly = False
            self.wc = wc
            self.hash_part = unhexlify(address_hex)
            self.is_test_only = False
            self.is_bounceable = False
        else:
            self.is_user_friendly = True
            parse_result = parse_friendly_address(any_form)
            self.wc = parse_result["workchain"]
            self.hash_part = parse_result["hash_part"]
            self.is_test_only = parse_result["is_test_only"]
            self.is_bounceable = parse_result["is_bounceable"]

    def to_string(
        self,
        is_user_friendly=None,
        is_url_safe=None,
        is_bounceable=None,
        is_test_only=None,
    ):
        if is_user_friendly is None:
            is_user_friendly = self.is_user_friendly
        if is_url_safe is None:
            is_url_safe = self.is_url_safe
        if is_bounceable is None:
            is_bounceable = self.is_bounceable
        if is_test_only is None:
            is_test_only = self.is_test_only

        if not is_user_friendly:
            return f"{self.wc}:{self.hash_part.hex()}"
        else:
            tag = (
                Address.BOUNCEABLE_TAG if is_bounceable else Address.NON_BOUNCEABLE_TAG
            )

            if is_test_only:
                tag |= Address.TEST_FLAG

            addr = bytearray(34)
            addr[0] = tag
            addr[1] = self.wc
            addr[2:] = self.hash_part
            address_with_checksum = bytearray(36)
            address_with_checksum[:34] = addr
            address_with_checksum[34:] = crc16(addr)

            address_base_64 = b2a_base64(address_with_checksum)[:-1].decode("utf-8")
            if is_url_safe:
                address_base_64 = address_base_64.replace("+", "-").replace("/", "_")

            return str(address_base_64)

    def get_hash_part(self):
        return self.hash_part

    def to_buffer(self):
        return self.hash_part + bytearray([self.wc, self.wc, self.wc, self.wc])
