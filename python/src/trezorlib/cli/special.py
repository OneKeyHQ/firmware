import click
import hashlib
import struct
from .. import special, tools
from . import with_client
from binascii import hexlify, unhexlify
import ecdsa


# Adapt OneKey Mini firmware header format
SLOTS = 4
PUBKEYS = 7
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

        used.append(indexes[x])
        print("Slot #%d signature: FOUND" % (x + 1), signature.hex())
        #TODO: verify all signature after pubkeys determined


@click.command()
# fmt: off
@click.option("-c", "--coin")
@click.option("-n", "--address", required=True, help="BIP-32 path")
@click.option("-e", "--extract", is_flag=True, help="Extract public key")
@click.option("-f", "--file", help="Firmware file path")
@click.option("-s", "--slot", type=int, help="Signature slot")
@click.option("-i", "--index", type=int, help="Pubkey index") #TODO: remove after all pubkeys determined
@click.option("-d", "--dry", is_flag=True, help="Dry run")
# fmt: on
@with_client
def sign_firmware(client, coin, address, extract, file, slot, index, dry):
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

    if data[:4] != b"TRZF":
        raise Exception("Firmware header expected")

    data = update_hashes_in_header(data)
    print("Firmware size %d bytes" % len(data))
    check_size(data)
    check_signatures(data)
    check_hashes(data)

    to_sign = get_header(data, zero_signatures=True)
    res = special.sign_data(client, coin, address_n, to_sign)

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
    else:
        if not slot:
            raise Exception("-s/--slot is required")
        if slot < 1 or slot > SLOTS:
            raise Exception("Invalid slot")

        if not index:
            raise Exception("-i/--index is required")
        if index < 1 or index > PUBKEYS:
            raise Exception("Invalid index")

        data = modify(data, slot, index, res.signature)
        fp = open(file, "wb")
        fp.write(data)
        fp.close()
        check_signatures(data)
        check_hashes(data)
