from .base import ScaleType


class Compact(ScaleType):
    def __init__(self, data=None, **kwargs):
        self.compact_length = 0
        self.compact_bytes = b""
        super().__init__(data, **kwargs)

    def process_compact_bytes(self):
        compact_byte = self.get_next_bytes(1)
        try:
            byte_mod = compact_byte[0] % 4
        except IndexError:
            raise Exception("Invalid byte for Compact")

        if byte_mod == 0:
            self.compact_length = 1
        elif byte_mod == 1:
            self.compact_length = 2
        elif byte_mod == 2:
            self.compact_length = 4
        else:
            self.compact_length = int(5 + (compact_byte[0] - 3) / 4)

        if self.compact_length == 1:
            self.compact_bytes = compact_byte
        elif self.compact_length in [2, 4]:
            self.compact_bytes = compact_byte + self.get_next_bytes(
                self.compact_length - 1
            )
        else:
            self.compact_bytes = self.get_next_bytes(self.compact_length - 1)

        return self.compact_bytes

    def process(self):
        self.process_compact_bytes()
        if self.compact_length <= 4:
            return int(int.from_bytes(self.compact_bytes, "little") / 4)
        else:
            return int.from_bytes(self.compact_bytes, "little")


class Era(ScaleType):
    """
    An Era represents a range of blocks in which a transaction is allowed to be
    executed.

    An Era may either be "immortal", in which case the transaction is always valid,
    or "mortal", in which case the transaction has a defined start block and period
    in which it is valid.
    """

    def __init__(self, data=None, **kwargs):
        self.period = None
        self.phase = None
        super().__init__(data, **kwargs)

    def process(self):

        option_byte = self.get_next_bytes(1)[0]
        if option_byte == 0:
            self.period = None
            self.phase = None
            return option_byte
        else:
            # encoded = int(option_byte, base=16) + (int(self.get_next_bytes(1).hex(), base=16) << 8)
            encoded = option_byte + (self.get_next_bytes(1)[0] << 8)
            self.period = 2 << (encoded % (1 << 4))
            quantize_factor = max(1, (self.period >> 12))
            self.phase = (encoded >> 4) * quantize_factor
            if self.period >= 4 and self.phase < self.period:
                return (self.period, self.phase)
            else:
                raise ValueError(
                    f"Invalid phase and period: {self.phase}, {self.period}"
                )
