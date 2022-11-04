# mp_dump.py A lightweight MessagePack serializer and deserializer module.

# Adapted for MicroPython by Peter Hinch
# Copyright (c) 2021 Peter Hinch Released under the MIT License see LICENSE

# Original source: https://github.com/vsergeev/u-msgpack-python
# See __init__.py for details of changes made for MicroPython.

import io
import struct

from . import UnsupportedTypeException

# try:
#     from .umsgpack_ext import mpext
# except ImportError:
#     mpext = lambda x, _: x


# Auto-detect system float precision
_float_precision = "single" if len(str(1 / 3)) < 13 else "double"


def _fail():  # Debug code should never be called.
    raise Exception("Logic error")


# struct.pack returns a bytes object


def _pack_integer(obj, fp):
    if obj < 0:
        if obj >= -32:
            fp.write(struct.pack("b", obj))
        elif obj >= -(2 ** (8 - 1)):
            fp.write(b"\xd0")
            fp.write(struct.pack("b", obj))
        elif obj >= -(2 ** (16 - 1)):
            fp.write(b"\xd1")
            fp.write(struct.pack(">h", obj))
        elif obj >= -(2 ** (32 - 1)):
            fp.write(b"\xd2")
            fp.write(struct.pack(">i", obj))
        elif obj >= -(2 ** (64 - 1)):
            fp.write(b"\xd3")
            fp.write(struct.pack(">q", obj))
        else:
            raise UnsupportedTypeException("huge signed int")
    else:
        if obj < 128:
            fp.write(struct.pack("B", obj))
        elif obj < 2 ** 8:
            fp.write(b"\xcc")
            fp.write(struct.pack("B", obj))
        elif obj < 2 ** 16:
            fp.write(b"\xcd")
            fp.write(struct.pack(">H", obj))
        elif obj < 2 ** 32:
            fp.write(b"\xce")
            fp.write(struct.pack(">I", obj))
        elif obj < 2 ** 64:
            fp.write(b"\xcf")
            fp.write(struct.pack(">Q", obj))
        else:
            raise UnsupportedTypeException("huge unsigned int")


def _pack_nil(obj, fp):
    fp.write(b"\xc0")


def _pack_boolean(obj, fp):
    fp.write(b"\xc3" if obj else b"\xc2")


def _pack_float(obj, fp, options):
    fpr = options.get("force_float_precision", _float_precision)
    if fpr == "double":
        fp.write(b"\xcb")
        fp.write(struct.pack(">d", obj))
    elif fpr == "single":
        fp.write(b"\xca")
        fp.write(struct.pack(">f", obj))
    else:
        raise ValueError("invalid float precision")


def _pack_string(obj, fp):
    obj = bytes(obj, "utf-8")  # Preferred MP encode method
    obj_len = len(obj)
    if obj_len < 32:
        fp.write(struct.pack("B", 0xA0 | obj_len))
    elif obj_len < 2 ** 8:
        fp.write(b"\xd9")
        fp.write(struct.pack("B", obj_len))
    elif obj_len < 2 ** 16:
        fp.write(b"\xda")
        fp.write(struct.pack(">H", obj_len))
    elif obj_len < 2 ** 32:
        fp.write(b"\xdb")
        fp.write(struct.pack(">I", obj_len))
    else:
        raise UnsupportedTypeException("huge string")
    fp.write(obj)


def _pack_binary(obj, fp):
    obj_len = len(obj)
    if obj_len < 2 ** 8:
        fp.write(b"\xc4")
        fp.write(struct.pack("B", obj_len))
    elif obj_len < 2 ** 16:
        fp.write(b"\xc5")
        fp.write(struct.pack(">H", obj_len))
    elif obj_len < 2 ** 32:
        fp.write(b"\xc6")
        fp.write(struct.pack(">I", obj_len))
    else:
        raise UnsupportedTypeException("huge binary string")
    fp.write(obj)


