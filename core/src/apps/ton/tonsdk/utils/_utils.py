import math
import ustruct as struct


def concat_bytes(a, b):
    return a + b  # ?


def move_to_end(index_hashmap, topological_order_arr, target):
    target_index = index_hashmap[target]
    for _hash in index_hashmap:
        if index_hashmap[_hash] > target_index:
            index_hashmap[_hash] -= 1
    index_hashmap[target] = len(topological_order_arr) - 1
    data = topological_order_arr.pop(target_index)
    topological_order_arr.append(data)
    for sub_cell in data[1].refs:
        topological_order_arr, index_hashmap = move_to_end(
            index_hashmap, topological_order_arr, sub_cell.bytes_hash()
        )
    return [topological_order_arr, index_hashmap]


def tree_walk(cell, topological_order_arr, index_hashmap, parent_hash=None):
    cell_hash = cell.bytes_hash()
    if cell_hash in index_hashmap:
        if parent_hash:
            if index_hashmap[parent_hash] > index_hashmap[cell_hash]:
                topological_order_arr, index_hashmap = move_to_end(
                    index_hashmap, topological_order_arr, cell_hash
                )
        return [topological_order_arr, index_hashmap]

    index_hashmap[cell_hash] = len(topological_order_arr)
    topological_order_arr.append([cell_hash, cell])
    for sub_cell in cell.refs:
        topological_order_arr, index_hashmap = tree_walk(
            sub_cell, topological_order_arr, index_hashmap, cell_hash
        )
    return [topological_order_arr, index_hashmap]


def _crc32c(crc, bytes_arr):
    POLY = 0x82F63B78

    crc ^= 0xFFFFFFFF

    for n in range(len(bytes_arr)):
        crc ^= bytes_arr[n]
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1
        crc = (crc >> 1) ^ POLY if crc & 1 else crc >> 1

    return crc ^ 0xFFFFFFFF


def crc32c(bytes_array):
    int_crc = _crc32c(0, bytes_array)

    # TODO: check mistakes
    arr = bytearray(4)
    struct.pack_into("<I", arr, 0, int_crc)
    return bytes(arr)


def crc16(data):
    POLY = 0x1021
    reg = 0
    message = bytes(data) + bytes(2)

    for byte in message:
        mask = 0x80
        while mask > 0:
            reg <<= 1
            if byte & mask:
                reg += 1
            mask >>= 1
            if reg > 0xFFFF:
                reg &= 0xFFFF
                reg ^= POLY

    return bytearray([math.floor(reg / 256), reg % 256])


def read_n_bytes_uint_from_array(size_bytes, uint8_array):
    res = 0
    for c in range(size_bytes):
        res *= 256
        res += uint8_array[c]  # must be uint8

    return res


def compare_bytes(bytes_1, bytes_2):
    return str(bytes_1) == str(bytes_2)  # why str?


def string_to_bytes(string, size=1):  # ?
    if size == 1:
        buf = bytearray(len(string))
    elif size == 2:
        buf = bytearray(len(string) * 2)
    elif size == 4:
        buf = bytearray(len(string) * 4)
    else:
        raise Exception("Invalid size")

    for i, c in enumerate(string):
        # buf[i] = ord(c)
        buf[i] = c  # ?

    return bytes(buf)


def int_to_hex(n):
    if n == 0:
        return "0"

    hex_digits = "0123456789abcdef"
    hex_string = ""

    while n > 0:
        remainder = n % 16
        hex_string = hex_digits[remainder] + hex_string
        n = n // 16

    return hex_string
