#!/usr/bin/env python3
import argparse
import hashlib
import struct

import ecdsa

SLOTS = 3

pubkeys = {
    1: "04ad9035d67ac4795c913c452d25156f0b094c34f656a249b94d8d6619ab0d92b1e8bcc328bdc833b9b51ca31bfd0136615153f93aba46d02ab5d925f4f364c666",
    2: "0495b83da442c689bda82e9f9543811fec2f58334f5c76361f5a49fbb6634a81152c6da7b8a1782ccaa9287ec7a8efe9d8bba7d90180f7b719c017d804d61c3d5d",
    3: "04e0d5ef94cf950b9f855fb5526764dc28d2d96582c1ca1ad39cabab653e6198f80e6495e536b8bce37857dafc5a51952624ab083c33168ee3ed83e036defab85c",
    4: "04c9ec740cb3328165dc3fdb93a54e7075569b7a54cdeec0211fa9d952c6647960bd95f994c817458892d5db6eb34da76ce03c9c04a4325c2752647590b1a4f365",
    5: "04d208aba79b6f60f178608167ddb777860a8155022e28f120a41630869a4e0f0d1698a40eed2aedf74846e1e401cee6fdb1e81116d14cd2971270f2cb0c562c2e",
}

FWHEADER_SIZE = 1024
SIGNATURES_START = 6 * 4 + 8 + 512
INDEXES_START = SIGNATURES_START + 3 * 64


def parse_args():
    parser = argparse.ArgumentParser(
        description="Commandline tool for signing Trezor firmware."
    )
    parser.add_argument("-f", "--file", dest="path", help="Firmware file to modify")
    parser.add_argument(
        "-s",
        "--sign",
        dest="sign",
        action="store_true",
        help="Add signature to firmware slot",
    )
    parser.add_argument(
        "-p", "--pem", dest="pem", action="store_true", help="Use PEM instead of SECEXP"
    )
    parser.add_argument(
        "-g",
        "--generate",
        dest="generate",
        action="store_true",
        help="Generate new ECDSA keypair",
    )

    return parser.parse_args()


def pad_to_size(data, size):
    if len(data) > size:
        raise ValueError("Chunk too big already")
    if len(data) == size:
        return data
    return data + b"\xFF" * (size - len(data))


# see memory.h for details


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
        data[SIGNATURES_START : SIGNATURES_START + 3 * 64 + 3] = b"\x00" * (3 * 64 + 3)
        return bytes(data)


def check_size(data):
    size = struct.unpack("<L", data[12:16])[0]
    assert size == len(data) - 1024


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
            print(f"Slot #{x + 1}", "is empty")
        else:
            pk = pubkeys[indexes[x]]
            verify = ecdsa.VerifyingKey.from_string(
                bytes.fromhex(pk)[1:],
                curve=ecdsa.curves.SECP256k1,
                hashfunc=hashlib.sha256,
            )

            try:
                verify.verify(signature, to_sign, hashfunc=hashlib.sha256)

                if indexes[x] in used:
                    print(f"Slot #{x + 1} signature: DUPLICATE", signature.hex())
                else:
                    used.append(indexes[x])
                    print(f"Slot #{x + 1} signature: VALID", signature.hex())

            except Exception:
                print(f"Slot #{x + 1} signature: INVALID", signature.hex())


def modify(data, slot, index, signature):
    data = bytearray(data)
    # put index to data
    data[INDEXES_START + slot - 1] = index
    # put signature to data
    data[SIGNATURES_START + 64 * (slot - 1) : SIGNATURES_START + 64 * slot] = signature
    return bytes(data)


def sign(data, is_pem):
    # Ask for index and private key and signs the firmware

    slot = int(input(f"Enter signature slot (1-{SLOTS}): "))
    if slot < 1 or slot > SLOTS:
        raise Exception("Invalid slot")

    if is_pem:
        print("Paste ECDSA private key in PEM format and press Enter:")
        print("(blank private key removes the signature on given index)")
        pem_key = ""
        while True:
            key = input()
            pem_key += key + "\n"
            if key == "":
                break
        if pem_key.strip() == "":
            # Blank key,let's remove existing signature from slot
            return modify(data, slot, 0, b"\x00" * 64)
        key = ecdsa.SigningKey.from_pem(pem_key)
    else:
        print("Paste SECEXP (in hex) and press Enter:")
        print("(blank private key removes the signature on given index)")
        secexp = input()
        if secexp.strip() == "":
            # Blank key,let's remove existing signature from slot
            return modify(data, slot, 0, b"\x00" * 64)
        key = ecdsa.SigningKey.from_secret_exponent(
            secexp=int(secexp, 16),
            curve=ecdsa.curves.SECP256k1,
            hashfunc=hashlib.sha256,
        )

    to_sign = get_header(data, zero_signatures=True)

    # Locate proper index of current signing key
    pubkey = "04" + key.get_verifying_key().to_string().hex()
    index = None
    for i, pk in pubkeys.items():
        if pk == pubkey:
            index = i
            break

    if index is None:
        raise Exception("Unable to find private key index. Unknown private key?")

    signature = key.sign_deterministic(to_sign, hashfunc=hashlib.sha256)

    return modify(data, slot, index, signature)


def main(args):
    if args.generate:
        key = ecdsa.SigningKey.generate(
            curve=ecdsa.curves.SECP256k1, hashfunc=hashlib.sha256
        )

        print("PRIVATE KEY (SECEXP):")
        print(key.to_string().hex())
        print()

        print("PRIVATE KEY (PEM):")
        print(key.to_pem())

        print("PUBLIC KEY:")
        print("04" + key.get_verifying_key().to_string().hex())

        print("PUBLIC KEY:")
        pub_hex = "04" + key.get_verifying_key().to_string().hex()
        chunks = len(pub_hex)
        split_pub = [pub_hex[i : i + 2] for i in range(0, chunks, 2)]
        print("\\x" + "\\x".join(split_pub))
        return

    if not args.path:
        raise Exception("-f/--file is required")

    data = open(args.path, "rb").read()
    assert len(data) % 4 == 0

    if data[:4] != b"TRZF" and data[:4] != b"MINI":
        raise Exception("Firmware header expected")

    data = update_hashes_in_header(data)

    print(f"Firmware size {len(data)} bytes")

    check_size(data)
    check_signatures(data)
    check_hashes(data)

    if args.sign:
        data = sign(data, args.pem)
        check_signatures(data)
        check_hashes(data)

    fp = open(args.path, "wb")
    fp.write(data)
    fp.close()


if __name__ == "__main__":
    args = parse_args()
    main(args)
