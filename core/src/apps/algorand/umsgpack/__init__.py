# __init__.py A lightweight MessagePack serializer and deserializer module.

# Adapted for MicroPython by Peter Hinch
# Copyright (c) 2021 Peter Hinch Released under the MIT License see LICENSE

# This is a port of the version at
# https://github.com/vsergeev/u-msgpack-python
# refactored to reduce RAM consumption and trimmed to remove functionality
# irrelevant to MicroPython. It is compliant with a subset of the latest
# MessagePack specification at
# https://github.com/msgpack/msgpack/blob/master/spec.md
# In particular, it supports the new binary, UTF-8 string, and application ext
# types. It does not support timestamps.

# Principal changes.
# Python2 code removed.
# Compatibility mode removed.
# Timestamps removed.
# Converted to Python package with lazy import to save RAM.
# Provide uasyncio StreamReader support.
# Exported functions now match ujson: dump, dumps, load, loads (only).
# Many functions refactored to save bytes, especially replace function
# dispatch table with code.
# Further refactoring to reduce allocations.
# InvalidString class removed because it is a subclass of a native type.
# Method of detecting platform's float size changed.
# Version reset to (0.1.0).

__version__ = (0, 1, 1)


##############################################################################
# Exceptions
##############################################################################


# Base Exception classes
class PackException(Exception):
    "Base class for exceptions encountered during packing."


class UnpackException(Exception):
    "Base class for exceptions encountered during unpacking."


# Packing error
class UnsupportedTypeException(PackException):
    "Object type not supported for packing."


# Unpacking error
class InsufficientDataException(UnpackException):
    "Insufficient data to unpack the serialized object."


class InvalidStringException(UnpackException):
    "Invalid UTF-8 string encountered during unpacking."


class ReservedCodeException(UnpackException):
    "Reserved code encountered during unpacking."


class UnhashableKeyException(UnpackException):
    """
    Unhashable key encountered during map unpacking.
    The serialized map cannot be deserialized into a Python dictionary.
    """


class DuplicateKeyException(UnpackException):
    "Duplicate key encountered during map unpacking."


# Lazy module load to save RAM: takes about 20μs on Pyboard 1.x after initial load


def load(fp, **options):
    """
    Deserialize MessagePack bytes into a Python object.

    Args:
        fp: a .read()-supporting file-like object

    Kwargs:
        ext_handlers (dict): dictionary of Ext handlers, mapping integer Ext
                             type to a callable that unpacks an instance of
                             Ext into an object
        use_ordered_dict (bool): unpack maps into OrderedDict, instead of
                                 unordered dict (default False)
        use_tuple (bool): unpacks arrays into tuples, instead of lists
                                 (default False)
        allow_invalid_utf8 (bool): unpack invalid strings into bytes
                                 (default False)

    Returns:
        A Python object.

    Raises:
        InsufficientDataException(UnpackException):
            Insufficient data to unpack the serialized object.
        InvalidStringException(UnpackException):
            Invalid UTF-8 string encountered during unpacking.
        ReservedCodeException(UnpackException):
            Reserved code encountered during unpacking.
        UnhashableKeyException(UnpackException):
            Unhashable key encountered during map unpacking.
            The serialized map cannot be deserialized into a Python dictionary.
        DuplicateKeyException(UnpackException):
            Duplicate key encountered during map unpacking.

    Example:
    >>> f = open('test.bin', 'rb')
    >>> umsgpack.loads(f)
    {'compact': True, 'schema': 0}
    >>>
    """
    from . import mp_load

    return mp_load.load(fp, options)


