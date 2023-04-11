APPVER = 1.8.0

FIRMWARE_BUILD_VERSION=$(shell cat version.h | grep "ONEKEY_VERSION " | awk -F "[\"\"]" '{print $$2}')
FIRMWARE_BUILD_DATE=$(shell date "+%m%d")
ifndef BUILD_ID
BUILD_COMMIT=$(shell git rev-parse HEAD | cut -c1-7)
NAME=mini.$(FIRMWARE_BUILD_VERSION)-Stable-$(FIRMWARE_BUILD_DATE)-$(BUILD_COMMIT)
else
NAME=mini.$(FIRMWARE_BUILD_VERSION)-Alpha-$(FIRMWARE_BUILD_DATE)-$(BUILD_ID)
endif

SECP256K1_ZKP ?= 1

ifeq ($(SECP256K1_ZKP),1)
CFLAGS   += -DSECP256K1_ZKP
CFLAGS   += -DUSE_SECP256K1_ZKP
CFLAGS   += -DUSE_SECP256K1_ZKP_ECDSA
ifeq ($(EMULATOR),1)
CFLAGS += -DSECP256K1_CONTEXT_SIZE=208
else
CFLAGS += -DSECP256K1_CONTEXT_SIZE=180
OBJS += field_10x26_arm.o
endif
ZKP_CFLAGS = \
	-DUSE_ASM_ARM \
	-DUSE_EXTERNAL_ASM \
	-DUSE_EXTERNAL_DEFAULT_CALLBACKS \
	-DECMULT_GEN_PREC_BITS=4 \
	-DECMULT_WINDOW_SIZE=8 \
	-DENABLE_MODULE_GENERATOR \
	-DENABLE_MODULE_RECOVERY \
	-DENABLE_MODULE_SCHNORRSIG \
	-DENABLE_MODULE_EXTRAKEYS

OBJS += secp256k1-zkp.o
OBJS += precomputed_ecmult.o
OBJS += precomputed_ecmult_gen.o
OBJS += ../vendor/trezor-crypto/zkp_bip340.o
OBJS += ../vendor/trezor-crypto/zkp_context.o
OBJS += ../vendor/trezor-crypto/zkp_ecdsa.o
endif

ifeq ($(EMULATOR),1)
OBJS += udp.o
else
ifeq ($(ONEKEY_MINI),1)
OBJS += font_ex.o
OBJS += device2.o
OBJS += flash_enc.o
OBJS += nft.o
endif
OBJS += usb.o
OBJS += bl_check.o
OBJS += header.o
OBJS += startup.o
endif

OBJS += font.o
OBJS += chinese.o
OBJS += gettext.o
OBJS += language.o
OBJS += menu_para.o
OBJS += menu_core.o
OBJS += menu_list.o
OBJS += messages.o
OBJS += config.o
OBJS += trezor.o
OBJS += pinmatrix.o
OBJS += fsm.o
OBJS += coins.o
OBJS += coin_info.o
OBJS += transaction.o
OBJS += protect.o
OBJS += layout2.o
OBJS += recovery.o
OBJS += reset.o
OBJS += signing.o
OBJS += crypto.o

