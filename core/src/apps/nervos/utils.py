def hex_to_int(i: str) -> int:
    return int(i, 0)


def extend_uint64(buffer: bytearray, n: str):
    buffer.extend(hex_to_int(n).to_bytes(8, "little"))


def bytes_to_hex_str(bytes_obj):
    return "".join(f"{byte:02x}" for byte in bytes_obj)


def hex_to_bytes_custom(hex_str: str) -> bytes:
    if hex_str.startswith("0x"):
        hex_str = hex_str[2:]
    if len(hex_str) % 2 != 0:
        hex_str = "0" + hex_str
    bytes_array = bytes(int(hex_str[i : i + 2], 16) for i in range(0, len(hex_str), 2))
    return bytes_array