def _pack_ext(
    obj, fp, tb=b"\x00\xd4\xd5\x00\xd6\x00\x00\x00\xd7\x00\x00\x00\x00\x00\x00\x00\xd8"
):
    od = obj.data
    obj_len = len(od)
    ot = obj.type & 0xFF
    code = tb[obj_len] if obj_len <= 16 else 0
    if code:
        fp.write(int.to_bytes(code, 1, "big"))
        fp.write(struct.pack("B", ot))
    elif obj_len < 2 ** 8:
        fp.write(b"\xc7")
        fp.write(struct.pack("BB", obj_len, ot))
    elif obj_len < 2 ** 16:
        fp.write(b"\xc8")
        fp.write(struct.pack(">HB", obj_len, ot))
    elif obj_len < 2 ** 32:
        fp.write(b"\xc9")
        fp.write(struct.pack(">IB", obj_len, ot))
    else:
        raise UnsupportedTypeException("huge ext data")
    fp.write(od)


def _pack_array(obj, fp, options):
    obj_len = len(obj)
    if obj_len < 16:
        fp.write(struct.pack("B", 0x90 | obj_len))
    elif obj_len < 2 ** 16:
        fp.write(b"\xdc")
        fp.write(struct.pack(">H", obj_len))
    elif obj_len < 2 ** 32:
        fp.write(b"\xdd")
        fp.write(struct.pack(">I", obj_len))
    else:
        raise UnsupportedTypeException("huge array")

    for e in obj:
        dump(e, fp, options)


def _pack_map(obj, fp, options):
    obj_len = len(obj)
    if obj_len < 16:
        fp.write(struct.pack("B", 0x80 | obj_len))
    elif obj_len < 2 ** 16:
        fp.write(b"\xde")
        fp.write(struct.pack(">H", obj_len))
    elif obj_len < 2 ** 32:
        fp.write(b"\xdf")
        fp.write(struct.pack(">I", obj_len))
    else:
        raise UnsupportedTypeException("huge array")

    for k, v in obj.items():
        dump(k, fp, options)
        dump(v, fp, options)


def _utype(obj):
    raise UnsupportedTypeException(f"unsupported type: {str(type(obj))}")


# Pack with unicode 'str' type, 'bytes' type
def dump(obj, fp, options):
    # return packable object if supported in umsgpack_ext, else return obj
    # obj = mpext(obj, options)
    # ext_handlers = options.get("ext_handlers")

    if obj is None:
        _pack_nil(obj, fp)
    # elif ext_handlers and obj.__class__ in ext_handlers:
    #     _pack_ext(ext_handlers[obj.__class__](obj), fp)
    # elif obj.__class__ in ext_class_to_type:
    #     try:
    #         _pack_ext(Ext(ext_class_to_type[obj.__class__], obj.packb()), fp)
    #     except AttributeError:
    #         raise NotImplementedError(
    #             "Ext class {:s} lacks packb()".format(repr(obj.__class__))
    #         )
    elif isinstance(obj, bool):
        _pack_boolean(obj, fp)
    elif isinstance(obj, int):
        _pack_integer(obj, fp)
    elif isinstance(obj, float):
        _pack_float(obj, fp, options)
    elif isinstance(obj, str):
        _pack_string(obj, fp)
    elif isinstance(obj, bytes):
        _pack_binary(obj, fp)
    elif isinstance(obj, (list, tuple)):
        _pack_array(obj, fp, options)
    elif isinstance(obj, dict):
        _pack_map(obj, fp, options)
    # elif isinstance(obj, Ext):
    #     _pack_ext(obj, fp)
    # elif ext_handlers:
    #     # Linear search for superclass
    #     t = next((t for t in ext_handlers.keys() if isinstance(obj, t)), None)
    #     if t:
    #         _pack_ext(ext_handlers[t](obj), fp)
    #     else:
    #         _utype(obj)  # UnsupportedType
    # elif ext_class_to_type:
    #     # Linear search for superclass
    #     t = next((t for t in ext_class_to_type if isinstance(obj, t)), None)
    #     if t:
    #         try:
    #             _pack_ext(Ext(ext_class_to_type[t], obj.packb()), fp)
    #         except AttributeError:
    #             _utype(obj)
    #     else:
    #         _utype(obj)
    else:
        _utype(obj)


# Interface to __init__.py


def dumps(obj, options):
    fp = io.BytesIO()
    dump(obj, fp, options)
    return fp.getvalue()