ifneq ($(BITCOIN_ONLY),1)
OBJS += u2f.o
OBJS += ethereum.o
OBJS += ethereum_tokens.o
OBJS += nem2.o
OBJS += nem_mosaics.o
OBJS += solana.o
OBJS += starcoin.o
OBJS += stellar.o
OBJS += tron.o
OBJS += tron_ui.o
OBJS += tron_tokens.o
OBJS += aptos.o
OBJS += near.o
OBJS += conflux.o
OBJS += conflux_tokens.o
OBJS += algorand.o
OBJS += algo/algo_asa.o
OBJS += algo/base64.o
OBJS += algo/parser_encoding.o
OBJS += algo/parser_impl.o
OBJS += algo/parser.o
OBJS += algo/tx.o
OBJS += ripple.o
OBJS += sui.o
OBJS += filecoin.o
OBJS += filecoin/cborparser.o
OBJS += filecoin/cborvalidation.o
OBJS += filecoin/parser_impl.o
OBJS += filecoin/tx.o
OBJS += filecoin/parser.o
OBJS += jsmn.o
OBJS += cosmos/json_parser.o
OBJS += cosmos/tx_validate.o
OBJS += cosmos/tx_parser.o
OBJS += cosmos/tx_display.o
OBJS += cosmos/parser.o
OBJS += cosmos_networks.o
OBJS += cosmos.o
OBJS += polkadot.o
OBJS += polkadot/crypto_helper.o
OBJS += polkadot/bignum.o
OBJS += polkadot/substrate/substrate_types_V18.o
OBJS += polkadot/substrate/substrate_types_V19.o
OBJS += polkadot/substrate/substrate_types.o
OBJS += polkadot/substrate/substrate_dispatch_V18.o
OBJS += polkadot/substrate/substrate_dispatch_V19.o
OBJS += polkadot/substrate/substrate_dispatch.o
OBJS += polkadot/parser_impl_common.o
OBJS += polkadot/parser_impl.o
OBJS += polkadot/parser.o
OBJS += polkadot/tx.o
endif

OBJS += debug.o

OBJS += ../vendor/trezor-crypto/address.o
OBJS += ../vendor/trezor-crypto/bignum.o
OBJS += ../vendor/trezor-crypto/ecdsa.o
OBJS += ../vendor/trezor-crypto/curves.o
OBJS += ../vendor/trezor-crypto/secp256k1.o
OBJS += ../vendor/trezor-crypto/nist256p1.o
OBJS += ../vendor/trezor-crypto/hmac_drbg.o
OBJS += ../vendor/trezor-crypto/rfc6979.o
OBJS += ../vendor/trezor-crypto/rand.o
OBJS += ../vendor/trezor-crypto/memzero.o

OBJS += ../vendor/trezor-crypto/ed25519-donna/curve25519-donna-32bit.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/curve25519-donna-helpers.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/modm-donna-32bit.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/ed25519-donna-basepoint-table.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/ed25519-donna-32bit-tables.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/ed25519-donna-impl-base.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/ed25519.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/curve25519-donna-scalarmult-base.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/ed25519-sha3.o
OBJS += ../vendor/trezor-crypto/ed25519-donna/ed25519-keccak.o

OBJS += ../vendor/trezor-crypto/hmac.o
OBJS += ../vendor/trezor-crypto/bip32.o
OBJS += ../vendor/trezor-crypto/bip39.o
OBJS += ../vendor/trezor-crypto/bip39_english.o
OBJS += ../vendor/trezor-crypto/pbkdf2.o
OBJS += ../vendor/trezor-crypto/base32.o
OBJS += ../vendor/trezor-crypto/base58.o
OBJS += ../vendor/trezor-crypto/segwit_addr.o

OBJS += ../vendor/trezor-crypto/ripemd160.o
OBJS += ../vendor/trezor-crypto/sha2.o
OBJS += ../vendor/trezor-crypto/sha3.o
OBJS += ../vendor/trezor-crypto/blake256.o
OBJS += ../vendor/trezor-crypto/blake2b.o
OBJS += ../vendor/trezor-crypto/blake2s.o
OBJS += ../vendor/trezor-crypto/groestl.o
OBJS += ../vendor/trezor-crypto/hasher.o

OBJS += ../vendor/trezor-crypto/aes/aescrypt.o
OBJS += ../vendor/trezor-crypto/aes/aeskey.o
OBJS += ../vendor/trezor-crypto/aes/aestab.o
OBJS += ../vendor/trezor-crypto/aes/aes_modes.o

OBJS += ../vendor/trezor-crypto/chacha20poly1305/chacha20poly1305.o
OBJS += ../vendor/trezor-crypto/chacha20poly1305/chacha_merged.o
OBJS += ../vendor/trezor-crypto/chacha20poly1305/poly1305-donna.o
OBJS += ../vendor/trezor-crypto/chacha20poly1305/rfc7539.o

OBJS += ../vendor/trezor-crypto/nem.o

OBJS += ../vendor/trezor-crypto/cardano.o

