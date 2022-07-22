from common import *
import binascii
from ubinascii import hexlify, unhexlify

from trezor.crypto.curve import secp256k1
from trezor.crypto.hashlib import sha256

if not utils.BITCOIN_ONLY:
    from apps.tron.address import get_address_from_public_key
    from apps.tron.serialize import serialize
    from trezor.messages import TronSignTx, TronContract, TronTransferContract, TronTriggerSmartContract


@unittest.skipUnless(not utils.BITCOIN_ONLY, "altcoin")
class TestTronSign(unittest.TestCase):
    def test_trx_transfer_signature(self):
        expected_signature = "0daa54799c4fbe813674b28cb8621cd212f469acb6b754a560aec2b2d4f88cb719d41c74649113e9cbe4c9478f97f8369cb94ed0d24efee2428c9146f753b23a1b"
        private_key = "8d596057e510b14cbb9bf24f88803a6d6dbd138dd303a62b175b9e6cc0f3f941"
        public_key = secp256k1.publickey(binascii.unhexlify(private_key)[:], False)
        address = get_address_from_public_key(public_key[:65])
        transfer_contract = TronTransferContract(to_address="TXrs7yxQLNzig7J9EbKhoEiUp6kWpdWKnD", amount=100)
        contract = TronContract(transfer_contract=transfer_contract)
        msg = TronSignTx(ref_block_bytes=binascii.unhexlify("38cc")[:],
                            ref_block_hash=binascii.unhexlify("c7a72f2cfe33ffdf")[:],
                            expiration=1658230926000,
                            contract=contract,
                            timestamp=1658230876592)

        raw_data = serialize(msg, address)
        data_hash = sha256(raw_data).digest()

        signature = secp256k1.sign(binascii.unhexlify(private_key)[:], data_hash, False)

        signature = signature[1:65] + bytes([(~signature[0] & 0x01) + 27])
        signature = hexlify(signature).decode()
        # check if the signed data is the same as test vector
        self.assertEqual(signature, expected_signature)

    def test_trc20_transfer_signature(self):
        expected_signature = "42f6cd49e551de2c35ffc98eddcea9b16cdf67cbd7a286b508a763246ba239150a1ba902de24b49f721bbd28d0288e0a9334f52a71164d684aa1d16dd6c394ea1c"
        private_key = "8d596057e510b14cbb9bf24f88803a6d6dbd138dd303a62b175b9e6cc0f3f941"
        data="a9059cbb000000000000000000000000f01fad0beb95a0a41cb1e68f384b33b846fe7d830000000000000000000000000000000000000000000000000000000000000001"
        public_key = secp256k1.publickey(binascii.unhexlify(private_key)[:], False)
        address = get_address_from_public_key(public_key[:65])
        trigger_smart_contract = TronTriggerSmartContract(contract_address="TXXLsmZo5yzbwGZLoh7znccamTQyJx6Z74",
                                    data=binascii.unhexlify(data)[:])
        contract = TronContract(trigger_smart_contract=trigger_smart_contract)
        msg = TronSignTx(ref_block_bytes=binascii.unhexlify("3924")[:],
                            ref_block_hash=binascii.unhexlify("452512119e6a5a57")[:],
                            expiration=1658231226000,
                            fee_limit=1000000,
                            contract=contract,
                            timestamp=1658231166581)

        raw_data = serialize(msg, address)
        data_hash = sha256(raw_data).digest()

        signature = secp256k1.sign(binascii.unhexlify(private_key)[:], data_hash, False)

        signature = signature[1:65] + bytes([(~signature[0] & 0x01) + 27])
        signature = hexlify(signature).decode()
        # check if the signed data is the same as test vector
        self.assertEqual(signature, expected_signature)

if __name__ == '__main__':
    unittest.main()
