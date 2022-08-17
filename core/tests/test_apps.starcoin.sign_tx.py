from common import *
import binascii
from ubinascii import hexlify, unhexlify

from trezor.crypto.curve import ed25519
from trezor.crypto.hashlib import sha3_256

if not utils.BITCOIN_ONLY:
    from apps.starcoin.helper import get_address_from_public_key, serialize_u32_as_uleb128


@unittest.skipUnless(not utils.BITCOIN_ONLY, "altcoin")
class TestStarcoinSign(unittest.TestCase):
    def test_sign_raw_transaction(self):
        raw_tx="ceae70e49dfd02a914cddcfd61ee72ca000000000000000002000000000000000000000000000000010f5472616e73666572536372697074730f706565725f746f5f706565725f763201070000000000000000000000000000000103535443035354430002104c7afc223df1d47072194dcefe26a4451000040000000000000000000000000000809698000000000001000000000000000d3078313a3a5354433a3a5354434eb6fb6200000000fb"
        expected_signature = "932a4f5b5d3b00871337899b9810a1e5586058f3769fc7e6a647b050ce49a9386d5a20a870ff2edfb3c8cbac9cdc724963cfdacaf2e365805f1dedff0c11290a"
        private_key = "27c2f608d2a574f76e215c5301fd5bbc9612930f94dd0e8203c97dea41114ad5"

        prefix = sha3_256("STARCOIN::RawUserTransaction", keccak=False).digest()
        data = prefix + unhexlify(raw_tx)
        signature = ed25519.sign(unhexlify(private_key), data)
        signature = hexlify(signature).decode()

        # check if the signed data is the same as test vector
        self.assertEqual(signature, expected_signature)


if __name__ == '__main__':
    unittest.main()
