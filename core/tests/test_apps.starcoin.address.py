from common import *

from trezor.crypto.curve import ed25519

if not utils.BITCOIN_ONLY:
    from apps.starcoin.helper import get_address_from_public_key


@unittest.skipUnless(not utils.BITCOIN_ONLY, "altcoin")
class TestStarcoinAddress(unittest.TestCase):
    def test_privkey_to_address(self):
        privkey = "27c2f608d2a574f76e215c5301fd5bbc9612930f94dd0e8203c97dea41114ad5"
        expected_address = "0xceae70e49dfd02a914cddcfd61ee72ca"

        pubkey = ed25519.publickey(unhexlify(privkey))
        address = get_address_from_public_key(pubkey)

        self.assertEqual(address, expected_address)


if __name__ == '__main__':
    unittest.main()
