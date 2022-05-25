import click
import hashlib
import struct
import json
from .. import special, tools
from . import with_client
from binascii import hexlify, unhexlify
import ecdsa

from trezorlib import cosi, _ed25519
from trezorlib._internal import firmware_headers


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
@click.option("-n", "--address", default="m/44h/0h/0h/0h/0h", help="BIP-32 path")
# fmt: on
@with_client
def export_ed25519_pubkey(client, address):
    address_n = tools.parse_path(address)
    res = special.export_ed25519_pubkey(client, address_n)
    print("pubkey:", res.pubkey.hex())


@click.command()
@with_client
def ed25519_test(client):
    data = bytes.fromhex("00" * 32)
    address_n_1 = tools.parse_path("m/44h/0h/0h/0h/0h")
    address_n_2 = tools.parse_path("m/44h/0h/0h/0h/1h")
    address_n_3 = tools.parse_path("m/44h/0h/0h/0h/2h")

    pk1 = special.export_ed25519_pubkey(client, address_n_1).pubkey
    pk2 = special.export_ed25519_pubkey(client, address_n_2).pubkey
    pk3 = special.export_ed25519_pubkey(client, address_n_3).pubkey
    global_pk = cosi.combine_keys([pk1, pk2, pk3])
    print("pubkeys %s:%s:%s" % (pk1.hex(), pk2.hex(), pk3.hex()))
    print("global_pk", global_pk.hex())

    R1 = special.get_ed25519_nonce(client, address_n_1, data, 0).R
    R2 = special.get_ed25519_nonce(client, address_n_2, data, 1).R
    R3 = special.get_ed25519_nonce(client, address_n_3, data, 2).R
    global_R = cosi.combine_keys([R1, R2, R3])
    print("Rs %s:%s:%s" % (R1.hex(), R2.hex(), R3.hex()))
    print("global_R", global_R.hex())

    sig1 = special.cosign_ed25519(client, address_n_1, data, 0, global_pk, global_R).sig
    sig2 = special.cosign_ed25519(client, address_n_2, data, 1, global_pk, global_R).sig
    sig3 = special.cosign_ed25519(client, address_n_3, data, 2, global_pk, global_R).sig
    sigs = [sig1, sig2, sig3]
    print("sigs %s:%s:%s" % (sig1.hex(), sig2.hex(), sig3.hex()))
    sig = cosi.combine_sig(global_R, sigs)
    print("sig", sig.hex())

    cosi.verify_combined(sig, data, global_pk)
    special.ed25519_verify(client, data, global_pk, sig)

    print("pass")


TOUCHPRO_PUBKEYS = [
    "154b8ab261cc8879483f689a2d41243ae7dbc4021672bbd25c338ae84d931154",
    "a9e65e07fe6d39a8a84e11a996a0283f881e175cba602eb5ac442fb75b39e8e0",
    "6c8805abb2df9d3679f1d28a40cd990399b99fc3ee4e0657d81d381ea1488a12",
    "3ed79779064d56571b29bcaa734cbb6db61d2e626566628ecf4c89e1db45eaec",
    "54a40633bfd9e60b8a391265b2e006374abe631d1e1107332bca56bf9f8c5c99",
    "4b71134f18e00787c583d40742cc188e17fc85ade4cb472dae5ef8e069f0fec5",
    "2ecf80c82b449848c000335092139551bfe47b3c7317b49950f65e1d82432024"
]


@click.command()
@with_client
@click.option("-n", "--address", default="m/44h/0h/0h/0h/0h", help="BIP-32 path")
@click.argument("firmware_file", type=click.File("rb+"))
def ed25519_commit(client, address, firmware_file):
    address_n = tools.parse_path(address)

    firmware_data = firmware_file.read()
    try:
        fw = firmware_headers.parse_image(firmware_data)
    except Exception as e:
        import traceback

        traceback.print_exc()
        magic = firmware_data[:4]
        raise click.ClickException(
            "Could not parse file (magic bytes: {!r})".format(magic)
        ) from e
    digest = fw.digest()

    pubkey = special.export_ed25519_pubkey(client, address_n).pubkey
    ctr = None
    for i, key in enumerate(TOUCHPRO_PUBKEYS):
        if pubkey.hex() == key:
            ctr = i
    if ctr is None:
        raise click.ClickException("Not found in signer public keys")

    R = special.get_ed25519_nonce(client, address_n, digest, ctr).R

    output = json.dumps({"magic": firmware_data[:4].decode('utf-8'),
            "digest": digest.hex(),
            "pubkey": pubkey.hex(),
            "R": R.hex()}, indent=4)
    click.echo(output)

    fname = "ed25519_commit_output_%s_%s.json" % (digest.hex()[0:4], pubkey.hex()[0:4])
    with open(fname, "w") as f:
        f.write(output)
    click.echo("output saved to file %s" % fname) 


