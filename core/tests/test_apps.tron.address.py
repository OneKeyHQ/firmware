from common import *
from apps.common.paths import HARDENED

from trezor.crypto.curve import secp256k1

if not utils.BITCOIN_ONLY:
    from apps.tron.address import get_address_from_public_key


@unittest.skipUnless(not utils.BITCOIN_ONLY, "altcoin")
class TestTronAddress(unittest.TestCase):
    def test_privkey_to_address(self):
        #source of test data - binance javascript SDK
        privkey = "8d596057e510b14cbb9bf24f88803a6d6dbd138dd303a62b175b9e6cc0f3f941"
        expected_address = "TY72iA3SBtrds3QLYsS7LwYfkzXwAXCRWT"

        pubkey = secp256k1.publickey(unhexlify(privkey), False)
        address = get_address_from_public_key(pubkey[:65])

        self.assertEqual(address, expected_address)


if __name__ == '__main__':
    unittest.main()
