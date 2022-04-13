import click
import hashlib
import struct
from .. import special, tools
from . import with_client
from binascii import hexlify, unhexlify
import ecdsa


# Adapt OneKey Mini firmware header format
SLOTS = 4
pubkeys = {
    1: "045c4ee8d360b8ed1725a3f85368a14bb06ddc1ec54c5cdbbdfd97170f11ef32d635e096b53a2d4433025e33392132e745c5198c3169f32fbd65a000d7aa36d759",
    2: "04757f1f9734fcdae4011c93400266b026dd099a6f100e9de90b16ae16262917e8805db5ecae3e27d455c973e4abf77c563bfa73056d42b6187f5a3684fe064471",
    3: "046dda87edc24289aea929299abb5a279e913734b60642ec7b734891c46cf096fbf52af0f1989a73d86a2fe4353e0552673b1bd30227585e645255448d72e37f2a",
    4: "049120571ac8cc9d55a291ba52a6ff236b365c830dee9e739c337d33e20644406621010433c4ee644987e5b575b3ba4bf13fa04cbeea4b25a04dbfb10edad29725",
    5: "04a546fb3af222e8fd2e1835d2e3f9e1fa4379991c9a43108d4c209257f311e4fac4d00a7c5fe3d3f2000edc25a58dd153028ed9cdb26ffb19512a3981d74aeed0",
    6: "04204766d969b73a0cfdeb1c4f865a193355ba71425f8b7f815be3d15b5b373db054955a1637eb58267dbaab091e9db58980512c067cc7c13b93ada87fad49162d",
    7: "04fea486ffa8df90b93c712313302e52d948dfc24941bc9ea7740bc20b9a49ebe5ca6549c2c6d2f02581b6ce79e442ab3cb1abc61a005af4b34e0945697b1d6243",
}
FWHEADER_SIZE = 1024
SIGNATURES_START = 6 * 4 + 8 + 512
INDEXES_START = SIGNATURES_START + 4 * 64


def pad_to_size(data, size):
    if len(data) > size:
        raise ValueError("Chunk too big already")
    if len(data) == size:
        return data
    return data + b"\xFF" * (size - len(data))


def prepare_hashes(data):
    # process chunks
    start = 0
    end = (64 - 1) * 1024
    hashes = []
    for i in range(16):
        sector = data[start:end]
        if len(sector) > 0:
            chunk = pad_to_size(sector, end - start)
            hashes.append(hashlib.sha256(chunk).digest())
        else:
            hashes.append(b"\x00" * 32)
        start = end
        end += 64 * 1024
    return hashes


def check_hashes(data):
    expected_hashes = data[0x20 : 0x20 + 16 * 32]
    hashes = b""
    for h in prepare_hashes(data[FWHEADER_SIZE:]):
        hashes += h

    if expected_hashes == hashes:
        print("HASHES OK")
    else:
        print("HASHES NOT OK")


def update_hashes_in_header(data):
    # Store hashes in the firmware header
    data = bytearray(data)
    o = 0
    for h in prepare_hashes(data[FWHEADER_SIZE:]):
        data[0x20 + o : 0x20 + o + 32] = h
        o += 32
    return bytes(data)


def get_header(data, zero_signatures=False):
    if not zero_signatures:
        return data[:FWHEADER_SIZE]
    else:
        data = bytearray(data[:FWHEADER_SIZE])
        data[SIGNATURES_START : SIGNATURES_START + SLOTS * 64 + SLOTS] = b"\x00" * (SLOTS * 64 + SLOTS)
        return bytes(data)


def check_size(data):
    size = struct.unpack("<L", data[12:16])[0]
    assert size == len(data) - FWHEADER_SIZE


def modify(data, slot, index, signature):
    data = bytearray(data)
    # put index to data
    data[INDEXES_START + slot - 1] = index
    # put signature to data
    data[SIGNATURES_START + 64 * (slot - 1) : SIGNATURES_START + 64 * slot] = signature
    return bytes(data)