@click.command()
@click.option("--input_files", "-i", required=True, multiple=True, help="ed25519_commit output files")
def ed25519_global_combine(input_files):
    if len(input_files) != 4:
        raise Exception("Need 4 input files for Global Key and Global R calculation")

    inputs = []
    for i in input_files:
        with open(i, "r") as f:
            inputs.append(json.loads(f.read()))

    magics = list(set([i['magic'] for i in inputs]))
    if len(magics) != 1:
        raise Exception("Image header magic number not matched")
    magic = magics[0]

    digests = list(set([i['digest'] for i in inputs]))
    if len(digests) != 1:
        raise Exception("Image digest not matched")
    digest = digests[0]

    R_pairs = [(i['pubkey'], i['R']) for i in inputs]
    pubkeys = list(set([p[0] for p in R_pairs]))
    if len(pubkeys) != 4:
        raise Exception("Need 4 different public keys for Global Key calculation")
    Rs = list(set([p[1] for p in R_pairs]))
    if len(Rs) != 4:
        raise Exception("Need 4 different Rs for Global R calculation")
    
    global_pubkey = cosi.combine_keys([bytes.fromhex(k) for k in pubkeys])
    global_commitment = cosi.combine_keys([bytes.fromhex(R) for R in Rs])

    output = json.dumps({"magic": magic,
            "digest": digest,
            "R_pairs": R_pairs,
            "global_pubkey": global_pubkey.hex(),
            "global_commitment": global_commitment.hex()}, indent=4)
    click.echo(output)

    fname = "ed25519_global_combine_output_%s.json" % digest[0:4]
    with open(fname, "w") as f:
        f.write(output)
    click.echo("output saved to file %s" % fname) 


@click.command()
@click.option("-c", "--commitment", required=True, help="the global R of ed25519 cosi")
@click.argument("sigs")
def ed25519_combine_sigs(sigs, commitment):
    sigs = [bytes.fromhex(k) for k in sigs.split(":")]
    global_R = bytes.fromhex(commitment)
    sig = cosi.combine_sig(global_R, sigs)
    click.echo(sig.hex())


@click.command()
@with_client
@click.option("-n", "--address", default="m/44h/0h/0h/0h/0h", help="BIP-32 path")
@click.option("-c", "--commitment_file", required=True, help="the global pubkey and commitment file")
@click.argument("firmware_file", type=click.File("rb+"))
def ed25519_cosign(client, address, commitment_file, firmware_file):
    address_n = tools.parse_path(address)

    firmware_data = firmware_file.read()
    try:
        fw = firmware_headers.parse_image(firmware_data)
    except Exception as e:
        import traceback

        traceback.print_exc()
        magic = firmware_data[:4]
        raise click.ClickException(
            "Could not parse file (magic bytes: {!r})".format(magic)
        ) from e

    with open(commitment_file, 'r') as f:
        commitment_data = json.loads(f.read())

    if firmware_data[:4].decode('utf-8') != commitment_data['magic']:
        raise Exception("Image file magic number not matched")

    digest = fw.digest()
    if digest.hex() != commitment_data['digest']:
        raise Exception("Image file digest not matched")

    pubkeys = [p[0] for p in commitment_data['R_pairs']]
    pubkey = special.export_ed25519_pubkey(client, address_n).pubkey
    if pubkey.hex() not in pubkeys:
        raise Exception("Public key not found in commitment data")

    try:
        ctr = TOUCHPRO_PUBKEYS.index(pubkey.hex())
    except ValueError:
        raise click.ClickException("Public key not owned by any holder")

    global_pubkey = bytes.fromhex(commitment_data['global_pubkey'])
    global_commitment = bytes.fromhex(commitment_data['global_commitment'])
    sig = special.cosign_ed25519(client, address_n, digest, ctr, global_pubkey, global_commitment).sig

    commitment_data['signing_pubkey'] = pubkey.hex()
    commitment_data['single_signature'] = sig.hex()
    output = json.dumps(commitment_data, indent=4)
    click.echo(output)

    fname = "ed25519_cosign_output_%s_%s_%s.json" % (digest.hex()[:4],
        global_commitment.hex()[:4], pubkey.hex()[:4])
    with open(fname, "w") as f:
        f.write(output)
    click.echo("output saved to file %s" % fname)


@click.command()
@with_client
@click.option("-s", "--signature", required=True, help="the combined cosi signature")
@click.option("-p", "--pubkeys", required=True, help="the pubkeys participate the cosigning")
@click.argument("firmware_file", type=click.File("rb+"))
def ed25519_insert_sig(client, signature, pubkeys, firmware_file):
    pubkeys = pubkeys.split(':')
    signature = bytes.fromhex(signature)
    firmware_data = firmware_file.read()

    try:
        fw = firmware_headers.parse_image(firmware_data)
    except Exception as e:
        import traceback

        traceback.print_exc()
        magic = firmware_data[:4]
        raise click.ClickException(
            "Could not parse file (magic bytes: {!r})".format(magic)
        ) from e
    click.echo(fw.format(True))

    sigmask = 0
    for pk in pubkeys:
        idx = TOUCHPRO_PUBKEYS.index(pk)
        sigmask |= 1 << idx

    fw.rehash()
    fw.insert_signature(signature, sigmask)
    cosi.verify(signature, fw.digest(), 4, [bytes.fromhex(k) for k in TOUCHPRO_PUBKEYS], sigmask)
    click.echo(fw.format(True))

    updated_data = fw.dump()
    firmware_file.seek(0)
    firmware_file.truncate(0)
    firmware_file.write(updated_data)
