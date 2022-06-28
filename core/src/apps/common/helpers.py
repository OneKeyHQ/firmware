MAX_MESSAGE_LENGTH = 1024 * 8


def validate_message(message: bytes) -> None:
    if len(message) > MAX_MESSAGE_LENGTH:
        raise ValueError("Message too long, max 8KB")