def loads(s, **options):
    """
    Deserialize MessagePack bytes into a Python object.

    Args:
        s: a 'bytes' or 'bytearray' containing serialized MessagePack bytes

    Kwargs:
        ext_handlers (dict): dictionary of Ext handlers, mapping integer Ext
                             type to a callable that unpacks an instance of
                             Ext into an object
        use_ordered_dict (bool): unpack maps into OrderedDict, instead of
                                 unordered dict (default False)
        use_tuple (bool): unpacks arrays into tuples, instead of lists
                                 (default False)
        allow_invalid_utf8 (bool): unpack invalid strings into bytes
                                 (default False)

    Returns:
        A Python object.

    Raises:
        TypeError:
            Packed data type is neither 'bytes' nor 'bytearray'.
        InsufficientDataException(UnpackException):
            Insufficient data to unpack the serialized object.
        InvalidStringException(UnpackException):
            Invalid UTF-8 string encountered during unpacking.
        ReservedCodeException(UnpackException):
            Reserved code encountered during unpacking.
        UnhashableKeyException(UnpackException):
            Unhashable key encountered during map unpacking.
            The serialized map cannot be deserialized into a Python dictionary.
        DuplicateKeyException(UnpackException):
            Duplicate key encountered during map unpacking.

    Example:
    >>> umsgpack.loads(b'\x82\xa7compact\xc3\xa6schema\x00')
    {'compact': True, 'schema': 0}
    >>>
    """
    from . import mp_load

    return mp_load.loads(s, options)


def dump(obj, fp, **options):
    """
    Serialize a Python object into MessagePack bytes.

    Args:
        obj: a Python object
        fp: a .write()-supporting file-like object

    Kwargs:
        ext_handlers (dict): dictionary of Ext handlers, mapping a custom type
                             to a callable that packs an instance of the type
                             into an Ext object
        force_float_precision (str): "single" to force packing floats as
                                     IEEE-754 single-precision floats,
                                     "double" to force packing floats as
                                     IEEE-754 double-precision floats.

    Returns:
        None.

    Raises:
        UnsupportedType(PackException):
            Object type not supported for packing.

    Example:
    >>> f = open('test.bin', 'wb')
    >>> umsgpack.dump({u"compact": True, u"schema": 0}, f)
    >>>
    """
    from . import mp_dump

    mp_dump.dump(obj, fp, options)


def dumps(obj, **options):
    """
    Serialize a Python object into MessagePack bytes.

    Args:
        obj: a Python object

    Kwargs:
        ext_handlers (dict): dictionary of Ext handlers, mapping a custom type
                             to a callable that packs an instance of the type
                             into an Ext object
        force_float_precision (str): "single" to force packing floats as
                                     IEEE-754 single-precision floats,
                                     "double" to force packing floats as
                                     IEEE-754 double-precision floats.

    Returns:
        A 'bytes' containing serialized MessagePack bytes.

    Raises:
        UnsupportedType(PackException):
            Object type not supported for packing.

    Example:
    >>> umsgpack.dumps({u"compact": True, u"schema": 0})
    b'\x82\xa7compact\xc3\xa6schema\x00'
    >>>
    """
    from . import mp_dump

    return mp_dump.dumps(obj, options)


async def aload(fp, **options):
    """
    Deserialize MessagePack bytes from a StreamReader into a Python object.

    Args:
        fp: a uasyncio StreamReader object

    Kwargs:
        ext_handlers (dict): dictionary of Ext handlers, mapping integer Ext
                             type to a callable that unpacks an instance of
                             Ext into an object
        use_ordered_dict (bool): unpack maps into OrderedDict, instead of
                                 unordered dict (default False)
        use_tuple (bool): unpacks arrays into tuples, instead of lists
                                 (default False)
        allow_invalid_utf8 (bool): unpack invalid strings into bytes
                                 (default False)

    Returns:
        A Python object.

    Raises:
        InsufficientDataException(UnpackException):
            Insufficient data to unpack the serialized object.
        InvalidStringException(UnpackException):
            Invalid UTF-8 string encountered during unpacking.
        ReservedCodeException(UnpackException):
            Reserved code encountered during unpacking.
        UnhashableKeyException(UnpackException):
            Unhashable key encountered during map unpacking.
            The serialized map cannot be deserialized into a Python dictionary.
        DuplicateKeyException(UnpackException):
            Duplicate key encountered during map unpacking.

    Example:
    >>> f = open('test.bin', 'rb')
    >>> umsgpack.loads(f)
    {'compact': True, 'schema': 0}
    >>>
    """
    from . import as_load

    return await as_load.aload(fp, options)
