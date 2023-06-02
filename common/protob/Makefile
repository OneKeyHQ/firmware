check: messages.pb messages-binance.pb messages-bitcoin.pb messages-bootloader.pb messages-cardano.pb messages-common.pb messages-crypto.pb messages-debug.pb messages-emmc.pb messages-ethereum.pb messages-management.pb messages-monero.pb messages-nem.pb messages-ripple.pb messages-stellar.pb messages-tezos.pb messages-eos.pb messages-conflux.pb messages-solana.pb messages-starcoin.pb messages-tron.pb messages-aptos.pb

%.pb: %.proto
	protoc -I/usr/include -I. $< -o $@

combine:
	echo 'syntax = "proto2";' > combined.proto
	echo 'import "google/protobuf/descriptor.proto";' >> combined.proto
	grep -hv -e '^import ' -e '^syntax' -e '^package' -e 'option java_' messages*.proto | sed 's/ hw\.trezor\.messages\.common\./ /' | sed 's/ common\./ /' | grep -v '    reserved '>> combined.proto

clean:
	rm -f *.pb
