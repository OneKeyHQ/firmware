from . import writers
from .common import create_hashwriter, tagged_hashwriter
from .scripts import write_output_script_p2pkh

UTXO = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
TAG = b"BIP0322-signed-message"


def create_to_spend(message: bytes, script_pub: bytes) -> bytes:
    tag_hash_writer = tagged_hashwriter(TAG)
    tag_hash_writer.write(message)
    message_hash = tag_hash_writer.get_digest()
    script_sig = b"\x00\x20" + message_hash
    h_tx = create_hashwriter()
    # nVersion
    writers.write_uint32(h_tx, 0)
    # inputs
    writers.write_compact_size(h_tx, 1)
    writers.write_bytes_reversed(h_tx, UTXO, writers.TX_HASH_SIZE)
    writers.write_uint32(h_tx, 0xFFFFFFFF)
    writers.write_bytes_prefixed(h_tx, script_sig)
    writers.write_uint32(h_tx, 0)
    # outputs
    writers.write_compact_size(h_tx, 1)
    writers.write_uint64(h_tx, 0)
    writers.write_bytes_prefixed(h_tx, script_pub)
    # nLockTime
    writers.write_uint32(h_tx, 0)

    return writers.get_tx_hash(h_tx, double=True, reverse=True)


def sighash_bip341(message: bytes, script_pub: bytes) -> bytes:
    tx_id = create_to_spend(message, script_pub)
    h_sigmsg = tagged_hashwriter(b"TapSighash")
    h_prevouts = create_hashwriter()
    h_amounts = create_hashwriter()
    h_scriptpubkeys = create_hashwriter()
    h_sequences = create_hashwriter()
    h_outputs = create_hashwriter()
    # sighash epoch 0
    writers.write_uint8(h_sigmsg, 0)

    # nHashType
    writers.write_uint8(h_sigmsg, 0)

    # nVersion
    writers.write_uint32(h_sigmsg, 0)

    # nLockTime
    writers.write_uint32(h_sigmsg, 0)

    # sha_prevouts
    writers.write_bytes_reversed(h_prevouts, tx_id, writers.TX_HASH_SIZE)
    writers.write_uint32(h_prevouts, 0)
    writers.write_bytes_fixed(h_sigmsg, h_prevouts.get_digest(), writers.TX_HASH_SIZE)

    # sha_amounts
    writers.write_uint64(h_amounts, 0)
    writers.write_bytes_fixed(h_sigmsg, h_amounts.get_digest(), writers.TX_HASH_SIZE)

    # sha_scriptpubkeys
    writers.write_bytes_prefixed(h_scriptpubkeys, script_pub)
    writers.write_bytes_fixed(
        h_sigmsg, h_scriptpubkeys.get_digest(), writers.TX_HASH_SIZE
    )

    # sha_sequences
    writers.write_uint32(h_sequences, 0)
    writers.write_bytes_fixed(h_sigmsg, h_sequences.get_digest(), writers.TX_HASH_SIZE)

    # sha_outputs
    writers.write_uint64(h_outputs, 0)
    writers.write_bytes_prefixed(h_outputs, b"\x6a")
    writers.write_bytes_fixed(h_sigmsg, h_outputs.get_digest(), writers.TX_HASH_SIZE)

    # spend_type 0 (no tapscript message extension, no annex)
    writers.write_uint8(h_sigmsg, 0)

    # input_index
    writers.write_uint32(h_sigmsg, 0)

    return h_sigmsg.get_digest()


def sighash_bip143(
    message: bytes, script_pub: bytes, pubkeyhash: bytes, sign_hash_double: bool = True
) -> bytes:
    tx_id = create_to_spend(message, script_pub)
    h_preimage = create_hashwriter()
    h_prevouts = create_hashwriter()
    h_sequences = create_hashwriter()
    h_outputs = create_hashwriter()

    # nVersion
    writers.write_uint32(h_preimage, 0)

    # hashPrevouts
    writers.write_bytes_reversed(h_prevouts, tx_id, writers.TX_HASH_SIZE)
    writers.write_uint32(h_prevouts, 0)
    prevouts_hash = writers.get_tx_hash(h_prevouts, double=sign_hash_double)
    writers.write_bytes_fixed(h_preimage, prevouts_hash, writers.TX_HASH_SIZE)

    # hashSequence
    writers.write_uint32(h_sequences, 0)
    sequence_hash = writers.get_tx_hash(h_sequences, double=sign_hash_double)
    writers.write_bytes_fixed(h_preimage, sequence_hash, writers.TX_HASH_SIZE)

    # outpoint
    writers.write_bytes_reversed(h_preimage, tx_id, writers.TX_HASH_SIZE)
    writers.write_uint32(h_preimage, 0)

    # scriptCode
    write_output_script_p2pkh(h_preimage, pubkeyhash, prefixed=True)

    # amount
    writers.write_uint64(h_preimage, 0)

    # nSequence
    writers.write_uint32(h_preimage, 0)

    # hashOutputs
    writers.write_uint64(h_outputs, 0)
    writers.write_bytes_prefixed(h_outputs, b"\x6a")
    outputs_hash = writers.get_tx_hash(h_outputs, double=sign_hash_double)
    writers.write_bytes_fixed(h_preimage, outputs_hash, writers.TX_HASH_SIZE)

    # nLockTime
    writers.write_uint32(h_preimage, 0)

    # nHashType
    writers.write_uint32(h_preimage, 1)

    return writers.get_tx_hash(h_preimage, double=sign_hash_double)
