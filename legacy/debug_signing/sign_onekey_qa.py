#!/usr/bin/env python3

from hashlib import sha256
from typing import List, Tuple

import click
import ecdsa

from fill_t1_fw_signatures import Signatures


def signmessage(digest: bytes, key: ecdsa.SigningKey) -> bytes:
    """Sign via SignMessage"""
    btc_digest = b"\x18Bitcoin Signed Message:\n\x20" + digest
    final_digest = sha256(sha256(btc_digest).digest()).digest()
    return key.sign_digest_deterministic(final_digest, hashfunc=sha256)


def parse_privkey_args(privkey_data: List[str]) -> Tuple[int, List[str]]:
    privkeys = []
    sigmask = 0
    for key in privkey_data:
        try:
            idx, key_hex = key.split(":", maxsplit=1)
            privkeys.append(key_hex)
            sigmask |= 1 << (int(idx) - 1)
        except ValueError:
            click.echo(f"Could not parse key: {key}")
            click.echo("Keys must be in the format: <key index>:<hex-encoded key>")
            raise click.ClickException("Unrecognized key format.")
    return sigmask, privkeys


@click.command()
@click.option(
    "-S",
    "--sign-private",
    "privkey_data",
    metavar="INDEX:PRIVKEY_HEX",
    multiple=True,
    help="Private key to use for signing. Can be repeated.",
)
@click.argument("firmware_file", type=click.File("rb+"))
def cli(
    firmware_file,
    privkey_data,
):
    in_fw_fname = firmware_file.name
    out_fw_fname = firmware_file.name + ".qa.signed.bin"

    _, privkeys = parse_privkey_args(privkey_data)
    signatures = Signatures(in_fw_fname)
    digest = signatures.header_hash

    sig_indices = [1, 2, 3, 4]
    for idx in sig_indices:
        sk = ecdsa.SigningKey.from_string(
            bytes.fromhex(privkeys[idx - 1]), curve=ecdsa.SECP256k1
        )
        sig = signmessage(digest, sk)
        signatures.signature_pairs.append((idx, sig))

    signatures.patch_signatures()
    signatures.write_output_fw(out_fw_fname)


if __name__ == "__main__":
    cli()
