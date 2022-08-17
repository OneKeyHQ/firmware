from trezor.crypto import base58


class PublicKey:

    LENGTH = 32
    """Constant for standard length of a public key."""

    def __init__(self, value: bytes | str) -> None:
        if isinstance(value, str):
            try:
                self._key = base58.decode(value)
            except ValueError as err:
                raise ValueError("invalid public key input:", value) from err
            if len(self._key) != self.LENGTH:
                raise ValueError("invalid public key input:", value)
        else:
            self._key = bytes(value)

        if len(self._key) > self.LENGTH:
            raise ValueError("invalid public key input:", value)

    def get(self) -> bytes:
        return self._key

    def __eq__(self, other: "PublicKey") -> bool:
        """Equality definition for PublicKeys."""
        return False if not isinstance(other, PublicKey) else self.get() == other.get()

    def __str__(self) -> str:
        """String definition for PublicKey."""
        return base58.encode(self._key)
