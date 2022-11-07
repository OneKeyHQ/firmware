class WrongAmountType(Exception):
    def __init(self):
        Exception.__init__(self, "amount (amt) must be a non-negative integer")


class WrongChecksumError(Exception):
    def __init__(self):
        Exception.__init__(self, "checksum failed to validate")


class WrongKeyLengthError(Exception):
    def __init__(self):
        Exception.__init__(self, "key length must be 58")


class WrongHashLengthError(Exception):
    """General error that is normally changed to be more specific"""

    def __init(self):
        Exception.__init__(self, "length must be 32 bytes")


class WrongMetadataLengthError(Exception):
    def __init(self):
        Exception.__init__(self, "metadata length must be 32 bytes")


class WrongLeaseLengthError(Exception):
    def __init(self):
        Exception.__init__(self, "lease length must be 32 bytes")


class WrongNoteType(Exception):
    def __init(self):
        Exception.__init__(self, 'note must be of type "bytes"')


class WrongNoteLength(Exception):
    def __init(self):
        Exception.__init__(self, "note length must be at most 1024")


class OutOfRangeDecimalsError(Exception):
    def __init__(self):
        Exception.__init__(self, "decimals must be between 0 and 19, inclusive")


class EmptyAddressError(Exception):
    def __init__(self):
        Exception.__init__(
            self,
            "manager, freeze, reserve, and clawback "
            "should not be empty unless "
            "strict_empty_address_check is set to False",
        )


class ZeroAddressError(Exception):
    def __init__(self):
        Exception.__init__(
            self,
            "For the zero address, please specify "
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAY5HFKQ",
        )


class KeyregOnlineTxnInitError(Exception):
    def __init__(self, attr):
        Exception.__init__(self, attr + " should not be None")