OBJS += ../vendor/trezor-crypto/sha512.o

OBJS += ../vendor/QR-Code-generator/c/qrcodegen.o

OBJS += ../vendor/trezor-storage/storage.o
OBJS += ../vendor/trezor-storage/norcow.o

OBJS += ../vendor/nanopb/pb_common.o
OBJS += ../vendor/nanopb/pb_decode.o
OBJS += ../vendor/nanopb/pb_encode.o

OBJS += protob/messages.pb.o
OBJS += protob/messages-bitcoin.pb.o
OBJS += protob/messages-common.pb.o
OBJS += protob/messages-crypto.pb.o
OBJS += protob/messages-debug.pb.o
OBJS += protob/messages-management.pb.o

ifneq ($(BITCOIN_ONLY),1)
OBJS += ../vendor/trezor-crypto/cash_addr.o
OBJS += protob/messages-ethereum.pb.o
OBJS += protob/messages-nem.pb.o
OBJS += protob/messages-solana.pb.o
OBJS += protob/messages-starcoin.pb.o
OBJS += protob/messages-stellar.pb.o
OBJS += protob/messages-tron.pb.o
OBJS += protob/messages-aptos.pb.o
OBJS += protob/messages-near.pb.o
OBJS += protob/messages-conflux.pb.o
OBJS += protob/messages-algorand.pb.o
OBJS += protob/messages-ripple.pb.o
OBJS += protob/messages-sui.pb.o
OBJS += protob/messages-filecoin.pb.o
OBJS += protob/messages-cosmos.pb.o
OBJS += protob/messages-polkadot.pb.o
endif

OPTFLAGS ?= -Os

../vendor/trezor-crypto/bip32.o: OPTFLAGS = -Os
../vendor/trezor-crypto/bip39.o: OPTFLAGS = -Os
../vendor/trezor-crypto/ecdsa.o: OPTFLAGS = -Os
../vendor/trezor-crypto/sha2.o: OPTFLAGS = -Os
../vendor/trezor-crypto/secp256k1.o: OPTFLAGS = -Os

include ../Makefile.include


ifeq ($(EMULATOR),1)
CFLAGS += --warn-no-unused-parameter
endif
CFLAGS += -fstack-protector-all
CFLAGS += -Wno-sequence-point
CFLAGS += -I../vendor/nanopb -Iprotob -DPB_FIELD_16BIT=1 -DPB_ENCODE_ARRAYS_UNPACKED=1 -DPB_VALIDATE_UTF8=1
CFLAGS += -DSCM_REVISION='"$(shell git rev-parse HEAD | sed 's:\(..\):\\x\1:g')"'
CFLAGS += -DBUILD_ID='"$(NAME)"'
CFLAGS += -DUSE_MONERO=0
ifneq ($(BITCOIN_ONLY),1)
CFLAGS += -DUSE_ETHEREUM=1
CFLAGS += -DUSE_NEM=1
MAKO_RENDER_FLAG =
else
CFLAGS += -DUSE_ETHEREUM=0
CFLAGS += -DUSE_NEM=0
MAKO_RENDER_FLAG = --bitcoin-only
endif

CFLAGS += -DCBOR_NO_FLOATING_POINT=l

CFLAGS += -DUSE_CARDANO=1

%:: %.mako defs
	@printf "  MAKO    $@\n"
	$(Q)$(PYTHON) ../vendor/trezor-common/tools/cointool.py render $(MAKO_RENDER_FLAG) $@.mako

ifeq ($(BOOTLOADER_QA), 0)
bl_data.h: bl_data.py bootloader.dat
	@printf "  PYTHON  bl_data.py bootloader.dat\n"
	$(Q)$(PYTHON) bl_data.py bootloader.dat
else
bl_data.h: bl_data.py bootloader_qa.dat
	@printf "  PYTHON  bl_data.py bootloader_qa.dat\n"
	$(Q)$(PYTHON) bl_data.py bootloader_qa.dat
endif

header.o: version.h

clean::
	rm -f bl_data.h
	find -maxdepth 1 -name "*.mako" | sed 's/.mako$$//' | xargs rm -f
