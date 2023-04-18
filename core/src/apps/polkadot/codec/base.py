from typing import Union
from ubinascii import hexlify, unhexlify


class ScaleBytes:
    def __init__(self, data: Union[str, bytes, bytearray]):
        self.offset = 0

        if type(data) is bytearray:
            self.data = data
        elif type(data) is bytes:
            self.data = bytearray(data)
        elif type(data) is str and data[0:2] == "0x":
            self.data = unhexlify(data[2:].lower().replace("0x", ""))
        else:
            raise ValueError(
                f"Provided data is not in supported format: provided '{type(data)}'"
            )

        self.length = len(self.data)

    def get_next_bytes(self, length: int) -> bytearray:
        data = self.data[self.offset : self.offset + length]
        self.offset += length
        return bytearray(data)

    def get_remaining_bytes(self) -> bytearray:
        data = self.data[self.offset :]
        self.offset = self.length
        return bytearray(data)

    def get_remaining_length(self) -> int:
        return self.length - self.offset

    def reset(self):
        self.offset = 0

    def __str__(self):
        return f"0x{hexlify(self.data).decode()}"

    def __eq__(self, other):
        if not hasattr(other, "data"):
            return False
        return self.data == other.data

    def __len__(self):
        return len(self.data)

    def __repr__(self):
        return f"<{self.__class__.__name__}(data=0x{hexlify(self.data).decode()})>"

    def __add__(self, data):
        if type(data) == ScaleBytes:
            return ScaleBytes(self.data + data.data)

        if type(data) == bytes:
            data = bytearray(data)
        elif type(data) == str and data[0:2] == "0x":
            data = unhexlify(data[2:].lower().replace("0x", ""))

        return ScaleBytes(self.data + data)

    def to_hex(self) -> str:
        return f"0x{hexlify(self.data).decode()}"


class ScaleDecoder:

    type_string = None

    type_mapping = None

    debug = False

    sub_type = None

    runtime_config = None

    def __init__(self, data: ScaleBytes, sub_type: str = None, runtime_config=None):

        if sub_type:
            self.sub_type = sub_type

        # if self.type_mapping is None and self.type_string:
        #     self.build_type_mapping()

        if data:
            assert type(data) == ScaleBytes

        if runtime_config:
            self.runtime_config = runtime_config

        # if not self.runtime_config:
        #     # if no runtime config is provided, fallback on singleton
        #     self.runtime_config = RuntimeConfiguration()

        self.data = data

        self.value_object = None
        self.value_serialized = None

        self.decoded = False

        self.data_start_offset = None
        self.data_end_offset = None

    @property
    def value(self):
        # TODO fix
        # if not self.decoded:
        #     self.decode()
        return self.value_serialized

    @value.setter
    def value(self, value):
        self.value_serialized = value

    @staticmethod
    def is_primitive(type_string: str) -> bool:
        return type_string in (
            "bool",
            "u8",
            "u16",
            "u32",
            "u64",
            "u128",
            "u256",
            "i8",
            "i16",
            "i32",
            "i64",
            "i128",
            "i256",
            "h160",
            "h256",
            "h512",
            "[u8; 4]",
            "[u8; 4]",
            "[u8; 8]",
            "[u8; 16]",
            "[u8; 32]",
            "&[u8]",
        )

    def get_next_bytes(self, length) -> bytearray:
        data = self.data.get_next_bytes(length)
        return data

    def get_next_u8(self) -> int:
        return int.from_bytes(self.get_next_bytes(1), byteorder="little")

    def get_next_bool(self) -> bool:
        data = self.get_next_bytes(1)
        if data not in [b"\x00", b"\x01"]:
            raise Exception("Invalid value for datatype: bool")
        return data == b"\x01"

    def get_remaining_bytes(self) -> bytearray:
        data = self.data.get_remaining_bytes()
        return bytearray(data)

    def get_used_bytes(self) -> bytearray:
        return bytearray(self.data.data[self.data_start_offset : self.data_end_offset])

    def decode(self, data: ScaleBytes = None, check_remaining=True):
        if data is not None:
            self.decoded = False
            self.data = data

        if not self.decoded:

            self.data_start_offset = self.data.offset
            self.value_serialized = self.process()  # # pylint: disable=E1128
            self.decoded = True

            if self.value_object is None:
                # Default for value_object if not explicitly defined
                self.value_object = self.value_serialized

            self.data_end_offset = self.data.offset

            if check_remaining and self.data.offset != self.data.length:
                raise Exception(
                    f"Decoding <{self.__class__.__name__}> - Current offset: {self.data.offset} / length: {self.data.length}"
                )

            if self.data.offset > self.data.length:
                raise Exception(
                    f"Decoding <{self.__class__.__name__}> - No more bytes available (needed: {self.data.offset} / total: {self.data.length})"
                )

        return self.value

    def __str__(self):
        return str(self.serialize()) or ""

    def __repr__(self):
        return f"<{self.__class__.__name__}(value={self.serialize()})>"

    def serialize(self):
        return self.value_serialized

    def process(self):
        return


class ScaleType(ScaleDecoder):

    # scale_info_type: 'GenericRegistryType' = None

    def __init__(self, data=None, sub_type=None, metadata=None, runtime_config=None):
        """

        Parameters
        ----------
        data: ScaleBytes
        sub_type: str
        metadata: VersionedMetadata
        runtime_config: RuntimeConfigurationObject
        """
        self.metadata = metadata

        # Container for meta information
        self.meta_info: dict = {}

        if not data:
            data = ScaleBytes(bytearray())
        super().__init__(data, sub_type, runtime_config=runtime_config)

    # def __iter__(self):
    #     for item in self.value_object:
    #         yield item

    # def __eq__(self, other):
    #     if isinstance(other, ScaleType):
    #         return other.value_serialized == self.value_serialized
    #     else:
    #         return other == self.value_serialized

    # def __gt__(self, other):
    #     if isinstance(other, ScaleType):
    #         return self.value_serialized > other.value_serialized
    #     else:
    #         return self.value_serialized > other

    # def __ge__(self, other):
    #     if isinstance(other, ScaleType):
    #         return self.value_serialized >= other.value_serialized
    #     else:
    #         return self.value_serialized >= other

    # def __lt__(self, other):
    #     if isinstance(other, ScaleType):
    #         return self.value_serialized < other.value_serialized
    #     else:
    #         return self.value_serialized < other

    # def __le__(self, other):
    #     if isinstance(other, ScaleType):
    #         return self.value_serialized <= other.value_serialized
    #     else:
    #         return self.value_serialized <= other