def check_signatures(data):
    # Analyses given firmware and prints out
    # status of included signatures

    indexes = [x for x in data[INDEXES_START : INDEXES_START + SLOTS]]

    to_sign = get_header(data, zero_signatures=True)
    fingerprint = hashlib.sha256(to_sign).hexdigest()

    print("Firmware fingerprint:", fingerprint)

    used = []
    for x in range(SLOTS):
        signature = data[SIGNATURES_START + 64 * x : SIGNATURES_START + 64 * x + 64]

        if indexes[x] == 0:
            print("Slot #%d" % (x + 1), "is empty")
            continue

        if indexes[x] in used:
            print("Slot #%d signature: DUPLICATE" % (x + 1), signature.hex())
            continue

        pk = pubkeys[indexes[x]]
        verify = ecdsa.VerifyingKey.from_string(
            bytes.fromhex(pk)[1:],
            curve=ecdsa.curves.SECP256k1,
            hashfunc=hashlib.sha256,
        )
        try:
            verify.verify(signature, to_sign, hashfunc=hashlib.sha256)
            used.append(indexes[x])
            print("Slot #%d signature: VALID" % (x + 1), signature.hex())
        except Exception:
            print("Slot #%d signature: INVALID" % (x + 1), signature.hex())


@click.command()
# fmt: off
@click.option("-c", "--coin")
@click.option("-n", "--address", default="m/44h/0h/0h/0/0", help="BIP-32 path")
@click.option("-e", "--extract", is_flag=True, help="Extract public key")
@click.option("-f", "--file", help="Firmware file path")
@click.option("-s", "--slot", type=int, help="Signature slot")
@click.option("-d", "--dry", is_flag=True, help="Dry run")
# fmt: on
@with_client
def sign_firmware(client, coin, address, extract, file, slot, dry):
    coin = coin or "Bitcoin"
    address_n = tools.parse_path(address)
    if extract:
        res = special.sign_digest(client, coin, address_n, bytes(32))
        return {
            "pubkey": res.pubkey.hex(),
        }

    if not file:
        raise Exception("-f/--file is required")

    data = open(file, "rb").read()
    assert len(data) % 4 == 0

    if data[:4] != b"TRZF" and data[:4] != b"MINI":
        raise Exception("Firmware header expected")

    data = update_hashes_in_header(data)
    print("Firmware size %d bytes" % len(data))
    check_size(data)
    check_signatures(data)
    check_hashes(data)

    to_sign = get_header(data, zero_signatures=True)
    res = special.sign_data(client, coin, address_n, to_sign)

    # Locate proper index of current signing key
    index = None
    for i, pk in pubkeys.items():
        if pk == res.pubkey.hex():
            index = i
            break
    if index is None:
        raise Exception("Unable to find public key index. Unknown public key?")

    # Cross-verify hardware-signed signature by software
    verify = ecdsa.VerifyingKey.from_string(
        res.pubkey[1:],
        curve=ecdsa.curves.SECP256k1,
        hashfunc=hashlib.sha256,
    )
    verify.verify(res.signature, to_sign, hashfunc=hashlib.sha256)
    print("Cross-Verification OK")

    if dry:
        return {
            "pubkey": res.pubkey.hex(),
            "digest": res.digest.hex(),
            "signature": res.signature.hex()
        }

    if not slot:
        slot = int(input("Enter signature slot (1-%d): " % SLOTS))
    if slot < 1 or slot > SLOTS:
        raise Exception("Invalid slot")

    data = modify(data, slot, index, res.signature)
    fp = open(file, "wb")
    fp.write(data)
    fp.close()
    check_signatures(data)
    check_hashes(data)


@click.command()
# fmt: off
@click.option("-n", "--address", default="m/44h/0h/0h/0/0", help="BIP-32 path")
# fmt: on
@with_client
def export_ed25519_pubkey(client, address):
    address_n = tools.parse_path(address)
    pubkey_res = special.export_ed25519_pubkey(client, address_n)

    data = bytes.fromhex("00" * 32)
    ctr = 1

    from trezorlib import cosi
    r, R = cosi.get_nonce(cosi.Ed25519PrivateKey(pubkey_res.privkey), data, ctr)
    print("cosi R:", R.hex())

    nonce_res = special.get_ed25519_nonce(client, address_n, data, ctr)
    return {
        "R": nonce_res.R.hex(),
        "r": nonce_res.r.hex(),
        #"r_src": nonce_res.r_src.hex(),
    }
